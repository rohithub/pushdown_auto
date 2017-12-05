#include<iostream>
#include<fstream>
#include<string>

#define DEBUG 		true
#define MAX_CHARS	26

#define NUM_RULES	10
#define NUM_STATES	250
#define NUM_CHAR	128

using namespace std;

struct grammer{
	char type; 		// This is left side of the rule
	string rule; 		// This is the right side of the rule
	unsigned int dot_pos;	// Tells where the dot is in the current grammer
};

struct states{
    unsigned int state_num;		// Number assigned to this state, such as S0, S1, etc
	grammer currentRules[NUM_RULES]; // What rules this states hold
	int next_states[NUM_CHAR]; 	// The states that are child of this
	bool isValid[NUM_CHAR];
	unsigned countOfRules;
	bool isEndState;				
	bool isReductionState;
    
    int gotoStates[NUM_CHAR];
    bool isGotoStatesValid[NUM_CHAR];
    


    bool seenExpandedStatesForType[NUM_RULES];
};

char transition_char[NUM_CHAR];
unsigned int num_transition_char = 0;
char rule_type_char[NUM_CHAR];
unsigned int num_rule_type_char = 0;


int rule_count = 0;
grammer lr0[NUM_RULES];
states stt[NUM_STATES]; //State Transition Table
int state_count = 0;

int sameCharRuleCount = 0;

int output[NUM_RULES];

void fillTransitionChar(string str)
{
	int add_char = 1;

	for(int j=0; j<str.length(); j++)
	{
		for(int i=0; i<num_transition_char; i++)
		{
			if(str.at(j) == transition_char[i])
			{
				add_char = 0;
				break;
			}
		}	
		if(add_char == 1)
		{
			transition_char[num_transition_char++] = str.at(j);
		}
		add_char = 1; //Get it ready for the next character
	}
}

void fillRuleType(char ch){
    bool add_char = true;
    for(int i = 0;i<num_rule_type_char;i++){
        if(rule_type_char[i] == ch){
            add_char = false;
            break;
        }
    }
    if(add_char){
        rule_type_char[num_rule_type_char] = ch;
        num_rule_type_char++;
    }
}

int compile(void)
{
	string expressionIn = "";

	ifstream inputChars;
	inputChars.open("regex.txt");
	if(inputChars.is_open()){
		while(getline(inputChars,expressionIn)){
			
			cout<<"Input String : "<< expressionIn <<endl;
			lr0[rule_count].type = expressionIn.at(0);	// Left side of the rule. Only has one character
			lr0[rule_count].rule = expressionIn.substr(2, expressionIn.length() - 2); //Right side of the rule. After >
			lr0[rule_count].dot_pos = 0;	//Initialize all the dot positions to 0

			fillTransitionChar(lr0[rule_count].rule); //Collect all the new characters 
			fillRuleType(lr0[rule_count].type);
            rule_count++;

	    }

	    cout << transition_char << endl;
	    	for(int i = 0;i<rule_count;i++){
			cout << "Rule "<<i<<" : "<<lr0[i].rule<< endl;
		}
	}
	else{
		cout<<"File not found";
		return 1;
	}

		
}

string printRuleWithDot(grammer inputRule){
	string out = "";
	out +=inputRule.type;
	out += '>';
	bool seenDot = false;
	for(int i = 0;i<inputRule.rule.length();i++){
		if(!seenDot && i==inputRule.dot_pos){
			out += '.';
			i--;
			seenDot = true;
		}
		else{
			out += inputRule.rule.at(i);
		}
	}
    if(!seenDot){
        out += '.';
    }
    return out;
}

bool checkForEquivalentStates(states in1, states in2){
    if(in1.countOfRules != in2.countOfRules){
        return false;
    }
    for(int i = 0;i<in1.countOfRules;i++){
        grammer ruleOfIn1 = in1.currentRules[i];
        grammer ruleOfIn2 = in2.currentRules[i];

        if(ruleOfIn1.type != ruleOfIn2.type){
            return false;
        }
        if(ruleOfIn1.rule.compare(ruleOfIn2.rule) != 0){
            return false;
        }
        if(ruleOfIn1.dot_pos != ruleOfIn2.dot_pos){
            return false;
        }
    }

    return true;
}

