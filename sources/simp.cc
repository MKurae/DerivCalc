/**
* klassenaam: Simplification functions 
* Luuk Daleman (3323560)
* Mirza Kuraesin (3278395)
* simp.cc
* 4/11/2022
**/

#include "../headers/token.h"

//operator overloads
double Token::operator+(const Token &token) const {
    return num + token.num;
}

double Token::operator-(const Token &token) const {
    return num - token.num;
}

double Token::operator*(const Token &token) const {
    return num * token.num;
}

double Token::operator/(const Token &token) const {
    return num / token.num;
}

//Vereenvoudigt de boom recursief
Token* Token::vereenvoudig(){
    //recursie naar bladeren
    if (left != nullptr){
        left = left->vereenvoudig();
    }
    if (right != nullptr){
        right = right->vereenvoudig();
    }
    Token* newTree = this;
    if (isBinaryOperator()){
        if ((left->isNumb() && left->num == 0) || (right->isNumb() &&right->num == 0)){ 
            newTree = vev0(); // speciale gevallen met 0
        }
        else if (left->isNumb() && right->isNumb()){ //als beide operanden een nummer zijn
            newTree = vevNums();
        }
        else if (left->num == 1 || right->num == 1){ // speciale gevallen met 1
            newTree = vev1();
        }
        else if (type == MIN || type == DIV){ //als het een min of een deling is
            newTree = vevWegStrepen();//checken of linker subboom = rechter subboom
        }
    }
    else if (isUnaryOperator()) { //unaire operator
        if (left->isNumb()){
            newTree = vevNums();
        }
    }
    if (newTree != this)
        delete this; // gooi oude boom weg (gebeurt allen als een aanpassing is gedaan)
    return newTree;
}

//Vereenvoudigd de boom doordat de kinderen van een operatie alleen getallen zijn
Token* Token::vevNums(){
    Token* newNode = new Token();
    if (isBinaryOperator()){
        switch (type)
        {
        case ADD:
            newNode->num = *(left) + *(right);
            break;
        case MIN:
            newNode->num = *(left) - *(right);
            break;
        case MUL:
            newNode->num = *(left) * *(right);
            break;
        case DIV:
            newNode->num = *(left) / *(right);
            break;
        case EXP:
            newNode->num = 1; //tot de macht 0
            for (double i = 0; i < right->num; i++){ //tot de macht n
                newNode->num *= left->num;
            }
            break;
        default:
            // switch komt hier niet, tegen compiler warning
            break;
        }
    }
    else{ //sin en cos
        if (type == SIN)
            newNode->num = sin(left->num);
        else
            newNode->num = cos(left->num);
    }
    if (newNode->num == -0)//anders krijg je -0
        newNode->num = 0;
    //afronden als getal +-1e-10 (0,0000000001) van een geheel getal ligt
    else if (std::abs(newNode->num - std::round(newNode->num)) <= 1e-10){ 
        newNode->num = std::round(newNode->num);
    }
    newNode->type = NUM;
    return newNode;
}

//Vereenvoudigd de boom als er een 0 in zit met de speciale eigenschappen van 0
Token* Token::vev0(){
    // E +- 0 = E
    // 0 ·/ E = 0
    // E^0 = 1
    Token* newNode = this;
    switch (type)
    {
    case ADD:
        if (left->isNumb() && left->num == 0){
            newNode = right->copy(); //0 + E  = E
            break;
        }
        newNode = left->copy();//E + 0 = E
        break;
    case MIN:
        //0-1 = -1 en 0-E = -e = -1*E, maar in onze boom is dat tweede niet per se handig of beter
        if (left->isNumb() && left->num == 0){
            if (right->isNumb())
                newNode = vevNums(); //0-1 = -1
            else
                newNode = copy(); //0-E = 0-E
            break;
        }
        newNode = left->copy(); // E-0 = E
        break;
    case DIV: 
        if (right->isNumb() && right->num == 0){ //delen door 0
            delenDoor0 = true; //error doorgeven
            break;
        }//falltrough naar MUL
    case MUL:
        newNode = new Token();
        newNode->num = 0;
        newNode->type = NUM;
        break;
    case EXP:
        newNode = new Token();
        if (right->num == 0)
            newNode->num = 1; //0^0 = 1
        else
            newNode->num = 0; //0^E = 0
        newNode->type = NUM;
        break;
    default:
        // switch komt hier niet, tegen compiler warning
        break;
    }
    return newNode;
}

//Vereenvoudigd de boom als er een 1 in zit met de speciale eigenschappen van 1
Token* Token::vev1(){
    // E · 1 = E, 1 · E = E
    // E / 1 = E
    // E^1 = E
    // 1^E = 1
    switch (type)
    {
    case MUL:
        if (right->type == NUM && right->num == 1)
            return left->copy();//E · 1 = E
        return right->copy();//1 · E = E

    case DIV:
        if (right->type == NUM && right->num == 1)
            return left->copy();//E / 1 = E
        break;

    case EXP:
        if (right->type == NUM && right->num == 1){
            return left->copy();// E^1 = E
        }
        break;

    default:
        // switch komt hier niet, tegen compiler warning
        break;
    }
    return this->copy();
}

//Vereenvoudigd de boom als er een - of / in zit
Token* Token::vevWegStrepen(){
    // x-x = 0 en x/x = 1
    if (left->type != right->type) 
        return this->copy();
    
    if (left->type < NUM) { //<NUM voor operatoren
        if (!subBoomGelijk(left,right)){
            return this->copy();
        }
    }
    else if (left->type == VAR) {//VAR
        if (left->var != right->var){
            return this->copy();
        }
    }
    else if (left->isNumb()){
        if (! left->num == right->num){
            return this->copy();
        }
    }
    Token* newNode = new Token();
    if (type == MIN)//als subbomen  van - gelijk zijn
        newNode->num = 0; // x-x = 0
    else // als subbomen van / gelijk zijn
        newNode->num = 1; // x/x = 1
    newNode->type = NUM;
    return newNode;
}

//Kijkt of de subbomen gelijk zijn, nodig voor operator - en /
//als ze gelijk zijn return true.
bool Token::subBoomGelijk(Token* const Lwrtl, Token* const Rwrtl) const{ 
    if (Lwrtl->type == Rwrtl->type){//als links en rechts gelijk zijn
        if (Lwrtl->isBinaryOperator()){
            //a+b == a+b maar ook a+b == b+a
            return ((subBoomGelijk(Lwrtl->left, Rwrtl->left) && subBoomGelijk(Lwrtl->right,Rwrtl->right)) ||
                    (subBoomGelijk(Lwrtl->left, Rwrtl->right) && subBoomGelijk(Lwrtl->right,Rwrtl->left)));
        }
        if (Lwrtl->isUnaryOperator()){//kijken naar kinderen van het kind van unair operator
            return subBoomGelijk(Lwrtl->left, Rwrtl->left);
        }
        if (Lwrtl->isNumb()){
            return Lwrtl->num == Rwrtl->num;
        }
        else{ //var
            return Lwrtl->var == Rwrtl->var;
        }
    }
    return false;
}