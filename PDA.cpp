/* Checks for expression matching of form a^i.b^j.c^k | i,j,k>0 and i==j or i==k */
#include<iostream>
#include<fstream>
#include<string>

#define STATE_A 1
#define INITIAL 0
#define STATE_B 2
#define STATE_C 3
#define END 4
#define DEBUG true

using namespace std;

 int state = INITIAL;
 int count_A = 0;
 int count_B = 0;
 int count_C = 0;

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

 Comparison allCompares[26];  //MAX NO of allowed conditions 

 int gotoComp[26]; //TODO: make -1 initially

 int numTerms = 0;
 int compares = 0;
 char componentsTochars[26];
 char countTermOfComponent[26];
 
bool pda(char inputChars){
    bool reset = false;
    bool compare = false;
    if(DEBUG){
        cout<<"Input char : "<<inputChars<<endl;
        cout<<"Current state : "<<state<<endl;
    }
    switch(state){
        case INITIAL:
            if(inputChars == first_term){
                state = STATE_A;
                count_A++;
            }
            break;
        case STATE_A:
            if(inputChars == second_term){
                state = STATE_B;
                count_B++;
            }
            else if(inputChars == first_term){
                count_A++;
            }
            else if(inputChars == third_term){
                state = STATE_C;
                count_C++;
            }
            else{
                reset = true;
            }
            break;
        case STATE_B:
            if(inputChars == third_term){
                state = STATE_C;
                count_C++;
                compare = true;
            }
            else if(inputChars == second_term){
                count_B++;
            }
            else{
                reset = true;
            }
            break;
        case STATE_C:
            if(inputChars == third_term){
                count_C++;
            }
            else{
                state = INITIAL;
                compare = true;
            }
            break;
    }
    if(DEBUG){
        cout<<"New state : "<<state<<endl;
        cout<<"Count A : "<<count_A<<endl;
        cout<<"Count B : "<<count_B<<endl;
        cout<<"Count C : "<<count_C<<endl;
        cout<<"---------------------"<<endl;
    }
    return compare && (count_A == count_B || count_A == count_C); 

}

bool run(string in){
    bool match = false;
    count_A = 0;
    count_B = 0;
    count_C = 0;
    state = INITIAL;
    for(int i = 0; i<in.length();i++){
        if(pda(in.at(i))){
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
    for(;i<in.length();i++){
        char ch = in.at(i);
        cout<<"Comparison chars : "<<ch<<endl;
        if(ch == '&'){
            allCompares[compares].progress = true;
            allCompares[compares].last = false;
            compares++;
            processingACompare = false;
        }
        else if(ch == '|'){
            allCompares[compares].last = false;
            compares++;
            processingACompare = false;
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
                cout <<"isComp(Left) : "<<isComp<<endl;
                if(isComp){
                    allCompares[compares].comparisonValueTypeLeft = COMP;
                    allCompares[compares].left = findComponentWithInputCountChar(ch);
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
                cout <<"isComp(Right) : "<<isComp<<endl;
                if(isComp){
                    allCompares[compares].comparisonValueTypeRight = COMP;
                    allCompares[compares].right = findComponentWithInputCountChar(ch);
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
                            cout<<"Adding : "<<num1<<endl;
                            allCompares[compares].right = (allCompares[compares].right * 10) + num1;
                            i++;
                        }
                    }
                }
            } // end of else of if(!processingACompare)
        } //end of big else
    } //end of second for    
    
    compares++;
    if(DEBUG){
        cout<<"Detected components are : "<<endl;
        for(int j = 0;j<numTerms;j++){
            cout<<j<<" => char : "<<componentsTochars[j]<<"; with count term : "<<countTermOfComponent[j]<<endl;
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
    string expressionIn = "";
    bool match = false;

    compile("0(i)1(j),i>=0&j>=0&i=j");
    return 0;
    cout << "Enter the characters to be matched (3 only) = ";
    cin >> user_chars;
    first_term = user_chars[0];
    second_term = user_chars[1];
    third_term = user_chars[2];

    if(user_chars.length() == 3)
    {	    
    	cout << "Selected chars: " << first_term << ", " << second_term << ", " << third_term << endl;
    }
    else
    {
	cout << "Input Error: Length of the matching characters should be only 3!! " << endl;
	return 1;
    }
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
