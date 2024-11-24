{
  inputs.nixGL.url = "github:nix-community/nixGL";
  inputs.nixGL.inputs.nixpkgs.follows = "nixpkgs";
  inputs.nixGL.inputs.flake-utils.follows = "flake-utils";

  outputs = { nixpkgs, flake-utils, nixGL, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ nixGL.overlay ];
        };
      in
      rec {
        inherit pkgs;
        defaultPackage = pkgs.stdenv.mkDerivation {
          name = "bounce";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkg-config
          ];

          buildInputs = with pkgs; [
            raylib
          ];
        };

        devShell = pkgs.mkShell {
          inputsFrom = [ defaultPackage ];
          packages = with pkgs; [
            bear
            clang-tools
            nixgl.auto.nixGLDefault
          ];
        };
      });
}
