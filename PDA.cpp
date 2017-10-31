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

bool pda(char inputChars){
    bool reset = false;
    bool compare = false;
    if(DEBUG){
        cout<<"Input char : "<<inputChars<<endl;
        cout<<"Current state : "<<state<<endl;
    }
    switch(state){
        case INITIAL:
            if(inputChars == 'a'){
                state = STATE_A;
                count_A++;
            }
            break;
        case STATE_A:
            if(inputChars == 'b'){
                state = STATE_B;
                count_B++;
            }
            else if(inputChars == 'a'){
                count_A++;
            }
            else if(inputChars == 'c'){
                state = STATE_C;
                count_C++;
            }
            else{
                reset = true;
            }
            break;
        case STATE_B:
            if(inputChars == 'c'){
                state = STATE_C;
                count_C++;
                compare = true;
            }
            else if(inputChars == 'b'){
                count_B++;
            }
            else{
                reset = true;
            }
            break;
        case STATE_C:
            if(inputChars == 'c'){
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
    ifstream inputChars;
    inputChars.open ("regexInput.txt");
    string in = "";
    bool match = false;
    if(inputChars.is_open()){
        while(getline(inputChars,in)){

            cout<<"Input String : "<<in<<endl;
            if(run(in)){
                cout<<"Regex matched"<<endl;
            }
            else{
                cout<<"Regex not matched"<<endl;
            }
        }
    }
    else{
        cout<<"File not found";
        return 1;
    }
    
    return 0;
}
