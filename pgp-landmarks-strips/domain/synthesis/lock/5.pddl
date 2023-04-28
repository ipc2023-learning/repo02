(define (problem LOCK-5)
 (:domain LOCK)
 (:objects  c1 c2 c3 c4 c5 c6 c7 c8 c9)
 (:init 
     (at-key c9) (at-lock c1)
      (at-agent c3) (adjacent c1 c2) (adjacent c2 c1) (adjacent c2 c3) (adjacent c3 c2) (adjacent c3 c4) (adjacent c4 c3) (adjacent c4 c5) (adjacent c5 c4) (adjacent c5 c6) (adjacent c6 c5) (adjacent c6 c7) (adjacent c7 c6) (adjacent c7 c8) (adjacent c8 c7) (adjacent c8 c9) (adjacent c9 c8)
)
 (:goal
  (and (unlocked))))
