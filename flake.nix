{
  description = "Triangles development environment and CMake scaffold checks";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

    generalFunctions = {
      url = "github:timoko3/generalFunctions";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, generalFunctions, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      llvm = pkgs.llvmPackages_18;

      mkShell = pkgs.mkShell.override {
        stdenv = llvm.stdenv;
      };

      python = pkgs.python3.withPackages (packages: [
        packages.pyyaml
      ]);

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
        ] ++ qualityTools;

        buildInputs = [
          pkgs.glfw
          pkgs.glm
        ];

        GENERAL_FUNCTIONS_DIR = "${generalFunctions}";

        shellHook = ''
          echo "Triangles environment: LLVM 18, CMake, Ninja, Python, GLFW and GLM."
        '';
      };

      checks.${system}.scaffold = pkgs.runCommand "triangles-scaffold" {
        nativeBuildInputs = [
          pkgs.cmake
          pkgs.ninja
          llvm.clang
        ];
      } ''
        export CC=clang CXX=clang++
        cmake -S ${self} -B build -G Ninja -DTRIANGLES_BUILD_APP=OFF
        cmake --build build
        touch "$out"
      '';
    };
}
