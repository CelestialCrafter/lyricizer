{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  outputs =
    { nixpkgs, ... }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
      nativeBuildInputs = with pkgs; [
        ninja
        clang
        libmpdclient
        curl
        tomlc99
        json_c
      ];
    in
    {
      packages.x86_64-linux.default = pkgs.stdenv.mkDerivation {
        name = "lyricizer";
        src = ./.;

		meta = {
			homepage = "https://github.com/CelestialCrafter/lyricizer";
			license = pkgs.lib.licenses.mpl20;
		};

        inherit nativeBuildInputs;

        buildPhase = "ninja";
        installPhase = ''
          mkdir -p $out/bin
          cp build/lyricizer $out/bin/lyricizer
        '';
      };

      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = nativeBuildInputs;
      };
    };
}
