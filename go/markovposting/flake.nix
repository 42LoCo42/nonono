{
  description = "Example go flake";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        defaultPackage = pkgs.buildGoModule {
          pname = "example";
          version = "0.0.1";
          src = ./.;

          vendorSha256 = pkgs.lib.fakeSha256;
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
