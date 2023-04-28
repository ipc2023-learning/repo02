#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "program_state.h"
#include "instruction.h"
#include "generalized_planning_problem.h"
#include "landmark_graph.h"

class Program{
public:
	explicit Program( int program_lines = 1, Instruction* end_instruction = nullptr ){
        assert( program_lines >= 1 );
		_instructions = vector<Instruction*>( program_lines, nullptr );
        setInstruction( program_lines - 1, end_instruction );
        _num_of_steps = 0;
        _lm_count = -1;
        _failed_instance_id = -1;
        _my_map={};
        _location_of_null={};
        _num_of_test=0;
        _num_of_goal=0;
        _num_of_max_line=-1;
        _goal_achieved = true;
        _num_of_cmp_pointer = 0;
        _num_of_cmp_location = 0;
        _end= {};
        _action_name={};
        _num_of_unrelated_actions=0;
	}
	
	explicit Program( Program* p ){
        map<string,int> my_map=p->getMap();
        _my_map=my_map;
		vector< Instruction* > vi = p->getInstructions();
		_instructions.resize( vi.size() );
		for( int i = 0; i < int( _instructions.size() ); i++ )
			_instructions[ i ] = vi[ i ];
        vector<int> end_vector=p->getEnd();
        _end=end_vector;
        vector<string> my_action_name=p->getActionName();
        _action_name=my_action_name;
        _num_of_steps = 0;
        _lm_count = -1;
        _failed_instance_id = -1;
        /*if( _landmark_graphs.size() > 0u ) {
            _num_visited_landmarks = 0;
            setLandmarkGraphs(p->getLandmarks());
            assert( p->getLandmarks().size() > 0u );
            assert( getLandmarks().size() > 0u );
        }*/
        _num_of_steps = 0;
        _location_of_null={};
        _num_of_test=0;
        _num_of_goal=0;
        _num_of_max_line=-1;
        _goal_achieved = true;
        _num_of_cmp_pointer = 0;
        _num_of_cmp_location = 0;
        _end={};
        _num_of_unrelated_actions=0;
	}
	
	~Program()= default;
	
	Program* copy(){
		return new Program( this );
	}
	
	vector< Instruction* > getInstructions() const{
		return _instructions;
	}
    map<string,int> getMap() const{
        return _my_map;
    }
//	void setInstruction( int pl, Instruction* ins ){
//	    assert( pl < int( _instructions.size() ) );
//		_instructions[ pl ] = ins;
//	}

    //chao edit
    void setInstruction( int pl, Instruction* ins ){
        assert( pl < int( _instructions.size() ) );
        _instructions[ pl ] = ins;
        PlanningAction *act = dynamic_cast< PlanningAction* >( ins );
        if(act) {
            string ActName = act->getActName();
            if (ActName != "") {
                if (_my_map.find(ActName) == _my_map.end()) {
                    _my_map.insert(std::pair<string, int>(ActName, 1));
                } else {
                    _my_map[ActName]++;
                }
            } else {
                string name = act->getName();
                if (_my_map.find(name) == _my_map.end()) {
                    _my_map.insert(std::pair<string, int>(name, 1));
                } else {
                    _my_map[name]++;
                }
            }
        }
        else{
            if (ins and ins->getName()=="end"){
                _end.push_back(pl);
            }
        }
    }
	Instruction* getInstruction( int pl ){
	    assert( pl < int( _instructions.size() ) );
		return _instructions[ pl ];
	}

	int getNumInstructions() const{
		return int( _instructions.size() );
	}
	
	bool haltingCondition( ProgramState* ps, int &error ){
		int line = ps->getLine();
		// EMPTY line is a halting condition (no transition defined)
		if( _instructions[ line ] == nullptr ){
			return true;
		}

		// Either True or False evaluation of End instructions is a halting condition
		End* ins_end = dynamic_cast< End* > ( _instructions[ line ] );
		if( ins_end ) return true;

        // Conditional effects, actions do not have to be necessarily applicable
		// (OLD) If it's a planning action, it must be applicable
		/*PlanningAction* ins_act = dynamic_cast< PlanningAction* > ( _instructions[ line ] );
		if( ins_act && !ins_act->isApplicable( ps ) ){
            #ifdef VALIDATOR
		    cout << ps->toString() << endl;
		    cout << ins_act->toString() << endl;
            #endif
			error = -2; // ERROR 2: inapplicable instruction
			return true;
		}*/
		
		return false;
	}
	
//	bool checkGoal( ProgramState *ps, Instance *ins, int &error ){
//		int line = ps->getLine();
//		if( _instructions[ line ] == nullptr ) return false;
//		End *end = dynamic_cast<End*>( _instructions[ line ] );
//		if( end and not end->isGoalState( ins, ps ) ){
//            #ifdef DEBUG
//			//cout << ps->toString() << endl;
//            #endif
//			error = -1; // ERROR 1: Incorrect program
//			return false;
//		}
//		return false;
//	}
//chao edit
    bool checkGoal( ProgramState *ps, Instance *ins, int &error ){
        int line = ps->getLine();
        if( _instructions[ line ] == NULL ) return false;
        End *end = dynamic_cast<End*>( _instructions[ line ] );
        if( end and !end->isGoalState( ins, ps ) ){
#ifdef DEBUG
            cout << ps->toString() << endl;
#endif
            error = -1; // ERROR 1: Incorrect program
            return false;
        }
        if (end and end->isGoalState( ins, ps )) {
            return true;
        }
        return false;
    }


