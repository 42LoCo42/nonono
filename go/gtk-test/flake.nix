{
  description = "Playing around with GTK3 in Golang";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        nativeBuildInputs = with pkgs; [
          makeWrapper
          pkg-config
        ];

        buildInputs = with pkgs; [
          cairo
          glib
          gtk3
        ];
      in
      {
        defaultPackage = pkgs.buildGoModule {
          pname = "gtk-test";
          version = "0.0.1";
          src = ./.;

          vendorSha256 = pkgs.lib.fakeSha256;

          inherit nativeBuildInputs buildInputs;
        };

        devShell = pkgs.mkShell {
          packages = with pkgs; [
            bashInteractive
            glade
            go
            gopls
          ];

          inherit nativeBuildInputs buildInputs;
        };
      });
}
