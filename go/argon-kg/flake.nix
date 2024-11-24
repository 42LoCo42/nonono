{
  description = "Argon2-based SSH key generator";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        defaultPackage = pkgs.buildGoModule {
          pname = "argon-kg";
          version = "1";
          src = ./.;

          vendorSha256 = "sha256-hrr1IYDqS7UyGjKPfd2EYZWkh8aOR+mC5XPgN5Eotqw=";
        };

        devShell = pkgs.mkShell {
          packages = with pkgs; [
            bashInteractive
            go
            gopls
          ];
        };
      });
}
