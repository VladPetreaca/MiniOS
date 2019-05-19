#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "lib.h"

/* Petreaca Vlad-Florin 323CB*/

void PushStiva(TLDI *lista, void* el) { //functie de introducere a elemntelor intr-o stiva
	TLDI aux = (TLDI)malloc(sizeof(TCelula2));
	if (aux == NULL)
		printf("Nu s-a putut crea spatiu pentru elementul ce trebuie pus in stiva\n");
	aux->info = el;
	aux->urm = NULL;

	if ((*lista) == NULL)
		*lista = aux;
	else {
		aux->urm = *lista;
		*lista = aux;
	}
}

void* PopStiva(TLDI *lista) {	//functie de scoatere a unui element dintr-o stiva

	if (*lista != NULL) {
		TLDI aux = *lista;
		void* el;
		if (aux->urm != NULL) {
			*lista = aux->urm;
			el = aux->info;
			free(aux);
		}
		else {
			el = (*lista)->info;
			*lista = NULL;
		}
		return el;
	}
	return NULL;

}

void* seekStiva(TLDI stiva) {//intoarce adresa unui element dintr-o stiva

	if (stiva != NULL) {
		return stiva->info; 
	}
	return NULL;
}

int eliberareMemorieProces(Memorie** memorie, int pid) {

	TLDI stivaAux = NULL;
	int ok = 0;//flag
	while ((*memorie)->stivaMemorieProcese) {
		void* proces = PopStiva(&(*memorie)->stivaMemorieProcese);
		if ((*(CelulaStivaMemorie*)proces).PID == pid) {	//daca gasesc procesul luat din stiva are acelasi pid cu cel cautat
			(*(CelulaStivaMemorie*)proces).PID = -1;		//facem pidul -1 pentru a sti ca aici este zona libera de memorie
			(*(CelulaStivaMemorie*)proces).memorie_ramasa = (*(CelulaStivaMemorie*)proces).dimensiune_memorie_proces;
			//memoria ramasa va fi egala cu cea a procesului care a fost eliminat

			(*(CelulaStivaMemorie*)proces).dimensiune_memorie_proces = 0;//memoria ocupata de proces va fi nula
			ok = 1; // am gasit procesul cautat
		}
		PushStiva(&stivaAux, proces);
	}

	while (stivaAux) {	//reintroduc elementele in stiva
		void* aux = PopStiva(&stivaAux);
		PushStiva(&(*memorie)->stivaMemorieProcese, aux);
	}

	return ok;
	//intorc 0 daca procesul cautat nu a fost gasit si 1 altfel
}

