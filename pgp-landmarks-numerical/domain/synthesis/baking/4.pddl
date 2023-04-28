(define (problem BAKING-4)
 (:domain BAKING)
 (:objects  e1 e2 e3 e4 - egg f1 f2 f3 f4 - flour c1 c2 c3 c4 - cake s2 s3 s4 s1 - soap p1 - pan o1 - oven)
 (:init 
     (panisclean p1)
)
 (:goal
  (and (panisclean p1) (baked c1) (baked c2) (baked c3) (baked c4))))
