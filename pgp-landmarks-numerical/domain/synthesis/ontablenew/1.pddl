(define (problem BLOCKS-1)
 (:domain BLOCKS)
 (:objects  b1 b2 b3 - object)
 (:init 
    (handempty)
    (clear b2)
    (ontable b2)
    (clear b3)
    (on b3 b1)
    (ontable b1)
)
 (:goal
  (and
   (ontable b1)
   (ontable b2)
   (ontable b3)
)))
