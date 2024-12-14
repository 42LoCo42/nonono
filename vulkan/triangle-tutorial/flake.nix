{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        dbg = {
          DEBUG = true;
          VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
        };

        pkg = pkgs.stdenv.mkDerivation rec {
          pname = "triangle";
          version = "0";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkg-config
            shaderc
            spirv-tools
            vulkan-headers
          ];

          buildInputs = with pkgs; [
            fmt
            glfw
            glm
            vulkan-loader
          ];

          meta.mainProgram = pname;
        };
      in
      rec {
        packages = rec {
          default = release;
          release = pkg;
          debug = pkg.overrideAttrs (old: dbg // {
            pname = "${old.pname}-debug";
          });
        };

        devShells.default = pkgs.mkShell (dbg // {
          inputsFrom = [ packages.debug ];
          packages = with pkgs; [ bear ];
        });
      });
}
