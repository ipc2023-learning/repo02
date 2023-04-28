(define (problem BLOCKS-3)
 (:domain BLOCKS)
 (:objects  b1 b2 b3 b4 - object)
 (:init 
    (handempty)
    (clear b1)
    (on b1 b4)
    (on b4 b2)
    (on b2 b3)
    (ontable b3)
)
 (:goal
  (and
   (ontable b1)
   (ontable b2)
   (ontable b3)
   (ontable b4)
)))
