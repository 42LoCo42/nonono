{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in rec {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "determssh";
          version = "0";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkg-config
          ];

          buildInputs = with pkgs; [
            libsodium
          ];
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
          packages = with pkgs; [
            bear
            clang-tools
          ];
        };
      });
}
