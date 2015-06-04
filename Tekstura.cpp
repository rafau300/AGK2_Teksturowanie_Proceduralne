#include "Tekstura.h"

Tekstura::Tekstura(){
    //ctor
    mapaPikseli = NULL;
}

Tekstura::Tekstura(unsigned int szer, unsigned int wys, string naz) {
    szerokosc = szer;
    wysokosc = wys;
    nazwa = naz;
}

Tekstura::~Tekstura(){
    //dtor
}

//Metoda, ktora zapisuje dane do zmiennych tekstury
void Tekstura::inicjalizujObiektTekstury(unsigned int szer, unsigned int wys, string naz) {
    szerokosc = szer;
    wysokosc = wys;
    nazwa = naz;

    mapaPikseli = new GLubyte [szerokosc * wysokosc * LICZBA_BAJTOW_NA_PIKSEL];

    cout << "Stworzono teksture: " + nazwa << " o rozmiarze [" << szerokosc << "," << wysokosc << "]" << endl;
}

//Gdyby istniala koniecznosc recznej modyfikacji kazdego z pikseli
//osobno, to ta metoda daje ta mozliwosc
GLubyte * Tekstura::odczytajPiksel(unsigned int x, unsigned int y) {
    GLubyte * piksel = new GLubyte [LICZBA_BAJTOW_NA_PIKSEL];
    piksel[0] = mapaPikseli[(x * szerokosc * LICZBA_BAJTOW_NA_PIKSEL) + (y * LICZBA_BAJTOW_NA_PIKSEL)];
    piksel[1] = mapaPikseli[(x * szerokosc * LICZBA_BAJTOW_NA_PIKSEL) + (y * LICZBA_BAJTOW_NA_PIKSEL) + 1];
    piksel[2] = mapaPikseli[(x * szerokosc * LICZBA_BAJTOW_NA_PIKSEL) + (y * LICZBA_BAJTOW_NA_PIKSEL) + 2];
    return piksel;
}

//Zapis analogicznie do porzedniej metody
void Tekstura::zapiszPiksel(unsigned int x, unsigned int y, GLubyte piksel[LICZBA_BAJTOW_NA_PIKSEL]) {
    mapaPikseli[(x * szerokosc) + (y * LICZBA_BAJTOW_NA_PIKSEL)] = piksel[0];
    mapaPikseli[(x * szerokosc) + (y * LICZBA_BAJTOW_NA_PIKSEL) + 1] = piksel[1];
    mapaPikseli[(x * szerokosc) + (y * LICZBA_BAJTOW_NA_PIKSEL) + 2] = piksel[2];
}

//Interpolacja liniowa dwóch wartosci
float Tekstura::interpolacja(float x0, float x1, float alpha) {
    return x0 * (1 - alpha) + alpha * x1;
}

