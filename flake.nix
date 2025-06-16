# SuperTux
# Copyright (C) 2021 Ingo Ruhnke <grumbel@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

{
  description = "A 2D platform game featuring Tux the penguin";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    sexpcpp.url = "github:lispparser/sexp-cpp";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";

    tinygettext.url = "github:tinygettext/tinygettext";
    tinygettext.inputs.nixpkgs.follows = "nixpkgs";
    tinygettext.inputs.flake-utils.follows = "flake-utils";
    tinygettext.inputs.tinycmmc.follows = "tinycmmc";

    SDL2_ttf.url = "github:SuperTux/SDL_ttf";
    SDL2_ttf.inputs.nixpkgs.follows = "nixpkgs";
    SDL2_ttf.inputs.flake-utils.follows = "flake-utils";

    squirrel_src.url = "github:albertodemichelis/squirrel";
    squirrel_src.flake = false;
  };

  outputs = { self, nixpkgs, flake-utils,
              tinycmmc, sexpcpp, tinygettext, SDL2_ttf,
              squirrel_src }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree rec {
          squirrel = pkgs.stdenv.mkDerivation {
            pname = "squirrel";
            version = "3.2";
            src = squirrel_src;
            nativeBuildInputs = [
              pkgs.cmake
            ];
          };

          supertux2 = pkgs.stdenv.mkDerivation rec {
            pname = "supertux2";
            # FIXME: Should use `git describe` to get the version
            # number or leave it to cmake, but the .git/ directory
            # isn't included in the Nix store.
            version = "0.6.3-${nixpkgs.lib.substring 0 8 self.lastModifiedDate}-${self.shortRev or "dirty"}";
            src = nixpkgs.lib.cleanSource ./.;
            patchPhase = let
              ver = builtins.splitVersion version;
            in ''
              substituteInPlace config.h.cmake \
                 --replace "#define _SQ64" ""

               cat > version.cmake <<EOF
SET(SUPERTUX_VERSION_MAJOR ${builtins.elemAt ver 0})
SET(SUPERTUX_VERSION_MINOR ${builtins.elemAt ver 1})
SET(SUPERTUX_VERSION_PATCH ${builtins.elemAt ver 2})
SET(SUPERTUX_VERSION_TWEAK ${builtins.elemAt ver 3})
SET(SUPERTUX_VERSION_STRING "v${version}")
SET(SUPERTUX_VERSION_BUILD "${builtins.elemAt ver 4}")
EOF
            '';
            cmakeFlags = [
              "-DINSTALL_SUBDIR_BIN=bin"
              "-DUSE_SYSTEM_SDL2_TTF=ON"
            ];
            enableParallelBuilding = true;
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.pkgconfig
              pkgs.makeWrapper
              pkgs.git
            ];
            postFixup = ''
                wrapProgram $out/bin/supertux2 \
                  --prefix LIBGL_DRIVERS_PATH ":" "${pkgs.mesa.drivers}/lib/dri" \
                  --prefix LD_LIBRARY_PATH ":" "${pkgs.mesa.drivers}/lib"
            '';
            buildInputs = [
              squirrel
              sexpcpp.packages.${system}.default
              tinygettext.packages.${system}.default
              SDL2_ttf.packages.${system}.default

              pkgs.physfs
              pkgs.libpng
              pkgs.curl
              pkgs.fmt_8
              pkgs.libGL
              pkgs.libGLU
              pkgs.glew
              pkgs.gtest

              pkgs.glm
              pkgs.SDL2
              pkgs.SDL2_image

              pkgs.openal
              pkgs.libvorbis
              pkgs.libogg
              pkgs.gtest
            ];
          };

          default = supertux2;
        };
      }
    );
}
