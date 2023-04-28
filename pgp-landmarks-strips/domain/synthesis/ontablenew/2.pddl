(define (problem BLOCKS-2)
 (:domain BLOCKS)
 (:objects  b1 b2 b3 b4 - object)
 (:init 
    (handempty)
    (clear b4)
    (on b4 b1)
    (on b1 b3)
    (ontable b3)
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
