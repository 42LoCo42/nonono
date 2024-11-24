{
  description = "gothere - an email forwarder written in Go";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      rec {
        defaultPackage = pkgs.buildGoModule {
          pname = "gothere";
          version = "1";
          src = ./.;

          vendorSha256 = "sha256-t0ZLmplfo3Za34T8AbOEZq510pk7JMWvP8QchZ1fxHw=";

          CGO_ENABLED = 0;
        };

        dockerImage = pkgs.dockerTools.buildImage {
          name = defaultPackage.pname;
          tag = defaultPackage.version;

          config.Cmd = [
            "${defaultPackage}/bin/gothere"
          ];
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
