#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "vector.h"
#include "spaceship.h"

//granice za Z clipping ravan 
#define Z_NEAR 1
#define Z_FAR 3000
#define TIMER_INTERVAL 1000/60 //~60fps

static void on_display();
static void on_keypress(unsigned char key, int x, int y);
static void on_release(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int id);

static void countdown(int id);

void moveCamera();

int score = 0; 		//Trenutni poeni
int secLeft = 10;	//Preostalo vreme da se prodje kroz sledeci krug
int flag_GO = 0;	// flag koji oznacava da li je igra pocela, ako igrac pritisne bil koji taster na tastaturi igra krece
int gameNotOver = 1;// flag koji oznacava da igrac nije izgubio igru

void gameOver();

const double deltaSpeed = 0.03; // Promena brzine kretanja
const double angleStep = 0.02; // Pomeraj x i y kordinata vektora kretanja

int flag_i = 0,flag_k = 0,flag_w = 0,flag_a = 0,flag_s = 0,flag_d = 0; // flagovi koji oznacavaju da li je pritisnuto odgovarajuce dugme

void initCircles();
void drawCircles();
vector circles[10]; //niz vektora koji su koordinate centra prstenova kroz koje se prolazi
vector colors[10]; //niz vektora koji predstavljaju RGB vrednosti boje odgovarajucih prstena

vector shipPos = {0,-5,680}; //Vektor pozicije broda, sadrzi koord. centra broda
vector shipForward = {0,0,-1};//Vektor smera broda
double speed = 0; //"brzina" broda

vector eye = {0,0,700}; //Pozicija kamere
vector look = {0,0,0}; //Tacka usmerenja kamere
vector up = {0,1,0}; //Up vector kamere

void checkPass();
void createNewCircle();

void setUpLight();

void keypressProcessing();

void drawBoundaries();

void drawHud();

// Funkcija koja iscrtava pojedinacni prsten pos je vektor koji predstavlja centar prstena a rgb vrednosti boje
void DrawRing(vector pos,vector rgb){

	// Koeficijenti ambijentalne refleksije materijala.
    GLfloat ambient_coeffs[] = { rgb.x,rgb.y,rgb.z, 1 };

    // Koeficijenti difuzne refleksije materijala.
    GLfloat diffuse_coeffs[] = { rgb.x,rgb.y,rgb.z, 1 };

    // Koeficijenti spekularne refleksije materijala.
    GLfloat specular_coeffs[] = { rgb.x,rgb.y,rgb.z, 1 };

    // Koeficijent glatkosti materijala.
    GLfloat shininess = 20;


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		//transliramo na odgovarajucu poziciju
		glTranslatef(pos.x, pos.y, pos.z);
		//podesavamo koeficijente materijala prstena
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    	//postavljamo vrednosti boje za pojedinacni prsten
		glColor3f(rgb.x,rgb.y,rgb.z);
		//Crtamo prsten kao solid torus
		glutSolidTorus(5,40,50,50);
	glPopMatrix();
}

int main(int argc, char** argv){
	//seedujemo rand fju sa vremenom
	srand(time(NULL));
	
	//Inicializujemo GLUT biblioteku i display mode na rgb sa proverom za dubinu i duplim baferom
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	//Pravimo prozor
	glutInitWindowPosition(500,100);
	glutInitWindowSize(1920,1080);
	glutCreateWindow("Space Racer!");
	//Inicializacija callback f-ja
	glutDisplayFunc(on_display);
	glutKeyboardFunc(on_keypress);
	glutKeyboardUpFunc(on_release);
	glutReshapeFunc(on_reshape);
	glutTimerFunc(TIMER_INTERVAL,on_timer,0);
	glutTimerFunc(1000,countdown,1);
	//Postavljamo pozadinsku boju ukljucujemo testiranje dubine i ulazimo u main glut loop
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	//postavljamo flag da se ne registruje ponovno pritiskanje tastera dok se drzi pritisnut
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	
	//vrsi se inicializacija pozicije i boja prstenova
	initCircles();
	glutMainLoop();

	return 0;
}