unsigned int giveEquivalentStateNumber(states in){  //do for every new state created to join equivalent states, returns equivalent state number
    for(int i = state_count - 1;i>=0;i--){
        if(stt[i].state_num != in.state_num && checkForEquivalentStates(stt[i],in)){
            return i;
        }
    }
    return -1;
}

int* rulesWithSameTypeAsChar(char in){
    sameCharRuleCount = 0;
    for(int i = 0;i<rule_count;i++){
        if(lr0[i].type == in){
            output[sameCharRuleCount] = i;
            sameCharRuleCount++;
        }
    }
    return output;
}

bool addMoreRulesWithChar(char in){
    for(int i = 0;i<rule_count;i++){
        if(lr0[i].type == in){
            return true;
        }
    }
    return false;
}

int ruleTypePosInArray(char ch){
    for(int i = 0;i<num_rule_type_char;i++){
        if(rule_type_char[i] == ch){
            return i;
        }
    }
    return -1;
}

void generateEquivalentRules(states* inputState){
    int i = 0;
    while(i<inputState->countOfRules){
        grammer currentRule = inputState->currentRules[i];
        if(currentRule.rule.length() > currentRule.dot_pos){
            char nextChar = currentRule.rule.at(currentRule.dot_pos);   //The next char of the new rule that must be considered for making a new rule
            if(addMoreRulesWithChar(nextChar)){
                int pos = ruleTypePosInArray(nextChar);
                if(pos >= 0 && !inputState->seenExpandedStatesForType[pos]){
                    int* ruleNumbersToAdd = rulesWithSameTypeAsChar(nextChar);
                    for(int j = 0;j<sameCharRuleCount;j++){
                        inputState->currentRules[inputState->countOfRules] = lr0[ruleNumbersToAdd[j]];
                        inputState->countOfRules++;
                    }
                    inputState->seenExpandedStatesForType[pos] = true;
                }
            }
        }
        i++;
    }
}

void combineEquivalentStates(){
    int newState = 0;
    for(int i = state_count-1;i>=0;i--){
        newState = giveEquivalentStateNumber(stt[i]);
        if(newState != -1){
            state_count--;
            for(int j = 0;j<state_count;j++){
                for(int k = 0;k<num_transition_char;k++){
                    if(stt[j].isValid[k] && stt[j].next_states[k] == i){
                        stt[j].next_states[k] = newState;
                    }
                }
            }

        }
    }
}

int giveCharPositionInTransitionCharArray(char ch){
    for(int i = 0;i<num_transition_char;i++){
        if(transition_char[i] == ch){
            return i;
        }
    }
    return -1;
}

void generateGotoStates(){
/*struct states{
    unsigned int state_num;		// Number assigned to this state, such as S0, S1, etc
	grammer currentRules[NUM_RULES]; // What rules this states hold
	int next_states[NUM_CHAR]; 	// The states that are child of this
	bool isValid[NUM_CHAR];
	unsigned countOfRules;
	bool isEndState;				
	bool isReductionState;
    
    int gotoStates[NUM_CHAR];
    bool isGotoStatesValid[NUM_CHAR];
    


    bool seenExpandedStatesForType[NUM_RULES];
};*/
    for(int i = 0;i<state_count;i++){
        for(int j = 0;j<num_transition_char;j++){
            if(stt[i].isValid[j] && stt[stt[i].next_states[j]].isReductionState){
                char createdRuleType = stt[stt[i].next_states[j]].currentRules[0].type;
                int nextStateNumberForNewType = giveCharPositionInTransitionCharArray(createdRuleType);
                if(nextStateNumberForNewType != -1 && stt[i].isValid[nextStateNumberForNewType]){
                    stt[i].gotoStates[j] = stt[i].next_states[nextStateNumberForNewType];
                    stt[i].isGotoStatesValid[j] = true;
                }
            }
        }
    }
}

