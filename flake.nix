{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  outputs =
    { nixpkgs, ... }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
      buildInputs = with pkgs; [
        ninja
        clang
        libmpdclient
        curl
        tomlc99
      ];
    in
    {
      packages.x86_64-linux.default = pkgs.stdenv.mkDerivation {
        name = "lyricizer";
        src = ./.;

        inherit buildInputs;

        buildPhase = "ninja";
        installPhase = ''
          mkdir -p $out/bin
          cp build/lyricizer $out/bin/lyricizer
        '';
      };

      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = buildInputs;
      };
    };
}
