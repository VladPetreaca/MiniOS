#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "lib.h"

/* Petreaca Vlad-Florin 323CB*/

int compararePID(void* el1, void* el2) {

	if (*(int*)el1 > *(int*)el2) {
		return 1;
	}
	else if (*(int*)el1 == *(int*)el2){
		return 2;
	}
	else
		return 3;
	return 0;
}

int comparareProces(void* element1, void* element2) {

	Proces proces1 = *(Procese)element1;
	Proces proces2 = *(Procese)element2;

	if (proces1.prioritate > proces2.prioritate) {
		return 1;
	}
	else if (proces1.prioritate == proces2.prioritate) {
		if (proces1.timpDeExecutie < proces2.timpDeExecutie) {
			return 2;
		}
		else if (proces1.timpDeExecutie == proces2.timpDeExecutie) {
			if (proces1.PID < proces2.PID) {
				return 3;
			}
		}
		else {
			return 4;
		}
	}
	else {
		return 5;
	}
	return 0;

}

TLDI AlocareCelula(void* element) {//aloc celula pentru un element de tip pid pentru proces(int)

	TLDI celula = (TLDI)malloc(sizeof(TCelula2));
	if (celula == NULL)
		return NULL;
	celula->info = element;
	celula->urm = NULL;

	return celula;
}

TLDI InserareOrdonataLista(TLDI lista, void* element, int(*cmp)(void*, void*)) {

	if (lista == NULL) {
		TLDI aux = AlocareCelula(element);
		if (aux == NULL)
			return lista;
		lista = aux;
		return lista;
	}
	else {
		TLDI prev = NULL;
		TLDI aux = AlocareCelula(element);
		TLDI park = lista;
		int ok = 0;

		while (park) {
			if (cmp(park->info, element) == 1) { // elementul listei este mai mare
				if (prev == NULL) {	//se introce noul element ca fiind primul element al listei
					aux->urm = lista;
					lista = aux;
				}
				else {	//se introduce pe pozitia care trebuie
					aux->urm = park;
					prev->urm = aux;
				}
				ok = 1; //flag-ul este activat
				break;
			}
			else {
				prev = park;
				park = park->urm;
			}
		}

		if (ok == 0) { 
			//daca flag-ul nu este activat,inseamanca elementul nou este mai mare decat toate elementele listei si se insereaza la final
			park = lista;
			while (park->urm != NULL)
				park = park->urm;
			park->urm = aux;
		}
		return lista;
	}
}

int takeFirst(TLDI *lista) {	//iau primul element ca fiind cel mai mic din lista de pid-uri si il elimin din lista
	if (*lista != NULL) {
		int res = *(int*)(*lista)->info;
		TLDI rm = (*lista);
		(*lista)->info = NULL;
		(*lista) = rm->urm;	
		free(rm);
		
		return res;
	}
	return -1;
}

void AfisareProces(FILE* out,void* proces,int cont1,int cont2,char* status) {

	Proces auxProces = *(Procese)proces;	//afisez procesele in functie de starea lor si dupa pozitie
	if (strcmp(status, "waiting") == 0) {
		if (cont1 == cont2) {
			if (strcmp(auxProces.stare, "waiting") == 0)
				fprintf(out, "(%d: priority = %d, remaining_time = %d)", auxProces.PID, auxProces.prioritate, auxProces.timpDeExecutie);
		}
		else {
			if (strcmp(auxProces.stare, "waiting") == 0)
				fprintf(out, "(%d: priority = %d, remaining_time = %d),\n", auxProces.PID, auxProces.prioritate, auxProces.timpDeExecutie);
		}
	}
	else if (strcmp(status, "finished") == 0) {
		if (cont1 == cont2) {
			fprintf(out, "(%d: priority = %d, executed_time = %d)", auxProces.PID, auxProces.prioritate, auxProces.executed_time);
		}
		else {			
			fprintf(out, "(%d: priority = %d, executed_time = %d),\n", auxProces.PID, auxProces.prioritate, auxProces.executed_time);
		}
	}
}

void AfiInt(FILE* out,void* element,int cont1,int cont2) {//afisez elementele stivei unnui proces in functie de pozitia lor

	if (cont1 != cont2) {
		fprintf(out, "%d ", *(int*)element);	
	}
	else if (cont1 == cont2)
		fprintf(out, "%d.\n", *(int*)element);
	
	
}