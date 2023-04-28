#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "common.h"
#include "node.h"
#include "heuristic.h"
#include "generalized_planning_problem.h"
#include "generalized_domain.h"

struct CmpNodes{
    bool operator()(const Node* lhs, const Node* rhs) const{
        return (lhs->f() > rhs->f()) or ( lhs->f() == rhs->f() and lhs->getID() > rhs->getID() );
    }
};

class Engine{
public:
	Engine( int program_lines, GeneralizedDomain *gd, GeneralizedPlanningProblem *gpp ){
		_evaluated_states = 0;
		_evaluated_nodes = 0;
		_expanded_nodes = 0;
        _deadends = 0;
		_program_lines = program_lines;
		_gd = gd;
		_gpp = gpp;
        _relax_evaluated_nodes=0;
	}
	
	virtual ~Engine(){
		for( auto h : _heuristics ){
			delete h;
		}
		delete _gd;
		delete _gpp;
        for( auto lg : _landmark_graphs ){
            delete lg;
        }
	}
	
	virtual bool isEmpty() const = 0;
	
	// Add a node to searching data structure
	virtual void addNode( Node *node ) = 0;
	
	// Select the next Node and update the data structure
	virtual Node* selectNode() = 0;
	
	// Check if goal conditions hold in the current Node
	virtual bool isGoal( Node *node, bool incremental_eval ) = 0;
	
	// Expand the current Node, adding all non-visited childs
	virtual vector< Node* > expandNode( Node *node, bool incremental_eval ) = 0;
	
	// accumulated cost
	virtual int g( Node *node ) = 0;
	
	// heuristic functions
	virtual vector<int> h( Node *node ) = 0;
	
	// evaluation functions
	virtual vector<int> f( Node *node ) = 0;
	
	// Solve the problem starting from init Node
	virtual Node* solve(bool progressive) = 0;
	
	// Add a new heuristic to the engine
	void addHeuristic( Heuristic *new_h ){ 
		cout << "[INFO] Heuristic " << new_h->getName() << " added." <<  endl;
		_heuristics.push_back( new_h );
	}
	
	int getExpanded() const{
		return _expanded_nodes;
	}
	
	int getEvaluated() const{
		return _evaluated_nodes;
	}
    int getRelaxEvaluated() const{
        return _relax_evaluated_nodes;
    }
    int getEvaluatedStates() const{
        return _evaluated_states;
    }

    int getDeadends() const{
        return _deadends;
    }

	virtual void setLandmarks( const vector< LandmarkGraph*> & landmarks ){
        _landmark_graphs = landmarks;
	}

    virtual void checkActionsBackward ( ) = 0;
	
protected:
	int _evaluated_states;
	int _evaluated_nodes;
    int _relax_evaluated_nodes;
	int _expanded_nodes;
    int _deadends;
	int _program_lines;
	vector< Heuristic* > _heuristics;
	GeneralizedPlanningProblem *_gpp;
	GeneralizedDomain *_gd;
	vector< LandmarkGraph* > _landmark_graphs;
};

class BFS : public Engine{
public:
	BFS(int program_lines, GeneralizedDomain *gd, GeneralizedPlanningProblem *gpp) : Engine( program_lines, gd, gpp ){
	}
	
	~BFS() override{
		while( !isEmpty() ){
			Node *n = _open.top();
			_open.pop();
			delete n;
		}
	}
	
	bool isEmpty() const override{
		return _open.empty();
	}
	
	void addNode( Node *node ) override{
		_open.push( node );
	}
	
	Node* selectNode() override{
		return _open.top();
	}
	
