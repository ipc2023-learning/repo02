(define (domain LOCK)
  (:requirements :strips)
  (:predicates (at-agent ?x) (at-key ?x) (at-lock ?x) (has-agent-key) (adjacent ?x ?y) (unlocked))

 (:action move
   :parameters  (?x ?y)
   :precondition (and (adjacent ?x ?y) (at-agent ?x)(not (at-agent ?y)))
   :effect (and (at-agent ?y) (not (at-agent ?x))))

 (:action pickup
   :parameters  (?x)
   :precondition (and (at-agent ?x) (at-key ?x))
   :effect (and (has-agent-key) (not (at-key ?x))))

 (:action drop
   :parameters  (?x)
   :precondition (and (at-agent ?x) (has-agent-key))
   :effect (and  (at-key ?x)))

  (:action open
   :parameters  (?x)
   :precondition (and (at-agent ?x) (at-lock ?x) (has-agent-key))
   :effect (and  (unlocked)))
)