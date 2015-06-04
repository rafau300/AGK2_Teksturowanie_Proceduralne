#ifndef PLIK_H
#define PLIK_H

#include "naglowek.h"
#include <fstream>
#include <sstream>


using namespace std;

class Plik
{
    public:
        Plik();
        virtual ~Plik();
        //ifstream getPlik() { return plik; }
        //void setPlik(ifstream val) { plik = val; }
        string Getsciezka() { return sciezka; }
        void setSciezka(string val) { sciezka = val; }
        string getLinia() { return linia; }
        void setLinia(string val) { linia = val; }

        int getLiczbaTekstur() {return liczbaTekstur;}
        void setLiczbaTekstur(int val) {liczbaTekstur = val;}
        int getJakoscTekstur() {return jakoscTekstur;}
        void setJakoscTekstur(int val) {jakoscTekstur = val;}

        int odczytajParametrTekstury(int numerTekstury, int ktoryParametr);
        void odczytajPlik(int numerPliku);
        void odczytajUstawienia();
    protected:
    private:
        ifstream plik;
        string sciezka;
        string linia;
        int liczbaTekstur;
        int jakoscTekstur;

        string wartosci[100][9];
        int parametry[10][9];

        string zamienIntaNaStringa(int wartosc);
        int zamienStringaNaInta(string wartosc);
};

#endif // PLIK_H
