/**
* klassenaam: Token class  
* Luuk Daleman (3323560)
* Mirza Kuraesin (3278395)
* Token.cc
* 4/11/2022
**/

#include "../headers/token.h"

//constructor
Token::Token(){
    var = '0'; //tegen gekke error in valgrind
    num = 0;
}

bool Token::delenDoor0 = false; //initialiseer de error flag

//destructor
Token::~Token(){ // delete ook de kinderen zo nodig
    if (left != nullptr)
        delete left;
    if (right != nullptr)
        delete right;
}


//Krijgt de token binnen en returnt de print notatie van de token
std::ostream &operator<<(std::ostream &s, const Token &token) const {
    if (token.type == Token::ADD)
        s << "+";
    else if (token.type == Token::MIN)
        s << "-";
    else if (token.type == Token::MUL)
        s << "*";
    else if (token.type == Token::DIV)
        s << "/";
    else if (token.type == Token::EXP)
        s << "^";
    else if (token.type == Token::SIN)
        s << "sin";
    else if (token.type == Token::COS)
        s << "cos";
    else if (token.type == Token::NUM){
        if (std::fmod(token.num, M_PI) == 0 && token.num != 0){
            s << "\u03C0";//wanneer num pi is, print pi
        }
        else{
            s << token.num; //naar union verwijzen
        }
    }
    else if (token.type == Token::VAR)
        s << token.var; //naar union verwijzen
    else if (token.type == Token::PI)
        s << "\u03C0"; //pi (unicode)
    return s;
}

//Checkt of het een binary operator is.
bool Token::isBinaryOperator() const {
   return (type < SIN);
}

//Checkt of het een leaf is
bool Token::isLeaf() const{
    return (type >= NUM); // == num || var || pi
}

//Checkt of het een unary operator is
bool Token::isUnaryOperator() const {
    return (type == SIN || type == COS);
}

//Checkt of het een getal is
bool Token::isNumb() const {
    return (type == NUM || type == PI);
}

//Checkt wat voor type de invoer is en returned een token met het type.
void Token::lexer(std::string const invoer){
    type = ERROR;
    if (invoer.length() == 1){//checkt of het 1 char groot is
        if(invoer ==  "+" )
            type = ADD;
        else if(invoer ==  "-" )
            type = MIN;
        else if(invoer ==  "*" )
            type = MUL;
        else if(invoer ==  "/" )
            type = DIV;
        else if(invoer ==  "^" )
            type = EXP;
    }
    else {
        if(invoer == "cos")
            type = COS;
        else if(invoer == "sin")
            type = SIN;
        else if(invoer == "pi" || invoer == "\u03C0") {
            type = PI;
            num = M_PI;//pi getal
        }
    }
    if (type == ERROR){
        int digits = 0;
        for (size_t i = 0; i < invoer.length(); i++){ //checkt of het een geldig nummer of var is
            if (isdigit(invoer[i]) || invoer[i] == '-' || invoer[i] == '.')
                digits++;
            else
                digits--;
        }
        if (digits == static_cast<int>(invoer.length())){//als invoer alleen nummers bevat
            num = std::stod(invoer); //stod = string to double
            type = NUM;
        }
        if (digits == -static_cast<int>(invoer.length())){//als invoer alleen letters bevat
            var = invoer[0];//var is een char dus invoer[0]
            type = VAR;
        }
        else{
            var = '?'; //type blijft error
        }
    }    
}


//Parser functie 
//Pakt de invoer 1 voor 1 en stuurt het naar de lexer om te checken wat voor type het is.
//Als het een leaf is wordt het gelijk terug gegeven.
//Als het een binary operator is wordt er een nieuwe token aangemaakt en de linker en rechterkant van de operator worden aangemaakt.
//Als het een unary operator is wordt er een nieuwe token aangemaakt en de linker kant van de operator wordt aangemaakt.
//De linker en rechterkant worden gevuld door de parser recursief aan te roepen.
Token* Token::parser(std::stringstream& ss){ 
    if (left != nullptr){ 
        delete left;
        left = nullptr;
    }
    if (right != nullptr){
        delete right;
        right = nullptr;
    }

    std::string str;
    Token* token = new Token();
    ss >> str;
    token->lexer(str); //checkt wat voor type het is
    if (token->isBinaryOperator()){
        if (!ss.eof()) // de incomplete binaire operator wordt later als fout gemeld
            token->left = parser(ss);
        if (!ss.eof()) // de incomplete binaire operator wordt later als fout gemeld
            token->right = parser(ss);
    }
    else if (token->isUnaryOperator()){
        if (!ss.eof()) // de incomplete unaire operator wordt later als fout gemeld
            token->left = parser(ss);
    }
    return token;
}

