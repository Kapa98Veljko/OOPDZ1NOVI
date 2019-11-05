#include "ImageEditor.h"
#include "Pixel.h"
#include"Layer.h"
#include<iostream>
#include<string>
using namespace std;
//radi
ImageEditor::ImageEditor()
{
	this->ImageName = "";
	this->height = 0;
	this->width = 0;
	this->sp = 0;
	this->isNamed = false;
	this->R = 0;
	this->B = 0;
	this->G = 0;
	this->glava = nullptr;
	this->active = nullptr;//cuvainformacije o aktivnom 
	this->last = nullptr;
}//radi
/*Napoemena:On brise sloj,a onda se poziva destrukor u Layer-u koji ce da obrise matricu!!! */
ImageEditor::~ImageEditor()
{  
	Layer* trenutni = this->glava;
	Layer* stari = nullptr;
	while (trenutni != nullptr) 
	{
		trenutni->setPrevious(nullptr);
		stari = trenutni;
		trenutni = trenutni->getNext();
		delete stari;
	}
	delete trenutni;
	
}
//radi
bool ImageEditor::loadImage(unsigned char* image)
{


	if (!image) { return false; }
	//loading format
	if (image[this->sp] == 'b' || image[this->sp] == 'B') {
		this->ImageFormat[this->sp++] = 'B';
	}
	else {
		return false;
	}

	if (image[this->sp] == 'm' || image[this->sp] == 'M') {
		this->ImageFormat[this->sp++] = 'M';
	}

	else {
		return false;
	}
	//PROVERENO//sp je poslednji popunjeni
	//loading name if there is any
	if (image[this->sp] == '=')
	{

		this->isNamed = true;
		sp++;
		while (image[this->sp] != '=')
		{
			this->ImageName += image[sp];
			sp++;

		}


		this->sp += 1;//za prelazak preko jednako
	}
	//ili ima ili nema imena svakako trazi poziciju
	if (!this->isNamed || this->sp % 4 != 0)
	{
		this->sp = FindPosition(this->sp);
		//vraca na prvu deljivu poziciju sa 4
	}
	//Ucitavanje sirine i visine
	this->width = (unsigned int)image[sp] + ((unsigned int)image[sp + 1]) * 256 + ((unsigned int)image[sp + 3]) * 256 * 256 + ((unsigned int)image[sp + 3]) * 256 * 256 * 256;
	sp += 4;
	this->height = (unsigned int)image[sp] + ((unsigned int)image[sp + 1]) * 256 + ((unsigned int)image[sp + 2]) * 256 * 256 + ((unsigned int)image[sp + 3]) * 256 * 256 * 256;
	sp += 4;

	//visina i sirina ucitane i postavljen sp na prvu poziciju nakon njih
	//nalazi deljivu sa cetiri ako nije na njoj

	this->sp = FindPosition(this->sp);

	Pixel*** matrix = new Pixel * *[this->height];//trenutna kopija

	for (int i = height - 1; i >= 0; i--)
	{

		matrix[i] = new Pixel * [this->width];
		for (unsigned int j = 0; j < width; j++)
		{   //da li je potrebna dodatna konverzija


			matrix[i][j] = new Pixel();
			matrix[i][j]->setBlue(image[this->sp++]);

			matrix[i][j]->setGreen(image[this->sp++]);

			matrix[i][j]->setRed(image[this->sp++]);
		}
		//treba da se prekoce prazna mesta ako postoje
		this->sp = FindPosition(this->sp);

	}

	//sad imam sve matrice ovu matrix treba ubaciti u listu
	if (this->glava != nullptr)
	{
		Layer* trenutni = this->glava;
		Layer* sledeci;
		while (this->glava->getNext() != nullptr)
		{
			sledeci = trenutni->getNext();
			delete trenutni;
			trenutni = sledeci;
		}
	}
	else
	{
		Layer* ptr1 = new Layer();
		ptr1->setLayer(matrix);
		this->glava = ptr1;
		this->active = this->last = ptr1;
	//	ptr1 = nullptr;
	//	delete ptr1;

	}

	return true;
}
//Nalazi poziciju deljivu sa cetiri i vraca njen indeks gde sp treba da bude
//radi
unsigned int ImageEditor::FindPosition(unsigned int sp)
{  //vraca na mesto koje je deljivo sa 4
	while (sp % 4)
	{
		sp++;
	}
	return sp;
}
//radi
int ImageEditor::fillBlanks(int i, unsigned char* image)//OVO MOZE DA JEBE STVAR!!!!!!!
{
	while (i % 4)
	{
		image[i] = '0';
		i++;
	}
	return i;
}
//radi
void ImageEditor::packTheRest(Layer* glava, int sp, unsigned char* image)
{
	//na prvoj sam ne popunjenoj poziciji i sada treba sabrati slojeve
	//ZELIM DA SABIRAM PIKSEL PO PIKSEL IZ SVAKE MATRICE NA ODGOVARAJUCIM POZICIJAMA MORA SE MENJATI MATRICA!!!!!
	
	//DA LI JE OVO POTREBNO
	float remained = 0;//refers to opacity of layer
	float percentage = 0;
	Pixel*** newOne = new Pixel** [height];
	Layer* trenutni = last;
	//pomocna matrica
	Pixel*** matrix = trenutni->getLayer();
	for (int i = 0; i < height; i++) 
	{
		newOne[i] = new Pixel * [width];//ovo je dobro
		for (int j = 0; j < width; j++) 
		{
			newOne[i][j] = new Pixel();//ako nema nicega ispod boji ce u crno
			while (trenutni != nullptr)
			{
				matrix = trenutni->getLayer();
				//ako mi je piksel nullptr onda ga ne sabiram!!!
				//ako je nullptr onda je u potpunosti providan pikesl!!!
				if (matrix[i][j] != nullptr)
				{

					percentage = ((100 - remained) * (trenutni->getOpacity()/100 ));
					newOne[i][j]->setBlue(newOne[i][j]->getBlue() + (unsigned int)(matrix[i][j]->getBlue() * percentage/100));
					newOne[i][j]->setGreen(newOne[i][j]->getGreen() + (unsigned int)(matrix[i][j]->getGreen() * percentage/100));
					newOne[i][j]->setRed(newOne[i][j]->getRed() + (unsigned int)(matrix[i][j]->getRed() *percentage/100));
				}
				//menjam sloj
				trenutni = trenutni->getPrevious();
				//ovaj uslov za izlazak je dobar!!!
                //ako nisam dosao do kraja onda uzmi novu matricu i njenu neprovodnost      				
				if (trenutni != nullptr) {
					
					remained += percentage;
				}
	
			}
			percentage= 0;
			remained = 0;
			trenutni = last;

		}//kada se vratim na kraj da bih opet racunao moram da resetujem neprovodnost 
	}

	for (int i = height - 1; i >= 0; i--)
	{
		for (int j = 0; j < width; j++)
		{
			image[sp++] = (unsigned char)newOne[i][j]->getBlue();
			image[sp++] = (unsigned char)newOne[i][j]->getGreen();
			image[sp++] = (unsigned char)newOne[i][j]->getRed();
		}
		if (i > 0)//zato sto ne treba da mi posle poslednjeg reda popunjava nulama
		{
			sp = fillBlanks(sp, image);
		}
	}
}