	static void freeProgramStates( vector< ProgramState* > &vps ){
		for( auto ps : vps ){
			delete ps;
		}
	}
	
//	bool isGoal( Node *node, bool progressive_eval ) override{
//		//Program *p = node->generateProgram( _program_lines );
//		auto *p = node->getProgram();
//		auto vps = p->run(_gpp, true, progressive_eval );
//
//		// Some error ocurred during execution
//		if( vps.empty() )
//			return false;
//
//		for( int id = 0; id < int( vps.size() ); id++ ){
//		    if(progressive_eval and not _gpp->isInstanceActive(id ) ) continue;
//			int line = vps[ id ]->getLine();
//			Instruction *ins = p->getInstruction( line );
//			if( ins == nullptr ){
//				freeProgramStates( vps );
//				return false;
//			}
//
//			End *end = dynamic_cast<End*>( ins );
//			if( end ){
//				if( not end->isGoalState( _gpp->getInstance( id ), vps[ id ] ) ){
//					freeProgramStates( vps );
//					return false;
//				}
//			}
//			else{ // The current instruction is not an end instruction
//				freeProgramStates( vps );
//				return false;
//			}
//		}
//
//		freeProgramStates( vps );
//		return true;
//	}
    bool progressive_flag= false;

    void setflag( bool flag) {
        progressive_flag=flag;
    }

    // chao edit

    virtual bool isGoal( Node *node,bool progressive_eval ){
        //Program *p = node->generateProgram( _program_lines );
        if (not progressive_flag)
        {
            Program *p = node->getProgram();
            return p->getGoalAchieved();
        }
        else{
            		auto *p = node->getProgram();
		auto vps = p->run(_gpp, true, progressive_eval );

		// Some error ocurred during execution
		if( vps.empty() )
			return false;

		for( int id = 0; id < int( vps.size() ); id++ ){
		    if(progressive_eval and not _gpp->isInstanceActive(id ) ) continue;
			int line = vps[ id ]->getLine();
			Instruction *ins = p->getInstruction( line );
			if( ins == nullptr ){
				freeProgramStates( vps );
				return false;
			}

			End *end = dynamic_cast<End*>( ins );
			if( end ){
				if( not end->isGoalState( _gpp->getInstance( id ), vps[ id ] ) ){
					freeProgramStates( vps );
					return false;
				}
			}
			else{ // The current instruction is not an end instruction
				freeProgramStates( vps );
				return false;
			}
		}

		freeProgramStates( vps );
		return true;
        }
    }


    void all_permutations(const vector<vector<Instruction*>>& vec, vector<vector<Instruction*>>& res, vector<int>& indexes, uint32_t level=0)
    {
        std::vector<Instruction*> vci;
        if (level < vec.size())
        {
            for (size_t i = 0; i < vec[level].size(); i++)
            {
                indexes[level] = i;
                all_permutations(vec, res, indexes,level + 1);
            }
        }
        else
        {
            for (size_t i = 0; i < vec.size(); i++)
            {
                vci.push_back(vec[i][indexes[i]]);
            }
            res.push_back(vci);
        }
    }

