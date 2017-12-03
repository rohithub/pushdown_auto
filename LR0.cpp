/* Checks for expression matching of form a^i.b^j.c^k | i,j,k>0 and i==j or i==k */
#include<iostream>
#include<fstream>
#include<string>

#define INITIAL 	0
#define STATE_COUNT 	1

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
};

char transition_char[NUM_CHAR];
unsigned int num_transition_char = 0;
int rule_count = 0;
grammer lr0[NUM_RULES];
states stt[NUM_STATES]; //State Transition Table
int state_count = 0;

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
}
void createTable(void)
{

	// Make S0
	stt[0].state_num = 0;
	for(int j = 0;j<rule_count;j++){
		stt[0].currentRules[j] = lr0[j];
	}
	stt[0].countOfRules = rule_count;
	for(int i=0;i<stt[0].countOfRules;i++)
	{	
		for(int j=0;j<num_transition_char;j++)
		{	
			if(stt[0].currentRules[i].rule.at(stt[0].currentRules[i].dot_pos) == transition_char[j])
			{
				if(!stt[0].isValid[j])
				{	
					stt[0].next_states[j] = state_count + 1;
					stt[0].isValid[j] = true;
					state_count++;
					stt[state_count].state_num = state_count;
					stt[state_count].countOfRules = 0;

					stt[state_count].currentRules[stt[state_count].countOfRules] = stt[0].currentRules[i];
					stt[state_count].currentRules[stt[state_count].countOfRules].dot_pos++;	
					stt[state_count].countOfRules++;
					

				}
				else
				{
					cout<<"In second loop"<<endl;
					int rulesInValidState = stt[stt[0].next_states[j]].countOfRules;
					int nextValidState = stt[0].next_states[j];
					stt[nextValidState].currentRules[rulesInValidState] = stt[0].currentRules[i];
					stt[nextValidState].currentRules[rulesInValidState].dot_pos++;
					stt[nextValidState].countOfRules++;
				}
				break;
			} 
		}
	}
	for(int i = 0;i<=state_count;i++){
		cout << "State "<<i<<endl;
		states currentState = stt[i];
		cout << "Rules "<<endl;
		for(int j = 0;j<currentState.countOfRules;j++){
			cout <<"	Rule "<<j<<" : "<<currentState.currentRules[j].rule<<" with dot before pos "<< currentState.currentRules[j].dot_pos<<endl;
		}
		
		cout << "Next States for chars  "<<endl;
		for(int j = 0;j<num_transition_char;j++){
			if(currentState.isValid[j]){
				cout<<"		"<<transition_char[j]<<" : "<<currentState.next_states[j]<<endl;
			}
		}
	}
}


int main(){

	compile();
    	createTable();
	
	return 0;
}













