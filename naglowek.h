#ifndef NAGLOWEK_H
#define NAGLOWEK_H



#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <string>
#include <iostream>
#include <math.h>
#include <sstream>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define GLUT_DISABLE_ATEXIT_HACK

using namespace std;

#define FPS 60  //docelowa ilosc klatek na sekunde

#define LICZBA_TEKSTUR 10
#define WYSOKA_JAKOSC_TEKSTUR 1
#define SREDNIA_JAKOSC_TEKSTUR 2
#define NISKA_JAKOSC_TEKSTUR 4

//numery tekstur dla wiekszej czytelnosci
#define TEX0 0
#define TEX1 1
#define TEX2 2
#define TEX3 3
#define TEX4 4
#define TEX5 5
#define TEX6 6
#define TEX7 7
#define TEX8 8
#define TEX9 9

//standardowa budowa piksela - 24 bity
#define LICZBA_BAJTOW_NA_PIKSEL 3

//Typy tekstur ktore beda generowane
#define TYP_TEKSTURY_JEDEN_KOLOR 1
#define TYP_TEKSTURY_GRADIENT 2
#define TYP_TEKSTURY_SZUM 4
#define TYP_TEKSTURY_SZUM_PERLINA 8
#define TYP_TEKSTURY_SZACHOWNICA 16
//32, 64 i 128 zachowane na ewentualne nowe typy tekstur

//Modyfikacje do tekstur
#define MOD_BLUR 256
#define MOD_WYOSTRZANIE 512
#define MOD_NEGACJA 1024
#define MOD_FILTR_MEDIANOWY 2048
#define MOD_SZUM_SOL_PIEPRZ 4096

//Stala oznaczajaca miesznie tekstur przy odczycie z pliku
#define MIESZANIE 100


//prototypy funkcji, zeby nie trzeba bylo ich sortowac
void wyswietlTekst(float x, float y, void *font, const char *string);
void wyswietlTekst(float x, float y, float z, void *font, const char *string);
void ustawWspolrzedne();
GLubyte *kolor(GLubyte r, GLubyte g, GLubyte b);
void stworzTekstury();
void stworzOteksturowanaSciane(int numerTekstury, int x, int y, int z,
                               int szerokosc, int wysokosc,
                               float obrotX, float obrotY, float obrotZ);
string zamienIntaNaStringa(int wartosc);
void ustawFiltryTekstur();
void inicjalizacja();
void inicjalizacjaOpenGL(int argc, char *argv[]);
void przestawNaObraz2D();
void narysujGornaBelke();
void HUD();
void display();
void klawiatura(unsigned char key, int x, int y);
void klawiszeSpecjalne( int key, int x, int y );
void reshape(int w, int h);
void zegar(int val);
int main(int argc, char *argv[]);

#endif // NAGLOWEK_H