    void all_permutations(const vector<vector<string>>& vec, vector<vector<string>>& res, vector<int>& indexes, uint32_t level=0)
    {
        std::vector<string> vci;
        if (level < vec.size())
        {
            for (size_t i = 0; i < vec[level].size(); i++)
            {
                indexes[level] = i;
                all_permutations(vec, res, indexes,level + 1);
            }
        }
        else
        {
            for (size_t i = 0; i < vec.size(); i++)
            {
                vci.push_back(vec[i][indexes[i]]);
            }
            res.push_back(vci);
        }
    }
    virtual void checkActionsBackward() {

        set<string> actions;
        for (auto &action: _gd->getPlanactions()) {
            actions.insert(action->getActName());
        }
        int size = actions.size();
        queue<string> quiz;
        actions = {};
        vector<string> initalState;
        vector<string> goalState;
        vector<vector<string>> nameStep;
        nameStep.resize(size);
        auto ins = _gpp->getInstance(0);
        for (auto &ini: ins->getinitWord()) {
            string initalName=ini.substr(0, ini.find("("));
            if (std::find(initalState.begin(), initalState.end(),initalName)==initalState.end())
            initalState.push_back(initalName);
        }
        for (auto &goal: ins->getgoalWord()) {
            string goalName=goal.substr(0, goal.find("("));
            if (find(goalState.begin(),goalState.end(),goalName)==goalState.end()){
                goalState.push_back(goalName);
            }
        }
        for (auto &goal: goalState) {
            quiz.push(goal);
            int step = 0;
            map<string,vector<string>> preaction;
            map<string,vector<string>> actionpre;
            vector<string> deletaction;
            vector<string> condcandidate;
            vector<string> truecond;
            vector<string> achived_effect;
            bool allused= false;
            while (!quiz.empty()) {
                string unGoal = quiz.front();
                if (find(achived_effect.begin(),achived_effect.end(),unGoal)!=achived_effect.end()){
                    quiz.pop();
                    continue;
                }
                bool goalAchieved = false;
                for (auto &action: _gd->getPlanactions()) {
                    vector<string> effects = action->getActionEffectWord();
                    if (find(effects.begin(), effects.end(), unGoal) != effects.end() and find(deletaction.begin(), deletaction.end(), action->getActName()) == deletaction.end()) {
                        string actionname = action->getActName();
                        if (find(nameStep[step].begin(),nameStep[step].end(),actionname)!=nameStep[step].end())
                        {
                            goalAchieved=true;
                            continue;
                        }
                        nameStep[step].push_back(actionname);
                        goalAchieved=true;
                        actions.insert(actionname);

                        if (actions.size() == size) {
                            allused=true;
                            break;
                        }
                        for (auto & effect : effects){
                            achived_effect.push_back(effect);
                        }
                        for (auto &pre: action->getActionPreWord()) {
                            if (find(initalState.begin(), initalState.end(), pre) == initalState.end()) {
                                if (find(truecond.begin(),truecond.end(),pre)==truecond.end() and find(achived_effect.begin(),achived_effect.end(),pre)==achived_effect.end() ) {
                                    condcandidate.push_back(pre);
                                    truecond.push_back(pre);
                                    if (preaction.find(pre)==preaction.end()){
                                        preaction[pre]={actionname};
                                    }
                                    else{
                                        preaction[pre].push_back(actionname);
                                    }
                                    if (actionpre.find(actionname)==actionpre.end()){
                                        actionpre[actionname]={pre};
                                    }
                                    else{
                                        actionpre[actionname].push_back(pre);
                                    }
                                }

                            }
                        }
                    }
                }
                if (allused){
                    break;
                }
                if (goalAchieved) {
                    quiz.pop();
                    step++;
                } else {
                    cout<<"useless action found!"<<endl;
                    vector<string> erroraction=preaction[unGoal];
                    queue<vector<string>>errorq;
                    errorq.push(erroraction);
                    while (!errorq.empty()) {
                        vector<string> actions=errorq.front();
                        for (auto eaction: actions) {
                            for (auto name: nameStep) {
                                vector<string>::iterator it;
                                for (it = name.begin(); it != name.end();) {
                                    if (*it == eaction) {
                                        it = name.erase(it);
                                        deletaction.push_back(eaction);
                                        if (actionpre.find(eaction)!=actionpre.end()){
                                            vector<string> precond=actionpre[eaction];
                                            for (auto pre : precond){
                                                if (preaction.find(pre)!=preaction.end()){
                                                    errorq.push(preaction[pre]);
                                                }
                                            }
                                        }
                                    } else {
                                        ++it;
                                    }
                                }

                            }
                        }
                        errorq.pop();
                    }

                    quiz.pop();

                }
                if (quiz.empty() and !condcandidate.empty()) {
                    for(auto &cond: condcandidate){
                        quiz.push(cond);
                    }
                    condcandidate={};
                }
            }
        }
        vector<vector<string>>::iterator it;
        for(it= nameStep.begin();it != nameStep.end();)
        {
            if(it->size()==0){
                it = nameStep.erase(it);
            }
            else{
                ++it;
            }
        }
        vector<vector<string>> res;
        vector<int> indexes;
        indexes.resize(nameStep.size());
        all_permutations(nameStep, res,indexes);
        _gd->setRelaxedActions(res);
        _gpp->setRelaxes(actions);

    }


