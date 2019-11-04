#ifndef LAYER_H
#define LAYER_H
#include"Pixel.h"

class Layer {
public:
	Layer();
	~Layer();
	void setOpacity(int i);//podesava providnost
	int getOpacity();//vraca providnost
	void setNext(Layer* sled);//za podesavanje pokazivaca next
	Layer* getNext();//vraca next
	void setLayer(Pixel*** newLayer);//za dodavanje novog sloja ali mi pre toga fali da kreiram praznu matricu 
	Pixel*** getLayer();//vraca ptptp odnosnu sliku koja je u listi slojeva
	void setSirina(int i);
	void setVisina(int i);
	void setPrevious(Layer* prethodni);
	Layer* getPrevious();
private:
	//ono sto mi je potrebno za listu
	Pixel*** layer;//sloj
	Layer* next;//pokazivac na sledeci
	Layer* previous;
	unsigned int layerOpacity;//providnost sloja
	int visina;//treba mi za destruktor
	int sirina;//treba mi za desktruktor

};
#endif //LAYER_H
