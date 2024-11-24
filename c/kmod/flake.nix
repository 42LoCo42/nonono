{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        kernel = pkgs.linuxKernel.kernels.linux_6_8.dev;
        KERNEL = "${kernel}/lib/modules/${kernel.version}";
      in
      rec {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "kmod";
          version = "0";
          src = ./.;
          inherit KERNEL;
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
          packages = with pkgs; [
            bear
            clang-tools
          ];
          inherit KERNEL;
        };
      });
}