//radi
//saveImage
unsigned char* ImageEditor::saveImage()
{

	unsigned int size = 10 + this->width * this->height * 3;//format,visina,sirina,i broj piksela
	unsigned int k = 0;
	if (this->isNamed) { k += 2 + this->ImageName.length() + 4 - ((4 + this->ImageName.length()) % 4); }
	else { k = 2; }
	size += k;
	if (!(width % 4)) { size += 4 - (width % 4); }
	unsigned char* image = new unsigned char[size];

	image[0] = 'B';
	image[1] = 'M';
	int sp = 2;//sp mi prati prvu ne popunjenu poziciju ujedno i broj elemenata niza jer indeks prvog ne popunjenog duzina samog niza
	if (this->isNamed)
	{
		image[sp] = '=';
		image[sp + ImageName.length() + 1] = '=';
		sp++;


		for (unsigned int i = 3; i < 3 + ImageName.length(); i++)
		{
			sp++;
			image[i] = this->ImageName[i - 3];
		}


	}
	sp++;
	sp = fillBlanks(sp, image);

	//sada treba popuniti visinu i sirinu

	int w = this->width;
	int h = this->height;

	for (int i = 0; i < 4; i++)
	{

		image[sp] = (unsigned char)(w % 256);
		image[sp + 4] = (unsigned char)(h % 256);
		w /= 256;
		h /= 256;
		sp++;
	}
	sp += 4;
	sp = fillBlanks(sp, image);
	//sada treba sabrati sliku koju dobijam sabirajuci Layer-e
	packTheRest(this->glava, sp, image);


	return image;
}

