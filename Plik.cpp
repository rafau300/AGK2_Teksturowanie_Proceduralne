#include "Plik.h"

Plik::Plik() {
    sciezka = "";
}

Plik::~Plik()
{
    //dtor
}

//Zamiana wartosci typu int na string
string Plik::zamienIntaNaStringa(int wartosc) {
    ostringstream oss;
    oss << wartosc;
    string przekonwertowany = "";
    przekonwertowany = oss.str();

    return przekonwertowany;
}

//Zamiana wartosci typu string na int
int Plik::zamienStringaNaInta(string wartosc) {
    int liczba;
    istringstream iss(wartosc);
    iss >> liczba;

    return liczba;
}

//Odczyt konkretnego parametru odczytanego z pliku
int Plik::odczytajParametrTekstury(int numerTekstury, int ktoryParametr) {
    return parametry[numerTekstury][ktoryParametr];
}

//Odczyt danych z pliku
void Plik::odczytajPlik(int numerPliku) {
    //plik.open("" + zamienIntaNaStringa(numerPliku) + ".txt");
    char nazwaPliku[10];
    sprintf(nazwaPliku,"%d.txt",numerPliku);
    cout << nazwaPliku << endl;
    plik.open(nazwaPliku);

    liczbaTekstur = 0;

    //Jezeli udalo sie otworzyc plik...
    if (plik.good()) {
        while (getline(plik, linia)) {
            if (linia[0] == '#') continue; //Pomijanie komentarzy
            cout << linia << endl;
            stringstream ss(linia);
            string parametr;

            int j = 0;
            while(getline(ss, parametr, ',')) {
                wartosci[liczbaTekstur][j] = parametr;
                j++;
            }

            //Jesli ustanowiono mieszanie tekstur
            if (wartosci[liczbaTekstur][0] == "MIESZANIE") {
                parametry[liczbaTekstur][0] = MIESZANIE;
                parametry[liczbaTekstur][1] = zamienStringaNaInta(wartosci[liczbaTekstur][1]);
                parametry[liczbaTekstur][2] = zamienStringaNaInta(wartosci[liczbaTekstur][2]);
            }
            else {
            //odczyt parametrow
            for (int i=0; i<9; i++) {
                if (i==0 || i==1) { //Dwa pierwszy parametry to stale
                    if (wartosci[liczbaTekstur][i] == "TYP_TEKSTURY_JEDEN_KOLOR") parametry[liczbaTekstur][i] = TYP_TEKSTURY_JEDEN_KOLOR;
                    if (wartosci[liczbaTekstur][i] == "TYP_TEKSTURY_GRADIENT") parametry[liczbaTekstur][i] = TYP_TEKSTURY_GRADIENT;
                    if (wartosci[liczbaTekstur][i] == "TYP_TEKSTURY_SZUM") parametry[liczbaTekstur][i] = TYP_TEKSTURY_SZUM;
                    if (wartosci[liczbaTekstur][i] == "TYP_TEKSTURY_SZUM_PERLINA") parametry[liczbaTekstur][i] = TYP_TEKSTURY_SZUM_PERLINA;
                    if (wartosci[liczbaTekstur][i] == "TYP_TEKSTURY_SZACHOWNICA") parametry[liczbaTekstur][i] = TYP_TEKSTURY_SZACHOWNICA;
                    if (wartosci[liczbaTekstur][i] == "MOD_BLUR") parametry[liczbaTekstur][i] = MOD_BLUR;
                    if (wartosci[liczbaTekstur][i] == "MOD_WYOSTRZANIE") parametry[liczbaTekstur][i] = MOD_WYOSTRZANIE;
                    if (wartosci[liczbaTekstur][i] == "MOD_NEGACJA") parametry[liczbaTekstur][i] = MOD_NEGACJA;
                    if (wartosci[liczbaTekstur][i] == "MOD_FILTR_MEDIANOWY") parametry[liczbaTekstur][i] = MOD_FILTR_MEDIANOWY;
                    if (wartosci[liczbaTekstur][i] == "MOD_SZUM_SOL_PIEPRZ") parametry[liczbaTekstur][i] = MOD_SZUM_SOL_PIEPRZ;
                    if (wartosci[liczbaTekstur][i] == "NULL") parametry[liczbaTekstur][i] = NULL;
                }
                else {//Kolejne parametry to liczby, wiec wystarczy zwykla konwersja
                    parametry[liczbaTekstur][i] = zamienStringaNaInta(wartosci[liczbaTekstur][i]);
                }
            }
            }

            liczbaTekstur++;
        }
    }
    //gdy nie udalo sie odczytac pliku nalezy wyswietlic stosowny komunikat
    else {
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED);
        cout << "Blad otwarcia pliku " << nazwaPliku << "!!!" << endl;
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }

    plik.close();
}

//Odczyt ustawien z pliku
void Plik::odczytajUstawienia() {
    plik.open("konfiguracja.txt");

    int ktoraLinia = 0;

    //Jezeli udalo sie otworzyc plik...
    if (plik.good()) {
        while (getline(plik, linia)) {
            if (linia[0] == '#') continue; //Pomijanie komentarzy
            cout << linia << endl;
            stringstream ss(linia);
            string parametr;

            int j = 0;
            while(getline(ss, parametr, ',')) {
                wartosci[ktoraLinia][j] = parametr;
                j++;
            }

            //odczyt parametrow
                //if (i==0 || i==1) { //Dwa pierwszy parametry to stale
                if (wartosci[ktoraLinia][0] == "jakosc") {
                    if (wartosci[ktoraLinia][1] == "NISKA_JAKOSC_TEKSTUR") jakoscTekstur = NISKA_JAKOSC_TEKSTUR;
                    if (wartosci[ktoraLinia][1] == "SREDNIA_JAKOSC_TEKSTUR") jakoscTekstur = SREDNIA_JAKOSC_TEKSTUR;
                    if (wartosci[ktoraLinia][1] == "WYSOKA_JAKOSC_TEKSTUR") jakoscTekstur = WYSOKA_JAKOSC_TEKSTUR;
                }
                if (wartosci[ktoraLinia][0] == "liczba_tekstur") liczbaTekstur = zamienStringaNaInta(wartosci[ktoraLinia][1]);


            ktoraLinia++;
        }
    }
    //gdy nie udalo sie odczytac pliku nalezy wyswietlic stosowny komunikat
    else {
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED);
        cout << "Blad otwarcia pliku konfiguracja.txt!!!" << endl;
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }

    plik.close();
}
