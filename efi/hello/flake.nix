{
  description = "UEFI development";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        gnu-efi-src = pkgs.stdenvNoCC.mkDerivation {
          name = "gnu-efi-src";
          src = pkgs.fetchzip {
            url = "file://${pkgs.gnu-efi.src}";
            hash = "sha256-flQJIRPKd0geQRAtJSu4vravJG0lTB6BfeIqpUM5P2I=";
          };

          dontBuild = true;
          installPhase = ''
            mkdir -p "$out"
            cp -r inc lib "$out"
          '';
        };

        vars = {
          GNU_EFI = "${gnu-efi-src}";
          OVMF = "${pkgs.OVMF.fd}";
        };
      in
      rec {
        defaultPackage = pkgs.stdenvNoCC.mkDerivation ({
          name = "uefi-hello";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkgsCross.mingwW64.buildPackages.gcc
          ];
        } // vars);

        devShell = pkgs.mkShell ({
          packages = with pkgs; [
            bashInteractive
            bear
            clang-tools
            libqalculate
          ];

          inputsFrom = [ defaultPackage ];
        } // vars);
      });
}