//return true als de ingelezen functie klopt, false zo niet
bool Token::checkBoom() const{
    if (isBinaryOperator() && (left == nullptr || right == nullptr)){//geen linker of rechter kind
        std::cout << "Binaire operator " << *this << " heeft niet twee operanden" <<std::endl;
        return false;
    }
    else if (type == DIV && right->type == NUM && right->num == 0){//deling door 0
        std::cout << "Delen door 0 error" << std::endl;
        return false;
    }
    else if (!isLeaf() && (left == nullptr)){//geen linker kind bij sin of cos
        std::cout << "Unare operator heeft geen operand" << std::endl;
        return false;
    }
    else if (type == ERROR){
        std::cout << "Invoer is onjuist, variable illegaal geformuleerd" << std::endl;
        return false;
    }
    bool ret = true;
    if (left != nullptr && !left->checkBoom())
        ret = false;
    if (right != nullptr && !right->checkBoom())
        ret = false;
    return ret;
}

//Print richting met haakjes recursief
//Als het geen leaf is worden er haakjes bijgevoegd.
//Wordt aangeroepen door inOrde en preOrde
void Token::printRichting(Token* const richting) const{
    if (richting != nullptr){
         if (!(richting->isLeaf() || richting->isUnaryOperator())){ //als het geen leaf of unaire operator is
            std::cout <<  "("; //print haakjes bij geen leaf en geen unaire operator
            richting->printBoom(); 
            std::cout <<  ")";
        }
        else{
            richting->printBoom();
        }
    }
}

//Print de boom in infix, bij Binary operators
void Token::inOrde() const{
    printRichting(left); 
    std::cout << *this; //print de operator
    printRichting(right);
}


//Print de boom in prefix, bij Unaire operators
void Token::preOrde() const{
    bool binary = left->isBinaryOperator();
    std::cout << *this; //print de operator
    if (!binary) std::cout << "("; //als binary operator onder sin of cos geen haakje extra
    printRichting(left);
    printRichting(right);
    if (!binary) std::cout << ")";
}

//Print de boom in prefix, bij Unaire operators
//Print de boom in infix, bij Binary operators
//Als het een leaf is wordt het gelijk geprint
void Token::printBoom() const{
    if(isLeaf()){
        std::cout << *this;
        return;
    }
    if(isBinaryOperator()){
        inOrde();
    }
    else{
        preOrde();
    }
}

//Kopieert de boom recursief
Token* Token::copy(){
    Token *copy = new Token();
    copy->type = type; //kopieer data
    copy->num = num;
    copy->var = var;
    if (left != nullptr){ //kopieer kinderen
        copy->left = left->copy();
    }
    if (right != nullptr){
        copy->right = right->copy();
    }
    return copy;
}

//Zet de dot notatie in een .dot file
void Token::writeDotWrap(std::stringstream& ss){
    std::ofstream output;
    int teller = 0;
    std::string fileNaam;
    ss >> fileNaam;
    if (fileNaam == "")
        output.open("dotFile.dot"); //standaard filenaam
    else
        output.open(fileNaam); //costum filenaam
    output << "digraph G {" << std::endl;
    writeDot(output, teller, 0); //Recursief wordt de dot notatie in de .gv file gezet
    output << "}";
    output.close();
}

//Creeert de dot notatie recursief
//De teller dat meegegeven werd wordt gebruikt om de nodes te nummeren
//De wrtl is de parent van de node
void Token::writeDot(std::ofstream &output, int &teller, int const wrtl){
    teller++;
    int thisTeller = teller;
    
    output << "\t" << thisTeller << "[label=\"" << *this << "\"]" << std::endl;
    if (wrtl != 0){ //als het wek een parent heeft
        output << "\t" << wrtl << " -> " << thisTeller << std::endl;
    } 
    if (left != nullptr){ //recursief de linker kant af gaan, wrtl is wordt huidige node 
        left->writeDot(output, teller, thisTeller);
    }
    if (right != nullptr){
        right->writeDot(output, teller, thisTeller);
    }
}

//vervang gegeven var met gegeven num
//recursief de boom aflopen
Token* Token::eval(int const x){
    // recursief naar bladeren
    if (left != nullptr){
        left = left->eval(x);
    }
    if (right != nullptr){
        right = right->eval(x);
    }
    if (isLeaf()){//alleen x vervangen
        if (type == VAR && var == 'x'){
            num = x; //gegeven getal
            type = NUM;
        }
    }
    return this;
}