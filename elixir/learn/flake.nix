{
  description = "Learning Elixir";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShell = pkgs.mkShell {
          packages = with pkgs; [
            beamPackages.hex
            elixir-ls
            elixir_1_15
          ];
        };
      });
}