int cauataMemorie_AdaugareProces(Memorie** memorie, void* element) {

	TLDI stivaAux = NULL;
	int start_memorie = -1;
	Procese procesNou = (Procese)element;
	int ok;
	int one_time = 0;
	while ((*memorie)->stivaMemorieProcese) {	//introduc elementele intr-o stiva auxiliara
		void* aux = PopStiva(&(*memorie)->stivaMemorieProcese);
		PushStiva(&stivaAux, aux);
	}

	while (stivaAux) {
		ok = 0;
		void* aux = PopStiva(&stivaAux);

		if ((*(CelulaStivaMemorie*)aux).PID == -1 && one_time == 0) { //am gasit o zona de memorie
			if (procesNou->dimensiuneMemorie <= (*(CelulaStivaMemorie*)aux).memorie_ramasa) {
				ok = 1; // am gasit memorie pentru procesul nou
				one_time=1;	//nu mai intru in block
				CelulaStivaMemorie* celulaMemorie = CreareCelulaMemorie();	//creeez o noua celula de tip memorie pentru stiva cu caracteristicile procesului
				celulaMemorie->start_memorie = (*(CelulaStivaMemorie*)aux).start_memorie;
				celulaMemorie->dimensiune_memorie_proces = procesNou->dimensiuneMemorie;
				celulaMemorie->sfarsit_memorie = procesNou->dimensiuneMemorie + celulaMemorie->start_memorie;
				celulaMemorie->memorie_ramasa = 0; 
				celulaMemorie->PID = procesNou->PID;
				start_memorie = celulaMemorie->start_memorie;

				if (procesNou->dimensiuneMemorie == (*(CelulaStivaMemorie*)aux).memorie_ramasa) {
					celulaMemorie->memorie_ramasa = 0;
					PushStiva(&(*memorie)->stivaMemorieProcese, celulaMemorie);	//procesul nou acopera toata memoria care a ramas intre procese
				}
				else {
					CelulaStivaMemorie* celulaMemorieRamasa = CreareCelulaMemorie();//memoria procesului este mai mica decat cea libera
					celulaMemorieRamasa->start_memorie = celulaMemorie->sfarsit_memorie;//mai creez o celula cu memoria ramasa
					celulaMemorieRamasa->dimensiune_memorie_proces = 0;					//care va fi memorie libera in stiva
					celulaMemorieRamasa->memorie_ramasa = (*(CelulaStivaMemorie*)aux).memorie_ramasa - celulaMemorie->dimensiune_memorie_proces;
					celulaMemorieRamasa->PID = -1;
					celulaMemorieRamasa->sfarsit_memorie = (*(CelulaStivaMemorie*)aux).sfarsit_memorie;

					PushStiva(&(*memorie)->stivaMemorieProcese, celulaMemorie);	//pun procesul nou in stiva de memorie
					PushStiva(&(*memorie)->stivaMemorieProcese, celulaMemorieRamasa);//adaug memoria ramasa in sriva
				}
			}

		}
		if (ok == 0)//nu am gasit memorie
			PushStiva(&(*memorie)->stivaMemorieProcese, aux);
	}
	return start_memorie; //intorc -1 in caz ca nu avem zona de memorie , altfel intorc zona de memorie la care incepe sa sse scrie procesul
}

void rearanjareMemorie(Memorie** memorie) {

	int ok = 0;
	TLDI stivaAxu = NULL;
	CelulaStivaMemorie* memorie_totala = CreareCelulaMemorie();

	while (1) {	//eliberez memoria "creata" din varful stivei(cele cu pid-1) pentru ca acolo este memorie libera oricum
		void* aux = ((CelulaStivaMemorie*)seekStiva((*memorie)->stivaMemorieProcese));
		if (((CelulaStivaMemorie*)aux)->PID == -1) {
			void* rm = PopStiva(&(*memorie)->stivaMemorieProcese);
			free(rm);
		}
		else
			break;	
	}

	while ((*memorie)->stivaMemorieProcese) {	//introduc elementele stivei intr-una auxiliara
		void* aux = PopStiva(&(*memorie)->stivaMemorieProcese);
		PushStiva(&stivaAxu, aux);
	}
	
	while (stivaAxu) {
		void* aux = PopStiva(&stivaAxu);
		if (((CelulaStivaMemorie*)aux)->PID == -1) {	
			ok++;
			if (ok == 1) {	//am doar o zona de memorie libera si o introduc in stiva la fel cum era
				memorie_totala->start_memorie = ((CelulaStivaMemorie*)aux)->start_memorie;
				memorie_totala->memorie_ramasa = ((CelulaStivaMemorie*)aux)->memorie_ramasa;
				memorie_totala->sfarsit_memorie = ((CelulaStivaMemorie*)aux)->sfarsit_memorie;
				memorie_totala->PID = ((CelulaStivaMemorie*)aux)->PID;
			}
			else {	//mai exista cel putin o memorie libera consecutiva
				//start_memorie ramane de la prima memorie libera
				memorie_totala->memorie_ramasa += ((CelulaStivaMemorie*)aux)->memorie_ramasa;	//se aduna memoria ramasa
				memorie_totala->sfarsit_memorie = memorie_totala->memorie_ramasa + memorie_totala->start_memorie;	//unde se termina memoria
			}
		}
		else {
			ok = 0;	//se face contorul memoriilor libere 0
			if (memorie_totala->PID != 0) { //memoria procesului idle are pid 0 iar cea libera pid -1
				PushStiva(& (*memorie)->stivaMemorieProcese, memorie_totala);
				memorie_totala = CreareCelulaMemorie();
			}
			PushStiva(&(*memorie)->stivaMemorieProcese, aux);
		}
	}
}