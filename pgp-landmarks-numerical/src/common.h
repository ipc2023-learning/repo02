#ifndef __COMMON_H__
#define __COMMON_H__

//#include<bits/stdc++.h>
#include "stdc++.h"

#define PROGRAM_FREQUENCY 10000
#define INFO false
#define INF 1000000000
//#define PRINT_LANDMARKS_INFO hline
#define DEADEND_DETECTION false
#define DEFERRED_EVALUATION false
#define EVALUATION_FREQUENCY 3
#define REEVALUATE_OPEN_QUEUE true
#define POINTER_LANDMARKS true
#define COPY_LANDMARKS false // true does not work properly
#define TESTS_ALLOWED false
#define NATURAL_ORDERING true

#define IFS_ALLOWED true
#define CARRY_FLAG true
#define CMP_INSTRUCTION_ALLOWED true
#define CHECK_ACTIONCONDITION false
#define CHECK_RELAXEDACTION false
#define CHECK_TEST false
#define CHECK_CMP true
#define CHECK_RELAXEDACTION_RODER false
#define NUMBER true
#define PRINT false
//for novelty
#define ADDCONSTRAIN false
#define SWAP false



#ifndef VALIDATOR
#define MAX_VAL 100
#else
#define MAX_VAL 1000000000
//#define MAX_STEPS 200
#endif

using namespace std;

#endif
