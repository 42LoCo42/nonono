{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        name = "svelte-vite-go-nix";
        version = "0.0.1";

        frontend = pkgs.stdenv.mkDerivation rec {
          pname = "${name}-frontend";
          inherit version;
          src = ./frontend;

          pnpmDeps = pkgs.pnpm.fetchDeps {
            inherit pname version src;
            hash = "sha256-OGn8kjlLEmadvLnkd24b2rNuczLdvrnbQaQSxoqL8XE=";
          };

          nativeBuildInputs = with pkgs; [
            nodejs
            pnpm.configHook
          ];

          buildPhase = "pnpm build";
          installPhase = "cp -r dist $out";
        };

        backend = pkgs.buildGoModule {
          pname = name;
          inherit version;
          src = ./backend;

          vendorHash = "sha256-tbYafktlM84YLYZvhY2kaeYwlFrWGeFXGnKCEbpAYos=";

          preBuild = "cp -r ${frontend} dist";

          CGO_ENABLED = "0";
          ldflags = [ "-s" "-w" ];
          tags = [ "prod" ];
        };

        dev = pkgs.writeShellApplication {
          name = "dev";
          runtimeInputs = with pkgs; [ air runit ];
          text = "runsvdir services";
        };
      in
      {
        packages.default = backend;

        devShells.default = pkgs.mkShell {
          inputsFrom = [ frontend backend ];

          packages = with pkgs; [
            dev
            svelte-language-server
          ];
        };
      });
}
