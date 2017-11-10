/* Checks for expression matching of form a^i.b^j.c^k | i,j,k>0 and i==j or i==k */
#include<iostream>
#include<fstream>
#include<string>

#define INITIAL 	0
#define STATE_COUNT 	1

#define DEBUG 		true
#define MAX_CHARS	26

using namespace std;

 char first_term = 0;
 char second_term = 0;
 char third_term = 0;
 enum ComparisonType {LT,LTE,GT,GTE,EQ};
 enum ComparisonValueType{IMM,COMP};

 string typeName[] = {"LT","LTE","GT","GTE","EQ"};   
 string valueTypeName[] = {"IMM","COMP"};   
 struct Comparison{
    ComparisonValueType comparisonValueTypeLeft;
    int left;
    ComparisonValueType comparisonValueTypeRight;
    int right;
    ComparisonType operation;
    bool progress; //true for AND and false for OR
    bool last;
 };


 Comparison allCompares[MAX_CHARS];  //MAX NO of allowed conditions 

 int numTerms = 0; // Carries the maximum type of characters that are to be analysed
 int compares = 0;
 char componentsTochars[MAX_CHARS];
 char countTermOfComponent[MAX_CHARS];
 bool compareBit[MAX_CHARS];
 int startCompareIndex[MAX_CHARS];
 int endCompareIndex[MAX_CHARS];
 
 int state = INITIAL;
 unsigned int char_count[MAX_CHARS] = {0};
 unsigned int curr_index = 0; //Points to the current index worked on
 
bool pda(char inputChars, Comparison *compiled_table){
    bool compare = false;
    bool match_det = false;
    switch(state){
        case INITIAL:
            if(inputChars == componentsTochars[curr_index]){
		state = STATE_COUNT;
                char_count[curr_index]++;
            }
            break;

	case STATE_COUNT:
		if(curr_index < numTerms)
		{		
			if(inputChars == componentsTochars[curr_index])
			{
				char_count[curr_index]++;
			}
			else
			{
                		compare = true;
				if(inputChars == componentsTochars[curr_index+1])
				{
					char_count[++curr_index]++;
				}
				else
				{
					for(int j=0; j<MAX_CHARS; j++)
					{
						char_count[j] = 0;	//Reset all counters before going to the initial state
					}
					curr_index = 0;
					state=INITIAL;
				}
			}
		}
		else
		{
			// if we reach here, we are processing more characters than the max number from our conditions
			for(int j=0; j<MAX_CHARS; j++)
			{
				char_count[j] = 0;	//Reset all counters before going to the initial state
			}
			curr_index = 0;
			state=INITIAL;
		}
		break;
    }

	if((compare == true) && (compareBit[curr_index] == 1)) //Means we can do the compare right now
	{
		match_det = true;
		compare = false;
		for(int k=startCompareIndex[curr_index]; k<=endCompareIndex[curr_index]; k++) //Compare for all the events till now
		{
    			if(DEBUG)
			{
        			cout<< "Input char : "<<inputChars << ", Operation " <<compiled_table[k].operation << ", Comparison type " << compiled_table[k].comparisonValueTypeRight << ", COMP value " << char_count[compiled_table[k].right] << endl;
			}
			
			switch(compiled_table[k].operation){
			case(LT):
				if(compiled_table[k].comparisonValueTypeRight == IMM)
				{
					if(char_count[k] >= compiled_table[k].right)
					{
						match_det = false;
						break;
					}
				}
				else	//else compare with count value
				{
					if(char_count[k] >= char_count[compiled_table[k].right])
					{
						match_det = false;
						break;
					}
				}
			break;

			case(LTE):
				if(compiled_table[k].comparisonValueTypeRight == IMM)
				{
					if(char_count[k] > compiled_table[k].right)
					{
						match_det = false;
						break;
					}
				}
				else	//else compare with count value
				{
					if(char_count[k] > char_count[compiled_table[k].right])
					{
						match_det = false;
						break;
					}
				}
			break;

			case(GT):
				if(compiled_table[curr_index].comparisonValueTypeRight == IMM)
				{
					if(char_count[k] <= compiled_table[k].right)
					{
						match_det = false;
						break;
					}
				}
				else	//else compare with count value
				{
					if(char_count[k] <= char_count[compiled_table[k].right])
					{
						match_det = false;
						break;
					}
				}
			break;

			case(GTE):
				if(compiled_table[curr_index].comparisonValueTypeRight == IMM)
				{
					if(char_count[k] < compiled_table[k].right)
					{
						match_det = false;
						break;
					}
				}
				else	//else compare with count value
				{
					if(char_count[k] < char_count[compiled_table[k].right])
					{
						match_det = false;
						break;
					}
				}
			break;

			case(EQ):
				if(compiled_table[curr_index].comparisonValueTypeRight == IMM)
				{
					if(char_count[k] != compiled_table[k].right)
					{
						match_det = false;
						break;
					}
				}
				else	//else compare with count value
				{
					if(char_count[k] != char_count[compiled_table[k].right])
					{
						match_det = false;
						break;
					}
				}
			break;
			}
		}
	}
    if(DEBUG){
        //cout<<"New state : "<<state<<endl;
        //cout<<"---------------------"<<endl;
    }
    return match_det; 

}

