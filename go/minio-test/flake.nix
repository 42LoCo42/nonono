{
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in rec {
        defaultPackage = pkgs.buildGoModule {
          pname = "minio-test";
          version = "1";
          src = ./.;
          vendorHash = "";
        };

        devShell = pkgs.mkShell {
          inputsFrom = [ defaultPackage ];
          packages = with pkgs; [
            gopls
          ];
        };
      });
}
