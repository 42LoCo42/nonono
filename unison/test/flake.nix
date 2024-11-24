{
  description = "A tiny flake";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        unison = pkgs.unison-ucm.overrideAttrs (_: rec {
          version = "M4i";
          src = pkgs.fetchurl {
            url = "https://github.com/unisonweb/unison/releases/download/release/${version}/ucm-linux.tar.gz";
            hash = "sha256-Qx8vO/Vaz0VdCGXwIwRQIuMlp44hxCroQ7m7Y+m7aXk=";
          };
        });
      in
      {
        devShell = pkgs.mkShell {
          packages = with pkgs; [
            bashInteractive
            unison
          ];
        };
      });
}
