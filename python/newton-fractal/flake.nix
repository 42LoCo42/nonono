{
  description = "Creating fractals from the Newton method";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        python = pkgs.python311;

        pyenv = python.withPackages (ps: with ps; [
          flake8
          matplotlib
          sympy
        ]);

        shellPackages = with pkgs; [
          bashInteractive
          pyenv

          (python.withPackages (p: with p; [
            python-lsp-server
          ]))
        ];
      in
      {
        devShell = pkgs.mkShell {
          packages = shellPackages;
          TK_LIBRARY = "${pkgs.tk}/lib/${pkgs.tk.libPrefix}";
        };
      });
}
