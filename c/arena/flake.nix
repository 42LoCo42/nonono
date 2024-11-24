{
  description = "Example C/C++ flake";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      rec {
        defaultPackage = pkgs.stdenv.mkDerivation {
          pname = "arena";
          version = "0.0.1";
          src = ./.;
        };

        devShell = pkgs.mkShell {
          packages = with pkgs; [
            bashInteractive
            bear
            clang-tools
            valgrind
          ]
          ++ defaultPackage.nativeBuildInputs
          ++ defaultPackage.buildInputs;
        };
      });
}