void ImageEditor::freeMatrixfromLayer(Pixel*** matrix)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			delete matrix[i][j];
		}
		delete[] matrix[i];
	}
	delete[] matrix;
}


//Manipulacija slikom

void ImageEditor::addLayer()
{
	Layer* ptr = new Layer();
	Pixel*** currentMatrix = makeCopy(height,width);
	//dvostruko ulancana lista
	ptr->setLayer(currentMatrix);
	ptr->setPrevious(this->last);
	//Nadovezivanje LinusTorvalds na kraj liste
	this->last = ptr;
	this->active = ptr;
	ptr = nullptr;
}

void ImageEditor::deleteLayer()
{
	Layer* prethodni = this->glava;
	if (prethodni = this->active)
	{
		std::cout << "<GRESKA!>Napravili ste gresku nije moguce izbrisati sloj sa ucitanom slikom!!!" << std::endl;
	}
	//ako nije uneta nijedna slika
	else if (this->glava == nullptr)
	{
		std::cout << "<GRESKA>Greska ne postoji ni jedan ucitani sloj!!!" << std::endl;
	}
	else
	{    //ili dok ne dodje do kraja liste ili dok ne dodje iza aktivnog koji nije ucitana slika
		//lose napisano while (prethodni->getNext() != nullptr && prethodni->getNext() != this->active)
		{
			prethodni = prethodni->getNext();
		}

		prethodni->setNext(this->active->getNext());
		freeMatrixfromLayer(this->active->getLayer());
		delete this->active;
		this->active = prethodni;
	}
}

void ImageEditor::selectLayer(int i)
{//pitati iliju da li i kako ide castovanje za float
	Layer* trenutni = this->glava;
	if (i < 0)
	{
		std::cout << "<GRESKA>Greska zadati sloj ne postoji" << std::endl;
	}
	else if (i == 0)
	{
		this->active = this->glava;
	}
	else
	{
		int k = 0;
		while (k < i && trenutni->getNext() != nullptr)
		{
			trenutni = trenutni->getNext();
			k++;
		}
		if (trenutni == nullptr)
		{
			std::cout << "<GRESKA>Greska prekoracili ste opseg!!!" << std::endl;
		}
		else
		{
			this->active = trenutni;
		}

	}
	trenutni = nullptr;
	delete trenutni;
}

void ImageEditor::setLayerOpacity(int i)
{
	char c = 'N';
	if (i < 0 || i>100)
	{
		std::cout << "Ovo sto cinite je nedozvoljeno jer broj ili nije u opsegu ili nije pozitivan!!" << std::endl;
		bool wish = false;
		std::cout << "Da li zelite da znate gde ste pogresili?Y/N" << std::endl;

		std::cin >> c;
	}
	else if (c == 'Y' || c == 'y')

	{
		if (i < 0) { std::cout << "Broj je negativan!!!" << std::endl; }
		else if (i > 100) { std::cout << "Broj je veci od 100!!!" << std::endl; }
	}
	else {
		this->active->setOpacity(i);
	}
	//provera za to da li je ceo broj!!!!!!
}