    vector< Node* > expandNode( Node *node, bool incremental_eval ) override{
		int pc_max = -1; 
		//Program *p = node->generateProgram( _program_lines );
		auto *p = node->getProgram();
		vector< ProgramState* > vps;
//        if( IFS_ALLOWED ) {
//            // find the next line to program
//            vps = p->runNovelty(_gpp, true, incremental_eval);
//
//            if (vps.empty()) // error while executing the program
//                return {};
//
//            for (int id = 0; id < int(vps.size()); id++) {
//                if (incremental_eval and not _gpp->isInstanceActive(id)) continue;
//                int line = vps[id]->getLine();
//                if (p->getInstruction(line) == nullptr) {
//                    pc_max = max(pc_max, line);
//                }
//            }
//
//            freeProgramStates(vps);
//        }
//        else{
//            auto instructions = p->getInstructions();
//            for( int ins_idx = 0; ins_idx < (int)instructions.size(); ins_idx++ ){
//                if( instructions[ ins_idx ] == nullptr ){
//                    pc_max = ins_idx; break;
//                }
//            }
//        }
        //chao edit
        if (p->getInstructions()[0]==NULL){
            pc_max=0;
        }
        else{
            pc_max=p->getNumOfmaxline();
        }
		 //Failure case when next valid line not found
		if( pc_max == -1 or p->getInstruction( pc_max ) != nullptr ){
			return {};
		}

		vector< Instruction* > vi = _gd->getInstructions( pc_max );
		
		// [1] Symmetry Breaking - only Gotos after CMP or TEST
		bool only_gotos = false;
		bool gotos_enabled = false;
		if( pc_max > 0 ){
			auto *pa = dynamic_cast<PlanningAction*>( p->getInstruction( pc_max-1 ) );
			if( pa ){
			    auto act_name = pa->getName();
			    size_t pos = act_name.find( '(' );
			    if( pos != string::npos ){
                    auto name = act_name.substr(0, pos );
			        only_gotos = ( name == "cmp" or name == "test-max" or name == "test-min" );
			    }
			    gotos_enabled = (pa->getType() == "math");
			}
		}
		
		vector< Node* > childs;
		int maxi = max( 1, int(_heuristics.size() ) );
		for(auto & instruction : vi){
        if (NUMBER) {
            if (instruction->getName().find("test-min") != std::string::npos) {
                continue;
            }
        }
//            if (instruction->getName().find("cmp")!=std::string::npos){
//                continue;
//            }
			// [1] S.B.
			if( only_gotos ){
				Goto *g = dynamic_cast<Goto*>( instruction );
				if( !g ) continue;
			}
			if( !gotos_enabled ){
                Goto *g = dynamic_cast<Goto*>( instruction );
                if( g ) continue;
			}

			Program *p2 = p->copy();
			p2->setInstruction(pc_max, instruction );
            p2->setLandmarkGraphs(_landmark_graphs);

            childs.push_back( new Node(p2,vector<int>(maxi,0)));
		}
		
		return childs;
	}
	
	int g( Node *node ) override{
		return 0;
	}
	
	vector<int> h( Node *node ) override{
		auto *p = node->getProgram();
		auto val_h = vector<int>( _heuristics.size(), 0 );
		for( int i = 0; i < int( _heuristics.size() ); i++ ){
			val_h[ i ] = _heuristics[ i ]->compute( p, _gpp );
		}
		return val_h;
	}
	
	vector<int> f( Node *node ) override{
        _evaluated_states += _gpp->getNumActiveInstances(); // all active instances evaluate state
		return h( node );
	}

    int findElement(vector<string> v, string key){

        int len = v.size();
        for(int i=0; i<len; i++){

            if(v.at(i) == key){

                return i;
            }
        }
        return -1;
    }

