{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in
      rec {
        packages.default = pkgs.buildGoModule {
          pname = "gomponents-test";
          version = "1";
          src = ./.;
          vendorHash = "sha256-9hzGXvJ7chXNhe1UwqKjeUu/FJjlLwIsrDT/6BTjpMU=";

          nativeBuildInputs = with pkgs; [
            tailwindcss
          ];

          preBuild = ''
            tailwindcss -i tailwind.css -o static/tailwind.css --minify
          '';
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
          packages = with pkgs; [
            air
            nodePackages.prettier
          ];
        };
      });
}
