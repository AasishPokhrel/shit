#!/usr/bin/guile -s
!#

;; SuperTux 0.1.3 to SuperTux 0.2.x level conversion helper
;; Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
                                        ;
;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License
;; as published by the Free Software Foundation; either version 2
;; of the License, or (at your option) any later version.
                                        ;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
                                        ;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

                                        ;
;; The rest of this file may seem like a Long Irritating Series of Parentheses,
;; but it's actually a program. Install a Scheme interpreter, e.g. scm, to run
;; it.
                                        ;
;; This program aids in the conversion of SuperTux levels from 0.1.3 level
;; format to the one used by SuperTux 0.2.x.
                                        ;
;; Usage:
;;   levelconverter-0.1.3_0.2.0.scm < oldformat.stl > newformat.stl
                                        ;
;; Bugs:
;;   background and music might need manual tweaking
;;   gradients aren't properly converted

(use-modules (ice-9 pretty-print))

;; return first sublist in haystack that starts with needle or #f if none is found
(define (find-sublist haystack needle)
  (cond
   ((not (pair? haystack))
    #f)
   ((and (pair? (car haystack)) (eq? (caar haystack) needle))
    (cdar haystack))
   (else
    (find-sublist (cdr haystack) needle))))

;; return SuperTux 0.1.3 object in SuperTux 0.2.x form
(define (convert-object object)
  (cond
   ((eq? (car object) 'money)
    (append '(jumpy) (cdr object)))
   (else
    object)))

(define (convert-music filename)
  (if (string-suffix? ".mod" filename)
      (string-append "music/" (substring filename 0 (- (string-length filename) 4)) ".ogg")
      (string-append "music/" filename)))

(define (convert-background filename)
  (if (equal? filename "arctis.png")
      (string-append "images/background/arctis.jpg")
      (string-append "images/background/" filename)))

;; return SuperTux 0.1.3 level in SuperTux 0.2.x form
(define (convert-level level)
  (let
      ((type (car level))
       (version (find-sublist level 'version))
       (author (find-sublist level 'author))
       (name (find-sublist level 'name))
       (width (find-sublist level 'width))
       (height (find-sublist level 'height))
       (start_pos_x (find-sublist level 'start_pos_x))
       (start_pos_y (find-sublist level 'start_pos_y))
       (interactive-tm (find-sublist level 'interactive-tm))
       (background-tm (find-sublist level 'background-tm))
       (foreground-tm (find-sublist level 'foreground-tm))
       (background (find-sublist level 'background))
       (music (find-sublist level 'music))
       (objects (find-sublist level 'objects))
       )

    (if (not (string=? (symbol->string type) "supertux-level")) (error "not a supertux-level:" type))
    (if (not (= (car version) 1)) (error "not a version 1 level"))
    (if (not author) (set! author '("Anonymous")))
    (if (not name) (set! name '("Unnamed Level")))
    (if (not width) (error "No level width given"))
    (if (not height) (set! height '(15)))
    (if (not start_pos_x) (set! start_pos_x '(100)))
    (if (not start_pos_y) (set! start_pos_y '(170)))
    (if (not interactive-tm) (error "No interactive tilemap given"))
    (if (not background-tm) (warn "No background tilemap given"))
    (if (not foreground-tm) (warn "No foreground tilemap given"))
    (if (not objects) (error "No objects list given"))

    (quasiquote
     (supertux-level
      (version 2)
      (name (_ ,(car name)))
      (author ,(car author))
      ,(append
        (quasiquote
         (sector
          (name "main")
          ,@(if music
                `((music ,(convert-music (car music))))
                '())
          ,@(if (and background (not (equal? background '(""))))
                `((background (image ,(convert-background (car background)))))
                '((gradient
                  (top_color 0 0 0.2)
                  (bottom_color 0 0 0.6)
                  )))
          ,@(if foreground-tm
                `((tilemap
                   (z-pos -100)
                   (solid #f)
                   (speed 1)
                   (width ,(car width))
                   (height ,(car height))
                   (tiles ,@background-tm)
                   ))
                '())
          (tilemap
           (z-pos 0)
           (solid #t)
           (speed 1)
           (width ,(car width))
           (height ,(car height))
           (tiles ,@interactive-tm)
           )
          ,@(if foreground-tm
                `((tilemap
                   (z-pos 100)
                   (solid #f)
                   (speed 1)
                   (width ,(car width))
                   (height ,(car height))
                   (tiles ,@foreground-tm)
                   )) '())
          (spawnpoint
           (name "main")
           (x ,(car start_pos_x))
           (y ,(car start_pos_y))
           )
          )
         )
        (map convert-object objects)
        )
      )
     )
    )
  )

;; run conversion on stdin, output to stdout
(pretty-print (convert-level (read)))
(newline)
(quit)

;; EOF ;;

