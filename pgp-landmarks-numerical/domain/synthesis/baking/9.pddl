(define (problem BAKING-9)
 (:domain BAKING)
 (:objects  e1 e2 e3 e4 e5 e6 e7 e8 e9 - egg f1 f2 f3 f4 f5 f6 f7 f8 f9 - flour c1 c2 c3 c4 c5 c6 c7 c8 c9 - cake s2 s3 s4 s5 s6 s7 s8 s9 s1 - soap p1 - pan o1 - oven)
 (:init 
     (panisclean p1)
)
 (:goal
  (and (panisclean p1) (baked c1) (baked c2) (baked c3) (baked c4) (baked c5) (baked c6) (baked c7) (baked c8) (baked c9))))
