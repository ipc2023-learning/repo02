(define (problem SPANNER-1)
 (:domain SPANNER)
 (:objects 
     bob - man
     spanner1 spanner2 - spanner
     nut1 nut2 - nut
     location1 - location
)
 (:init 
    (atm bob location1)
    (ats spanner1 location1)
    (useable spanner1)
    (ats spanner2 location1)
    (useable spanner2)
    (loose nut1)
    (atn nut1 location1)
    (loose nut2)
    (atn nut2 location1)
)
 (:goal
  (and
   (tightened nut1)
   (tightened nut2)
)))
