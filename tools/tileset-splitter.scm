;
;
; $Id$
;
; SuperTux 0.3.1 tileset splitter 
; Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;

; ---------------------------------------------------------------------------
; Reads a SuperTux tileset on stdin, outputs a SuperTux tileset with only
; those tiles with a tile-id in the tileset's first group.
; This means that if the tileset starts with a tilegroup "snow", you
; will get a tileset with only snow tiles in the output
; ---------------------------------------------------------------------------

; return first sublist in haystack that starts with needle or #f if none is found
(define (find-sublist haystack needle)
  (cond
    (
     (not (pair? haystack))
     #f
     )
    (
     (and (pair? (car haystack)) (eq? (caar haystack) needle))
     (cdar haystack)
     )
    (
     else
     (find-sublist (cdr haystack) needle)
     )
    )
  )

; input: (tile ... (id 1) ...) (1 2 3 4 5)
; output: #t if "id" in "valid-ids", #f otherwise
(define (output-tile? children valid-ids)
  (let
    ((id (car (find-sublist children 'id))))
    (if (not id) 
      #f 
      )
    (if (member id valid-ids) 
      #t
      #f  
      )
    )
  )

; input: (1 7 8) (1 2 3 4 5) 
; output: #t if any of "needles" in "haystack", #f otherwise
(define (any-member needles haystack)
  (if (null? needles) 
    #f
    (or
      (member (car needles) haystack)
      (any-member (cdr needles) haystack)
      )
    )
  )

; input: (tiles ... (ids 1 7 8) ...) (1 2 3 4 5)
; output: #t if any of "ids" in "valid-ids", #f otherwise
(define (output-tiles? children valid-ids)
  (let
    ((ids (find-sublist children 'ids)))
    (if (not ids) 
      #f 
      )
    (if (any-member ids valid-ids) 
      #t            
      #f  
      )
    )
  )

; input: ((tilegroup ...) (tilegroup ...) (tile ...) (tiles ...))
; output: ((tilegroup ...) (tile ...) (tiles ...))
(define (output-and-next children valid-ids)
  (if (null? valid-ids)
    ; tilegroup not yet found
    (if (not (string=? (symbol->string (caar children)) "tilegroup")) 
      ; no tilegroup: just continue
      (if (not (null? (cdr children))) (output-and-next (cdr children) valid-ids) '())
      (begin
        ; tilegroup: get valid-ids
        (set! valid-ids (find-sublist (car children) 'tiles))
        ; output tilegroup and continue
        (cons
          (car children)
          (if (not (null? (cdr children))) (output-and-next (cdr children) valid-ids) '())
          )
        )
      )
    (begin
      ; tilegroup already found
      (cond 
        ((string=? (symbol->string (caar children)) "tile")
         (if (output-tile? (car children) valid-ids)
           (cons
             (car children)
             (if (not (null? (cdr children))) (output-and-next (cdr children) valid-ids) '())
             )
           (if (not (null? (cdr children))) (output-and-next (cdr children) valid-ids) '())
           )
         )
        ((string=? (symbol->string (caar children)) "tiles")
         (if (output-tiles? (car children) valid-ids)
           (cons
             (car children)
             (if (not (null? (cdr children))) (output-and-next (cdr children) valid-ids) '())
             )
           (if (not (null? (cdr children))) (output-and-next (cdr children) valid-ids) '())
           )
         )
        (else
          (if (not (null? (cdr children))) (output-and-next (cdr children) valid-ids) '())
          )
        )
      )
    )  
  )

; input: (supertux-tiles ... (tilegroup ...) (tilegroup ...) (tile ...) (tiles ...))
; output: (supertux-tiles (tilegroup ...) (tile ...) (tiles ...))
(define (clip-tileset supertux-tiles)
  (let ()
    (if (not (string=? (symbol->string (car supertux-tiles)) "supertux-tiles")) (error "not a supertux-tileset:" type))
    (output-and-next (cdr supertux-tiles) '())
    )
  )

; run conversion on stdin, output to stdout
(write (clip-tileset (read)))
(newline)
(quit)

