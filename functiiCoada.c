#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "lib.h"

/* Petreaca Vlad-Florin 323CB*/

void PushCoada(TLDI *coada, void* el) {

	TLDI aux = (TLDI)malloc(sizeof(TCelula2)); //aloc memorie pentru un element pentru coada
	if (aux == NULL)
		printf("Eroare la adaugarea elementului in coada\n");
	aux->info = el;//ii actualizez informatia
	aux->urm = NULL;

	if ((*coada) == NULL)
		*coada = aux;
	else {
		TLDI park = *coada;	//pun elementul la sfarsitul cozii
		while (park->urm != NULL)
			park = park->urm;
		park->urm = aux;
	}
}

void* PopCoada(TLDI *lista) {

	void* info;			//iau adresa elementului informatie pe care vreau sa il scot din coada
	TLDI rm = *lista;
	info = (*lista)->info;
	(*lista) = rm->urm;
	free(rm);

	return info;
}

void* topCoada(TLDI *coada) {
	if (*coada != NULL) {		
		return (*coada)->info; //intorc adresa primului element din coada
	}
	return NULL;
}

void InserareOrdonataCoada(TLDI *coada, void* element, int(*cmp)(void*, void*)) {

	TLDI coadaAux = NULL;
	if ((*coada) == NULL) {	//cand coada este nula, introduc direct elementul
		PushCoada(&(*coada), element);
	}
	else {
		void* el = topCoada(&(*coada));	//intorc primul element din coada fara a-l scoate
		int ok = 0;
		int switcher;	//folosesc switcher pentru diferitele cazuri ale ordonarii
		while ((*coada) != NULL && ok == 0) {
			switcher = cmp(el, element);
			switch (switcher)
			{
				case 1: {	//prioritatea primului proces este mai mare decat celui de-al doilea
					void* aux = PopCoada(&(*coada));
					PushCoada(&coadaAux, aux);
					el = topCoada(&(*coada));
					break;
				}
				case 2: { //au aceeasi prioritate dar timpul de executie ai primului proces este mai mic decat celui de-al doilea
					void* aux = PopCoada(&(*coada));
					PushCoada(&coadaAux, aux);
					el = topCoada(&(*coada));
					break;
				}
				case 3: { // au aceeasi prioritate, acelasi timp de executie dar primul proces are pid-ul mai mic decat al doilea
					void* aux = PopCoada(&(*coada));
					PushCoada(&coadaAux, aux);
					el = topCoada(&(*coada));
					break;
				}
				case 4: {//au aceeasi prioritate dar timpul de executie ai primului proces este mai mare decat celui de-al doilea
					PushCoada(&coadaAux, element);
					ok = 1;
					break;
				}
				default: { //procesul 1 are prioritatea mai mica decat procesul 2
					PushCoada(&coadaAux, element);
					ok = 1;
				}
			}

		}
		if ((*coada) == NULL && ok == 0) {	//daca flag-ul ok = 0 toate elementele au fost in ordine
			PushCoada(&coadaAux, element);//se introduce ultimul element in coada
		}
		else {
			while ((*coada) != NULL){ //s-a pus elementul care trebuie adaugat pe pozitia care trebuie si se introduc restul elementelor
										//in coada auxiliara
				void* aux = PopCoada(&(*coada));
				PushCoada(&coadaAux, aux);
			}
		}

		while (coadaAux != NULL){	//se reintroduc elementele in coada principala
			void* aux = PopCoada(&coadaAux);
			PushCoada(&(*coada), aux);
		}
	}
}

