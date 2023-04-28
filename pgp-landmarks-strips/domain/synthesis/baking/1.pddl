(define (problem BAKING-1)
 (:domain BAKING)
 (:objects  e1 - egg f1 - flour c1 - cake s1 - soap p1 - pan o1 - oven)
 (:init 
     (panisclean p1)
)
 (:goal
  (and (panisclean p1) (baked c1))))
