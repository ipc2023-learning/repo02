(define (problem BLOCKS-3)
 (:domain BLOCKS)
 (:objects  b1 b2 b3 b4 - object)
 (:init 
    (handempty)
    (clear b4)
    (on b4 b3)
    (on b3 b1)
    (ontable b1)
    (clear b2)
    (ontable b2)
)
 (:goal
  (and
   (ontable b1)
   (ontable b2)
   (ontable b3)
   (ontable b4)
)))
