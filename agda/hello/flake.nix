{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        agda-env = pkgs.agda.withPackages (p: with p; [
          standard-library
        ]);
      in
      {
        devShells.default = pkgs.mkShell {
          packages = [
            agda-env
          ];
        };
      });
}
