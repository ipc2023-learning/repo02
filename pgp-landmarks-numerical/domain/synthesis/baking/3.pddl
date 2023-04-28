(define (problem BAKING-3)
 (:domain BAKING)
 (:objects  e1 e2 e3 - egg f1 f2 f3 - flour c1 c2 c3 - cake s2 s3 s1 - soap p1 - pan o1 - oven)
 (:init 
     (panisclean p1)
)
 (:goal
  (and (panisclean p1) (baked c1) (baked c2) (baked c3))))
