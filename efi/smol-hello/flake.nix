{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        tgt = pkgs.pkgsCross.mingwW64.buildPackages;

        vars = {
          GNU_EFI = pkgs.gnu-efi;
          OVMF = "${pkgs.OVMF.fd}/FV/OVMF.fd";
        };
      in
      rec {
        packages.default = tgt.stdenv.mkDerivation (vars // {
          name = "smol-hello";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            gnu-efi
            libisoburn
            mtools
          ];
        });

        devShells.default = pkgs.mkShell (vars // {
          inputsFrom = [ packages.default ];
          packages = with pkgs; [
            bear
          ];
        });
      });
}
