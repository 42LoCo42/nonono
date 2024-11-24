{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        inherit (pkgs.lib)
          fileContents
          pipe
          splitString
          ;
      in
      rec {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "nst";
          version = "0";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkg-config
          ];
        };

        packages.deps = pipe pkgs.python3 [
          builtins.toJSON
          (pkgs.writeText "cmd")
          (x: pipe x [
            pkgs.writeClosure
            fileContents
            (splitString "\n")
            (builtins.filter (y: toString x != y))
          ])
          builtins.toJSON
        ];

        devShells.default = pkgs.mkShell {
          inputsFrom = [ packages.default ];
          packages = with pkgs; [
            bear
            clang-tools
            crun
          ];
        };
      });
}
