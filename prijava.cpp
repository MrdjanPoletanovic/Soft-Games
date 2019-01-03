#include "prijava.h"
#include <iostream>
#include <string>
#include "provjeraPrijave.h"

bool logIn()
{
    std::cout<<"User name: ";
    std::string name;
    std::getline(std::cin, name);
    std::cout<<"Password: ";
    std::string password;
    char x;
    do
    {
	    x=std::getchar();
	    if(x=='\r') break;                                  //unos lozinke zavrsava pritiskom tipke "Enter"
	    if(x=='\b')                                         //omogucavanje ponistavanje poslednjeg karaktera kad se pritisne "Backspace"
	    {
		    if(password.length()>0)                         //sprecavanje brisanja labele "Password: " pritiskom tipke "Backspace"
		    {
			    password=password.substr(0,password.length()-1);
			    std::cout<<"\b"<<" \b";
		    }
	    }
	    else
	    {
		    std::cout<<"*";
		    password+=x;
	    }
    }
    while(x!='\r');
    if(provjera(name, password)) return true;
    else return false;
}
