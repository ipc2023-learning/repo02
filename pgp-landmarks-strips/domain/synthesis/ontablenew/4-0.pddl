(define (problem BLOCKS-4)
 (:domain BLOCKS)
 (:objects  b1 b2 b3 b4 b5 - object)
 (:init 
    (handempty)
    (clear b4)
    (on b4 b2)
    (on b2 b3)
    (on b3 b1)
    (on b1 b5)
    (ontable b5)
)
 (:goal
  (and
   (ontable b1)
   (ontable b2)
   (ontable b3)
   (ontable b4)
   (ontable b5)
)))