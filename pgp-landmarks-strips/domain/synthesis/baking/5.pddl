(define (problem BAKING-5)
 (:domain BAKING)
 (:objects  e1 e2 e3 e4 e5 - egg f1 f2 f3 f4 f5 - flour c1 c2 c3 c4 c5 - cake s2 s3 s4 s5 s1 - soap p1 - pan o1 - oven)
 (:init 
     (panisclean p1)
)
 (:goal
  (and (panisclean p1) (baked c1) (baked c2) (baked c3) (baked c4) (baked c5))))
