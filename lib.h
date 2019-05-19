#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/* Petreaca Vlad-Florin 323CB*/

typedef struct celula {
	void* info;
	struct celula* urm;
}TCelula2, *TLDI;

typedef struct proces {
	int PID;
	int prioritate;
	int timpDeExecutie;
	int dimensiuneMemorie;
	int startMemorie;
	int executed_time;
	int back_in_time;
	char* stare;
	TLDI stivaMemorie;
}Proces,*Procese;

typedef struct memorie {
	int dimensiuneMemorieMax;
	TLDI listaPidDisponibil;
	TLDI stivaMemorieProcese;
	TLDI coadaDeAsteptare;
	TLDI coadaProceseTerminate;
	Procese procesInExecutie;
}Memorie;

typedef struct celulaMemorie {
	int PID;
	int start_memorie;
	int sfarsit_memorie;
	int dimensiune_memorie_proces;
	int memorie_ramasa;
}CelulaStivaMemorie;

//Alocare structuri---//
Memorie* CreareMemorie();
Procese CreareProces(int memorieOcupata, int timpExecutie, int prioritate);
CelulaStivaMemorie* CreareCelulaMemorie();

///--Operatii Coadaa---////
void PushCoada(TLDI *lista, void* el);
void* PopCoada(TLDI *lista);
void* topCoada(TLDI *coada);
void InserareOrdonataCoada(TLDI *coada, void* element, int(*cmp)(void*, void*));

///--Operatii Stiva--///
void PushStiva(TLDI *lista, void* el);
void* PopStiva(TLDI *lista);
void* seekStiva(TLDI stiva);
int eliberareMemorieProces(Memorie** memorie, int pid);
int cauataMemorie_AdaugareProces(Memorie** memorie, void* element);
void rearanjareMemorie(Memorie** memorie);


//operatii-----------/////////

void add_remake(FILE* in, FILE* out, Memorie** memorie, int* contorPid,int cuanta);
void print(FILE* in, FILE* out, Memorie** memorie);
void get(FILE*in, FILE* out, Memorie** memorie);
void push(FILE* in, FILE* out, Memorie** memorie);
void pop(FILE* in, FILE* out, Memorie** memorie);
void run_remake(FILE* in, FILE* out, Memorie** memorie, int cuanta);
void finish(FILE* in, FILE* out, Memorie** memorie);
//--------------------///

//--Functii auxiliare---///
int comparareProces(void* element1, void* element2);
int compararePID(void* element1, void* element2);
TLDI AlocareCelula(void* element);
TLDI InserareOrdonataLista(TLDI lista, void* element, int(*cmp)(void*, void*));
int takeFirst(TLDI *lista);
void AfisareProces(FILE* out, void* proces,int cont1,int cont2,char* status);
void AfiInt(FILE* out, void* element,int cont1,int cont2);
