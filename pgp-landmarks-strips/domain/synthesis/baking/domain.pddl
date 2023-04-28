(define (domain BAKING)
 (:requirements :typing)
 (:types egg flour cake pan oven soap)
 (:predicates (panhasegg ?p - pan) (panhasflour ?p - pan) (panisclean ?p - pan) (paninoven ?p - pan) (inoven ?p - pan ?o - oven) (ovenisfull ?o - oven) (ismixed ?p - pan) (baked ?n - cake) (flourconsumed ?f - flour) (eggconsumed ?e - egg) (soapconsumed ?s - soap))

 (:action putegginpan
    :parameters (?e - egg ?p - pan)
    :precondition (and (not(eggconsumed ?e)) (not (panhasegg ?p)) (not (ismixed ?p)) (panisclean ?p) (not (paninoven ?p)))
    :effect (and (eggconsumed ?e) (panhasegg ?p)))

 (:action putflourinpan
    :parameters (?f - flour ?p - pan)
    :precondition (and (not(flourconsumed ?f)) (not (panhasflour ?p)) (not (ismixed ?p)) (panisclean ?p) (not (paninoven ?p)))
    :effect (and (flourconsumed ?f) (panhasflour ?p)))

 (:action mix
    :parameters (?e - egg ?f - flour ?p - pan)
    :precondition (and (panhasflour ?p) (panhasegg ?p) (not (paninoven ?p)))
    :effect (and (ismixed ?p) (not (panhasflour ?p)) (not (panhasegg ?p))))

 (:action putpaninoven
    :parameters (?p - pan ?o - oven)
    :precondition (and (not (ovenisfull ?o)) (not (paninoven ?p)))
    :effect (and (ovenisfull ?o) (inoven ?p ?o) (paninoven ?p)))

 (:action removepanfromoven
    :parameters (?p - pan ?o - oven)
    :precondition (and (inoven ?p ?o))
    :effect (and (not (ovenisfull ?o)) (not (inoven ?p ?o)) (not (paninoven ?p))))

 (:action bakecake
    :parameters (?o - oven ?p - pan ?c - cake)
    :precondition (and (ismixed ?p) (inoven ?p ?o))
    :effect (and (not (ismixed ?p)) (not (panisclean ?p)) (baked ?c)))

  (:action cleanpan
    :parameters (?p - pan ?s - soap)
    :precondition (and (not (soapconsumed ?s))
                  (not (paninoven ?p)))
    :effect (and (panisclean ?p) (soapconsumed ?s)))
)