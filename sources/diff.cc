/**
* klassenaam: Derivative functions 
* Luuk Daleman (3323560)
* Mirza Kuraesin (3278395)
* diff.cc
* 4/11/2022 
**/

#include "../headers/token.h"

//Deriveert de boom recursief
//In plaats van de oude boom te veranderen, maakt deze een nieuwe boom aan
//Aan het eind wordt de oude boom verwijderd
Token* Token::diff(){
    Token *newTree = new Token();//nieuwe boom
    switch (type){
        case VAR:
            diffVar(newTree);
            break;
        case NUM://fallthrough naar PI
        case PI:    
            diffNum(newTree);
            break;
        case ADD: 
        case MIN: //diff van + en - is diff van beide subbomen  
            diffAddSub(newTree);
            break;
        case MUL: 
            diffMul(newTree);
            break;

        case DIV: // (L'*R - R'*L) / R^2 quotientregel
            diffDiv(newTree);
            break;

        case EXP: // x^n, x kan ook functie met x erin zijn (ketting regel)
            diffExp(newTree);
            break;

        case SIN: //cos(L)*L'
            diffSin(newTree);
            break;

        case COS: //sin(L)*L'
            diffCos(newTree);
            break;
        case ERROR:
            std::cout << "dit is absoluut niet de bedoeling maar wel knap" << std::endl;
            break;
        
    }
    return newTree;
}

//Deriveert een variabele
void Token::diffVar(Token* &newTree){
    if (var == 'x'){ //x wordt 1
        newTree->num = 1;
        newTree->type = NUM;
    }
    else{ //elk ander variabele wordt 0
        newTree->num = 0;
        newTree->type = NUM;
    }
}

//Deriveert een nummer
void Token::diffNum(Token* &newTree){
    newTree->type = NUM;
    newTree->num = 0; //losse getal wordt 0
}

//Deriveert een + of -
void Token::diffAddSub(Token* &newTree){
    newTree->type = type; //f + g = f' + g'
    newTree->left = left->diff(); 
    newTree->right = right->diff();
}

//Deriveert een *
//2 opties: kettingregel of productregel
void Token::diffMul(Token* &newTree){
    if (right->type == EXP && left->type == NUM){ // a*(x^n)
        //wanneer er een macht met constante is
        delete newTree;
        newTree = right->diff(); //gedeelte met macht diff 
        newTree->left->num *= left->num;//diff macht * originele num   
    }
    else {
        //left * diff(right) + right * diff(left)
        newTree->type = ADD; //bovenste + maken, takken zijn *
        newTree->left = new Token();
        newTree->right = new Token();
        newTree->left->type = MUL;
        newTree->right->type = MUL;

        newTree->left->left = left->copy();//L*R'
        newTree->left->right = right->diff();
        
        newTree->right->left = right->copy();//R*L'
        newTree->right->right = left->diff();
    }
}
 

//Deriveert exponenten
void Token::diffExp(Token* &newTree){
    //exp = right
    //base = left

    newTree->type = MUL;//bovenste token is *
    if(left->isBinaryOperator() || left->isUnaryOperator()){
        // exp*(base^(exp-1)) * base'
        newTree->left = new Token();
        newTree->left->type = MUL;//links is *
        
        newTree->left->left = right->copy();//exp*(base^(exp-1))

        newTree->left->right = new Token();
        newTree->left->right->type = EXP;
        newTree->left->right->left = left->copy();
        newTree->left->right->right = right->copy();
        newTree->left->right->right->num --;

        newTree->right = left->diff();//base'
    }
    else{//exp*(base^(exp-1)) exponentieelregel
        newTree->left = right->copy();//links is oude exp
        newTree->right = new Token();//rechts is base^(exp-1)
        newTree->right->type = EXP;
        newTree->right->left = left->copy();
        newTree->right->right = right->copy();
        newTree->right->right->num --;//exp - 1
    }
}

//Deriveert een /
void Token::diffDiv(Token* &newTree){
    // (L'*R - R'*L)/(R^2)
    newTree->type = DIV;//bovenste token is /

    newTree->left = new Token();
    newTree->right = new Token();
    newTree->left->type = MIN;//links is - (L'*R - L*R')
    newTree->right->type = EXP;//rechts is ^ (R^2)

    newTree->left->left = new Token();
    newTree->left->right = new Token();
    newTree->left->left->type = MUL;//links van - is *
    newTree->left->right->type = MUL;//rechts van - is *

    newTree->left->left->left = left->diff();//L'*R
    newTree->left->left->right = right->copy();

    newTree->left->right->left = left->copy();//L*R'
    newTree->left->right->right = right->diff();

    newTree->right->left = right->copy();//R^2
    newTree->right->right = new Token();
    newTree->right->right->type = NUM;
    newTree->right->right->num = 2;
}

//Deriveert een sin
void Token::diffSin(Token* &newTree){
    newTree->type = MUL;
    newTree->left = new Token();
    newTree->left->type = COS;//links van mul is cos
    newTree->left->left = left->copy();//cos(L)
    newTree->right = left->diff();//L'
}

//Deriveert een cos
void Token::diffCos(Token* &newTree){
    newTree->type = MUL;
    newTree->left = new Token();
    newTree->left->type = SIN;//rechts van mul is sin
    newTree->left->left = left->copy();//sin(L)
    //omdat cos(x) = -sin(x) moet er een - voor
    newTree->right = new Token();
    newTree->right->type = MUL;//L' * -1
    newTree->right->left = new Token();
    newTree->right->left->type = NUM;
    newTree->right->left->num = -1;
    newTree->right->right = left->diff();
}