void createTable(void)
{
	// For a valid grammer, S) will always exist. Make S0
	stt[0].state_num = 0;
	for(int j = 0;j<rule_count;j++){
		stt[0].currentRules[j] = lr0[j];
	}
	stt[0].countOfRules = rule_count;
	state_count++;

	//Start exapnading the state diagram
	for(int itr = 0; itr <state_count; itr++)
	{   //itr is the current state which is being processed
		//cout << "**** State " << itr << endl;
		if(!stt[itr].isEndState)
		{
			for(int i=0;i<stt[itr].countOfRules;i++)
			{	
				
				//cout<<"        Count of Rules "<< i << endl;
				for(int j=0;j < num_transition_char;j++)
				{	
					//cout<<"***************Transition char num "<< j << endl;
					if(stt[itr].currentRules[i].rule.length() <= stt[itr].currentRules[i].dot_pos){
                        stt[itr].isReductionState = true;
                    }
                    else{
                        if(stt[itr].currentRules[i].rule.at(stt[itr].currentRules[i].dot_pos) == transition_char[j])
                        {
                            //cout<<"         Char matched "<< transition_char[j] << endl;
                            if(!stt[itr].isValid[j])
                            {
            
                                stt[itr].next_states[j] = state_count;
                                stt[itr].isValid[j] = true;
                                //cout<<"            State Created "<< state_count;
                                
                                stt[state_count].state_num = state_count;
                                stt[state_count].countOfRules = 0;

                                //TODO: We have to add a for lopp which will check all kind of transitions given a character	
                                stt[state_count].currentRules[stt[state_count].countOfRules] = stt[itr].currentRules[i];
                                stt[state_count].currentRules[stt[state_count].countOfRules].dot_pos++;	
                                stt[state_count].countOfRules++;
                                //cout << ", Rules created " << stt[state_count].countOfRules << endl;
                                if(transition_char[j] == '$')
                                {
                                    stt[state_count].isEndState = true; 
                                }
                                else
                                {
                                    stt[state_count].isEndState = false;
                                }
                                state_count++;
                                generateEquivalentRules(&stt[state_count-1]);
                                
                            }
                            else
                            {
                                //cout<<"            In second loop for "<< transition_char[j] << endl;
                                int rulesInValidState = stt[stt[itr].next_states[j]].countOfRules;
                                int nextValidState = stt[itr].next_states[j];
                                stt[nextValidState].currentRules[rulesInValidState] = stt[itr].currentRules[i];
                                stt[nextValidState].currentRules[rulesInValidState].dot_pos++;
                                stt[nextValidState].countOfRules++;
                                generateEquivalentRules(&stt[nextValidState]);
                            }
                            break;
                        }
                    }
				}
			}
		}
        combineEquivalentStates();
	}
    generateGotoStates();

    cout<<"Number of Generated States : "<<state_count<<endl;
	for(int i = 0;i<state_count;i++){
		cout << "************************************"<<endl;
		cout << "State "<<i<<endl;
		states currentState = stt[i];
		cout << "Rules "<<endl;
		for(int j = 0;j<currentState.countOfRules;j++){
			//cout <<"	Rule "<<j<<" : "<<currentState.currentRules[j].rule<<" with dot before pos "<< currentState.currentRules[j].dot_pos<<endl;
			cout <<"	Rule "<<j<<" : "<<printRuleWithDot(currentState.currentRules[j])<<endl;
		}
		
		cout << "Next States for chars  "<<endl;
		for(int j = 0;j<num_transition_char;j++){
			if(currentState.isValid[j]){
				cout<<"		"<<transition_char[j]<<" : "<<currentState.next_states[j];
                if(currentState.isGotoStatesValid[j]){
                    cout<<"("<<currentState.gotoStates[j]<<")"<<endl;
                }
                else{
                    cout<<endl;
                }
			}
		}
        if(currentState.isReductionState){
            cout << "This is a reduction state"<<endl;
        }
        if(currentState.isEndState){
            cout << "This is an end state"<<endl;
        }
	}
}


int main(){

	compile();
    createTable();
	
	return 0;
}


