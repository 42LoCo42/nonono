{
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in rec {
        packages.default = pkgs.buildGoModule {
          pname = "argon-pk-auth";
          version = "0";
          src = ./.;
          vendorHash = "";

          nativeBuildInputs = with pkgs; [ pkg-config ];
          buildInputs = with pkgs; [ libsodium ];
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
          packages = with pkgs; [ gopls ];
        };
      });
}
