{
  description = "This hack blocks wormhole upload in Reassembly";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        python = pkgs.python311;
        pyenv = python.withPackages (ps: with ps; [
          dns
          requests
        ]);
      in
      {
        defaultPackage = python.pkgs.buildPythonApplication {
          pname = "reassembly-wormhole-hack";
          version = "1";
          src = ./.;
          buildInputs = [
            pyenv
          ];
        };

        devShell = pkgs.mkShell {
          packages = with pkgs; [
            bashInteractive
            pyenv
          ];
        };
      });
}