//radi
unsigned int* ImageEditor::findAverage(int h, int w, int size)
{
	unsigned int* prosek = new unsigned int[3];//treba ga izbrisati!!!
	unsigned int k = 0;//brojac
	unsigned int red = 0, blue = 0, green = 0;
	Pixel*** matrix = this->active->getLayer();//kako ovo da se unisti!!!
	//gornje leve koordinate pravougaonika/kvadrata iz kog se vadi preosek 
	//nalazenje koorsinata je valjda dobro sada to treba prilagoditi za visinu i sirinu
	int x = ((w - size) <=0) ? 0 : (w - size);//ove dve koordinate su iste
	int y = ((h - size) <=0) ? 0 : (h - size);//ostavicu ovako zbog moguce modifikacije!!!
	
	int visina = ((h + size) >= height) ? height : (h + size + 1);
	int sirina = ((w + size) >= width) ? width : (w + size + 1);

	for (unsigned int i = y; i < visina; i++)
	{
		for (unsigned int j = x; j<sirina; j++)
		{
			//ako je neki piksel nullptr onda ga ne sabirm
			if (matrix[i][j]!= nullptr)//ovde ce morati da se prepravi
			{
				red += matrix[i][j]->getRed();
				green += matrix[i][j]->getGreen();
				blue += matrix[i][j]->getBlue();
				k++;
			}
		}
	}
	prosek[0] = (unsigned int)(blue/k );
	prosek[1] = (unsigned int)(green /k);
	prosek[2] = (unsigned int)(red/ k);
	

	return prosek;

}
//radi i vraca matricu nullptr pokazivaca pa svaki put treba to ispitati
Pixel*** ImageEditor::makeCopy(int visina, int sirina)
{
	Pixel*** copy = new Pixel** [visina];
	for (int i = 0; i < visina; i++)
	{
		copy[i] = new Pixel * [sirina];
		for (int j = 0; j < sirina; j++)
		{
			copy[i][j] = nullptr;

		}

	}

	return copy;
}
//radi
void ImageEditor::invertColors()
{
	Pixel*** trenutni = this->active->getLayer();
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			if (trenutni[i][j] != nullptr)
			{
				trenutni[i][j]->setRed((unsigned int)(255 - trenutni[i][j]->getRed()));
				trenutni[i][j]->setGreen(255 - trenutni[i][j]->getGreen());
				trenutni[i][j]->setBlue(255 - trenutni[i][j]->getBlue());
			}
		}
	}
	trenutni = nullptr;
}
//radi
void ImageEditor::toGrayScale()
{
	Pixel*** trenutni = this->active->getLayer();
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			unsigned int togray = (unsigned int)(0.3 * trenutni[i][j]->getRed()) + (unsigned int)(0.59 * trenutni[i][j]->getGreen()) + (unsigned int)(0.11 * trenutni[i][j]->getBlue());
			if (trenutni[i][j] == nullptr) { trenutni[i][j] = new Pixel; }
			trenutni[i][j]->setRed(togray);
			trenutni[i][j]->setGreen(togray);
			trenutni[i][j]->setBlue(togray);
		}
	}
	trenutni = nullptr;
	delete trenutni;
	//Ne zelim da mi vise nekih nekom greskom probije opseg i da pokazuju na nesto zapravo
	//Vazno Iliji si prepravio da bude sa kasovanjem inacu baguje !!!
}
//radi
void ImageEditor::blur(int size)
{
	
	Pixel*** matrix = makeCopy(height, width);//Napomena vraca matricu nullptr pokazivaca!!!
	unsigned int* prosek = new unsigned int[3];//Ovo stvara niz velicine tri u koji mi funkcija vraca proseke BGR!!!
	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			//if (matrix[i][j] = nullptr) {} TREBA RESITI STA AKO SE DESI DA JE NULLPTR TREBA DA SE BLURUJE ZA SAD APRETPOSTAVKA DA NIJE!!
				prosek = findAverage(i, j, size);
				matrix[i][j] = new Pixel();
				matrix[i][j]->setRed(prosek[2]);
				matrix[i][j]->setGreen(prosek[1]);
				matrix[i][j]->setBlue(prosek[0]);
			
		}
	}
	freeMatrixfromLayer(this->active->getLayer());
	active->setLayer(matrix);
	delete[] prosek;
	matrix = nullptr;
}
//radi
void ImageEditor::flipHorizontal()
{
	Pixel*** trenutni;
	Pixel*** copy = makeCopy(height, width);

	trenutni = this->active->getLayer();
	for (int i = 0; i < this->height; i++)
	{
		int k = this->width - 1;
		for (int j = 0; j < width; j++)
		{
			copy[i][k] = new Pixel;
			copy[i][k]->setBlue(trenutni[i][j]->getBlue());
			copy[i][k]->setGreen(trenutni[i][j]->getGreen());
			copy[i][k]->setRed(trenutni[i][j]->getRed());
			k--;

		}
	}
	this->active->setLayer(copy);
	//izbrisati trenutni 
	freeMatrixfromLayer(trenutni);

}
//radi
void ImageEditor::flipVertical()
{
	Pixel*** trenutni = new Pixel**;
	Pixel*** copy = makeCopy(height, width);

	trenutni = this->active->getLayer();
	
	for (unsigned int i = 0; i < this->width; i++)
	{
		int k = this->height - 1;
		for (unsigned int j = 0; j < this->height; j++)
		{
			copy[k][i] = new Pixel();
			copy[k][i]->setBlue(trenutni[j][i]->getBlue());
			copy[k][i]->setRed(trenutni[j][i]->getRed());
			copy[k][i]->setGreen(trenutni[j][i]->getGreen());
			k--;
		}
	}
	this->active->setLayer(copy);
	freeMatrixfromLayer(trenutni);
}
//radi za divno cudo
void ImageEditor::crop(int x, int y, int w, int h)
{	//zavrsena 
	if (x < 0 || y < 0 || w < 0 || h < 0 || x>width || y>height )
	{
		std::cout << "<GESKA>Pogresno formatirano!!!" << std::endl;
	}
	else {

		

		int sirina= ((w + x) > width) ? (width-x) : w;
		int visina = ((h + y) > height) ? (height-y) : h;
		Layer* trenutni = this->glava;
		while (trenutni!= nullptr) {
			Pixel*** matrix = makeCopy(visina, sirina);
			Pixel*** pom = trenutni->getLayer();
	
			for (unsigned int i = 0; i < visina; i++)
			{
				for (unsigned int j = 0; j < sirina; j++)
				{
					matrix[i][j] = new Pixel(pom[i + y][j + x]->getRed(), pom[i + y][j + x]->getGreen(), pom[i + y][j + x]->getBlue());
				}
			}
			freeMatrixfromLayer(trenutni->getLayer());//brise starumatricu
			trenutni->setLayer(matrix);//postavlja novu na mestu stare
			trenutni = trenutni->getNext();
			matrix = nullptr;
		}

		//to je na kraju
		width = w;
		height = h;
	}
}
//Crtanje po slici
void ImageEditor::setActiveColor(string hex)
{
	bool format = true; int i = 1;
	if (hex[0] != '#') { format = false; }
	while (i < 7)
	{   
		if (!(hex[i]>='a'&&hex[i]<='f')||!(hex[i] >= 'A' && hex[i] <= 'F')|| !(hex[i] >= '0' && hex[i] <= '9'))
		{
			format = false;
		}
		i++;
	}
	if (format)
	{
		for (int i = 1; i < 7; i++)
		{
			if (hex[i] == 'a') { hex[i] = 'A'; }
			if (hex[i] == 'b') { hex[i] = 'B'; }
			if (hex[i] == 'c') { hex[i] = 'C'; }
			if (hex[i] == 'd') { hex[i] = 'D'; }
			if (hex[i] == 'e') { hex[i] = 'E'; }
			if (hex[i] == 'f') { hex[i] = 'F'; }

		}
		this->R = fromHextoDecimal(hex[1], hex[2]);
		this->G = fromHextoDecimal(hex[3], hex[4]);
		this->B = fromHextoDecimal(hex[5], hex[6]);

	}
}

