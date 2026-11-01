{
  description = "Triangles development environment and build checks";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

    nixGL = {
      url = "github:nix-community/nixGL";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    generalFunctions = {
      url = "github:DREC-Drillers/generalFunctions/main";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, nixGL, generalFunctions, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      llvm = pkgs.llvmPackages_18;

      # Install all required components together so find_package sees one prefix.
      generalFunctionsPackage = generalFunctions.packages.${system}.default.override {
        buildFiles = true;
      };

      mkShell = pkgs.mkShell.override {
        stdenv = llvm.stdenv;
      };

      python = pkgs.python3.withPackages (packages: [
        packages.pyyaml
      ]);

      gladSources = pkgs.runCommand "glad-opengl-3.3" {
        nativeBuildInputs = [
          pkgs.python3Packages.glad2
        ];
      } ''
        glad \
          --api gl:core=3.3 \
          --extensions "" \
          --reproducible \
          --out-path "$out" \
          c
      '';

      mkQualityTool = tool: pkgs.writeShellScriptBin "${tool}-18" ''
        exec ${llvm.clang-tools}/bin/${tool} "$@"
      '';

      qualityTools = map mkQualityTool [
        "clang-format"
        "clang-tidy"
      ];
    in
    {
      devShells.${system}.default = mkShell {
        packages = [
          pkgs.cmake
          pkgs.ninja
          pkgs.git
          python
          llvm.clang-tools
          nixGL.packages.${system}.nixGLIntel
        ] ++ qualityTools;

        buildInputs = [
          pkgs.glfw
          pkgs.glm
          generalFunctionsPackage
        ];

        GLAD_SOURCE_DIR = "${gladSources}";
        GENERAL_FUNCTIONS_CLANG_SCAN_DEPS = "${llvm.clang-tools}/bin/clang-scan-deps";
        # Expose wrapper settings checked when clang-tidy loads a compiled module.
        CXXFLAGS = "-fstack-protector-strong -fwrapv -fPIC -fstrict-flex-arrays=1";

        shellHook = ''
          echo "Triangles environment: LLVM 18, CMake, Ninja, Python, GLFW and GLM."
        '';
      };

      checks.${system} = {
        scaffold = pkgs.runCommand "triangles-scaffold" {
          nativeBuildInputs = [ pkgs.cmake pkgs.ninja llvm.clang ];
        } ''
          export CC=clang CXX=clang++
          cmake -S ${self} -B build -G Ninja -DTRIANGLES_BUILD_APP=OFF
          cmake --build build
          touch "$out"
        '';

        application = pkgs.runCommand "triangles-application" {
          nativeBuildInputs = [ pkgs.cmake pkgs.ninja llvm.stdenv.cc llvm.clang-tools ];
          buildInputs = [ generalFunctionsPackage ];
        } ''
          set -euo pipefail
          cmake -S ${self} -B build -G Ninja \
            -DTRIANGLES_BUILD_APP=ON \
            -DCMAKE_CXX_COMPILER=${llvm.stdenv.cc}/bin/clang++ \
            -DCMAKE_CXX_COMPILER_CLANG_SCAN_DEPS=${llvm.clang-tools}/bin/clang-scan-deps
          cmake --build build
          ctest --test-dir build --output-on-failure
          ./build/triangles ${self}/inputs/input_1.txt | grep '^Read 1 triangles$'
          ./build/triangles ${self}/inputs/input_2.txt | grep '^Read 2 triangles$'
          touch "$out"
        '';
      };
    };
}
