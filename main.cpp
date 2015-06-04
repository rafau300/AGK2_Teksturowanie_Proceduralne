/**
Politechnika Swietokrzyska w Kielcach
Przedmiot:      Algorytmy Grafiki Komputerowej 2
Projekt:        Teksturowanie proceduralne w OpenGL
Wykonanie:      Rafal Bebenek i Jacek Kominek
Grupa:          2ID22B
Rok akademicki: 2014/2015
**/

#include "naglowek.h"
#include "Tekstura.h"
#include "Plik.h"

float rotS=0, rotX=0,rotY=0,rotZ=0; //rotacja
float przyrostRotY = 1.0;           //przyrost rotacji w osi Y

//Wspolrzedne uzywane do przesuwania kamery w rozne strony
float kameraX;
float kameraY;
float kameraZ;
float przesuniecieX;
float przesuniecieY;
float przesuniecieZ;


//Tworzenie tablicy obiektow przechowujacych tekstury
Tekstura *tekstura;

//Tworzenie obiektu odczytujacego konfiguracje tekstur z pliku tekstowego
Plik plik;

//jakosc tekstur - im lepsza, tym tekstury maja wiekszy rozmiar
int wybranaJakoscTekstur = SREDNIA_JAKOSC_TEKSTUR;
GLuint *tex;//[LICZBA_TEKSTUR];
bool czyZaladowanoTekstury = false;
bool teksturyLadowaneZPliku = false;
int ktoraTeksturaDoZaladowania = 0;
int liczbaTekstur;