//Wygenerowanie "miekkiego" szumu na potrzeby szumu Perlina
float *Tekstura::wygenerujMiekkiSzum(float *szumBazowy, int iteracja) {
    float *miekkiSzum = new float[szerokosc*wysokosc*LICZBA_BAJTOW_NA_PIKSEL];

    int okres = 1<<iteracja; //2^k
    float czestotliwosc = 1.0 / float(okres);

    for (int i=0; i<szerokosc; i++) {
        //calculate the horizontal sampling indices
        int sample_i0 = (i / okres) * okres;
        int sample_i1 = (sample_i0 + okres) % szerokosc; //wrap around
        float horizontal_blend = (i - sample_i0) * czestotliwosc;

        for (int j=0; j<wysokosc; j++) {
            //calculate the vertical sampling indices
            int sample_j0 = (j / okres) * okres;
            int sample_j1 = (sample_j0 + okres) % wysokosc; //wrap around
            float vertical_blend = (j - sample_j0) * czestotliwosc;

             //blend the top two corners
            float p1,p2;
            p1 = szumBazowy[sample_i0*szerokosc*LICZBA_BAJTOW_NA_PIKSEL + sample_j0*LICZBA_BAJTOW_NA_PIKSEL];
            p2 = szumBazowy[sample_i1*szerokosc*LICZBA_BAJTOW_NA_PIKSEL + sample_j0*LICZBA_BAJTOW_NA_PIKSEL];
            float top = interpolacja(p1,p2, horizontal_blend);

            //blend the bottom two corners
            p1 = szumBazowy[sample_i0*szerokosc*LICZBA_BAJTOW_NA_PIKSEL + sample_j1*LICZBA_BAJTOW_NA_PIKSEL];
            p2 = szumBazowy[sample_i1*szerokosc*LICZBA_BAJTOW_NA_PIKSEL + sample_j1*LICZBA_BAJTOW_NA_PIKSEL];
            float bottom = interpolacja(p1,p2, horizontal_blend);

            //final blend
            miekkiSzum[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = interpolacja(top, bottom, vertical_blend);
            miekkiSzum[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = interpolacja(top, bottom, vertical_blend);
            miekkiSzum[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = interpolacja(top, bottom, vertical_blend);
        }
    }
    return miekkiSzum;
}

//Najwazniejsza metoda w klasie
//Tworzy teksture na podstawie podanych parametrow
void Tekstura::stworzProceduralnaTeksture(unsigned int typ,
                                        GLubyte kolor1[LICZBA_BAJTOW_NA_PIKSEL],
                                        GLubyte kolor2[LICZBA_BAJTOW_NA_PIKSEL],
                                        unsigned int liczbaPowtorzen) {
    //Najprostszy typ tekstury - jeden kolor
    if (typ & TYP_TEKSTURY_JEDEN_KOLOR) {
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = kolor1[0];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = kolor1[1];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = kolor1[2];
            }
        }
    }

    //Gradient wykorzystujacy interpolacje liniowa
    if (typ & TYP_TEKSTURY_GRADIENT) {
        GLfloat przyrostR,przyrostG,przyrostB;
        GLfloat funkcja[szerokosc][3];

        //o ile wzrasta wartosc koloru po przejsciu o 1 piksel
        przyrostR = float(float(kolor2[0] - kolor1[0])/szerokosc);
        przyrostG = float(float(kolor2[1] - kolor1[1])/szerokosc);
        przyrostB = float(float(kolor2[2] - kolor1[2])/szerokosc);

        //Zapis wartosci funkcji liniowej do tablicy
        for (int j=0; j<liczbaPowtorzen; j++)
        for (int i=0; i<szerokosc/liczbaPowtorzen; i++) {
            if (j%2 == 0) {
                funkcja[(j*szerokosc/liczbaPowtorzen)+i][0] = float(kolor1[0]) + (przyrostR * i * liczbaPowtorzen);
                funkcja[(j*szerokosc/liczbaPowtorzen)+i][1] = float(kolor1[1]) + (przyrostG * i * liczbaPowtorzen);
                funkcja[(j*szerokosc/liczbaPowtorzen)+i][2] = float(kolor1[2]) + (przyrostB * i * liczbaPowtorzen);
            }
            else if (j%2 == 1){
                funkcja[(j*szerokosc/liczbaPowtorzen)+i][0] = float(kolor2[0]) - (przyrostR * i * liczbaPowtorzen);
                funkcja[(j*szerokosc/liczbaPowtorzen)+i][1] = float(kolor2[1]) - (przyrostG * i * liczbaPowtorzen);
                funkcja[(j*szerokosc/liczbaPowtorzen)+i][2] = float(kolor2[2]) - (przyrostB * i * liczbaPowtorzen);
            }
        }

        //Zapis pikseli
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = GLubyte(funkcja[i][0]);
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = GLubyte(funkcja[i][1]);
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = GLubyte(funkcja[i][2]);
            }
        }
    }

    //losowy szum
    if (typ & TYP_TEKSTURY_SZUM) {
        srand(time(NULL));
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                GLubyte random = rand()%255;
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = (kolor1[0] + random)/2;
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = (kolor1[1] + random)/2;
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = (kolor1[2] + random)/2;

                if (kolor2 != NULL && rand()%2 != 0) {
                    mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = (kolor2[0] + random)/2;
                    mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = (kolor2[1] + random)/2;
                    mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = (kolor2[2] + random)/2;
                }
            }
        }
    }

    //szachownica
    if (typ & TYP_TEKSTURY_SZACHOWNICA) {
        for (int i=0; i<liczbaPowtorzen; i++) {
            for (int j=0; j<liczbaPowtorzen; j++) {
                for (int k=0; k<szerokosc/liczbaPowtorzen; k++) {
                    for (int l=0; l<wysokosc/liczbaPowtorzen; l++) {
                        if ((i+j)%2 == 0) {
                            mapaPikseli[(j*szerokosc*szerokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (i*wysokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (szerokosc*k*LICZBA_BAJTOW_NA_PIKSEL) + (l*LICZBA_BAJTOW_NA_PIKSEL)] = kolor1[0];
                            mapaPikseli[(j*szerokosc*szerokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (i*wysokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (szerokosc*k*LICZBA_BAJTOW_NA_PIKSEL) + (l*LICZBA_BAJTOW_NA_PIKSEL) + 1] = kolor1[1];
                            mapaPikseli[(j*szerokosc*szerokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (i*wysokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (szerokosc*k*LICZBA_BAJTOW_NA_PIKSEL) + (l*LICZBA_BAJTOW_NA_PIKSEL) + 2] = kolor1[2];
                        }
                        if ((i+j)%2) {
                            mapaPikseli[(j*szerokosc*szerokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (i*wysokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (szerokosc*k*LICZBA_BAJTOW_NA_PIKSEL) + (l*LICZBA_BAJTOW_NA_PIKSEL)] = kolor2[0];
                            mapaPikseli[(j*szerokosc*szerokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (i*wysokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (szerokosc*k*LICZBA_BAJTOW_NA_PIKSEL) + (l*LICZBA_BAJTOW_NA_PIKSEL) + 1] = kolor2[1];
                            mapaPikseli[(j*szerokosc*szerokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (i*wysokosc/liczbaPowtorzen*LICZBA_BAJTOW_NA_PIKSEL) + (szerokosc*k*LICZBA_BAJTOW_NA_PIKSEL) + (l*LICZBA_BAJTOW_NA_PIKSEL) + 2] = kolor2[2];
                        }
                    }
                }
            }
        }
    }

    //Szum Perlina
    if (typ & TYP_TEKSTURY_SZUM_PERLINA) {
        srand(time(NULL));
        float *szumBazowy = new float[szerokosc * wysokosc * LICZBA_BAJTOW_NA_PIKSEL];

        //Na poczatku jest losowany pseudolosowy szum
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                int random = rand()%255;
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = float(random)/255.0;
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = float(random)/255.0;
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = float(random)/255.0;
            }
        }

        float **szumMiekki = new float*[liczbaPowtorzen];
        for (int i=0; i<liczbaPowtorzen; i++) {
            szumMiekki[i] = new float [szerokosc*wysokosc*LICZBA_BAJTOW_NA_PIKSEL];
        }

        for (int i=0; i<liczbaPowtorzen; i++) {
            szumMiekki[i] = wygenerujMiekkiSzum(szumBazowy,i);
        }

        float persistance = 0.7f;
        float amplitude = 1.0f;
        float totalAmplitude = 0.0f;

        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = 0;
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = 0;
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = 0;
            }
        }

        //Interpolacja wszystkich tablic z szumem
        for (int k=0; k<liczbaPowtorzen; k++) {
            amplitude *= persistance;
            totalAmplitude += amplitude;
            for (int i=0; i<szerokosc; i++) {
                for (int j=0; j<wysokosc; j++) {
                    szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] += (szumMiekki[liczbaPowtorzen-1-k][(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] * amplitude);
                    szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] += (szumMiekki[liczbaPowtorzen-1-k][(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] * amplitude);
                    szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] += (szumMiekki[liczbaPowtorzen-1-k][(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] * amplitude);
                }
            }
        }

        //normalizacja
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] /= totalAmplitude;
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] /= totalAmplitude;
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] /= totalAmplitude;
            }
        }

        //zamiana koloru uzytkownika na liczbe float
        float r1,r2,g1,g2,b1,b2;
        r1 = (float)kolor1[0] / 255.0;
        r2 = (float)kolor2[0] / 255.0;
        g1 = (float)kolor1[1] / 255.0;
        g2 = (float)kolor2[1] / 255.0;
        b1 = (float)kolor1[2] / 255.0;
        b2 = (float)kolor2[2] / 255.0;


        //kolorowanie wygenerowanej tekstury
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                if (szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] > 0.5) {
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] =
                    (szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] * r1);
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] =
                    (szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] * g1);
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] =
                    (szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] * b1);
                }
                else {
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] =
                    (szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] * r2);
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] =
                    (szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] * g2);
                szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] =
                    (szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] * b2);
                }
            }
        }

        //zamiana z tabic float [0..1] na tablice GLubyte [0..255]
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] * 255.0;
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] * 255.0;
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = szumBazowy[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] * 255.0;
            }
        }
    }

    //Przekazanie flagi do metody dodajacej modyfikacje
    if (typ & MOD_SZUM_SOL_PIEPRZ) dodajModyfikacjeDoTekstury(MOD_SZUM_SOL_PIEPRZ);
    if (typ & MOD_BLUR) dodajModyfikacjeDoTekstury(MOD_BLUR);
    if (typ & MOD_WYOSTRZANIE) dodajModyfikacjeDoTekstury(MOD_WYOSTRZANIE);
    if (typ & MOD_NEGACJA) dodajModyfikacjeDoTekstury(MOD_NEGACJA);
    if (typ & MOD_FILTR_MEDIANOWY) dodajModyfikacjeDoTekstury(MOD_FILTR_MEDIANOWY);
}

