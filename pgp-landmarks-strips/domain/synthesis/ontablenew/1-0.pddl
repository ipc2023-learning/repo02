(define (problem BLOCKS-1)
 (:domain BLOCKS)
 (:objects  b1 b2 - object)
 (:init 
    (handempty)
    (clear b1)
    (on b1 b2)
    (ontable b2)
)
 (:goal
  (and
   (ontable b1)
   (ontable b2)
)))
