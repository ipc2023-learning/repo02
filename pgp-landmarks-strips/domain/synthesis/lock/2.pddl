(define (problem LOCK-2)
 (:domain LOCK)
 (:objects  c1 c2 c3 c4 c5 c6)
 (:init 
     (at-key c6) (at-lock c1)
      (at-agent c5) (adjacent c1 c2) (adjacent c2 c1) (adjacent c2 c3) (adjacent c3 c2) (adjacent c3 c4) (adjacent c4 c3) (adjacent c4 c5) (adjacent c5 c4) (adjacent c5 c6) (adjacent c6 c5)
)
 (:goal
  (and (unlocked))))
