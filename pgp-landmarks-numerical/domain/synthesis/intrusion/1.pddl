(define (problem INTRUSION-1)
 (:domain INTRUSION)
 (:objects  h1 - host)
 (:init 
     (dummy)
)
 (:goal
  (and (data-stolen-from h1))))
