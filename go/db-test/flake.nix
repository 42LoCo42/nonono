{
  description = "Playing around with databases";

  inputs.flake-utils.url = "github:numtide/flake-utils";
  inputs.ref-merge.url = "github:42loco42/ref-merge";

  outputs = { self, nixpkgs, flake-utils, ref-merge }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        packages = with pkgs; [
          bashInteractive
          oapi-codegen
          ref-merge.outputs.defaultPackage.${system}
          yq
        ];
      in
      {
        defaultPackage = pkgs.buildGoModule {
          pname = "dbtest";
          version = "0.0.1";
          src = ./.;
          vendorSha256 = "sha256-hIP6h8VDt2yZk51fCSgZjvU+FUV/Oh+U3Gs5iqZy0uw=";
          patchPhase = "bash generate.sh";
          nativeBuildInputs = packages;
        };

        devShell = pkgs.mkShell {
          packages = packages ++ (with pkgs; [
            boltbrowser
            go
            gopls
          ]);
        };
      });
}
