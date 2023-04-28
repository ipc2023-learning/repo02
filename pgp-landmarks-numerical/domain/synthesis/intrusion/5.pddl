(define (problem INTRUSION-5)
 (:domain INTRUSION)
 (:objects  h1 h2 h3 h4 h5 - host)
 (:init 
     (dummy)
)
 (:goal
  (and (data-stolen-from h1) (data-stolen-from h2) (data-stolen-from h3) (data-stolen-from h4) (data-stolen-from h5))))
