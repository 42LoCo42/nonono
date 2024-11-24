{
  description = "Golang values to 9P adapter";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        mkApps = (names: builtins.listToAttrs (map
          (name: {
            inherit name;
            value = flake-utils.lib.mkApp {
              drv = self.defaultPackage.${system};
              exePath = "/bin/${name}";
            };
          })
          names));
      in
      {
        defaultPackage = pkgs.buildGoModule {
          pname = "adapter";
          version = "0.0.1";
          src = ./.;

          vendorSha256 = "sha256-J0mcm+VMJLcmuYOJvwhAYZTcS+QmAxcZwS8KSpSeCSc=";
        };

        apps = mkApps [
          "adapter"
          "call"
        ];

        devShell = pkgs.mkShell {
          packages = with pkgs; [
            bashInteractive
            go
            gopls
            msgpack-tools
            python311
          ];
        };
      });
}
