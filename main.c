#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "lib.h"

/* Petreaca Vlad-Florin 323CB*/

//Alocarii de diferite tipuri de celule
Memorie* CreareMemorie() {

	Memorie* memorie = (Memorie*)malloc(sizeof(Memorie));
	if (memorie == NULL)
		return NULL;
	memorie->dimensiuneMemorieMax = 3 * 1024 * 1024;
	memorie->listaPidDisponibil = NULL;
	memorie->stivaMemorieProcese = NULL;
	memorie->coadaDeAsteptare = NULL;
	memorie->coadaProceseTerminate = NULL;
	memorie->procesInExecutie = NULL;
	return memorie;
}

Procese CreareProces(int memorieOcupata, int timpExecutie, int prioritate) {

	Procese proces = (Procese)malloc(sizeof(Proces));
	if (proces == NULL) {
		printf("Nu s-a putut crea procesul\n");
		return NULL;
	}
	proces->timpDeExecutie = timpExecutie;
	proces->dimensiuneMemorie = memorieOcupata;
	proces->stivaMemorie = NULL;
	proces->prioritate = prioritate;
	proces->stare = (char*)malloc(sizeof(char) * 20);
	strcpy(proces->stare, "waiting");
	proces->executed_time = 0;
	proces->back_in_time = 0;

	return proces;
}

CelulaStivaMemorie* CreareCelulaMemorie() {

	CelulaStivaMemorie* celulaMemorie = (CelulaStivaMemorie*)malloc(sizeof(CelulaStivaMemorie));
	if (celulaMemorie == NULL)
		return NULL;
	celulaMemorie->dimensiune_memorie_proces = 0;
	celulaMemorie->sfarsit_memorie = 0;
	celulaMemorie->start_memorie = 0;
	celulaMemorie->PID = 0;
	celulaMemorie->memorie_ramasa = 0;

	return celulaMemorie;

}

//-------------------------------///

//------Dezalocari pentru diferite tipuri---////

void DistrugereTLDI(TLDI* lista) {

	TLDI aux;
	while (*lista) {
		aux = *lista;
		*lista = aux->urm;
		free(aux->info);		
		free(aux);
	}
}

void DistrugereProces(Procese* proces) {

	free((*proces)->stare);
	DistrugereTLDI(&(*proces)->stivaMemorie);
	free(*proces);
	proces = NULL;
}

void DistrugereMemorie(Memorie** memorie) {

	DistrugereTLDI(&(*memorie)->coadaDeAsteptare);
	DistrugereTLDI(&(*memorie)->listaPidDisponibil);
	DistrugereTLDI(&(*memorie)->coadaProceseTerminate);
	DistrugereTLDI(&(*memorie)->stivaMemorieProcese);
	DistrugereProces(&(*memorie)->procesInExecutie);
	free(*memorie);

}

//---------------------------------///
int main(int argc, char** argv) {

	FILE* in = fopen(argv[1], "r");
	FILE* out = fopen(argv[2], "wr");

	if (in == NULL || out == NULL) {
		printf("Eroare la fisiere\n");
		return -1;
	}
	else {
		int cuantaTimp;
		fscanf(in, "%d", &cuantaTimp);
		Memorie* memorie = CreareMemorie(); // creez memoria pentru procese
		int contorPid=1;

		if (memorie == NULL) {
			printf("Nu a resuit crearea memoriei\n");
			return -1;
		}

		CelulaStivaMemorie* celulaMemorie = CreareCelulaMemorie();	//creez elementul de tip celula de memorie pentru stiva
		PushStiva(&memorie->stivaMemorieProcese, celulaMemorie);	//adaug in stiva de memorie dimensiunea procesului IDLE

		char* numeActiune = (char*)malloc(sizeof(char) * 20);	//numele comenzii pe care il citesc din fisier

		while (!feof(in)) {
			fscanf(in, "%s", numeActiune);
			
			if (strcmp(numeActiune, "add") == 0) {
				add_remake(in,out, &memorie, &contorPid,cuantaTimp);
			}
			else if (strcmp(numeActiune, "print") == 0) {
				print(in, out, &memorie);
			}
			else if (strcmp(numeActiune, "get") == 0) {
				get(in, out, &memorie);
			}
			else if (strcmp(numeActiune, "push") == 0) {
				push(in, out, &memorie);
			}
			else if (strcmp(numeActiune, "pop") == 0) {
				pop(in, out, &memorie);
			}
			else if (strcmp(numeActiune, "run") == 0) {
				run_remake(in, out, &memorie, cuantaTimp);
			}
			else if (strcmp(numeActiune, "finish") == 0) {
				finish(in, out, &memorie);
				break;
			}
		}

		free(celulaMemorie);
		//DistrugereMemorie(&memorie);
		free(numeActiune);

		fclose(in);
		fclose(out);
	}
	

	return 0;
}
