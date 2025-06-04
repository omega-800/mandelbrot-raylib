{
  description = "mandelbrot written in c";

  inputs.nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";

  outputs =
    { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      eachSystem = f: nixpkgs.lib.genAttrs systems f;
    in
    {
      devShells = eachSystem (system: rec {
        mandelbroetli-c =
          let
            pkgs = import nixpkgs { inherit system; };
          in
          pkgs.mkShell {
            packages = with pkgs; [
              pkg-config
              raylib
              cmake
              gdb
              gcc
            ];
          };
        default = mandelbroetli-c;
      });
      packages = eachSystem (system: rec {
        mandelbroetli-c =
          let
            pkgs = import nixpkgs { inherit system; };
          in
          pkgs.stdenv.mkDerivation {
            name = "mandelbroetli-c";
            version = "0.0.1";
            installPhase = ''
              mkdir -p $out/bin
              make 
              mv mandelbroetli-c $out/bin
            '';
            src = ./.;
            buildInputs = with pkgs; [ raylib ];
          };
        default = mandelbroetli-c;
      });
      apps = eachSystem (system: rec {
        mandelbroetli-c = {
          type = "app";
          program = "${self.packages.${system}.mandelbroetli-c}/bin/mandelbroetli-c";
        };
        default = mandelbroetli-c;
        debug =
          let
            pkgs = import nixpkgs { inherit system; };
          in
          {
            type = "app";
            program = "${pkgs.writeShellScript "debug-mandelbroetli-c" "${pkgs.gdb}/bin/gdb -ex r ${self.packages.${system}.mandelbroetli-c}/bin/mandelbroetli-c"}";
          };
      });
    };
}
