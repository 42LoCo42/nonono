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
          pname = "ref-merge";
          version = "0.0.1";
          src = ./.;
          vendorSha256 = "sha256-xzSzwLZk6seEcUZoguGYCfOy0/PwYWgoT/2AkoiAaUA=";
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