	Node* solve(bool progressive) override{
        setflag(progressive);
	    auto *root_program = new Program( _program_lines, _gd->getInstruction( _program_lines - 1, 0) );

        root_program->setLandmarkGraphs(_landmark_graphs);

        if( progressive ){
            _gpp->activateInstance( 0 ); // activate the first instance
        }

#ifdef PRINT_LANDMARKS_INFO
        // LOCK DOMAIN
        vector< string > ins_names = {"inc(ptr-object1)", "move(ptr-object2,ptr-object1)","inc(ptr-object1)",
                                      "inc(ptr-object2)", "goto(1,!(( zf = 1 )))", "pickup(ptr-object1)",
                                      "dec(ptr-object1)", "move(ptr-object2,ptr-object1)", "dec(ptr-object2)",
                                      "goto(5,!(( zf = 1 )))", "open(ptr-object1)" };
        // BAKING DOMAIN
        /*vector<string> ins_names = { "putegginpan(ptr-egg1,ptr-pan1)","putflourinpan(ptr-flour1,ptr-pan1)",
                                     "mix(ptr-egg1,ptr-flour1,ptr-pan1)", "putpaninoven(ptr-pan1,ptr-oven1)",
                                     "bakecake(ptr-oven1,ptr-pan1,ptr-cake1)", "removepanfromoven(ptr-pan1,ptr-oven1)",
                                     "cleanpan(ptr-pan1,ptr-soap1)","inc(ptr-egg1)", "inc(ptr-flour1)", "inc(ptr-cake1)",
                                     "inc(ptr-soap1)", "goto(0,!(( zf = 1 )))", "end" };*/
        for( int pl = 0; pl < (int) ins_names.size(); pl++ ) {
            root_program->setInstruction(pl, _gd->getInstruction( pl, ins_names[ pl ] ));
            auto vps = root_program->run(_gpp, true, progressive);
            freeProgramStates(vps);
            cout << root_program->toString(false);
            cout << "[INFO] LM count = " << _heuristics[0]->compute(root_program, _gpp) << endl;
        }
        //return NULL;
#endif

        _evaluated_nodes = 0;
		Node *root = new Node( root_program, vector<int>( _heuristics.size(), INF ), _evaluated_nodes++ );
        _evaluated_states += _gpp->getNumActiveInstances();
// chao edit
//		if( isGoal( root, false ) )
//			return root;
			
		if( not DEFERRED_EVALUATION ) {
		    auto vps = root_program->runNovelty(_gpp,true,progressive);
            freeProgramStates(vps);
            root->setF( f( root ) );
		}

		addNode( root );

		vector< int > best_evaluations( _heuristics.size(), INF );

		while( !isEmpty() ){		
			_expanded_nodes++;
			
			Node *current = selectNode();
			// remove current node from open
			_open.pop();
			if( DEFERRED_EVALUATION ) {
                current->setF( f(current) );
            }

			auto current_evaluations = current->f();
//            Goto *goto1= dynamic_cast< Goto* >( current->getProgram()->getInstructions()[ 1 ] );
//            Goto *goto6= dynamic_cast< Goto* >( current->getProgram()->getInstructions()[ 6 ] );
//            if (current->getID()==17
//            if (current->getID()==107)
//            if (current->getID()==10202)
//            if (current->getProgram()->getInstruction(0)!=NULL and current->getProgram()->getInstruction(0)->getName()=="cmp(vector(i),vector(j))"
//            and goto1 and goto1->getDestinationLine()==4 and current->getProgram()->getInstruction(4)!=NULL and current->getProgram()->getInstruction(4)->getName()=="set(i,j)"
//            and current->getProgram()->getInstruction(5)!=NULL and current->getProgram()->getInstruction(5)->getName()=="inc(j)" and goto6 and goto6->getDestinationLine()==0
//            and  current->getProgram()->getInstruction(2)!=NULL and current->getProgram()->getInstruction(2)->getName()=="swap(i,j)")
//
//            {
//                cout <<"#######################################################"<< endl;
//                cout << current->toString()<< endl;
//            }

			vector< Node* > childs = expandNode( current, progressive );
//            if (current->getID()==25410){
//                cout<<"find"<<endl;
//            }
            if(PRINT) {
                if (current_evaluations < best_evaluations) {
                    best_evaluations = current_evaluations;
                    cout << "[ENGINE] NODE ID: " << current->getID() << "; EXPANDED: " << _expanded_nodes <<
                         "; EVALUATED: " << _evaluated_nodes << "; OPEN QUEUE SIZE: " << _open.size() << endl;
                    cout << current->toString() << endl;
                } else if (_expanded_nodes % PROGRAM_FREQUENCY == 0) {
                    cout << "[ENGINE] NODE ID: " << current->getID() << "; EXPANDED: " << _expanded_nodes <<
                         "; EVALUATED: " << _evaluated_nodes << "; OPEN QUEUE SIZE: " << _open.size() << endl;
                    cout << current->toString() << endl;
//                std::this_thread::sleep_for (std::chrono::seconds(1));
                }
            }

			delete current;
			
			for( int i = 0; i < int( childs.size() ); i++ ) {
                childs[i]->setID(_evaluated_nodes++);
//                if (childs[i]->getID()==5566){
//                    cout<<"find"<<endl;
//                }
//                if (childs[i]->getProgram()->getInstruction(0)!=NULL and childs[i]->getProgram()->getInstruction(0)->getName()=="cmp(vector(i),vector(j))"
//                and childs[i]->getProgram()->getInstruction(5)!=NULL and childs[i]->getProgram()->getInstruction(5)->getName()=="set(i,j)"
//                and childs[i]->getProgram()->getInstruction(6)!=NULL and childs[i]->getProgram()->getInstruction(6)->getName()=="inc(j)"
//
//                )
//                {
//                    cout << childs[i]->toString()<< endl;
//                }
                auto vps = childs[i]->getProgram()->runNovelty(_gpp, true, progressive);

                freeProgramStates(vps);
                if (CHECK_RELAXEDACTION_RODER) {
                if (!vps.empty()) {
                    Program *p2 = childs[i]->getProgram();

                    vector<int> nullLocation = p2->getLocofNull();
                    map<string, int> action_map = p2->getMap();
                    vector<vector<string>> relaxactions = _gd->getRelaxedActions();
                    for (auto &relaxaction: relaxactions) {
                        map<string, vector<Instruction *>> map_action;
                        for (auto &action: _gd->getPlanactions()) {
                            string ActName = action->getActName();
                            if (std::find(relaxaction.begin(), relaxaction.end(), ActName) == relaxaction.end())
                                continue;
                            bool check_flag=true;
                            if (action_map.find(ActName) == action_map.end()) {

                                int index = findElement(relaxaction, ActName);
                                for (auto &act: relaxactions) {
                                    if (action_map.find(act[index]) != action_map.end()) {
                                        check_flag = false;
                                        break;
                                    }
                                }
                                if (check_flag) {
                                if (map_action.find(ActName) == map_action.end()) {
                                    map_action.insert(std::pair<string, vector<Instruction *>>(ActName,
                                                                                               {new PlanningAction(
                                                                                                       action)}));
                                } else {
                                    map_action[ActName].push_back(new PlanningAction(action));
                                }
                            }
                            }
                        }

                        if (map_action.size() != 0 and nullLocation.size() == map_action.size()) {
                            vector<vector<Instruction *>> action_vectors;
                            action_vectors.resize(map_action.size());
                            int number = 0;
                            int maxi = max(1, int(_heuristics.size()));
                            for (auto &action: map_action) {
                                for (int j = 0; j < action.second.size(); j++) {
                                    { action_vectors[number].push_back(action.second[j]); }
                                }
                                number++;
                            }
                            vector<vector<Instruction *>> res;
                            vector<int> indexes;
                            indexes.resize(action_vectors.size());
                            all_permutations(action_vectors, res, indexes);

                            vector<vector<int>> ins_location;
                            sort(nullLocation.begin(), nullLocation.end());
                            do {
                                ins_location.push_back(nullLocation);
                            } while (next_permutation(nullLocation.begin(), nullLocation.end()));

                            for (auto &action: res) {

                                for (int i = 0; i < ins_location.size(); i++) {
                                    Program *p3 = p2->copy();
                                    for (int j = 0; j < ins_location[i].size(); j++) {
                                        p3->setInstruction(ins_location[i][j], action[j]);
                                    }
//                            cout << "B"+p3->toString( false ) << endl;
                                    auto vps2 = p3->runNovelty(_gpp, true, progressive);
                                    _relax_evaluated_nodes++;
                                    if (vps2.empty()) {
                                        _deadends++;
                                        delete p3;
                                        freeProgramStates(vps2);
                                        continue;
                                    } else {
                                        Node *newNode = new Node(p3, vector<int>(maxi, 0));
                                        freeProgramStates(vps2);
                                        if (not DEFERRED_EVALUATION) {
                                            newNode->setF(f(newNode));
                                        } else {
                                            newNode->setF(current_evaluations);
                                        }

                                        if (isGoal(newNode, progressive)) {
                                            if (progressive) {
                                                cout << "[INFO] Possible solution found at node id " << newNode->getID()
                                                     << " when solving instance ids:";
                                                for (int aux_id = 0; aux_id < _gpp->getNumInstances(); aux_id++)
                                                    if (_gpp->isInstanceActive(aux_id)) cout << " #" << aux_id;
                                                cout << endl << newNode->toString() << endl;
                                            }
                                            // if the search is not incremental, or if the program solves all instances finish
                                            if (not progressive or isGoal(newNode, false)) {
                                                newNode->setF(f(newNode));
                                                addNode(newNode);
                                                return newNode;
                                            } else if (progressive) {
                                                // Otherwise, if it is incremental but not a goal for all instances,
                                                // activate the instance where the program failed
                                                int id = newNode->getProgram()->getFailedInstanceID();
                                                _gpp->activateInstance(id);
                                                newNode->setF(f(newNode));
                                            }

                                            if (not DEFERRED_EVALUATION and REEVALUATE_OPEN_QUEUE) {
                                                cout << "[INFO] Failed on instance id "
                                                     << newNode->getProgram()->getFailedInstanceID()
                                                     << ". Reevaluating the queue... " << endl;
                                                // This snippet is not compatible with DEFERRED_EVALUATION
                                                priority_queue<Node *, vector<Node *>, CmpNodes> aux_open;
                                                while (!_open.empty()) {
                                                    Node *aux_current = _open.top();
                                                    _open.pop();
                                                    auto vps3 = aux_current->getProgram()->runNovelty(_gpp, true,
                                                                                                      progressive);
                                                    freeProgramStates(vps3);
                                                    if (vps3.empty()) {
                                                        delete aux_current;
                                                        continue;
                                                    }
                                                    aux_current->setF(f(aux_current));
                                                    aux_open.push(aux_current);
                                                }
                                                // Reset the new best evaluations
                                                best_evaluations = vector<int>(_heuristics.size(), INF);
                                                // Save the reevaluated open queue
                                                swap(aux_open, _open);
                                                cout << "[INFO] DONE!" << endl;
                                                // Delete child and continue
                                                delete newNode;
                                                continue;
                                            }
                                        }
                                        addNode(newNode);
                                    }
                                }
                            }
                        }
                        else{
                            if( not DEFERRED_EVALUATION ) {
                                childs[i]->setF(f(childs[i]));
                            }
                            else{
                                childs[i]->setF( current_evaluations );
                            }

                            if( isGoal( childs[ i ], progressive ) ){
                                if( progressive ){
                                    cout << "[INFO] Possible solution found at node id " << childs[i]->getID() << " when solving instance ids:";
                                    for( int aux_id = 0; aux_id < _gpp->getNumInstances(); aux_id++ )
                                        if( _gpp->isInstanceActive(aux_id) ) cout << " #" << aux_id;
                                    cout << endl << childs[i]->toString() << endl;
                                }
                                // if the search is not incremental, or if the program solves all instances finish
                                if( not progressive or isGoal( childs[i], false ) ){
                                    childs[i]->setF(f(childs[i]));
                                    addNode( childs[ i ] );
                                    for( int j = i+1; j < int( childs.size() ); j++ )
                                        if( childs[ j ] )
                                            delete childs[ j ];
                                    return childs[ i ];
                                }
                                else if( progressive ){
                                    // Otherwise, if it is incremental but not a goal for all instances,
                                    // activate the instance where the program failed
                                    int id = childs[i]->getProgram()->getFailedInstanceID();
                                    _gpp->activateInstance( id );
                                    childs[i]->setF(f(childs[i]));
                                }
                                // This point is only reachable from progressive method
                                // where not all instances are covered by the current solution, so
                                // the first instance where the generalized planning problem is now
                                // active and the open queue can be reevaluated
                                if( not DEFERRED_EVALUATION and REEVALUATE_OPEN_QUEUE ){
                                    cout << "[INFO] Failed on instance id " << childs[i]->getProgram()->getFailedInstanceID()
                                         << ". Reevaluating the queue... " << endl;
                                    // This snippet is not compatible with DEFERRED_EVALUATION
                                    priority_queue< Node*, vector< Node* >, CmpNodes > aux_open;
                                    while( !_open.empty() ) {
                                        Node *aux_current = _open.top(); _open.pop();
                                        auto vps2 = aux_current->getProgram()->runNovelty(_gpp,true,progressive);
                                        freeProgramStates(vps2 );
                                        if( vps2.empty() ){
                                            delete aux_current;
                                            continue;
                                        }
                                        aux_current->setF( f(aux_current) );
                                        aux_open.push( aux_current );
                                    }
                                    // Reset the new best evaluations
                                    best_evaluations = vector<int>(_heuristics.size(),INF);
                                    // Save the reevaluated open queue
                                    swap( aux_open, _open );
                                    cout << "[INFO] DONE!" << endl;
                                    // Delete child and continue
                                    delete childs[i];
                                    break;
                                }
                            }

                            addNode(childs[i]);
                            break;
                        }
                    }
                }
                else{
                        _deadends++;
                        delete childs[ i ];
                        continue;
                }
            }
                else{
                    if( vps.empty() ) {
                        _deadends++;
                        delete childs[ i ];
                        continue;
                    }

                    if( not DEFERRED_EVALUATION ) {
                        childs[i]->setF(f(childs[i]));
                    }
                    else{
                        childs[i]->setF( current_evaluations );
                    }

                    if( isGoal( childs[ i ], progressive ) ){
                        if( progressive ){
                            cout << "[INFO] Possible solution found at node id " << childs[i]->getID() << " when solving instance ids:";
                            for( int aux_id = 0; aux_id < _gpp->getNumInstances(); aux_id++ )
                                if( _gpp->isInstanceActive(aux_id) ) cout << " #" << aux_id;
                            cout << endl << childs[i]->toString() << endl;
                        }
                        // if the search is not incremental, or if the program solves all instances finish
                        if( not progressive or isGoal( childs[i], false ) ){
                            childs[i]->setF(f(childs[i]));
                            addNode( childs[ i ] );
                            for( int j = i+1; j < int( childs.size() ); j++ )
                                if( childs[ j ] )
                                    delete childs[ j ];
                            return childs[ i ];
                        }
                        else if( progressive ){
                            // Otherwise, if it is incremental but not a goal for all instances,
                            // activate the instance where the program failed
                            int id = childs[i]->getProgram()->getFailedInstanceID();
                            _gpp->activateInstance( id );
                            childs[i]->setF(f(childs[i]));
                        }
                        // This point is only reachable from progressive method
                        // where not all instances are covered by the current solution, so
                        // the first instance where the generalized planning problem is now
                        // active and the open queue can be reevaluated
                        if( not DEFERRED_EVALUATION and REEVALUATE_OPEN_QUEUE ){
                            cout << "[INFO] Failed on instance id " << childs[i]->getProgram()->getFailedInstanceID()
                                 << ". Reevaluating the queue... " << endl;
                            // This snippet is not compatible with DEFERRED_EVALUATION
                            priority_queue< Node*, vector< Node* >, CmpNodes > aux_open;
                            while( !_open.empty() ) {
                                Node *aux_current = _open.top(); _open.pop();
                                auto vps2 = aux_current->getProgram()->runNovelty(_gpp,true,progressive);
                                freeProgramStates(vps2 );
                                if( vps2.empty() ){
                                    delete aux_current;
                                    continue;
                                }
                                aux_current->setF( f(aux_current) );
                                aux_open.push( aux_current );
                            }
                            // Reset the new best evaluations
                            best_evaluations = vector<int>(_heuristics.size(),INF);
                            // Save the reevaluated open queue
                            swap( aux_open, _open );
                            cout << "[INFO] DONE!" << endl;
                            // Delete child and continue
                            delete childs[i];
                            continue;
                        }
                    }
                    addNode(childs[i]);

                }

			}
		}
		
		return nullptr;
	}
	
private:
	priority_queue< Node*, vector< Node* >, CmpNodes > _open;
};

#endif