void on_display(){
	//Cistimo bafere
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//podesava se osvetljenje
	setUpLight();
	//iscrtava se interfejs
	drawHud();
	//prekidamo igru ukoliko je ponisten flag
	if(!gameNotOver)
		gameOver();

	//iscrtava se brod
	ShipDraw(shipPos,shipForward);
	//iscrtavaju se prstenovi
	drawCircles();
	//iscrtava se pozadinska granica
	drawBoundaries();
	//zamenjujemo bafere posto koristimo double buffer mode
	glutSwapBuffers();
}
// po pritisku tastera postavlja se flag da je taster pritisnut, osim za ESC koji automatski gasi program.
void on_keypress(unsigned char key, int x, int y){
	//kada se pritisne bilo koji taster postavlja se flag da je igrica pocela
	flag_GO = 1;
	switch(key) {
		case 27: exit(0); 	  break; //ESC(27) gasi program
		case 'i': flag_i = 1; break;
		case 'k': flag_k = 1; break;
		case 'w': flag_w = 1; break;
		case 's': flag_s = 1; break;
		case 'a': flag_a = 1; break;
		case 'd': flag_d = 1; break;
	}
}
// po pustanju tastera ponistava se odgovarajuci flag da je taster pritisnut
void on_release(unsigned char key, int x, int y){
	const double deltaSpeed = 0.03; //Promena brzine
	const double angle = 0.045;
	switch(key) {
		case 27: exit(0); 	  break; //ESC(27) gasi program
		case 'i': flag_i = 0; break;
		case 'k': flag_k = 0; break;
		case 'w': flag_w = 0; break;
		case 's': flag_s = 0; break;
		case 'a': flag_a = 0; break;
		case 'd': flag_d = 0; break;
	}
}
//GLUT timer callback fja 
void on_timer(int id){
	//ukoliko callback dolazi od odgovarajuceg timera, i ukoliko igra nije gotova se vrsi izvrsavanje
	if(id == 0 && gameNotOver){
		//procesuiramo komande u zavisnosti koji tasteri su pritisnuti
		keypressProcessing();
		//vrsi se provera da li je brod prosao "kroz"(dovoljno blizu centra) prsten
		checkPass();
		
		//Pomeramo brod tj menjamo njegove koord centra u zavisnosti na vektor pravca i brzinu
		moveShip(&shipPos, shipForward, speed);
		//pomera se kamera da prati brod
		moveCamera();

		glLoadIdentity();
		//postavljamo kameru
		gluLookAt(eye.x, eye.y, eye.z, look.x, look.y, look.z, up.x, up.y, up.z);
		//forsiramo ponovno iscrtavanje ekrana
		glutPostRedisplay();

		//Automatsko usporenje
		if(speed > 0)
			speed-=0.005;
		if(speed < 0)
			speed += 0.005;
	}
	//ponovo podesavamo timer
	glutTimerFunc(TIMER_INTERVAL,on_timer,0);
}
//Postavljamo pogled na ceo prozor, podesavamo perspektivu i vracamo na modelview matricu
void on_reshape(int width, int height){
	glViewport(0, 0, width, height);
  	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();
  	gluPerspective(60, (GLfloat)width/(GLfloat)height, Z_NEAR, Z_FAR);
  	glMatrixMode(GL_MODELVIEW);
}

//Fja koja vrsi pomeranje kamere tako sto podesava parametre u zavisnosti od pozicije broda
void moveCamera(){

	//pozicija kamere prati poziciju broda samo "malo iza"
	eye.x = shipPos.x;
	eye.y = shipPos.y + 5;
	eye.z = shipPos.z + 25;
	//postavlja se tacka u koju kamera gleda da se pomera zajedno sa brodom
	look.x = shipPos.x + shipForward.x;
	look.y = shipPos.y + shipForward.y;
	look.z = shipPos.z + shipForward.z;
}
/*Inicializacija pozicija centrova i boje prstenova, na pocetku ih ima 10
  Kada se prodje kroz prsten drugi postaje prvi, a na kraj se dodaje i crta novi*/
