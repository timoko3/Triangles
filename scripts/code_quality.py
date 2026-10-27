"""Run the same C++ quality checks locally and in CI (LLVM tools version 18)."""

import argparse
import json
import re
from pathlib import Path
import subprocess
import sys
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".cxx"}
HEADER_SUFFIXES = {".h", ".hh", ".hpp", ".hxx"}
FORMAT_MAJOR_VERSION = 18


def check_format_version():
    output = subprocess.check_output(
        ["clang-format-18", "--version"], text=True, cwd=ROOT
    )
    match = re.search(r"\bversion\s+(\d+\.\d+\.\d+)\b", output)
    if match is None or int(match.group(1).split(".")[0]) != FORMAT_MAJOR_VERSION:
        raise SystemExit(
            f"Expected clang-format {FORMAT_MAJOR_VERSION}.x; got: {output.strip()}. "
            "Use the project's LLVM major version before checking or applying style."
        )


def project_files():
    # Gitlinks are not expanded: submodule sources and generated files are excluded.
    tracked = subprocess.check_output(["git", "ls-files", "-z"], cwd=ROOT)
    return [
        Path(name.decode("utf-8"))
        for name in tracked.split(b"\0")
        if name
        and Path(name.decode("utf-8")).suffix in SOURCE_SUFFIXES | HEADER_SUFFIXES
        and Path(name.decode("utf-8")).parts[0] not in {"third_party", "generalFunctions"}
    ]


def tidy_command(source, build_dir, compiled, options=()):
    command = ["clang-tidy-18", str(source), "--config-file=.clang-tidy", *options]
    if (ROOT / source).resolve() in compiled:
        command.extend(["-p", str(build_dir)])
    else:
        # Standalone tools not declared in CMake still receive analysis.
        command.extend(["--", "-std=c++17", "-I", str(ROOT)])
    return command


def fix_naming(files, build_dir, compiled):
    try:
        import yaml
    except ImportError:
        raise SystemExit("style-fix requires PyYAML: python -m pip install PyYAML")

    # Analyze every translation unit before changing shared headers.
    originals = {}
    for source in files:
        path = (ROOT / source).resolve()
        if not path.is_relative_to(ROOT.resolve()):
            raise SystemExit(f"Refusing to rename outside the repository: {path}")
        originals[path] = path.read_bytes()
    replacements = {}
    with tempfile.TemporaryDirectory(prefix="triangles-naming-", dir=build_dir) as temporary:
        for index, source in enumerate(files):
            if source.suffix not in SOURCE_SUFFIXES:
                continue
            fixes = Path(temporary) / f"{index}.yaml"
            command = tidy_command(source, build_dir, compiled, [
                "--checks=-*,readability-identifier-naming",
                "--warnings-as-errors=-*",
                f"--export-fixes={fixes}",
            ])
            print(f"Collecting naming fixes for {source}", flush=True)
            if subprocess.call(command, cwd=ROOT) != 0:
                return 1
            if not fixes.exists():
                continue
            report = yaml.safe_load(fixes.read_text(encoding="utf-8")) or {}
            for diagnostic in report.get("Diagnostics", []):
                if diagnostic["DiagnosticName"] != "readability-identifier-naming":
                    continue
                messages = [diagnostic["DiagnosticMessage"], *diagnostic.get("Notes", [])]
                for message in messages:
                    for fix in message.get("Replacements", []):
                        path = Path(fix["FilePath"]).resolve()
                        if path not in originals:
                            raise SystemExit(f"Refusing to rename outside project files: {path}")
                        replacements.setdefault(path, set()).add((
                            fix["Offset"], fix["Length"], fix["ReplacementText"].encode("utf-8")
                        ))

    updated = {}
    for path, fixes in replacements.items():
        original = originals[path]
        end = 0
        for offset, length, text in sorted(fixes):
            if offset < end or offset < 0 or offset + length > len(original):
                raise SystemExit(f"Conflicting naming fixes in {path}; no fixes applied")
            end = offset + length
        contents = original
        # LLVM offsets count bytes; replace from the end to preserve earlier offsets.
        for offset, length, text in sorted(fixes, reverse=True):
            contents = contents[:offset] + text + contents[offset + length:]
        updated[path] = contents
    if any(path.read_bytes() != original for path, original in originals.items()):
        raise SystemExit("Project files changed during analysis; no fixes applied")
    for path, contents in updated.items():
        path.write_bytes(contents)
    print(f"Applied naming fixes to {len(updated)} files", flush=True)
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("check", choices=["format", "format-fix", "tidy", "style-fix"])
    parser.add_argument("--build-dir", default="build/quality")
    args = parser.parse_args()
    files = project_files()
    if not files:
        parser.error("No tracked project C/C++ files found")

    if args.check.startswith("format"):
        check_format_version()
        flags = ["-i"] if args.check == "format-fix" else ["--dry-run", "--Werror"]
        return subprocess.call(
            ["clang-format-18", "--style=file", *flags, *map(str, files)], cwd=ROOT
        )

    build_dir = (ROOT / args.build_dir).resolve()
    with (build_dir / "compile_commands.json").open(encoding="utf-8") as stream:
        database = json.load(stream)
    compiled = {
        (Path(entry["directory"]) / entry["file"]).resolve() for entry in database
    }
    options = []
    if args.check == "style-fix":
        check_format_version()
        if fix_naming(files, build_dir, compiled) != 0:
            return 1
        if subprocess.call(
            ["clang-format-18", "--style=file", "-i", *map(str, files)], cwd=ROOT
        ) != 0:
            return 1
        options = ["--checks=-*,readability-identifier-naming"]

    failed = False
    for source in files:
        if source.suffix not in SOURCE_SUFFIXES:
            continue
        command = tidy_command(source, build_dir, compiled, options)
        print(f"Analyzing {source}", flush=True)
        failed |= subprocess.call(command, cwd=ROOT) != 0
    return int(failed)


if __name__ == "__main__":
    sys.exit(main())
