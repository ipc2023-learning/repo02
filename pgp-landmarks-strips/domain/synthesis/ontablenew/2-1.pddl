(define (problem BLOCKS-2)
 (:domain BLOCKS)
 (:objects  b1 b2 b3 - object)
 (:init 
    (handempty)
    (clear b3)
    (on b3 b1)
    (on b1 b2)
    (ontable b2)
)
 (:goal
  (and
   (ontable b1)
   (ontable b2)
   (ontable b3)
)))