void initCircles(){
	//uvek imamo 10 prstenova
	for (int i = 0; i < 10; i++)
	{	
		//vrednosti boje se nasumicno postavljaju
		colors[i].x = rand()/(float)RAND_MAX;
		colors[i].y = rand()/(float)RAND_MAX;
		colors[i].z = rand()/(float)RAND_MAX;

		//x  i y koord prstena se nasumicno postavljaju ali tako da ne predju granicu -200/200 od centra broda
		double tmp = 400 * (rand()/(float)RAND_MAX);
		circles[i].x = shipPos.x+200 - tmp;

		tmp = 400 * (rand()/(float)RAND_MAX);
		circles[i].y = shipPos.y+200 - tmp;

		//z koord centra prstenova se postavljaju na razmaku od 500 jedinica izmedju sebe i 400 izmedju prvog i broda
		circles[i].z = shipPos.z -400 - 500*(i+1);
	}
	//prvi krug je uvek crven
	colors[0].x = 1;
	colors[0].y = 0;
	colors[0].z = 0;
}
//crtamo prstenove tako sto za svaki pozivamo fju koja crta prsten
void drawCircles(){
	for (int i = 0; i < 10; i++)
		DrawRing(circles[i], colors[i]);
}
/* fja koja proverava da li je brod "prosao" kroz prsten
   tako sto meri da li je rastojanje centra broda i centra prstena manje od 34 jedinica*/
void checkPass(){
	if(dist(shipPos, circles[0]) <= 34){
		//ako je prosao uslov prolaznosti pravimo novi prsten
		createNewCircle();
		//povecavamo score
		score++;
		//i dodajemo 2 sekunde na tajmer
		secLeft += 2;
	}
}
/* Fja koja pravi novi prsten na kraju reda prstenova
   tako sto prvo pomeri redosled svih za 1 da drugi bude prvi, prvi se brise a na kraj se dodaje novi*/
void createNewCircle(){
	for(int i = 0; i < 9; i++){
		circles[i] = circles[i+1];
	}
	//Novi krug x i y koord dobija na isti nacin kao i ostali
	double tmp = 400 * (rand()/(float)RAND_MAX);
	circles[9].x = shipPos.x+200 - tmp;

	tmp = 400 * (rand()/(float)RAND_MAX);
	circles[9].y = shipPos.y+200 - tmp;

	//i na istom je razmaku od prethodnog kruga, tj 5400 od broda
	circles[9].z = shipPos.z -400 - 5000;//*(9+1);
}
//Fja koja podesava parametre svetla
void setUpLight(){
	GLfloat light_position[] = { 1, 2, 1, 0 };

    // Ambijentalna boja svetla.
    GLfloat light_ambient[] = { 0.2, 0.45, 0.5, 1 };

    // Difuzna boja svetla.
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };

    // Spekularna boja svetla.
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

	// Ukljucuje se osvjetljenje i podesavaju parametri svetla.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

/*Fja koja zapravo vrsi radnju kada se pritisne taster
  Sve dok je odgovarajuci taster pritisnut izvrsava ce se efekti*/
