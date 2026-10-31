{
    description = "Triangles development environment and CMake scaffold checks";

    inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

    inputs.generalFunctions = {
        url = "github:timoko3/generalFunctions";
        flake = false;
    };

    outputs = { self, nixpkgs, generalFunctions, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      llvm = pkgs.llvmPackages_18;
      python = pkgs.python3.withPackages (packages: [ packages.pyyaml ]);
      qualityTools = map (tool: pkgs.writeShellScriptBin "${tool}-18" ''
        exec ${llvm.clang-tools}/bin/${tool} "$@"
      '') [ "clang-format" "clang-tidy" ];
    in
    {
      checks.${system}.scaffold = pkgs.runCommand "triangles-scaffold" {
        nativeBuildInputs = [ pkgs.cmake pkgs.ninja llvm.clang ];
      } ''
        export CC=clang CXX=clang++
        cmake -S ${self} -B build -G Ninja -DTRIANGLES_BUILD_APP=OFF
        cmake --build build
        touch "$out"
      '';

      devShells.${system}.default = (pkgs.mkShell.override { stdenv = llvm.stdenv; }) {
        packages = [ pkgs.cmake pkgs.ninja pkgs.git python ] ++ qualityTools;
        GENERAL_FUNCTIONS_DIR = "${generalFunctions}";
        shellHook = ''
          echo "Triangles environment: LLVM 18, CMake, Ninja and Python."
        '';
      };
    };
}
