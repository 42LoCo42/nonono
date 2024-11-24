{
  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in rec {
        packages.default = pkgs.buildGoModule {
          name = "uploader";
          src = ./.;
          vendorHash = "sha256-+vAYw4o4msiio+dqqFpLc+dxmWvPk1plAPlWPNi99GM=";
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
          packages = with pkgs; [
            gopls
          ];
        };
      });
}