unsigned int ImageEditor::fromHextoDecimal(char a, char b)
{
	unsigned int boja = 0;
	if (a >= 'A') { boja = 16 * (a - 'A' + 10); }
	else (boja = 16 * (a - '0' + 10));
	if (b >= 'A') { boja = (b - 'A' + 10); }
	else (boja = (b - '0' + 10));
	return boja;
}

void ImageEditor::fillRect(int x, int y, int w, int h)
{//(x,y) koordianata gornjeg levog ugla
 //(w,h) sirina i visina pravougaonika ako izlazi van opsega onda samo do kraja matrice
 //prvo cu da preracunam visinu i sirinu i treba ispitati da li je pokazivac nullptr jer NE treba sve na pocetku postavlajti na 0,0,0
	int visina = ((y + h)>=height)?height:(y+h);
	int sirina = ((x + w) >= width) ? width : (x + w);
	
	Pixel*** matrica= this->active->getLayer();


	for (int i = y; i < visina; i++)
	{
		for (int j = x; j < sirina; j++)
		{
			if (matrica[i][j] == nullptr)
			{
				matrica[i][j] = new Pixel(this->R, this->G, this->B);
			}
		}
	}

	matrica = nullptr;
}

void ImageEditor::eraseRect(int x, int y, int w, int h)
{
	Pixel*** matrica = active->getLayer();
	int visina = y + h;
	int sirina = x + w;

	if (visina > height)
	{
		visina = height;
	}
	if (sirina > width)
	{
		sirina = width;
	}

	for (unsigned int i = y; i < visina; i++)
	{
		for (unsigned int j = h; j < sirina; j++)
		{
			delete matrica[i][j];
			matrica[i][j] = nullptr;
		}
	}
}
