/* Checks for expression matching of form a^i.b^j.c^k | i,j,k>0 and i==j or i==k */
#include<iostream>
#include<fstream>
#include<string>

#define STATE_A 1
#define INITIAL 0
#define STATE_B 2
#define STATE_C 3
#define END 4
#define DEBUG false

using namespace std;

 int state = INITIAL;
 int count_A = 0;
 int count_B = 0;
 int count_C = 0;

 char first_term = 0;
 char second_term = 0;
 char third_term = 0;

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

int main(){
    string user_chars;
    ifstream inputChars;
    string expressionIn = "";
    bool match = false;

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
