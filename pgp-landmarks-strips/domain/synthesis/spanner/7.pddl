(define (problem SPANNER-7)
 (:domain SPANNER)
 (:objects 
     bob - man
     spanner1 spanner2 spanner3 spanner4 spanner5 spanner6 spanner7 spanner8 spanner9 spanner10 spanner11 spanner12 spanner13 spanner14 - spanner
     nut1 nut2 nut3 nut4 nut5 nut6 nut7 nut8 nut9 nut10 nut11 nut12 nut13 nut14 - nut
     location1 location2 location3 location4 location5 location6 location7 - location
)
 (:init 
    (atm bob location1)
    (ats spanner1 location7)
    (useable spanner1)
    (ats spanner2 location1)
    (useable spanner2)
    (ats spanner3 location4)
    (useable spanner3)
    (ats spanner4 location3)
    (useable spanner4)
    (ats spanner5 location7)
    (useable spanner5)
    (ats spanner6 location4)
    (useable spanner6)
    (ats spanner7 location2)
    (useable spanner7)
    (ats spanner8 location2)
    (useable spanner8)
    (ats spanner9 location1)
    (useable spanner9)
    (ats spanner10 location5)
    (useable spanner10)
    (ats spanner11 location4)
    (useable spanner11)
    (ats spanner12 location4)
    (useable spanner12)
    (ats spanner13 location5)
    (useable spanner13)
    (ats spanner14 location7)
    (useable spanner14)
    (loose nut1)
    (atn nut1 location7)
    (loose nut2)
    (atn nut2 location7)
    (loose nut3)
    (atn nut3 location7)
    (loose nut4)
    (atn nut4 location7)
    (loose nut5)
    (atn nut5 location7)
    (loose nut6)
    (atn nut6 location7)
    (loose nut7)
    (atn nut7 location7)
    (loose nut8)
    (atn nut8 location7)
    (loose nut9)
    (atn nut9 location7)
    (loose nut10)
    (atn nut10 location7)
    (loose nut11)
    (atn nut11 location7)
    (loose nut12)
    (atn nut12 location7)
    (loose nut13)
    (atn nut13 location7)
    (loose nut14)
    (atn nut14 location7)
    (link location1 location2)
    (link location2 location3)
    (link location3 location4)
    (link location4 location5)
    (link location5 location6)
    (link location6 location7)
)
 (:goal
  (and
   (tightened nut1)
   (tightened nut2)
   (tightened nut3)
   (tightened nut4)
   (tightened nut5)
   (tightened nut6)
   (tightened nut7)
   (tightened nut8)
   (tightened nut9)
   (tightened nut10)
   (tightened nut11)
   (tightened nut12)
   (tightened nut13)
   (tightened nut14)
)))
