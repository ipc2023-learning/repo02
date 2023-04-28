(define (problem INTRUSION-3)
 (:domain INTRUSION)
 (:objects  h1 h2 h3 - host)
 (:init 
     (dummy)
)
 (:goal
  (and (data-stolen-from h1) (data-stolen-from h2) (data-stolen-from h3))))
