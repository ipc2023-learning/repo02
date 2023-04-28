(define (problem BAKING-2)
 (:domain BAKING)
 (:objects  e1 e2 - egg f1 f2 - flour c1 c2 - cake s2 s1 - soap p1 - pan o1 - oven)
 (:init 
     (panisclean p1)
)
 (:goal
  (and (panisclean p1) (baked c1) (baked c2))))
