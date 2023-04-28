(define (problem LOCK-1)
 (:domain LOCK)
 (:objects  c1 c2 c3 c4 c5)
 (:init 
     (at-key c5) (at-lock c1)
      (at-agent c1) (adjacent c1 c2) (adjacent c2 c1) (adjacent c2 c3) (adjacent c3 c2) (adjacent c3 c4) (adjacent c4 c3) (adjacent c4 c5) (adjacent c5 c4)
)
 (:goal
  (and (unlocked))))
