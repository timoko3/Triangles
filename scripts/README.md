# Development scaffolding

Infrastructure adapted from cacheResearch: CMake 3.28+, Ninja, C++20 for the app,
LLVM 18 and Python 3.10+. On x86_64 Linux, `nix develop` provides the tools;
`nix flake check` verifies both the CMake scaffold and the parser application.
Add infrastructure files
to Git before using Nix: flakes use Git's file list. The lock file retains
cacheResearch's pinned nixpkgs revision.

```sh
cmake --preset debug
cmake --build --preset debug
cmake --preset release
cmake --build --preset release
```

These presets fetch no dependencies and do not compile unfinished headers.
`TRIANGLES_BUILD_APP` defaults to `OFF`. The `app` preset builds the parser CLI;
the `quality` preset also generates `compile_commands.json` for analysis.
Both use the installed GeneralFunctions package with `lexer`, `files` and
`geometry`. The parser uses lexer/files; a smoke test imports the geometry module
and checks a 3-4-5 triangle. The app links the geometry target for future use.
Use a toolchain supported by CMake C++ modules: the Nix shell provides Clang 18
and the wrapped clang-scan-deps. GCC 13 is insufficient for this CMake workflow.

```sh
nix develop
cmake --preset app
cmake --build --preset app
ctest --test-dir build/app --output-on-failure
./build/app/triangles inputs/input_1.txt
```

## GeneralFunctions dependency

The `generalFunctions` flake input selects a revision from
`DREC-Drillers/generalFunctions`; `flake.lock` pins the exact commit.
The dev shell builds a single package containing lexer, file utilities and
geometry. CMake consumes `GeneralFunctions::lexer`, `GeneralFunctions::files`
and `GeneralFunctions::geometry` instead of listing internal source files.
The common nixpkgs input and LLVM 18 keep the library and app toolchains aligned.

The selected revision must contain `file.cpp`, `file.h`, the CMake project and
`cmake/GeneralFunctionsConfig.cmake.in`. Nix does not combine Git branches.

To update the pinned revision after changing the input URL:

```sh
nix flake lock --update-input generalFunctions
```

To test a local checkout from WSL without changing the committed lock file,
run this command from the Triangles directory:

```sh
nix develop --override-input generalFunctions \
  path:/mnt/c/MIPT/proga/vlados/generalFunctions --no-write-lock-file
```

Re-enter the shell after editing the dependency: a Nix package is a source
snapshot, not a live link to the checkout. For live source development without
rebuilding a Nix package, use the explicit CMake override instead:

```sh
cmake -S . -B build/local -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DTRIANGLES_BUILD_APP=ON \
  -DGENERAL_FUNCTIONS_SOURCE_DIR=/path/to/generalFunctions
cmake --build build/local
```

The old `GENERAL_FUNCTIONS_DIR` setting is no longer used. For a manually
installed package outside Nix, pass its prefix through `CMAKE_PREFIX_PATH`.
The `app` and `quality` presets read the wrapped scanner path from
`GENERAL_FUNCTIONS_CLANG_SCAN_DEPS`, which is set by `nix develop`.
The shell exposes the pinned Nix wrapper's stack-protector, signed-overflow, PIC
and flexible-array settings in `CXXFLAGS`. The quality preset copies them into
the compilation database so clang-tidy can load the same module interfaces as
the compiler. Build the quality preset before analysis; recheck these settings
when updating nixpkgs or LLVM.

## Code quality

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
