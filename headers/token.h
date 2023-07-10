/**
* klassenaam: Token class
* Luuk Daleman (3323560)
* Mirza Kuraesin (3278395)
* Token.h
* 4/11/2022
**/


#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

struct Token{
    Token();
    ~Token(); // ruim boom op
    void destroy();
    enum{
        ADD, //0 Binary operator
        MIN, //1 Binary operator
        MUL, //2 Binary operator
        DIV, //3 Binary operator
        EXP, //4 Binary operator
        SIN, //5 Unary operator
        COS, //6 Unary operator
        NUM, //7 Leaf   
        VAR, //8 Leaf
        PI,  //9 leaf
        ERROR
    } type;

    union
    {
        char var;
        double num;
    };

    Token* left = nullptr;//linker kind
    Token* right = nullptr;//rechter kind
    static bool delenDoor0;//delen door 0 niet toegestaan, in main aangeroepen

    // operator overloads
    double operator+(const Token &token) const;
    double operator-(const Token &token) const;
    double operator*(const Token &token) const;
    double operator/(const Token &token) const;
    
    //zodat elke file die overload kan gebruiken
    friend std::ostream& operator<<(std::ostream& s, const Token& token);

    bool isBinaryOperator() const; //checkt of het een binary operator is
    bool isLeaf() const; //var en num (en pi)
    bool isUnaryOperator() const; //sin en cos
    bool isNumb() const; //return true if type is NUM or PI
    bool checkBoom() const; // check of de ingelezen expressie goed geformuleerd is
    Token* parser(std::stringstream& ss); //parsen van de expressie
    void lexer(std::string const invoer); //lexen van de expressie

    Token* copy(); //kopieer subboom
    
    Token* vereenvoudig(); //afkorting = vev
    Token* vevNums();//vereenvoudig operatie met alleen nummers tot antwoord
    Token* vev0();//regelt de speciale eigenschappen van 0
    Token* vev1();//regelt de speciale eigenschappen van 1
    Token* vevWegStrepen();// x-x = 0 en x/x = 1
    bool subBoomGelijk(Token* const Lwrtl, Token* const Rwrtl) const;//voor (a-b)/(a-b) gevallen

    Token* eval(int const x);//vul getal voor x in en evalueert de boom

    Token* diff();//differentieer boom
    void diffVar(Token* &node);//differentieer boom met x
    void diffNum(Token* &node);//differentieer boom met nummers
    void diffAddSub(Token* &node);//differentieer boom met + en -
    void diffExp(Token* &node);//differentieer exponent
    void diffMul(Token* &node);//differentieer boom met *
    void diffDiv(Token* &node);//differentieer boom met /
    void diffCos(Token* &node);//differentieer boom met cos
    void diffSin(Token* &node);//differentieer boom met sin


    void printRichting(Token* const richting) const;//hulpfunctie voor print
    void printBoom() const; //roept pre en in orde aan
    void preOrde() const; //voor niet binaire operatoren
    void inOrde() const; //voor wel binaire operatoren

    void writeDotWrap(std::stringstream& ss);//schrijf boom naar dot formaat
    void writeDot(std::ofstream &output, int &teller, int const wrtl);
};