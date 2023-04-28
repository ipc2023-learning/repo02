(define (domain SPANNER)
 (:requirements :typing :strips)
 (:types location man nut spanner - object)

 (:predicates (atm ?m - man ?l - location) (ats ?s - spanner ?l - location) (atn ?n - nut ?l - location) (carrying ?m - man ?s - spanner) (useable ?s - spanner) (link ?l1 - location ?l2 - location) (tightened ?n - nut) (loose ?n - nut))

 (:action walk
    :parameters (?start - location ?end - location ?m - man)
    :precondition (and (atm ?m ?start) (link ?start ?end))
    :effect (and (not (atm ?m ?start)) (atm ?m ?end)))

 (:action pickup_spanner
    :parameters (?l - location ?s - spanner ?m - man)
    :precondition (and (atm ?m ?l) (ats ?s ?l))
    :effect (and (not (ats ?s ?l)) (carrying ?m ?s)))

 (:action tighten_nut
    :parameters (?l - location ?s - spanner ?m - man ?n - nut)
    :precondition (and (atm ?m ?l) (atn ?n ?l) (carrying ?m ?s) (useable ?s) (loose ?n))
    :effect (and (not (loose ?n))(not (useable ?s)) (tightened ?n)))
)