    vector< ProgramState* > run_validation( GeneralizedPlanningProblem *gpp,  vector<vector<string>> &plans, bool infinite_detection = true,
                                            bool progressive_eval = false ){
        _num_of_steps = 0;

        int num_instances = gpp->getNumInstances();
        StateDescriptor *sd = (gpp->getDomain())->getStateDescriptor();
        auto pred_names = sd->getPredicateTypes();

        // One program state per instance
        vector< ProgramState* > pss( num_instances ) ;
        for( int i = 0; i < num_instances; i++ )
            pss[ i ] = new ProgramState();


#ifdef VALIDATOR
        time_t start_instance, end_instance;
        time( &start_instance );
#endif
        int errors = 0;

        bool use_landmarks = (!_landmark_graph.empty() );
        if( use_landmarks ){
            _lm_count = 0;
            _unachieved_landmarks.resize(num_instances, 0 );
        }

        for( int id = 0; id < num_instances; id++ ){
            plans.emplace_back(vector<string>());
            if(progressive_eval and not gpp->isInstanceActive(id) ) continue;
            // Initialize local initial state
            Instance *ins = gpp->getInstance( id );
            ProgramState *ps = pss[ id ];
            State *s = ( ins->getInitialState() )->copy();
            int line = ps->getLine();
            ps->setState( s );

            // Process the landmarks of the initial state
            set< int > accepted;
            set< Landmark* > landmarks;
            RelaxedGraph *rg = nullptr;
            vector< int > goal_landmarks_ids;
            if( use_landmarks ) {
                landmarks = _landmark_graph[ id ]->getLandmarks();
                rg = _landmark_graph[ id ]->getRelaxedGraph();
                for( auto lm_id : _landmark_graph[ id ]->getInitialLandmarksIDs() ){
                    accepted.insert( lm_id );
                }
                goal_landmarks_ids = _landmark_graph[ id ]->getGoalLandmarksIDs();
            }

            // For detecting infinite loops (it can be enhanced with a hash or bigint identifier)
            set< vector<int> > visited;
            int error = 0;

            while( !haltingCondition( ps, error ) ){
                if( infinite_detection ){
                    // Checking infinite loop (only for backward loops)
                    Goto *g = dynamic_cast<Goto*>( _instructions[ line ] );
                    if( g and g->getDestinationLine() < line ){
                        // Infinite loop detected
#ifdef MAX_STEPS
                        if( MAX_STEPS*num_instances < _num_of_steps ){
                            error = -3; break; // ERROR 3: Infinite program
                        }
#else
                        vector<int> state_id = ps->asVector();
                        if( visited.find( state_id ) != visited.end() ){
                            error = -3; // ERROR 3: Infinite program
                            break;
                        }
                        visited.insert( state_id );
#endif
                    }
                }

                // Retrieving program line
                line = ps->getLine();
                // cout << ps->toString(sd) << endl;
                // Applying current instruction

                auto *act = dynamic_cast< PlanningAction* >( _instructions[ line ] );
                bool applied= false;
                if (act) {
                    applied=act->isApplicable(ps);
                }

                int res = _instructions[ line ]->apply( ps );
                _num_of_steps++;

                // Mathematical planning actions update zero and carry flags
//                auto *act = dynamic_cast< PlanningAction* >( _instructions[ line ] );
                if( act && act->getType() == "math" ){
                    s->setPointer(sd, "zf", ( res == 0 ?1:0) ); // zero-flag
                    if( CARRY_FLAG )
                        s->setPointer(sd, "cf", ( res > 0 ? 1:0) ); // carry-flag
                }
#ifdef VALIDATOR
                else if(act and applied){
					auto act_name = act->getName();
					auto it_left_par = act_name.find_first_of('(');
					auto act_symb = act_name.substr(0,it_left_par);
					auto act_args = act_name.substr(it_left_par+1, act_name.size() - 2 - it_left_par); // skip last ')'

					string pddl_action = "(" + act_symb;
					size_t it_comma;
					while( (it_comma = act_args.find_first_of(',')) != string::npos){
						auto ptr_name = act_args.substr(0,it_comma);
						auto ptr_var_type_id = sd->getPointerVarType(ptr_name);
						auto ptr_type = sd->getVarName(ptr_var_type_id);
						auto ptr_val = s->getPointer(sd, ptr_name);
						pddl_action += " " + ins->getObjectName(ptr_type, ptr_val);
						act_args = act_args.substr(it_comma+1);
					}
					if(act_args.size() > 0u){
						auto ptr_name = act_args;
						auto ptr_var_type_id = sd->getPointerVarType(ptr_name);
						auto ptr_type = sd->getVarName(ptr_var_type_id);
						auto ptr_val = s->getPointer(sd, ptr_name);
						pddl_action += " " + ins->getObjectName(ptr_type, ptr_val);
					}
					pddl_action += ")";
					plans[id].emplace_back(pddl_action);
				}
#endif

                if( use_landmarks ){
                    processReachedLandmarks( sd, rg, s, id, landmarks, accepted, false );
                    processReachedLandmarks( sd, rg, s, id, landmarks, accepted, true );
                }
            }

            if( use_landmarks ){
                set< int > not_accepted;
                // Add all non-accepted landmarks
                for( auto l : landmarks ){
                    auto lm_id = l->getID();
                    if( accepted.find( lm_id ) == accepted.end() )
                        not_accepted.insert( lm_id );
                }

                // Add required preconditions
                set< int > required_again;
                for( auto lm_id : accepted ){
                    auto landmark = _landmark_graph[ id ]->getLandmark( lm_id );
                    bool landmark_holds;
                    if( landmark->isPointerLandmark() ) landmark_holds = landmark->holds(sd,s);
                    else landmark_holds = landmark->holds(rg, s);
                    if( not landmark_holds ){
                        // Required again if it is greedy necessary for a non-accepted landmark
                        bool is_req_again = false;
                        auto gn_landmarks = _landmark_graph[ id ]->getGreedyNecessaryLandmarkOrdering( lm_id );
                        for( auto gn_id : gn_landmarks ){
                            if( not_accepted.find( gn_id ) != not_accepted.end() ){
                                is_req_again = true; break;
                            }
                        }
                        if( is_req_again ){
                            required_again.insert( lm_id );
                        }
                    }
                }

                // Add required again goals
                for( auto lm_id : goal_landmarks_ids ){
                    // If the goal has not been accepted yet continue
                    if( accepted.find( lm_id ) == accepted.end() ) continue;
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    if( not l->holds( rg, s ) )
                        required_again.insert( lm_id );
                }
                // Alternative required again
                /*set< int > potential_required_again;
                for( auto lm_id : accepted ){
                    auto landmark = _landmark_graph[ id ]->getLandmark( lm_id );
                    bool landmark_holds;
                    if( landmark->isPointerLandmark() ) landmark_holds = landmark->holds(sd,s);
                    else landmark_holds = landmark->holds(rg, s);
                    if( not landmark_holds ){
                        potential_required_again.insert( lm_id );
                    }
                }

                // Add required again goals
                for( auto lm_id : goal_landmarks_ids ){
                    // If the goal has not been accepted yet continue
                    if( accepted.find( lm_id ) == accepted.end() ) continue;
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    if( not l->holds( rg, s ) )
                        potential_required_again.insert( lm_id );
                }

                set< int > required_again;
                int num_of_required_again = 0;
                do {
                    num_of_required_again = required_again.size();
                    for (auto lm_id: potential_required_again) {
                        // Required again if it is greedy necessary for a non-accepted landmark
                        bool is_req_again = false;
                        auto gn_landmarks = _landmark_graph[id]->getGreedyNecessaryLandmarkOrdering(lm_id);
                        for (auto gn_id: gn_landmarks) {
                            if (not_accepted.find(gn_id) != not_accepted.end()) {
                                is_req_again = true;
                                break;
                            } else if (required_again.find(gn_id) != required_again.end()) {
                                is_req_again = true;
                                break;
                            }
                        }
                        if (is_req_again) {
                            required_again.insert(lm_id);
                        }
                    }
                }while( required_again.size() > num_of_required_again );*/

                //[ToDo] Check deadend here, required again must hold in the final relaxed state
#ifdef PRINT_LANDMARKS_INFO
                cout << "[INFO] LANDMARK GRAPH " << id << endl;
                cout << "[INFO] ACCEPTED LANDMARKS:" << endl;
                for( auto lm_id : accepted ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
                cout << "[INFO] NOT ACCEPTED LANDMARKS:" << endl;
                for( auto lm_id : not_accepted ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
                cout << "[INFO] REQUIRED AGAIN:" << endl;
                for( auto lm_id : required_again ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
#endif
                _lm_count += (int)not_accepted.size() + (int)required_again.size();

                // NEW FOR NORMALIZATION
                _unachieved_landmarks[id] = (int)not_accepted.size() + (int)required_again.size();
            }

            if( error == 0 )
                checkGoal( ps, ins, error );
            if( DEADEND_DETECTION and use_landmarks and error == 0) {
                checkDeadEnd(sd, ps, _landmark_graph[id], error );
            }
            // [ENHANCEMENT] Continue execution in validation for coverage metric
#ifdef VALIDATOR
            time(&end_instance);
			if( error < 0 ){
			    errors++;
				cout << "INSTANCE #" << (id+1) << " ERROR " << error << "... :( [" << difftime(end_instance,start_instance) << "]" << endl;
				for( int i = 0; i < int( pss.size() ); i++ ){
				    cout << pss[i]->toString( sd ) << endl;
				}
			}
			else
				cout << "INSTANCE #" << (id+1) << " SOLVED! [" << difftime(end_instance,start_instance) <<"]" << endl;
			start_instance = end_instance;
#else
            if( error < 0 ){
                _failed_instance_id = id;
                errors++;
                break;
                /*for( int i = 0; i < int( pss.size() ); i++ ){
                    //cout << pss[i]->toString( sd ) << endl;
                    if( pss[i] ) delete pss[i];
                }
                return vector<ProgramState*>();*/
            }
#endif
        }

        if( errors ){
            for(auto & ps : pss){
                delete ps;
            }
            return {};
        }

        return pss;
    }


    static bool checkDeadEnd(StateDescriptor *sd, ProgramState *ps, LandmarkGraph *lm, int &error ){
	    unique_ptr<RelaxedState> rs = make_unique<RelaxedState>( sd );
	    auto reg = ps->getState()->getTypedRegisters();
	    for( int pred_idx = 0; pred_idx < (int)reg.size(); pred_idx++ ) {
	        for( const auto& m : reg[ pred_idx ] )
                rs->addFact(pred_idx, m.first, m.second );
        }
	    bool res = lm->isDeadEnd( rs );
	    if( res ) error = -4;

	    return res;
	}
	
	vector< ProgramState* > run( GeneralizedPlanningProblem *gpp, bool infinite_detection = true,
                                 bool progressive_eval = false ){
        _num_of_steps = 0;

		int num_instances = gpp->getNumInstances();
		StateDescriptor *sd = (gpp->getDomain())->getStateDescriptor();
		auto pred_names = sd->getPredicateTypes();

		// One program state per instance
		vector< ProgramState* > pss( num_instances ) ;
		for( int i = 0; i < num_instances; i++ )
			pss[ i ] = new ProgramState();


        #ifdef VALIDATOR
        time_t start_instance, end_instance;
        time( &start_instance );
        #endif
        int errors = 0;

        bool use_landmarks = (!_landmark_graph.empty() );
        if( use_landmarks ){
            _lm_count = 0;
            _unachieved_landmarks.resize(num_instances, 0 );
        }
        bool false_instance=true;
        for( int id = 0; id < num_instances; id++ ){
            if(progressive_eval and not gpp->isInstanceActive(id) ) continue;
			// Initialize local initial state
			Instance *ins = gpp->getInstance( id );			
			ProgramState *ps = pss[ id ];
			State *s = ( ins->getInitialState() )->copy();
			int line = ps->getLine();
			ps->setState( s );

			// Process the landmarks of the initial state
			set< int > accepted;
			set< Landmark* > landmarks;
			RelaxedGraph *rg = nullptr;
			vector< int > goal_landmarks_ids;
			if( use_landmarks ) {
			    landmarks = _landmark_graph[ id ]->getLandmarks();
			    rg = _landmark_graph[ id ]->getRelaxedGraph();
			    for( auto lm_id : _landmark_graph[ id ]->getInitialLandmarksIDs() ){
                    accepted.insert( lm_id );
			    }
			    goal_landmarks_ids = _landmark_graph[ id ]->getGoalLandmarksIDs();
            }

			// For detecting infinite loops (it can be enhanced with a hash or bigint identifier)
			set< vector<int> > visited;
			int error = 0;

			while( !haltingCondition( ps, error ) ){
				if( infinite_detection ){
					// Checking infinite loop (only for backward loops)
					Goto *g = dynamic_cast<Goto*>( _instructions[ line ] );
					if( g and g->getDestinationLine() < line ){
						// Infinite loop detected
#ifdef MAX_STEPS
                        if( MAX_STEPS*num_instances < _num_of_steps ){
                            error = -3; break; // ERROR 3: Infinite program
                        }
#else
                        vector<int> state_id = ps->asVector();
                        if( visited.find( state_id ) != visited.end() ){
                            error = -3; // ERROR 3: Infinite program
                            break;
                        }
                        visited.insert( state_id );
#endif
					}
				}
				
				// Retrieving program line
				line = ps->getLine();
                // cout << ps->toString(sd) << endl;
				// Applying current instruction
//                PlanningAction *act1 = dynamic_cast< PlanningAction* >( _instructions[ line ] );
//                if (act1 and act1->getPlanningAction() and !act1->isApplicable(ps)){
//                    error = -3; // ERROR 3: Infinite program
//                    break;
//                }
				int res = _instructions[ line ]->apply( ps );
				_num_of_steps++;

				// Mathematical planning actions update zero and carry flags
				auto *act = dynamic_cast< PlanningAction* >( _instructions[ line ] );
				if( act && act->getType() == "math" ){
					s->setPointer(sd, "zf", ( res == 0 ?1:0) ); // zero-flag
                    if( CARRY_FLAG )
					    s->setPointer(sd, "cf", ( res > 0 ? 1:0) ); // carry-flag
				}

				if( use_landmarks ){
                    processReachedLandmarks( sd, rg, s, id, landmarks, accepted, false );
                    processReachedLandmarks( sd, rg, s, id, landmarks, accepted, true );
				}
			}

			if( use_landmarks ){
			    set< int > not_accepted;
			    // Add all non-accepted landmarks
			    for( auto l : landmarks ){
			        auto lm_id = l->getID();
			        if( accepted.find( lm_id ) == accepted.end() )
			            not_accepted.insert( lm_id );
			    }

			    // Add required preconditions
			    set< int > required_again;
			    for( auto lm_id : accepted ){
			        auto landmark = _landmark_graph[ id ]->getLandmark( lm_id );
                    bool landmark_holds;
                    if( landmark->isPointerLandmark() ) landmark_holds = landmark->holds(sd,s);
                    else landmark_holds = landmark->holds(rg, s);
			        if( not landmark_holds ){
			            // Required again if it is greedy necessary for a non-accepted landmark
			            bool is_req_again = false;
			            auto gn_landmarks = _landmark_graph[ id ]->getGreedyNecessaryLandmarkOrdering( lm_id );
			            for( auto gn_id : gn_landmarks ){
			                if( not_accepted.find( gn_id ) != not_accepted.end() ){
			                    is_req_again = true; break;
			                }
			            }
			            if( is_req_again ){
			                required_again.insert( lm_id );
			            }
			        }
			    }

			    // Add required again goals
			    for( auto lm_id : goal_landmarks_ids ){
			        // If the goal has not been accepted yet continue
			        if( accepted.find( lm_id ) == accepted.end() ) continue;
			        auto l = _landmark_graph[ id ]->getLandmark( lm_id );
			        if( not l->holds( rg, s ) )
			            required_again.insert( lm_id );
			    }
                // Alternative required again
                /*set< int > potential_required_again;
                for( auto lm_id : accepted ){
                    auto landmark = _landmark_graph[ id ]->getLandmark( lm_id );
                    bool landmark_holds;
                    if( landmark->isPointerLandmark() ) landmark_holds = landmark->holds(sd,s);
                    else landmark_holds = landmark->holds(rg, s);
                    if( not landmark_holds ){
                        potential_required_again.insert( lm_id );
                    }
                }

                // Add required again goals
                for( auto lm_id : goal_landmarks_ids ){
                    // If the goal has not been accepted yet continue
                    if( accepted.find( lm_id ) == accepted.end() ) continue;
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    if( not l->holds( rg, s ) )
                        potential_required_again.insert( lm_id );
                }

                set< int > required_again;
                int num_of_required_again = 0;
                do {
                    num_of_required_again = required_again.size();
                    for (auto lm_id: potential_required_again) {
                        // Required again if it is greedy necessary for a non-accepted landmark
                        bool is_req_again = false;
                        auto gn_landmarks = _landmark_graph[id]->getGreedyNecessaryLandmarkOrdering(lm_id);
                        for (auto gn_id: gn_landmarks) {
                            if (not_accepted.find(gn_id) != not_accepted.end()) {
                                is_req_again = true;
                                break;
                            } else if (required_again.find(gn_id) != required_again.end()) {
                                is_req_again = true;
                                break;
                            }
                        }
                        if (is_req_again) {
                            required_again.insert(lm_id);
                        }
                    }
                }while( required_again.size() > num_of_required_again );*/

                //[ToDo] Check deadend here, required again must hold in the final relaxed state
#ifdef PRINT_LANDMARKS_INFO
                cout << "[INFO] LANDMARK GRAPH " << id << endl;
                cout << "[INFO] ACCEPTED LANDMARKS:" << endl;
                for( auto lm_id : accepted ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
                cout << "[INFO] NOT ACCEPTED LANDMARKS:" << endl;
                for( auto lm_id : not_accepted ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
                cout << "[INFO] REQUIRED AGAIN:" << endl;
                for( auto lm_id : required_again ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
#endif
			    _lm_count += (int)not_accepted.size() + (int)required_again.size();

                // NEW FOR NORMALIZATION
                _unachieved_landmarks[id] = (int)not_accepted.size() + (int)required_again.size();
			}

            if( error == 0 )
			    if (not checkGoal( ps, ins, error ) and false_instance) {
                    _failed_instance_id = id;
                    false_instance= false;
                }
			if( DEADEND_DETECTION and use_landmarks and error == 0) {
                checkDeadEnd(sd, ps, _landmark_graph[id], error );
			}
			// [ENHANCEMENT] Continue execution in validation for coverage metric
			#ifdef VALIDATOR
            time(&end_instance);
			if( error < 0 ){
			    errors++;
				cout << "INSTANCE #" << (id+1) << " ERROR " << error << "... :( [" << difftime(end_instance,start_instance) << "]" << endl;
				for( int i = 0; i < int( pss.size() ); i++ ){
				    cout << pss[i]->toString( sd ) << endl;
				}
			}
			else
				cout << "INSTANCE #" << (id+1) << " SOLVED! [" << difftime(end_instance,start_instance) <<"]" << endl;
			start_instance = end_instance;
            #else

            if( error < 0 ){
                _failed_instance_id = id;
                errors++;
                break;
                /*for( int i = 0; i < int( pss.size() ); i++ ){
                    //cout << pss[i]->toString( sd ) << endl;
                    if( pss[i] ) delete pss[i];
                }
                return vector<ProgramState*>();*/
            }
			#endif
		}

        if( errors ){
            for(auto & ps : pss){
                delete ps;
            }
            return {};
        }


		
		return pss;
	}

    vector< ProgramState* > runNovelty( GeneralizedPlanningProblem *gpp, bool infinite_detection = true,
                                 bool progressive_eval = false ){
        _num_of_steps = 0;
        _num_of_test  = 0;
        _num_of_goal  = 0;
        _num_of_max_line =-1;
        _goal_achieved = true;
        _num_of_cmp_pointer = 0;
        _num_of_cmp_location = 0;
        _end={};

        int num_instances = gpp->getNumInstances();
        StateDescriptor *sd = (gpp->getDomain())->getStateDescriptor();
        auto pred_names = sd->getPredicateTypes();

        // One program state per instance
        vector< ProgramState* > pss( num_instances ) ;
        for( int i = 0; i < num_instances; i++ )
            pss[ i ] = new ProgramState();


#ifdef VALIDATOR
        time_t start_instance, end_instance;
        time( &start_instance );
#endif
        int errors = 0;
        int width= gpp->getDomain()->getWidth();
        for( auto & actions : _my_map ){
            if (actions.second>width){
                for(auto & ps : pss){
                    delete ps;
                }
                return {};
            }
        }

            set<string> relaxactions = gpp->getRelaxes();
            for (int i=0; i<_instructions.size();i++){
                Goto *gofrom = dynamic_cast<Goto*>( _instructions[ i ] );
                if (_instructions[i]==NULL){
                    _location_of_null.push_back(i);
                }
                if (CHECK_RELAXEDACTION) {
                    if (_instructions[i] != NULL) {
                        PlanningAction *act = dynamic_cast< PlanningAction * >( _instructions[i] );
                        if (act) {
                            if (act->getPlanningAction()){
                                if (relaxactions.find(act->getActName()) == relaxactions.end()){
                                    _num_of_unrelated_actions++;
                                }
                            }
                        }

                    }
                }

                if (gofrom){
                    Goto *godes = dynamic_cast<Goto*>( _instructions[gofrom->getDestinationLine()]);
                    if (godes){
                        for(auto & ps : pss){
                            delete ps;
                        }
                        return {};
                    }


                }
            }

        bool use_landmarks = (!_landmark_graph.empty() );
        if( use_landmarks ){
            _lm_count = 0;
            _unachieved_landmarks.resize(num_instances, 0 );
        }

        for( int id = 0; id < num_instances; id++ ){
            if(progressive_eval and not gpp->isInstanceActive(id) ) continue;
            // Initialize local initial state
            Instance *ins = gpp->getInstance( id );
            ProgramState *ps = pss[ id ];
            State *s = ( ins->getInitialState() )->copy();
            int line = ps->getLine();
            ps->setState( s );

            // Process the landmarks of the initial state
            set< int > accepted;
            set< Landmark* > landmarks;
            RelaxedGraph *rg = nullptr;
            vector< int > goal_landmarks_ids;
            if( use_landmarks ) {
                landmarks = _landmark_graph[ id ]->getLandmarks();
                rg = _landmark_graph[ id ]->getRelaxedGraph();
                for( auto lm_id : _landmark_graph[ id ]->getInitialLandmarksIDs() ){
                    accepted.insert( lm_id );
                }
                goal_landmarks_ids = _landmark_graph[ id ]->getGoalLandmarksIDs();
            }

            // For detecting infinite loops (it can be enhanced with a hash or bigint identifier)
            set< vector<int> > visited;
            int error = 0;
            vector<vector<int>> test_visited;
            vector<vector<int>> cmp_pointers;
            vector<vector<int>> cmp_locations;
            while( !haltingCondition( ps, error ) ){
                if( infinite_detection ){
                    // Checking infinite loop (only for backward loops)
                    Goto *g = dynamic_cast<Goto*>( _instructions[ line ] );
                    if( g and g->getDestinationLine() < line ){
                        // Infinite loop detected
#ifdef MAX_STEPS
                        if( MAX_STEPS*num_instances < _num_of_steps ){
                            error = -3; break; // ERROR 3: Infinite program
                        }
#else
                        vector<int> state_id = ps->asVector();
                        if( visited.find( state_id ) != visited.end() ){
                            error = -3; // ERROR 3: Infinite program
                            break;
                        }
                        visited.insert( state_id );
#endif
                    }
                }

                // Retrieving program line
                line = ps->getLine();
                // cout << ps->toString(sd) << endl;
                // Applying current instruction
                // take effects for FIBO domains, so delete it
//                if( NUMBER) {
//                    PlanningAction *act1 = dynamic_cast< PlanningAction * >( _instructions[line] );
//                    if (act1 and act1->getPlanningAction() and !act1->isApplicable(ps)) {
//                        error = -3; // ERROR 3: Infinite program
//                        break;
//                    }
//                }
                int res = _instructions[ line ]->apply( ps );
                _num_of_steps++;


                // Mathematical planning actions update zero and carry flags
                auto *act = dynamic_cast< PlanningAction* >( _instructions[ line ] );
                auto pointers=sd->getPointerNames();
                if(SWAP and NUMBER) {
                    if (act and act->getActName() == "swap") {
                        vector<int> pointervalue;
                        for (auto &pointer: pointers) {
                            if (pointer=="zf" or pointer=="cf")
                                continue;
                                auto value = s->getPointer(sd, pointer);
                                if (std::find(pointervalue.begin(), pointervalue.end(), value) != pointervalue.end()) {
                                    error = -3;
                                    break;
                                }
                                pointervalue.push_back(value);
                        }
                    }
                }

                if (act) {
                    string name = act->getName();
                    if (CHECK_CMP){
                    if (name.find("cmp") != std::string::npos) {
                        string flag = act->getActioVartype();
                        if (flag == "predicate") {
                            vector<int> iterms;
                            for (auto &pointer: pointers) {
                                if (pointer=="zf" or pointer=="cf")
                                    continue;
                                iterms.push_back(s->getPointer(sd, pointer));
                            }
                            if (std::find(cmp_locations.begin(), cmp_locations.end(), iterms) == cmp_locations.end()) {
                                _num_of_cmp_location++;
                                cmp_locations.push_back(iterms);

                            }
                        }
                        if (flag == "pointer") {
                            vector<int> iterms;
                            for (auto &pointer: pointers) {
                                if (pointer=="zf" or pointer=="cf")
                                    continue;
                                iterms.push_back(s->getPointer(sd, pointer));
                            }
                            if (std::find(cmp_pointers.begin(), cmp_pointers.end(), iterms) == cmp_pointers.end()) {
                                _num_of_cmp_pointer++;
                                cmp_pointers.push_back(iterms);

                            }
                        }
                    }
                    }
                    if (CHECK_TEST) {
                        if (name.find("test-min") != std::string::npos) {
                            vector<int> iterms;
                            for (auto &pointer: pointers) {
                                if (pointer=="zf" or pointer=="cf")
                                    continue;
                                iterms.push_back(s->getPointer(sd, pointer));
                            }
                            if (std::find(test_visited.begin(), test_visited.end(), iterms) == test_visited.end()) {
                                _num_of_test++;
                                test_visited.push_back(iterms);

                            }

                        }
                    }
                }

                if( act && act->getType() == "math" ){
                    s->setPointer(sd, "zf", ( res == 0 ?1:0) ); // zero-flag
                    if( CARRY_FLAG )
                        s->setPointer(sd, "cf", ( res > 0 ? 1:0) ); // carry-flag
                }

                if( use_landmarks ){
                    processReachedLandmarks( sd, rg, s, id, landmarks, accepted, false );
                    processReachedLandmarks( sd, rg, s, id, landmarks, accepted, true );
                }
            }

            if( use_landmarks ){
                set< int > not_accepted;
                // Add all non-accepted landmarks
                for( auto l : landmarks ){
                    auto lm_id = l->getID();
                    if( accepted.find( lm_id ) == accepted.end() )
                        not_accepted.insert( lm_id );
                }

                // Add required preconditions
                set< int > required_again;
                for( auto lm_id : accepted ){
                    auto landmark = _landmark_graph[ id ]->getLandmark( lm_id );
                    bool landmark_holds;
                    if( landmark->isPointerLandmark() ) landmark_holds = landmark->holds(sd,s);
                    else landmark_holds = landmark->holds(rg, s);
                    if( not landmark_holds ){
                        // Required again if it is greedy necessary for a non-accepted landmark
                        bool is_req_again = false;
                        auto gn_landmarks = _landmark_graph[ id ]->getGreedyNecessaryLandmarkOrdering( lm_id );
                        for( auto gn_id : gn_landmarks ){
                            if( not_accepted.find( gn_id ) != not_accepted.end() ){
                                is_req_again = true; break;
                            }
                        }
                        if( is_req_again ){
                            required_again.insert( lm_id );
                        }
                    }
                }

                // Add required again goals
                for( auto lm_id : goal_landmarks_ids ){
                    // If the goal has not been accepted yet continue
                    if( accepted.find( lm_id ) == accepted.end() ) continue;
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    if( not l->holds( rg, s ) )
                        required_again.insert( lm_id );
                }
                // Alternative required again
                /*set< int > potential_required_again;
                for( auto lm_id : accepted ){
                    auto landmark = _landmark_graph[ id ]->getLandmark( lm_id );
                    bool landmark_holds;
                    if( landmark->isPointerLandmark() ) landmark_holds = landmark->holds(sd,s);
                    else landmark_holds = landmark->holds(rg, s);
                    if( not landmark_holds ){
                        potential_required_again.insert( lm_id );
                    }
                }

                // Add required again goals
                for( auto lm_id : goal_landmarks_ids ){
                    // If the goal has not been accepted yet continue
                    if( accepted.find( lm_id ) == accepted.end() ) continue;
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    if( not l->holds( rg, s ) )
                        potential_required_again.insert( lm_id );
                }

                set< int > required_again;
                int num_of_required_again = 0;
                do {
                    num_of_required_again = required_again.size();
                    for (auto lm_id: potential_required_again) {
                        // Required again if it is greedy necessary for a non-accepted landmark
                        bool is_req_again = false;
                        auto gn_landmarks = _landmark_graph[id]->getGreedyNecessaryLandmarkOrdering(lm_id);
                        for (auto gn_id: gn_landmarks) {
                            if (not_accepted.find(gn_id) != not_accepted.end()) {
                                is_req_again = true;
                                break;
                            } else if (required_again.find(gn_id) != required_again.end()) {
                                is_req_again = true;
                                break;
                            }
                        }
                        if (is_req_again) {
                            required_again.insert(lm_id);
                        }
                    }
                }while( required_again.size() > num_of_required_again );*/

                //[ToDo] Check deadend here, required again must hold in the final relaxed state
#ifdef PRINT_LANDMARKS_INFO
                cout << "[INFO] LANDMARK GRAPH " << id << endl;
                cout << "[INFO] ACCEPTED LANDMARKS:" << endl;
                for( auto lm_id : accepted ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
                cout << "[INFO] NOT ACCEPTED LANDMARKS:" << endl;
                for( auto lm_id : not_accepted ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
                cout << "[INFO] REQUIRED AGAIN:" << endl;
                for( auto lm_id : required_again ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
#endif
                _lm_count += (int)not_accepted.size() + (int)required_again.size();

                // NEW FOR NORMALIZATION
                _unachieved_landmarks[id] = (int)not_accepted.size() + (int)required_again.size();
            }

            if( error == 0 )
            {
                if (_goal_achieved)
                {
                    _goal_achieved=checkGoal( ps, ins, error );
                }
                else{
                    checkGoal( ps, ins, error );
                }
            }
//                checkGoal( ps, ins, error );

            if( DEADEND_DETECTION and use_landmarks and error == 0) {
                checkDeadEnd(sd, ps, _landmark_graph[id], error );
            }
            if( DEADEND_DETECTION and use_landmarks and error == 0) {
                checkDeadEnd(sd, ps, _landmark_graph[id], error );
            }
            // [ENHANCEMENT] Continue execution in validation for coverage metric
#ifdef VALIDATOR
            time(&end_instance);
			if( error < 0 ){
			    errors++;
				cout << "INSTANCE #" << (id+1) << " ERROR " << error << "... :( [" << difftime(end_instance,start_instance) << "]" << endl;
				for( int i = 0; i < int( pss.size() ); i++ ){
				    cout << pss[i]->toString( sd ) << endl;
				}
			}
			else
				cout << "INSTANCE #" << (id+1) << " SOLVED! [" << difftime(end_instance,start_instance) <<"]" << endl;
			start_instance = end_instance;
#else


            if( error < 0 ){
                _failed_instance_id = id;
                errors++;
                break;
                /*for( int i = 0; i < int( pss.size() ); i++ ){
                    //cout << pss[i]->toString( sd ) << endl;
                    if( pss[i] ) delete pss[i];
                }
                return vector<ProgramState*>();*/
            }

            vector< Condition* > goal = ins->getGoalCondition();
            long long ins_res = 0;
            for( int j = 0; j < int( goal.size() ); j++ ){
                long long lhs = goal[j]->getLHS(sd, ps->getState() );
                long long rhs = goal[j]->getRHS(sd, ps->getState() );
//                if (lhs>rhs){
//                    for( int i = 0; i < int( pss.size() ); i++ ){
//                        if( pss[i] ) delete pss[i];
//                    }
//                    return {};
//                }

                ins_res += ((lhs-rhs)*(lhs-rhs));
            }
            _num_of_goal += ins_res;

            int max_line = ps->getLine();
            if(_instructions[max_line] == NULL ){
                _num_of_max_line = max( _num_of_max_line, max_line );
            }

#endif
        }

        if( errors ){
            for(auto & ps : pss){
                delete ps;
            }
            return {};
        }
        return pss;
    }

    void processReachedLandmarks( StateDescriptor *sd, RelaxedGraph *rg, State *s, int landmark_graph_id,
                                  set<Landmark*> &landmarks, set<int> &accepted, bool pointer_landmarks = false ){
        vector<int> reached;
        for( auto l :  landmarks ){
            if( pointer_landmarks and not l->isPointerLandmark() ) continue;
            if( not pointer_landmarks and l->isPointerLandmark() ) continue;
            int lm_id = l->getID();
            // If the landmark was already accepted continue;
            if( accepted.find( lm_id ) != accepted.end() ) continue;
            bool landmark_holds;
            if( l->isPointerLandmark() ) landmark_holds = l->holds(sd,s);
            else landmark_holds = l->holds(rg, s);
            // New landmark reached
            if( landmark_holds ){
                bool is_reached = true;
                // check if gn landmarks hold
                auto gn_landmarks = _landmark_graph[ landmark_graph_id ]->getInverseGreedyNecessaryLandmarkOrdering( lm_id );
                for( auto gn_l_id : gn_landmarks ){
                    // check if a gn condition has not been accepted yet
                    if( accepted.find( gn_l_id ) == accepted.end() ){
                        is_reached = false; break;
                    }
                }
                auto nat_landmarks = _landmark_graph[ landmark_graph_id ]->getInverseNaturalLandmarkOrdering( lm_id );
                for( auto nat_l_id : nat_landmarks ){
                    // check if a nat condition has not been accepted yet
                    if( accepted.find( nat_l_id ) == accepted.end() ){
                        is_reached = false; break;
                    }
                }
                // if all gn and nat landmarks have been accepted, include the new landmark as reached
                if( is_reached )
                    reached.push_back( lm_id );
            }
        }
        // Union of reached and accepted
        std::copy( reached.begin(), reached.end(), inserter( accepted, accepted.end() ) );
    }

	int getNumOfSteps() const{
	    return _num_of_steps;
	}
    int getNumOfgoal() const{
        return _num_of_goal;
    }
    int getNumOfmaxline() const{
        return _num_of_max_line;
    }
    bool getGoalAchieved() const{
        return _goal_achieved;
    }
    vector<int> getEnd() const{
        return _end;
    }
    vector<string> getActionName() const{
        return _action_name;
    }
    int getNumOftest() const{
        return _num_of_test;
    }
    int getNumOfcmpPointer() const{
        return _num_of_cmp_pointer;
    }
    int getNumOfcmpLocation() const{
        return _num_of_cmp_location;
    }
    int getNumOfrelatedActions() const{
        return _num_of_unrelated_actions;
    }
    vector<int> getLocofNull() const{
        return _location_of_null;
    }

	void setLandmarkGraphs(const vector< LandmarkGraph* > &landmark_graph ){
        _landmark_graph = landmark_graph;
	}

    int getLMCount() const{
	    return _lm_count;
    }

    int getFailedInstanceID() const{
        return _failed_instance_id;
    }

    int getFirstEmptyLine() const {
        int line = 0;
        for( auto ins : _instructions ){
            if( ins == nullptr )
                return line;
            line++;
        }
        return -1;
    }

    int getTotalLandmarks(int id) {
        assert(id < (int)_landmark_graph.size() );
        return _landmark_graph[id]->getNumLandmarks();
    }

    int getUnachieveLandmarks(int id) {
        assert(id < (int)_unachieved_landmarks.size());
        return _unachieved_landmarks[id];
    }
	
	string toString( bool full_info = true ) const{
		string ret;
		if( full_info ){
			ret = "[PROGRAM]:\n";
			for( int i = 0; i < int( _instructions.size() ); i++ ){
				ret += "LINE " + to_string( i ) + ": ";
				if( _instructions[ i ] != nullptr )
					ret += _instructions[ i ]->toString(true);
				else ret += "[INSTRUCTION] EMPTY\n";
			}
		}
		else{
			for( int i = 0; i < int( _instructions.size() ); i++ ){
				ret += to_string( i ) + ". ";
				if( _instructions[ i ] != nullptr )
					ret += _instructions[ i ]->toString( full_info );
				else ret += "empty\n";
			}
		}
		return ret;
	}
	
private:
    vector< LandmarkGraph* > _landmark_graph;
	int _lm_count;
	vector< Instruction* > _instructions;
	int _num_of_steps;
    int _failed_instance_id;
    vector<int> _unachieved_landmarks;
    int _num_of_test;
    int _num_of_cmp_pointer;
    int _num_of_cmp_location;
    vector<int> _location_of_null;
    int _num_of_goal;
    int _num_of_max_line;
    bool _goal_achieved;
    int _num_of_unrelated_actions;
    std::map<string, int> _my_map;
    vector<int> _end;
    vector<string> _action_name;
};

#endif
