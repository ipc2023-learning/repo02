(define (problem INTRUSION-10)
 (:domain INTRUSION)
 (:objects  h1 h2 h3 h4 h5 h6 h7 h8 h9 h10 - host)
 (:init 
     (dummy)
)
 (:goal
  (and (data-stolen-from h1) (data-stolen-from h2) (data-stolen-from h3) (data-stolen-from h4) (data-stolen-from h5) (data-stolen-from h6) (data-stolen-from h7) (data-stolen-from h8) (data-stolen-from h9) (data-stolen-from h10))))
