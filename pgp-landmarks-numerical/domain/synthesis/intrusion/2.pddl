(define (problem INTRUSION-2)
 (:domain INTRUSION)
 (:objects  h1 h2 - host)
 (:init 
     (dummy)
)
 (:goal
  (and (data-stolen-from h1) (data-stolen-from h2))))
