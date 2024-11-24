{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in rec {
        packages.default = pkgs.buildGoModule rec {
          pname = "github-newest-tag";
          version = "1";
          src = ./.;
          vendorHash = "sha256-Phe6GXRi+mUk2bArydPR4II+NUhKI/+bOG12eqYBRz8=";

          CGO_ENABLED = "0";

          stripAllList = [ "bin" ];

          meta = {
            description = "Get the newest tag of a GitHub repo through the GraphQL API";
            homepage = "https://github.com/42LoCo42/github-newest-tag";
            mainProgram = pname;
          };
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
        };
      });
}
