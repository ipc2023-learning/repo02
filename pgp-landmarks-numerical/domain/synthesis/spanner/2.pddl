(define (problem SPANNER-2)
 (:domain SPANNER)
 (:objects 
     bob - man
     spanner1 spanner2 spanner3 spanner4 - spanner
     nut1 nut2 nut3 nut4 - nut
     location1 location2 - location
)
 (:init 
    (atm bob location1)
    (ats spanner1 location2)
    (useable spanner1)
    (ats spanner2 location1)
    (useable spanner2)
    (ats spanner3 location1)
    (useable spanner3)
    (ats spanner4 location1)
    (useable spanner4)
    (loose nut1)
    (atn nut1 location2)
    (loose nut2)
    (atn nut2 location2)
    (loose nut3)
    (atn nut3 location2)
    (loose nut4)
    (atn nut4 location2)
    (link location1 location2)
)
 (:goal
  (and
   (tightened nut1)
   (tightened nut2)
   (tightened nut3)
   (tightened nut4)
)))