bool run(string in){
    bool match = false;
    state = INITIAL;
    for(int i = 0; i<in.length();i++){
        if(pda(in.at(i), allCompares)){
            match = true; 
            break;
        }
    }
    return match;
}


string removeWhiteSpaces(string in){
    string out = "";

    for(int i = 0;i<in.length();i++){
        char ch = in.at(i);
        if(ch != ' '){
            out+=ch;
        }
    }
    return out;
}


int findComponentWithInputCountChar(char ch){
    for(int i = 0;i<numTerms;i++){
        if(countTermOfComponent[i] == ch){
            return i;
        }
    }
    return -1;
}

bool isAlphabet(char ch){
    int num = (int) ch;
    if((num <= 90 && num >= 65) || (num <= 122 && num >= 97)){
        return true;
    }
    return false;
}

//restriction on regex --> cannot use compiler keychars
//only enter char searches
//only 26 chars allowed
void compile(string in){
    numTerms = 0;
    bool seeCounter = false;
    int i = 0;
    for(;i<in.length();i++){
        char indChar = in.at(i);
        if(indChar == '('){
            seeCounter = true;
        }
        else if(indChar == ')'){
            seeCounter = false;
            numTerms++;
        }
        else if(indChar == ','){
            i++;
            break;
        }
        else{ //any other character
            if(seeCounter){
                countTermOfComponent[numTerms] = indChar;
            }
            else{
                componentsTochars[numTerms] = indChar;
            }
        }
    }
    
    compares = 0;
    bool processingACompare = false;
    bool isComp;
    int maxComponentSeen = -1;
    int startDepthOfComparisonTable = 0;
    for(;i<in.length();i++){
        char ch = in.at(i);
        if(ch == '&'){
            allCompares[compares].progress = true;
            allCompares[compares].last = false;
            compares++;
            processingACompare = false;
        }
        else if(ch == '|'){
            allCompares[compares].last = false;
            processingACompare = false;
            if(maxComponentSeen != -1){
                compareBit[maxComponentSeen] = true;
                startCompareIndex[maxComponentSeen] = startDepthOfComparisonTable;
                endCompareIndex[maxComponentSeen] = compares;

            }
            maxComponentSeen = -1;
            compares++;
            startDepthOfComparisonTable = compares;
            
        }
        else if(ch == '>'){
            allCompares[compares].operation = GT;
        }
        else if(ch == '<'){
            allCompares[compares].operation = LT;
        }
        else if(ch == '='){
            char prev = in.at(i-1);
            if(prev == '>'){
                allCompares[compares].operation = GTE;
            }
            else if(prev == '<'){
                allCompares[compares].operation = LTE;
            }
            else{
                allCompares[compares].operation = EQ;
            }
        }
        else{
            if(!processingACompare){
                processingACompare = true;
                allCompares[compares].progress = false;
                allCompares[compares].last = true;
                isComp = isAlphabet(ch);
                if(isComp){
                    allCompares[compares].comparisonValueTypeLeft = COMP;
                    allCompares[compares].left = findComponentWithInputCountChar(ch);
                    if(maxComponentSeen < allCompares[compares].left){
                        maxComponentSeen = allCompares[compares].left;
                    }
                }
                else{
                    allCompares[compares].comparisonValueTypeLeft = IMM;
                    allCompares[compares].left = 0;
                    while(i<in.length()){
                        int num1 = (int)in.at(i) - 48;
                        if(num1 < 0 && num1 > 9){
                            break;
                        }
                        else{
                            allCompares[compares].left = (allCompares[compares].left * 10) + num1;
                            i++;
                        }
                    }
                }
            } // end of if(!processingACompare)
            else{
                isComp =  isAlphabet(ch);
                if(isComp){
                    allCompares[compares].comparisonValueTypeRight = COMP;
                    allCompares[compares].right = findComponentWithInputCountChar(ch);
                    if(maxComponentSeen < allCompares[compares].right){
                        maxComponentSeen = allCompares[compares].right;
                    }
                }
                else{
                    allCompares[compares].comparisonValueTypeRight = IMM;
                    allCompares[compares].right = 0;
                    while(i<in.length()){
                        int num1 = (int)in.at(i) - 48;
                        if(num1 < 0 || num1 > 9){
                            i--;
                            cout<<allCompares[compares].right<<endl;
                            break;
                        }
                        else{
                            allCompares[compares].right = (allCompares[compares].right * 10) + num1;
                            i++;
                        }
                    }
                }
            } // end of else of if(!processingACompare)
        } //end of big else
    } //end of second for    
  
                cout<<"Max component seen : "<<maxComponentSeen<<endl;
                cout<<"Max component seen : "<<maxComponentSeen<<endl;
                compareBit[maxComponentSeen] = true;
                startCompareIndex[maxComponentSeen] = startDepthOfComparisonTable;
                endCompareIndex[maxComponentSeen] = compares;
    compares++;
    if(DEBUG){
        cout<<"index,chars,compare bit,start comparison index, end comparison index"<<endl;
        for(int j = 0;j<numTerms;j++){
            cout<<j<<" , "<<componentsTochars[j]<<" , "<<compareBit[j]<<" , "<<startCompareIndex[j]<<" , "<<endCompareIndex[j]<<endl;
        }

        cout<<"Comparisons Detected are : "<<endl;
        cout<<"#Compares,leftType,leftValue,RightType,rightValue,comparison,progress,isLast"<<endl;
        for(int j = 0;j<compares;j++){
            Comparison c = allCompares[j];
            cout<<j<<" , "<<valueTypeName[c.comparisonValueTypeLeft]<<" , "<<c.left<<" , "<<valueTypeName[c.comparisonValueTypeRight]<<" , "<<c.right<<" , "<<typeName[c.operation]<<" , "<<c.progress<<" , "<<c.last<<endl;
        }
    }
    
}

int main(){
    string user_chars;
    ifstream inputChars;
    ifstream inputRegex;
    string expressionIn = "";
    string regexIn = "";
    bool match = false;

    inputRegex.open ("regex.txt");
    if(inputRegex.is_open()){
        getline(inputRegex,regexIn);
    }
    compile(regexIn);
    //return(0);
    inputChars.open ("regexInput.txt");
    if(inputChars.is_open()){
        while(getline(inputChars,expressionIn)){
            cout<<"Input String : "<< expressionIn <<endl;
            if(run(expressionIn)){
                cout<< "Regex matched" <<endl;
            }
            else{
                cout<< "Regex NOT matched" <<endl;
            }
        }
    }
    else{
        cout<<"File not found";
        return 1;
    }
    
    return 0;
}
