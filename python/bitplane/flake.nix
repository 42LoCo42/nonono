{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        py = pkgs.python3.pkgs;

        inherit (pkgs.lib) pipe;
        inherit (pkgs.lib.fileset) toSource unions;

        project = pipe ./pyproject.toml [
          builtins.readFile
          builtins.fromTOML
          (x: x.project)
        ];

        pname = project.name;

        src = toSource {
          root = ./.;
          fileset = unions [
            ./${pname}.py
            ./pyproject.toml
          ];
        };
      in
      rec {
        packages.default = py.buildPythonApplication {
          inherit pname src;
          inherit (project) version;

          pyproject = true;

          build-system = with py; [
            setuptools
          ];

          dependencies = with py; [
            matplotlib
            numpy
            pillow
          ];
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
        };
      });
}