void keypressProcessing(){
	//Taster I i K povecavaju odnosno smanjuju brzinu broda
	if(flag_i)
		speed += deltaSpeed;
	if(flag_k)
		speed -= deltaSpeed;

	//Tasteri w i s vrse pomeraj vektora pravca broda ka "gore" odnosno "dole" tj po y osi
	if(flag_w){
    	if (shipForward.y + angleStep < 1)
      		shipForward.y += angleStep;
  	}
	else if(flag_s){
    	if (shipForward.y - angleStep > -1)
      		shipForward.y -= angleStep;
 	}
  	else
    	shipForward.y *= 0.98; // ako taster nije pritisnut y koord se postepeno vraca ka 0
    //Tasteri a i d vrse pomeraj vektora pravca broda ka "levo" odnosno "desno" tj po x osi
  	if(flag_d){
    	if (shipForward.x + angleStep < 1)
      	shipForward.x += angleStep;
  	}
	else if(flag_a){
    	if (shipForward.x - angleStep > -1)
      		shipForward.x -= angleStep;
  	}
  	else
    	shipForward.x *= 0.98; // ako taster nije pritisnut x koord se postepeno vraca ka 0
}
//Fja koja iscrtava pozadinsku granicu broda
void drawBoundaries(){
	//gasimo osvetljenje za granicu
	glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glMatrixMode(GL_MODELVIEW);

    double distance;
    //pocinjemo od pozicije prvog prstena pa do 5000 jedinica posle poslednjeg i pravimo razmak od 50 jedinica
	for(int i = circles[0].z; i > circles[9].z - 5000; i-= 50 ){
		 vector dot = {0,0,i};
		 distance = fabs(shipPos.z - i);//rastojanje pozicije broda od centra granicnog prstena
		 
		 /*iscrtavamo granicni prsten kao torus sa veoma malim unutrasnjim 
		   precnikom i veoma velikim spoljasnjim precnikom po z osi*/
		glPushMatrix();
			glTranslatef(0, 0, i);
			//boja se podesava tako da je tamnija sto je brod dalji od prstena granice da bi dobili efekat nestajanja u daljini
			glColor3f(40/distance,150/distance,250/distance);
			glutSolidTorus(2,1000,10,10);
		glPopMatrix();
	}

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}
//Fja koja iscrtava interfejs
void drawHud(){
	//gasimo osvetljenje za interfejs
	glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

	glMatrixMode(GL_MODELVIEW);
	//alociramo string za tekst poena
	char* text = malloc(sizeof(char) * strlen("SCORE:") + 5*sizeof(char) );

	//ubacujemo poene u string
	sprintf(text, "SCORE: %d", score);

	//postavljamo boju teksta na belu
	glColor3f(1,1,1);
	glPushMatrix();
		//podesavamo raster poziciju pixela slova
		glRasterPos3f(shipPos.x - 20, shipPos.y - 12, shipPos.z);
 		for (int i=0; text[i]; i++) {
 				//iscrtavamo pojedinacno slovo
    			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
  		}
  	glPopMatrix();

  	//isto i za tajmer
  	char* text2 = malloc(sizeof(char) * strlen("TIMER:") + 10*sizeof(char) );

	sprintf(text2, "TIMER: %d", secLeft);

  	glPushMatrix();
		glRasterPos3f(shipPos.x + 20, shipPos.y - 12, shipPos.z);
 		for (int i=0; text2[i]; i++) {
    			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text2[i]);
  		}
  	glPopMatrix();
  	//oslobadjamo memoriju zauzetu stringovima
  	free(text);
  	free(text2);
  	//ukljucujemo ponovo osvetljenje
  	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}
//Callback fja glut timera ali je koristimo za merenje preostalog vremena
static void countdown(int id){
	//ako nije ostalo vise vremena, prekini igru tako sto ponistis flag i forsiraj redisplay
	if(secLeft <= 0){
		gameNotOver = 0;
		glutPostRedisplay();
		return;
	}
	//ako je igra pocela i callback dolazi od odgovarajuceg tajmera smanji sekund za 1
	if(id == 1 && flag_GO == 1){
		secLeft--;
	}
	//ponovo postavi tajmer na svaki sekund
	glutTimerFunc(1000,countdown,1);
}
//Fja koja "prekida" igru ispisuje tekst "GAME OVER" na ekran
void gameOver(){
	glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

	glMatrixMode(GL_MODELVIEW);
	char text[] = "GAME OVER!";

  	glPushMatrix();
		glRasterPos3f(shipPos.x - 2, shipPos.y - 12, shipPos.z);
		glColor3f(1,1,1);
 		for (int i=0; text[i]; i++) {
    			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
  		}
  	glPopMatrix();

  	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}