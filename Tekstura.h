#ifndef TEKSTURA_H
#define TEKSTURA_H

#include "naglowek.h"

class Tekstura
{
    public:
        Tekstura();
        Tekstura(unsigned int szerokosc, unsigned int wysokosc, string nazwa);
        virtual ~Tekstura();
        unsigned int getSzerokosc() { return szerokosc; }
        void setSzerokosc(unsigned int val) { szerokosc = val; }
        unsigned int getWysokosc() { return wysokosc; }
        void setWysokosc(unsigned int val) { wysokosc = val; }
        GLubyte *getMapaPikseli() { return mapaPikseli; }
        void setMapaPikseli(GLubyte *val) { mapaPikseli = val; }
        string getNazwa() { return nazwa; }
        void setNazwa(string val) { nazwa = val; }

        void inicjalizujObiektTekstury(unsigned int szer, unsigned int wys, string naz);
        unsigned char * odczytajPiksel(unsigned int x, unsigned int y);
        void zapiszPiksel(unsigned int x, unsigned int y, unsigned char piksel[LICZBA_BAJTOW_NA_PIKSEL]);
        void stworzProceduralnaTeksture(unsigned int typ,
                                        GLubyte kolor1[LICZBA_BAJTOW_NA_PIKSEL],
                                        GLubyte kolor2[LICZBA_BAJTOW_NA_PIKSEL],
                                        unsigned int liczbaPowtorzen);
        void mieszajTekstury(GLubyte *piksele, GLubyte *piksele2);
    protected:
    private:
        unsigned int szerokosc;
        unsigned int wysokosc;
        GLubyte *mapaPikseli;
        string nazwa;

        float *wygenerujMiekkiSzum(float *szumBazowy, int iteracja);
        float interpolacja(float x0, float x1, float alpha);
        void dodajModyfikacjeDoTekstury(int flagaModyfikacji);
};

#endif // TEKSTURA_H
