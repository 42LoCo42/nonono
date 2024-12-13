{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in rec {
        packages.default = pkgs.stdenv.mkDerivation rec {
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
            vulkan-validation-layers
          ];

          meta.mainProgram = pname;
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
          packages = with pkgs; [
            bear
          ];
        };
      });
}
