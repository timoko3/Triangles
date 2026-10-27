# Development scaffolding

Infrastructure adapted from cacheResearch: CMake 3.24+, Ninja, C++17,
LLVM 18 and Python 3.10+. On x86_64 Linux, `nix develop` provides the tools;
`nix flake check` verifies the empty CMake scaffold. Add infrastructure files
to Git before using Nix: flakes use Git's file list. The lock file retains
cacheResearch's pinned nixpkgs revision.

```sh
cmake --preset debug
cmake --build --preset debug
cmake --preset release
cmake --build --preset release
```

These presets fetch no dependencies and do not compile unfinished headers.
`TRIANGLES_BUILD_APP` defaults to `OFF`. The `quality` preset enables the existing
`main.cpp` placeholder to generate `compile_commands.json`. Real application
targets, OpenGL dependencies and tests can be added when implementations are
ready; no dummy tests are registered.

```sh
python3 scripts/code_quality.py format
python3 scripts/code_quality.py format-fix
cmake --preset quality
cmake --build --preset quality
python3 scripts/code_quality.py tidy --build-dir build/quality
python3 scripts/code_quality.py style-fix --build-dir build/quality
```

Formatting uses the complete clang-format 18 configuration from cacheResearch
(four spaces, attached braces, 85 columns, LF). Naming and defect checks also
come from that project. `style-fix` requires PyYAML (included in `nix develop`),
collects naming replacements before applying them and formats the result.
It changes public names too: review its diff before committing.

Checks use tracked C/C++ files, excluding `generalFunctions`, `third_party`
and submodules. Add new sources to Git to include them. Clang-tidy analyzes
translation units and included project headers; unfinished headers are only
formatted until real translation units include them. CI checks formatting,
clang-tidy and all three CMake presets without modifying the repository.