//Dodanie odpowiedniej modyfikacji do tekstury
void Tekstura::dodajModyfikacjeDoTekstury(int flagaModyfikacji) {

        //Dodawanie losowego szumu typu "sol i pieprz"
    if (flagaModyfikacji & MOD_SZUM_SOL_PIEPRZ) {
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                int czyDodacDoTegoPikselaSzum = rand()%10;
                if (!czyDodacDoTegoPikselaSzum) {
                    GLubyte wartosc = rand()%2;
                    if (wartosc) wartosc = 255;
                    mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = wartosc;
                    mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = wartosc;
                    mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = wartosc;
                }
            }
        }
    }

    //Wygladzanie filtrem dolnoprzepustowym
    if (flagaModyfikacji & MOD_BLUR) {

        GLubyte *bitmapaPomocnicza = mapaPikseli;

        int filtrSplotowy[7][7];
        float sumaWag = 0;
        for (int i=0; i<7; i++) {
            for (int j=0; j<7; j++) {
                filtrSplotowy[i][j] = 1;
                sumaWag += (float)filtrSplotowy[i][j];
            }
        }

        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                float r = 0;
                float g = 0;
                float b = 0;
                for (int k=-3; k<4; k++) {
                    for (int l=-3; l<4; l++) {

                        if ((i+k < 0) || (i+k > szerokosc-1) || (j+l < 0) || (j+l > wysokosc-1))
                            break;

                        r += (float)mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL)] * filtrSplotowy[k+3][l+3];
                        g += (float)mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL) + 1] * filtrSplotowy[k+3][l+3];
                        b += (float)mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL) + 2] * filtrSplotowy[k+3][l+3];
                    }
                }

            r /= sumaWag;
            g /= sumaWag;
            b /= sumaWag;

            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = (GLubyte)r;
            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] = (GLubyte)g;
            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] = (GLubyte)b;

            }
        }

        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2];
            }
        }
    }

    ///Wyostrzanie filtrem gornoprzepustowym
    if (flagaModyfikacji & MOD_WYOSTRZANIE) {
        GLubyte *bitmapaPomocnicza = mapaPikseli;

        float filtrSplotowy[7][7];
        float sumaWag = 0;
        for (int i=0; i<7; i++) {
            for (int j=0; j<7; j++) {
                filtrSplotowy[i][j] = -1;
                if (i==3 && j==3)
                    filtrSplotowy[i][j] = 20;
                sumaWag += filtrSplotowy[i][j];
            }
        }

        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                float r = 0;
                float g = 0;
                float b = 0;
                for (int k=-3; k<4; k++) {
                    for (int l=-3; l<4; l++) {

                        if ((i+k < 0) || (i+k > szerokosc-1) || (j+l < 0) || (j+l > wysokosc-1))
                            break;

                        r += (float)mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL)] * filtrSplotowy[k+3][l+3];
                        g += (float)mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL) + 1] * filtrSplotowy[k+3][l+3];
                        b += (float)mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL) + 2] * filtrSplotowy[k+3][l+3];
                    }
                }

            r /= sumaWag;
            g /= sumaWag;
            b /= sumaWag;

            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = (GLubyte)r;
            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] = (GLubyte)g;
            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] = (GLubyte)b;

            }
        }

        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2];
            }
        }
    }

    //Odwrotnosc kazego z kolorow
    if (flagaModyfikacji & MOD_NEGACJA) {
        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = 255 - mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] = 255 - mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] = 255 - mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2];
            }
        }
    }

    //Wygladzanie za pomoca filtru medianowego
    if (flagaModyfikacji & MOD_FILTR_MEDIANOWY) {
            //___________________________________________________________________________________________________//
        GLubyte *bitmapaPomocnicza = mapaPikseli;

        int rozmiarMacierzy = 7;

        int wartosci[rozmiarMacierzy][rozmiarMacierzy][3];
        int wartoscSrodkowa[3];
        int posortowaneWartosci[rozmiarMacierzy*rozmiarMacierzy][3];

        for (int i=0; i<rozmiarMacierzy; i++) {
            for (int j=0; j<rozmiarMacierzy; j++) {
                for (int k=0; k<3; k++) {
                    wartosci[i][j][k] = -1;
                }
            }
        }

        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                for (int k=0-(rozmiarMacierzy/2); k<(rozmiarMacierzy/2 + 1); k++) {
                    for (int l=0-(rozmiarMacierzy/2); l<(rozmiarMacierzy/2 + 1); l++) {

                        if ((i+k < 0) || (i+k > szerokosc-1) || (j+l < 0) || (j+l > wysokosc-1))
                            break;

                        wartosci[k+3][l+3][0] = mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL)];
                        wartosci[k+3][l+3][1] = mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL) + 1];
                        wartosci[k+3][l+3][2] = mapaPikseli[((j+l)*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + ((i+k)*LICZBA_BAJTOW_NA_PIKSEL) + 2];
                    }
                }

                for (int k=0; k<rozmiarMacierzy; k++) {
                    for (int l=0; l<rozmiarMacierzy; l++) {
                        for (int m=0; m<3; m++) {
                            posortowaneWartosci[k*rozmiarMacierzy + l][m] = wartosci[k][l][m];
                        }
                    }
                }

               ///trzeba uzyc algorytmu Hoare'a zamiast zwyklego sortowania

                int sumaSkladowych1 = 0;
                int sumaSkladowych2 = 0;


                for (int k=0; k<rozmiarMacierzy*rozmiarMacierzy-1; k++) {
                    for (int l=0; l<rozmiarMacierzy*rozmiarMacierzy-1-k; l++) {
                        sumaSkladowych1 = 0;
                        sumaSkladowych2 = 0;

                        for (int m=0; m<3; m++) {

                            sumaSkladowych1 += posortowaneWartosci[l][m];
                            sumaSkladowych2 += posortowaneWartosci[l+1][m];

                            if (m==2 && (sumaSkladowych1 > sumaSkladowych2)) {
                                //cout << "zamiana elementow: " << l << "[" << m << "] i " << l+1 << endl;
                                for (int n=0; n<3; n++) {
                                    int zmiennaPomocnicza = posortowaneWartosci[l+1][n];
                                    posortowaneWartosci[l+1][n] = posortowaneWartosci[l][n];
                                    posortowaneWartosci[l][n] = zmiennaPomocnicza;
                                }
                        }
                    }
                }
                }

                for (int m=0; m<3; m++) {
                    wartoscSrodkowa[m] = posortowaneWartosci[49/2][m];
                }

            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = wartoscSrodkowa[0];
            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] = wartoscSrodkowa[1];
            bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] = wartoscSrodkowa[2];

            }
        }

        for (int i=0; i<szerokosc; i++) {
            for (int j=0; j<wysokosc; j++) {
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+1];
                mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2] = bitmapaPomocnicza[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)+2];
            }
        }
    }
}


//Mieszanie dwoch tekstur podanych jako parametr
void Tekstura::mieszajTekstury(GLubyte *piksele, GLubyte *piksele2) {
    for (int i=0; i<szerokosc; i++) {
        for (int j=0; j<wysokosc; j++) {
            mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] =
                (piksele[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)] *
                 piksele2[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL)]) / 255;
            mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] =
                (piksele[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1] *
                 piksele2[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 1]) / 255;
            mapaPikseli[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] =
                (piksele[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2] *
                 piksele2[(j*szerokosc*LICZBA_BAJTOW_NA_PIKSEL) + (i*LICZBA_BAJTOW_NA_PIKSEL) + 2]) / 255;
        }
    }
}
