;; SuperTux
;; Copyright (C) 2019 Ingo Ruhnke <grumbel@gmail.com>
;;
;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

(set! %load-path
  (cons* "/ipfs/QmNbci3WpEWumBVvfbqsKhKnJTtdmkyVwYtSaPr5jVZo2m/guix-cocfree_0.0.0-56-g3a32b64"
         %load-path))

(use-modules (guix build utils)
             (guix build-system cmake)
             ((guix licenses) #:prefix license:)
             (guix packages)
             (gnu packages audio)
             (gnu packages curl)
             (gnu packages fontutils)
             (gnu packages fribidi)
             (gnu packages game-development)
             (gnu packages gcc)
             (gnu packages gl)
             (gnu packages gtk)
             (gnu packages maths)
             (gnu packages pkg-config)
             (gnu packages python)
             (gnu packages sdl)
             (gnu packages squirrel)
             (gnu packages version-control)
             (gnu packages xiph)
             (guix-cocfree utils))

(define %source-dir (dirname (current-filename)))

(define-public supertux
  (package
   (name "supertux")
   (version (version-from-source %source-dir))
   (source (source-from-source %source-dir))
   (arguments
    `(#:tests? #f
      #:configure-flags '("-DINSTALL_SUBDIR_BIN=bin"
                          "-DUSE_SYSTEM_PHYSFS=ON")
      #:phases
      (modify-phases %standard-phases
         (add-after 'unpack 'set-version-number
                    (lambda _
                      (substitute* "version.cmake.in"
                       (("\\$\\{MAJOR_VERSION_GIT\\}") ,"0")
                       (("\\$\\{MINOR_VERSION_GIT\\}") ,"6")
                       (("\\$\\{PATCH_VERSION_GIT\\}") ,"2")
                       (("\\$\\{TWEAK_VERSION_GIT\\}") ,"")
                       (("\\$\\{VERSION_STRING_GIT\\}") ,version))
                      (copy-file "version.cmake.in" "version.cmake")
             #t)))))
   (build-system cmake-build-system)
   (native-inputs
    `(("pkg-config" ,pkg-config)
      ("python" ,python)
      ("gcc-10" ,gcc-10)))
   (inputs
    `(("sdl2" ,sdl2)
      ("sdl2-image" ,sdl2-image)
      ("openal" ,openal)
      ("mesa" ,mesa)
      ("glew" ,glew)
      ("libvorbis" ,libvorbis)
      ("libogg" ,libogg)
      ("physfs" ,physfs)
      ("curl" ,curl)
      ("boost" ,boost-1.74)
      ("freetype" ,freetype)
      ("libraqm" ,libraqm)
      ("fribidi" ,fribidi)
      ("harfbuzz" ,harfbuzz)
      ("squirrel" ,squirrel)
      ("glm", glm)))
   (synopsis (synopsis-from-source %source-dir))
   (description (description-from-source %source-dir))
   (home-page (homepage-from-source %source-dir))
   (license license:gpl3+)))

supertux

;; EOF ;;
