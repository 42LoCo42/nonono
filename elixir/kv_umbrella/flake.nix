{
  description = "A simple key-value store";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        defaultPackage = pkgs.beamPackages.mixRelease {
          pname = "kv";
          version = "0.1.0";
          src = ./.;
        };

        devShell = pkgs.mkShell {
          packages = with pkgs; [
            beamPackages.hex
            elixir-ls
            elixir_1_15
          ];
        };
      });
}
