/**
* klassenaam: main en menu  
* Luuk Daleman (3323560)
* Mirza Kuraesin (3278395)
* Main
* 4/11/2022
**/

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "../headers/token.h"


//Menu functie met parameter debugMode. Deze functie wordt aangeroepen in main.
//Als debugMode false is krijgt de gebruiker een menu te zien.
//Opties in de menu worden aangeroepen door de eerste argument van de invoer.
void menu(bool debugMode){
    std::stringstream ss;
    std::string arg1;
    std::string invoer;
    Token* object = nullptr;
    if (!debugMode){//men te zien als debugMode false is
        std::cout <<"Menu opties:\n"
        <<"--de laatst ingelezen expressie wordt voor de rest van de opties gebruikt--\n"
        <<"-exp <expressie>\n"
        <<"-print\n"
        <<"-dot\n"
        <<"-simp\n"
        <<"-eval <waarde>\n"
        <<"-diff\n"
        <<"-end\n" 
        <<"uw invoer: "<<std::endl;
    }
    while (getline(std::cin, invoer)){ //leest de invoer van de gebruiker
        ss << invoer;
        ss >> arg1;//leest de eerste argument van de invoer
        if (arg1 == "exp" && !ss.eof()){//maakt nieuw object aan
            if (object != nullptr){ // oude opruimen
                delete object;
                object = nullptr;
            }
            Token* wrtl = new Token(); // Token om een boom aan te maken
            object = wrtl->parser(ss); // maak boom aan
            delete wrtl;
            if ( ! object->checkBoom()){ // check of de invoer legaal is
                delete object;
                object = nullptr;
            }
        }
        //als er geen object/boom is, maar wel een optie wordt ingevoerd
        else if (object == nullptr && (arg1 == "print" || arg1 == "dot" || 
                 arg1 == "simp" || arg1 == "eval" || arg1 == "diff")){
            std::cout <<"voer eerst een expressie in voordat " << arg1 
            << " wordt aangeroepen" <<std::endl;
            ss.str(""); //zorgt dat de stream leeg is en dat arg1 andere value kan krijgen
            ss.clear(); //reset de error state zodat de stream weer gebruikt kan worden
            arg1 = ""; //maak arg1 leeg zodat er geen raar gedrag kan plaatsvinden
            continue;
        }

        else if (arg1 == "print"){//print de expressie
            object->printBoom();
            std::cout << std::endl;
        }
        
        else if (arg1 == "dot"){//print de expressie in dot formaat
            object->writeDotWrap(ss);
        }

        else if (arg1 == "simp"){//vereenvoudig de expressie
            object = object->vereenvoudig();
        }

        else if (arg1 == "eval"){//vul getal in voor x    
            int x;
            if (debugMode || !ss.eof()){ //getal moet na 'eval' worden ingevoerd, bijv "eval 5"
                ss >> x;
            }
            else{ // voor als er geen waarde wordt meegegeven
                std::cout << "voer waarde voor x: ";
                std::cin >> x;
            }
            object = object->eval(x);
            object = object->vereenvoudig();
        }

        else if (arg1 == "diff"){//differentieer de functie
            Token* tmp = object; // om de oude boom zo weg te gooien
            object = object->vereenvoudig();//voor differentieren moet de boom vereenvoudigd zijn
            object = object->diff();
            delete tmp; //gooi de oude boom weg
            // vereenvoudig nog een keer, 
            // zodat er geen gekke lange sommen komen die op 0 uitkomen,
            // maar gewoon 0
            object = object->vereenvoudig();
        }

        else if (arg1 == "end"){//eindigt het programma
            break;
        }

        else if (arg1 != "") {// bij verkeerde invoer
            std::cout << arg1 << std::endl;
            std::cout << "verkeerde invoer" << std::endl;
        }

        //error handling als er niet letterlijk x/0 wordt ingevuld,
        //na bijvoorbeeld een simp of een eval
        if (object != nullptr && object->delenDoor0){ 
            std::cout << "Delen door 0 error" << std::endl;
            delete object;
            object = nullptr;
        }

        ss.str("");//zorgt dat de stream leeg is en dat arg1 andere value kan krijgen
        ss.clear();//reset de error state zodat de stream weer gebruikt kan worden
        arg1 = ""; //maak arg1 leeg zodat er geen raar gedrag plaats vindt
    }
    delete object;//verwijderd het object
}


//main functie
//Als er 'd' als argument is, wordt de menu functie aangeroepen met debugMode true.
int main(int argc, char** argv){
    // deze code is gekopieerd van het document en geeft daarom de warning
    // oplossing is debugMode te initialiseren met false 
    // en alleen te checken of dit true hoeft te worden
    bool debugMode;
    std::string textfile;
    if(argc < 2){
        debugMode = false;
    }
    else{
        if(std::string(argv[1]) == "d"){
            debugMode = true;
        }
        
    }
    menu(debugMode);
    return 0;
}