//funkcja wyswietlajaca ciag znakow korzystajac z 2 wspolrzednych (XY)
void wyswietlTekst(float x, float y, void *font, const char *string) {
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

//funkcja wyswietlajaca ciag znakow znakow korzystajac z 3 wspolrzednych (XYZ)
void wyswietlTekst(float x, float y, float z, void *font, const char *string) {
    const char *c;
    glRasterPos3f(x, y, z);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}


//funkcja resetujaca/ustawiajca podstawowe wspolrzedne obrotu i przesuniecia
void ustawWspolrzedne() {
    kameraX = 0;
    kameraY = 50;
    kameraZ = -500;
    przesuniecieX = 0;
    przesuniecieY = 50;
    przesuniecieZ = 0;
}

void przestawNaObraz2D() {
    //Wylaczenie glebi
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glMatrixMode(GL_PROJECTION);					// Select Projection
	//glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho( 0, 800 , 600 , 0, -1, 1 );				// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);					// Select Modelview Matrix
	//glPushMatrix();							// Push The Matrix
	glLoadIdentity();
}

void narysujGornaBelke() {
    //Rysowanie HUDa
    glBegin(GL_QUADS);
        glColor3f(0.2,0.2,0.2);
        glVertex2d(0,0);
        glVertex2d(0,30);
        glColor3f(0.7,0.7,0.7);
        glVertex2d(800,30);
        glVertex2d(800,0);
    glEnd();

    float x = 20;
    float y = 20;
    float odlegloscWJednymNapisie = 65;
    float odlegloscPomiedzyNapisami = 200;
    glColor3f(1.0,0.0,0.0);
    wyswietlTekst(x,y,GLUT_BITMAP_TIMES_ROMAN_24,"[ESC]");
    glColor3f(1.0,1.0,1.0);
    wyswietlTekst(x+odlegloscWJednymNapisie,y,GLUT_BITMAP_TIMES_ROMAN_24,"Wyjscie");
    glColor3f(1.0,0.0,0.0);
    wyswietlTekst(x+odlegloscPomiedzyNapisami+odlegloscWJednymNapisie,y,GLUT_BITMAP_TIMES_ROMAN_24,"[1/2]");
    glColor3f(1.0,1.0,1.0);
    wyswietlTekst(x+odlegloscPomiedzyNapisami+odlegloscWJednymNapisie*1.8,y,GLUT_BITMAP_TIMES_ROMAN_24,"Odczytaj dany plik z parametrami tekstur");

}

//Rysowanie 2-wymiarowego obrazu nad obrazem 3D
//dodatkowo ladowanie tekstur i wyswietlanie paska
//postepu w przypadku gdy tekstury nie sa jeszcze zaladowane
void HUD() {
    przestawNaObraz2D();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	narysujGornaBelke();

    ///Tworzenie tekstury o zadanych parametrach
    if (!czyZaladowanoTekstury) {
        //Ladowanie tekstur zapisanych w programie
        if (!teksturyLadowaneZPliku)
        switch (ktoraTeksturaDoZaladowania) {
            case 0:
                tekstura[0].stworzProceduralnaTeksture(TYP_TEKSTURY_JEDEN_KOLOR, kolor(0,128,0), NULL, NULL);
            break;
            case 1:
                tekstura[1].stworzProceduralnaTeksture(TYP_TEKSTURY_GRADIENT|MOD_NEGACJA|MOD_WYOSTRZANIE, kolor(0,0,0), kolor(255,255,50),7);
            break;
            case 2:
                //tekstura[2].stworzProceduralnaTeksture(TYP_TEKSTURY_SZUM|MOD_WYOSTRZANIE, kolor(255,255,0), NULL, NULL);
                tekstura[2].stworzProceduralnaTeksture(TYP_TEKSTURY_SZUM_PERLINA|MOD_SZUM_SOL_PIEPRZ, kolor(255,0,0),kolor(0,0,0),8);
            break;
            case 3:
                tekstura[3].stworzProceduralnaTeksture(TYP_TEKSTURY_SZACHOWNICA|MOD_BLUR|MOD_WYOSTRZANIE, kolor(235,235,164), kolor(64,32,32),8);
            break;
            case 4:
                tekstura[4].stworzProceduralnaTeksture(TYP_TEKSTURY_SZUM_PERLINA|MOD_WYOSTRZANIE, kolor (0,255,0), kolor(0,0,255),8);
            break;
            case 5:
                tekstura[5].mieszajTekstury(tekstura[3].getMapaPikseli(), tekstura[4].getMapaPikseli());
            break;
            case 6:
                tekstura[6].stworzProceduralnaTeksture(TYP_TEKSTURY_SZUM_PERLINA|MOD_SZUM_SOL_PIEPRZ|MOD_FILTR_MEDIANOWY, kolor(255,0,0),kolor(0,0,0),8);
            break;
            case 7:
                tekstura[7].stworzProceduralnaTeksture(TYP_TEKSTURY_SZUM, kolor(255,255,0), kolor(0,0,255), NULL);
            break;
            default:
                Sleep(100);
            }

            //Ladowanie tekstur z pliku tekstowego
            if (teksturyLadowaneZPliku)
                switch (ktoraTeksturaDoZaladowania) {
                case 0: {
                    if (plik.odczytajParametrTekstury(0,0) == MIESZANIE) {
                        tekstura[0].mieszajTekstury(tekstura[plik.odczytajParametrTekstury(0,1)].getMapaPikseli(), tekstura[plik.odczytajParametrTekstury(0,2)].getMapaPikseli());
                    }
                    else {
                    GLubyte *k1 = kolor((GLubyte)plik.odczytajParametrTekstury(0,2),(GLubyte)plik.odczytajParametrTekstury(0,3), (GLubyte)plik.odczytajParametrTekstury(0,4));
                    GLubyte *k2 = kolor((GLubyte)plik.odczytajParametrTekstury(0,5),(GLubyte)plik.odczytajParametrTekstury(0,6), (GLubyte)plik.odczytajParametrTekstury(0,7));
                    tekstura[0].stworzProceduralnaTeksture(plik.odczytajParametrTekstury(0,0)|plik.odczytajParametrTekstury(0,1),k1,k2,plik.odczytajParametrTekstury(0,8));
                    }
                }
                case 1: {
                    if (plik.odczytajParametrTekstury(1,0) == MIESZANIE) {
                        tekstura[1].mieszajTekstury(tekstura[plik.odczytajParametrTekstury(1,1)].getMapaPikseli(), tekstura[plik.odczytajParametrTekstury(1,2)].getMapaPikseli());
                    }
                    else {
                    GLubyte *k1 = kolor((GLubyte)plik.odczytajParametrTekstury(1,2),(GLubyte)plik.odczytajParametrTekstury(1,3), (GLubyte)plik.odczytajParametrTekstury(1,4));
                    GLubyte *k2 = kolor((GLubyte)plik.odczytajParametrTekstury(1,5),(GLubyte)plik.odczytajParametrTekstury(1,6), (GLubyte)plik.odczytajParametrTekstury(1,7));
                    tekstura[1].stworzProceduralnaTeksture(plik.odczytajParametrTekstury(1,0)|plik.odczytajParametrTekstury(1,1),k1,k2,plik.odczytajParametrTekstury(1,8));
                    }
                }
                case 2: {
                    if (plik.odczytajParametrTekstury(2,0) == MIESZANIE) {
                        tekstura[2].mieszajTekstury(tekstura[plik.odczytajParametrTekstury(2,1)].getMapaPikseli(), tekstura[plik.odczytajParametrTekstury(2,2)].getMapaPikseli());
                    }
                    else {
                    GLubyte *k1 = kolor((GLubyte)plik.odczytajParametrTekstury(2,2),(GLubyte)plik.odczytajParametrTekstury(2,3), (GLubyte)plik.odczytajParametrTekstury(2,4));
                    GLubyte *k2 = kolor((GLubyte)plik.odczytajParametrTekstury(2,5),(GLubyte)plik.odczytajParametrTekstury(2,6), (GLubyte)plik.odczytajParametrTekstury(2,7));
                    tekstura[2].stworzProceduralnaTeksture(plik.odczytajParametrTekstury(2,0)|plik.odczytajParametrTekstury(2,1),k1,k2,plik.odczytajParametrTekstury(2,8));
                    }
                }
                break;
                case 3: {
                    if (plik.odczytajParametrTekstury(3,0) == MIESZANIE) {
                        tekstura[3].mieszajTekstury(tekstura[plik.odczytajParametrTekstury(3,1)].getMapaPikseli(), tekstura[plik.odczytajParametrTekstury(3,2)].getMapaPikseli());
                    }
                    else {
                    GLubyte *k1 = kolor((GLubyte)plik.odczytajParametrTekstury(3,2),(GLubyte)plik.odczytajParametrTekstury(3,3), (GLubyte)plik.odczytajParametrTekstury(3,4));
                    GLubyte *k2 = kolor((GLubyte)plik.odczytajParametrTekstury(3,5),(GLubyte)plik.odczytajParametrTekstury(3,6), (GLubyte)plik.odczytajParametrTekstury(3,7));
                    tekstura[3].stworzProceduralnaTeksture(plik.odczytajParametrTekstury(3,0)|plik.odczytajParametrTekstury(3,1),k1,k2,plik.odczytajParametrTekstury(3,8));
                    }
                }
                break;
                case 4: {
                    if (plik.odczytajParametrTekstury(4,0) == MIESZANIE) {
                        tekstura[4].mieszajTekstury(tekstura[plik.odczytajParametrTekstury(4,1)].getMapaPikseli(), tekstura[plik.odczytajParametrTekstury(4,2)].getMapaPikseli());
                    }
                    else {
                    GLubyte *k1 = kolor((GLubyte)plik.odczytajParametrTekstury(4,2),(GLubyte)plik.odczytajParametrTekstury(4,3), (GLubyte)plik.odczytajParametrTekstury(4,4));
                    GLubyte *k2 = kolor((GLubyte)plik.odczytajParametrTekstury(4,5),(GLubyte)plik.odczytajParametrTekstury(4,6), (GLubyte)plik.odczytajParametrTekstury(4,7));
                    tekstura[4].stworzProceduralnaTeksture(plik.odczytajParametrTekstury(4,0)|plik.odczytajParametrTekstury(4,1),k1,k2,plik.odczytajParametrTekstury(4,8));
                    }
                }
                break;
                case 5: {
                    if (plik.odczytajParametrTekstury(5,0) == MIESZANIE) {
                        tekstura[5].mieszajTekstury(tekstura[plik.odczytajParametrTekstury(5,1)].getMapaPikseli(), tekstura[plik.odczytajParametrTekstury(5,2)].getMapaPikseli());
                    }
                    else {
                    GLubyte *k1 = kolor((GLubyte)plik.odczytajParametrTekstury(5,2),(GLubyte)plik.odczytajParametrTekstury(5,3), (GLubyte)plik.odczytajParametrTekstury(5,4));
                    GLubyte *k2 = kolor((GLubyte)plik.odczytajParametrTekstury(5,5),(GLubyte)plik.odczytajParametrTekstury(5,6), (GLubyte)plik.odczytajParametrTekstury(5,7));
                    tekstura[5].stworzProceduralnaTeksture(plik.odczytajParametrTekstury(5,0)|plik.odczytajParametrTekstury(5,1),k1,k2,plik.odczytajParametrTekstury(5,8));
                    }
                }
                break;
                default:
                    Sleep(300);
                break;
                }


            //Ustawienie obrazu jako teksture
            glBindTexture(GL_TEXTURE_2D, tex[ktoraTeksturaDoZaladowania]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tekstura[ktoraTeksturaDoZaladowania].getSzerokosc(),
                         tekstura[ktoraTeksturaDoZaladowania].getWysokosc(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                         tekstura[ktoraTeksturaDoZaladowania].getMapaPikseli());
            ustawFiltryTekstur();

            int lt = liczbaTekstur;

            //Rysowanie paska postepu ladowania tekstur
            glBegin(GL_QUADS);
                glColor3f(0.2,0.2,0.2);
                glVertex2d(0,0);
                glVertex2d(0,600);
                glColor3f(0.7,0.7,0.7);
                glVertex2d(800,600);
                glVertex2d(800,0);

                glColor3f(0.0,1.0,0.0);
                glVertex2f(300,400);
                glColor3f(0.0,0.2,0.0);
                glVertex2f(300,430);
                glVertex2f(320 + float((float(ktoraTeksturaDoZaladowania)/float(lt)) * 200),430);
                glColor3f(0.0,1.0,0.0);
                glVertex2f(320 + float((float(ktoraTeksturaDoZaladowania)/float(lt)) * 200),400);
            glEnd();

            glColor3f(0.0,0.0,0.0);
            wyswietlTekst(289.0,298.0,GLUT_BITMAP_TIMES_ROMAN_24,"Generowanie tekstur");
            wyswietlTekst(289.0,347.0,GLUT_BITMAP_TIMES_ROMAN_24,"Prosze czekac...");
            glColor3f(0.0,1.0,0.0);
            wyswietlTekst(290.0,300.0,GLUT_BITMAP_TIMES_ROMAN_24,"Generowanie tekstur");
            wyswietlTekst(290.0,350.0,GLUT_BITMAP_TIMES_ROMAN_24,"Prosze czekac...");

            ktoraTeksturaDoZaladowania++;

            if (ktoraTeksturaDoZaladowania >= liczbaTekstur) czyZaladowanoTekstury = true;
    }


    //glColor3f(1.0,0.0,0.0);
    //wyswietlTekst(350.0,21.0,GLUT_BITMAP_TIMES_ROMAN_24,"[F1] - pomoc");
}

//Metoda zwracajaca tablice typu GLubyte, ktora jest uzywana w klasie z teksturami
GLubyte *kolor(GLubyte r, GLubyte g, GLubyte b) {
    GLubyte *k = new GLubyte [LICZBA_BAJTOW_NA_PIKSEL];

    k[0] = r;
    k[1] = g;
    k[2] = b;

    return k;
}

//Rysuje we wskazanym miejscu sciane, na ktorej jest nalozona wygenerowana tekstura
void stworzOteksturowanaSciane(int numerTekstury, int x, int y, int z,
                               int szerokosc, int wysokosc,
                               float obrotX, float obrotY, float obrotZ) {

glBindTexture(GL_TEXTURE_2D,tex[numerTekstury]);

glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(obrotX,1,0,0);
    glRotatef(obrotY,0,1,0);
    glRotatef(obrotZ,0,0,1);
    glTranslatef(-x,-y,-z);

    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
        glTexCoord2d(0,0);
        glVertex3d(x,y,z);
        glTexCoord2d(1,0);
        glVertex3d(x+szerokosc,y,z);
        glTexCoord2d(1,1);
        glVertex3d(x+szerokosc,y+wysokosc,z);
        glTexCoord2d(0,1);
        glVertex3d(x,y+szerokosc,z);
    glEnd();
glPopMatrix();

}

//-----------------------------------
// ---funkcja wyswietlajaca obraz---
void display() {
    if (czyZaladowanoTekstury) {

    glEnable(GL_DEPTH_TEST);    /* W³¹czenie Z-Bufora */
    glDepthMask(GL_TRUE);

    // wybór macierzy modelowania
    glMatrixMode( GL_PROJECTION );					// Select Projection
    glLoadIdentity();
	//glPopMatrix();
	gluPerspective(60, (double)800/600, 10.0, 10000.0);
	glMatrixMode( GL_MODELVIEW );					// Select Modelview
	//glPopMatrix();
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glClearColor(0.0, 0.0, 1.0, 1.0);
    gluLookAt(kameraX,kameraY,kameraZ,przesuniecieX,przesuniecieY,przesuniecieZ,0,1,0);


    //Rysowanie osi wspolrzednych
    glPushMatrix();
    glColor3f(1.0,1.0,0.5);
    glRotatef(rotY, 0, 1, 0);
    //glTranslatef(10,0,0);
    //glutSolidCube(10);

    glLineWidth(1);

    //Linie bedace odzwieciedleniem osi wspolrzednych
    glBegin(GL_LINES);
        glColor3f(0,0,1.0);
        glTranslatef(0,0,0);
        glVertex3f(0,0,0);
        glVertex3f(255.0,0,0);

        glColor3f(0,1.0,0);
        glTranslatef(0,0,0);
        glVertex3f(0,0,0);
        glVertex3f(0,255,0);

        glColor3f(1.0,0,0);
        glTranslatef(0,0,0);
        glVertex3f(0,0,0);
        glVertex3f(0,0,255);
    glEnd();

    //wlaczenie tekstur i blendingu
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //Rysowanie scian z teksturami
    glPushMatrix();
        stworzOteksturowanaSciane(TEX0,0,0,0,200,200,0,0,0);
        stworzOteksturowanaSciane(TEX1,200,0,0,200,200,0,-90,0);
        stworzOteksturowanaSciane(TEX2,0,0,0,200,200,0,-90,0);
        stworzOteksturowanaSciane(TEX3,0,0,200,200,200,0,0,0);
        stworzOteksturowanaSciane(TEX4,0,0,200,200,200,90,0,0);
        stworzOteksturowanaSciane(TEX5,200,0,0,200,200,90,0,0);
        stworzOteksturowanaSciane(TEX6,-200,0,0,200,200,90,0,0);
        stworzOteksturowanaSciane(TEX7,0,0,0,200,200,-90,0,0);
    glPopMatrix;

    glDisable(GL_TEXTURE_2D);

	glPopMatrix();

    //wyswietlenie liter opisujacych osie
    glColor3f(0,0,0);
	glRotatef(rotY, 0, 1, 0);
	wyswietlTekst(0,250,0,GLUT_BITMAP_TIMES_ROMAN_24,"Y");
	wyswietlTekst(250,0,0,GLUT_BITMAP_TIMES_ROMAN_24,"X");
	wyswietlTekst(0,0,250,GLUT_BITMAP_TIMES_ROMAN_24,"Z");


	}

	//rysowanie obiektow 2-wymiarowego menu
	HUD();

    glFlush ();
	glutSwapBuffers();
	glutPostRedisplay();
}


// funkcja obslugi klawiatury
void klawiatura(unsigned char key, int x, int y) {
switch (key) {
    case 'q':
        exit(0);
    break;
    case 27:    //ESCAPE
        exit(0);
    break;
    case 'w':
        kameraY += 5;
    break;
    case 's':
        kameraY -= 5;
    break;
    case 'a':
        kameraX -= 5;
    break;
    case 'd':
        kameraX += 5;
    break;
    case '+':
        kameraZ += 5;
    break;
    case '-':
        kameraZ -= 5;
    break;
    case ' ':   //Spacja
        ustawWspolrzedne(); //resetowanie wspolrzednych do wartosci poczatkowej
    break;
    case 'z':
        przyrostRotY -= 0.1;
    break;
    case 'x':
        przyrostRotY += 0.1;
    break;
    case '1':
        plik.odczytajPlik(1);
        czyZaladowanoTekstury = false;
        teksturyLadowaneZPliku = true;
        ktoraTeksturaDoZaladowania = 0;
    break;
    case '2':
        plik.odczytajPlik(2);
        czyZaladowanoTekstury = false;
        teksturyLadowaneZPliku = true;
        ktoraTeksturaDoZaladowania = 0;
    break;
}
}

//funkcja obslugujaca klawisze specjalne (np. strzalki)
void klawiszeSpecjalne( int key, int x, int y ) {
    switch( key ) {
        // kursor w lewo
    case GLUT_KEY_LEFT:
        przesuniecieX +=10;
        break;
        // kursor w górê
    case GLUT_KEY_UP:
        przesuniecieY += 10;
        break;
        // kursor w prawo
    case GLUT_KEY_RIGHT:
        przesuniecieX-= 10;
        break;
        // kursor w dó³
    case GLUT_KEY_DOWN:
        przesuniecieY -= 10;
        break;
    }
}

//funkcja wywolywana przy zmianie rozmiaru okna
void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(60, (float)w/h, 100, -100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

//Cyklicznie wywolywana funkcja, pozwala na prace z podobna szybkoscia na roznych komputerach.
//Domyslnie 60 klatek na sekunde
void zegar(int val) {
    rotY += przyrostRotY;
    glutPostRedisplay();
    glutTimerFunc(1000/FPS, zegar, 0);
}

//Zamiana wartosci typu int na string, zeby mozna bylo wypisywac zmienne
//do stringa podobnie jak w Javie
string zamienIntaNaStringa(int wartosc) {
    ostringstream oss;
    oss << wartosc;
    string przekonwertowany = "";
    przekonwertowany = oss.str();

    return przekonwertowany;
}

//Po prostu ustawienie filtru dla tekstury, bez tego nic sie nie wyswietli
void ustawFiltryTekstur() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

//inicjalizowanie tekstur i ich tworzenie
void inicjalizacja() {
    //Odczytanie ustawien z pliku
    //Jesli odczyt nie powiedzie sie, uzywane sa wartosci domyslne
    plik.odczytajUstawienia();

    if (plik.getLiczbaTekstur())
        liczbaTekstur = plik.getLiczbaTekstur();
    else liczbaTekstur = LICZBA_TEKSTUR;

    tex = new GLuint[liczbaTekstur];
    tekstura = new Tekstura[liczbaTekstur];

    if (plik.getJakoscTekstur()) wybranaJakoscTekstur = plik.getJakoscTekstur();

    //Ustalenie rozmiaru tekstur w zaleznosci od wybranej jakosci
    int rozmiarX, rozmiarY;
    for (int i=0; i<liczbaTekstur; i++) {
        if (wybranaJakoscTekstur & WYSOKA_JAKOSC_TEKSTUR) {
            rozmiarX = 1024;
            rozmiarY = 1024;
        }
        else if (wybranaJakoscTekstur & SREDNIA_JAKOSC_TEKSTUR) {
            rozmiarX = 512;
            rozmiarY = 512;
        }
        else {
            rozmiarX = 128;
            rozmiarY = 128;
        }

        tekstura[i].inicjalizujObiektTekstury(rozmiarX,rozmiarY,"Tekstura" + zamienIntaNaStringa(i));
    }

	//glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(liczbaTekstur, tex);
}

//inicjalizacja ktora musi byc wykonana, by OpenGL mogl normalnie funkcjonowac
void inicjalizacjaOpenGL(int argc, char *argv[]) {
    glutInit(&argc, argv);
    // ustawienie podwójnego  buforowania i kolorów RGB
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Projekt AGK2 - Rafal Bebenek i Jacek Kominek, grupa 1ID22B");
    // ustawienie funkcji odpowiedzialnej za  rysowanie
    glutDisplayFunc(display);
    // ustawienie funkcji wywo³ywanej przy zmianie rozmiaru okna
    glutReshapeFunc(reshape);

    // ustawienie funkcji obs³ugi klawiatury
    glutKeyboardFunc(klawiatura);
    glutSpecialFunc(klawiszeSpecjalne);

    glutTimerFunc(1000/FPS, zegar, 0);
}


//glowna metoda programu, ktora wywoluje inne
int main(int argc, char *argv[]) {
    //Zielony napis (z wykorzystaniem WinApi)
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN);
    cout << "Projekt AGK2 - Teksturowanie proceduralne" << endl
        << "Wykonanie: Rafal Bebenek i Jacek Kominek" << endl
        << "Grupa 1ID22B, rok akademicki 2014/2015" << endl << endl;

    //powrot do bialych liter
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

    srand(time(NULL));

    inicjalizacjaOpenGL(argc, argv);
    ustawWspolrzedne();
    inicjalizacja();
    glutMainLoop();
    return 0;
}

