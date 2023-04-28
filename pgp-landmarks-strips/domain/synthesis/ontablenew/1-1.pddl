(define (problem BLOCKS-1)
 (:domain BLOCKS)
 (:objects  b1 b2 - object)
 (:init 
    (handempty)
    (clear b2)
    (on b2 b1)
    (ontable b1)
)
 (:goal
  (and
   (ontable b1)
   (ontable b2)
)))
