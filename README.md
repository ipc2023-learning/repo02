# Novelty Based Progressive Generalized Planner

## Build learning imag for STRIPS domain 

    apptainer build learn.img Apptainer.ngpstrips.learn
  
## Build learning imag for Numerical domain 
     apptainer build learn.img Apptainer.ngpnum.learn

## Run learning img for STRIPS domain (for numerical domain, just replace the domain and instance files)
     
     ./learn.img dk.1 domains/gripper/domain.pddl domains/gripper/p-2-0.pddl domains/gripper/p-3-0.pddl domains/gripper/p-4-0.pddl domains/gripper/p-5-0.pddl ...


## Build and run planning image for STRIPS domain (the same for numerical domain)

    apptainer build plan.img Apptainer.ngpstrips.plan
    ./plan.img dk.1 domains/gripper/domain.pddl domains/gripper/p-10-0.pddl plan.10
