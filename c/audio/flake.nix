{
  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        MINIAUDIO = pkgs.miniaudio;
      in
      rec {
        defaultPackage = pkgs.stdenv.mkDerivation {
          pname = "audio";
          version = "0.0.1";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkg-config
          ];

          buildInputs = with pkgs; [
            pulseaudio.dev
          ];

          inherit MINIAUDIO;
        };

        devShell = pkgs.mkShell {
          inputsFrom = [ defaultPackage ];
          packages = with pkgs; [
            bear
            clang-tools
          ];

          inherit MINIAUDIO;
        };
      });
}
