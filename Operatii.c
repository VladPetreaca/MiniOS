#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "lib.h"

/* Petreaca Vlad-Florin 323CB*/

void add_remake(FILE* in, FILE* out, Memorie** memorie,int* contorPid,int cuanta) {
	int memSize, execTime, priorty;
	fscanf(in, "%d%d%d", &memSize, &execTime, &priorty);

	Procese procesNou = CreareProces(memSize, execTime, priorty);
	if (procesNou == NULL) {
		printf("Nu S-a putut crea procesul in operatia de adaugare\n");
	}
	else {
		CelulaStivaMemorie* celulaMemorie = (CelulaStivaMemorie*)seekStiva((*memorie)->stivaMemorieProcese);//iau ultimul proces adaugat in memorie
		procesNou->back_in_time = cuanta;	//salvez pentru procesul respectiv timmpul maxim de rulare
		if (celulaMemorie->sfarsit_memorie + procesNou->dimensiuneMemorie <= (*memorie)->dimensiuneMemorieMax) {//am loc sa pun elementul in memorie
			if ((*memorie)->listaPidDisponibil == NULL) {	//daca nu este niciun proces terminat
				procesNou->PID = (*contorPid);	//iau pidul la care am ajuns
				(*contorPid)++;
			}
			else {
				procesNou->PID = takeFirst(&(*memorie)->listaPidDisponibil);//iau cel mai mic pid din lista de piduri disponibile			
			}
			int check = cauataMemorie_AdaugareProces(&(*memorie), procesNou);//verific daca pot adauga procesul in memorie
			if (check == -1) {//nu am niciun loc gol de memorie intre 2 procese

				CelulaStivaMemorie* memorieNoua = CreareCelulaMemorie();	//creez memoria pentru proces
				memorieNoua->start_memorie = celulaMemorie->sfarsit_memorie;//procesul incepe de unde se termina ultimul
				memorieNoua->dimensiune_memorie_proces = procesNou->dimensiuneMemorie;//are dimensiunea procesului
				memorieNoua->sfarsit_memorie = memorieNoua->start_memorie + memorieNoua->dimensiune_memorie_proces;
				//memoria procesului se termina la (inceput_memorie + dimensiunea memoriei lui)
				memorieNoua->memorie_ramasa = 0;//toata memoria dispoibila este ocupata
				memorieNoua->PID = procesNou->PID;//are pidul procesului creat
				procesNou->startMemorie = memorieNoua->start_memorie;//unde incepe procesul sa scrie in memorie
				PushStiva(&(*memorie)->stivaMemorieProcese, memorieNoua);//pun elementul de tip memorie pe stiva de memorie
			}
			else {
				procesNou->startMemorie = check;//am gasit un loc gol si pun procesul in acel spatiu
			}
			
					
			if ((*memorie)->procesInExecutie == NULL) {//daca nu am proces in executie inseamna ca coada de asteptare este nula
				procesNou->stare = "running";
				(*memorie)->procesInExecutie = procesNou; // procesul este pus direct in executie
			}
			else {
				InserareOrdonataCoada(&(*memorie)->coadaDeAsteptare, procesNou, comparareProces);//inserez in coada de asteptare procesul
																								//conform cerintelor
			}

			fprintf(out, "Process created successfully: PID: %d, Memory starts at 0x%x.\n", procesNou->PID, procesNou->startMemorie);
		}
		else {//nu mai am spatiu sa pun procesul nou in varful stivei
			int ok = 0;
			if ((*memorie)->listaPidDisponibil == NULL) {	//setex pridul procesului
				procesNou->PID = (*contorPid);
				(*contorPid)++;
			}
			else {
				procesNou->PID = takeFirst(&(*memorie)->listaPidDisponibil);
				int* pid = (int*)malloc(sizeof(int));
				*pid = procesNou->PID;			
				ok = 1;//flag
			}
	
			int check = cauataMemorie_AdaugareProces(&(*memorie), procesNou);//vad daca am un spatiu gol de memorie intre procese
			if (check != -1) {	//am spatiu si pun procesul pe locul respectiv

				procesNou->startMemorie = check;	//procesul incepe sa se scrie in memorie unde returneaza functia
				fprintf(out, "Process created successfully: PID: %d, Memory starts at 0x%x.\n", procesNou->PID, procesNou->startMemorie);
				

				if ((*memorie)->procesInExecutie == NULL) {
					procesNou->stare = "running";
					(*memorie)->procesInExecutie = procesNou; // procesul este prus direct in executie
				}
				else {
					InserareOrdonataCoada(&(*memorie)->coadaDeAsteptare, procesNou, comparareProces);//procesul este pus in coada de astepare
				}
			}
			else {//daca nu am spatiu, readuc pidul la forma la care l-am lasat
				if (ok == 0) {
					(*contorPid)--;
				}
				else {
					int* pid = (int*)malloc(sizeof(int));
					*pid = procesNou->PID;	
					(*memorie)->listaPidDisponibil = InserareOrdonataLista((*memorie)->listaPidDisponibil, pid, compararePID);
					//inserez ordonat pidul in lista de piduri disponibile
				}
				fprintf(out, "Cannot reserve memory for PID %d.\n", procesNou->PID);//nu pot crea procesul in memorie
			}
			
		}
	}
}

void print(FILE* in, FILE* out, Memorie** memorie) {

	char* optiune = (char*)malloc(sizeof(char) * 20);
	fscanf(in, "%s", optiune);	//citesc ce trebuie sa afisez

	if (strcmp(optiune, "waiting") == 0) { //daca print-ul are ca argument coada de asteptare
		fprintf(out, "Waiting queue:\n[");
		TLDI coadaAux = NULL;
		int cont = 0;
		while ((*memorie)->coadaDeAsteptare != NULL) {			
			void* element = PopCoada(&(*memorie)->coadaDeAsteptare); //iau elementul	
			if (strcmp((*(Procese)element).stare, "waiting") == 0) {				
				cont++;			//numar procese cu starea waiting
			}
			PushCoada(&coadaAux, element);//le introduc in coada auxiliara
		}
		int cont2 = 0;
		while (coadaAux != NULL) {
			void* element = PopCoada(&coadaAux);
			if (strcmp((*(Procese)element).stare, "waiting") == 0) {
				cont2++;	//numar procesele cu starea waiting
			}
			AfisareProces(out, element, cont,cont2,"waiting");//afisez procesul
			//cand cont2 < cont inseamna ca mai am procese de afisat iar cand sunt egale nu mai am procese

			PushCoada(&(*memorie)->coadaDeAsteptare, element);//le reintroduc in coada de asteptare
		}
		fprintf(out, "]\n");
	}
	else if (strcmp(optiune, "finished") == 0) {	//afisez elementele din coada de procese terminate similar cu coada de procese in asteptare

		fprintf(out, "Finished queue:\n[");
		TLDI coadaAux = NULL;
		int cont = 0;
		while ((*memorie)->coadaProceseTerminate != NULL) {
			void* element = PopCoada(&(*memorie)->coadaProceseTerminate);
			PushCoada(&coadaAux, element);
			cont++;
		}
		int cont2 = 0;
		while (coadaAux != NULL) {
			void* element = PopCoada(&coadaAux);
			cont2++;
			AfisareProces(out, element, cont, cont2,"finished");
			PushCoada(&(*memorie)->coadaProceseTerminate, element);
		}
		fprintf(out, "]\n");
	}
	else if (strcmp(optiune, "stack") == 0) { //daca print-ul are ca argument o anumita stiva
		int pid;
		fscanf(in, "%d", &pid);
		TLDI stivaAux = NULL;
		TLDI coadaAux = NULL;
		int ok = 0;//flag

		if ((*memorie)->procesInExecutie != NULL && (*memorie)->procesInExecutie->PID == pid) {//verific daca procesul cautat se afla in rulare
			ok = 1;
			if ((*memorie)->procesInExecutie->stivaMemorie == NULL) {	//nu are elemente in stiva
				fprintf(out, "Empty stack PID %d.\n", (*memorie)->procesInExecutie->PID);
			}
			else {
				int cont = 0;
				while ((*memorie)->procesInExecutie->stivaMemorie){
					void* aux = PopStiva(&(*memorie)->procesInExecutie->stivaMemorie);
					PushStiva(&stivaAux, aux);	
					cont++; //contorizez numarul de elemente al stivei procesului
				}
				fprintf(out, "Stack of PID %d: ", (*memorie)->procesInExecutie->PID);
				int cont2 = 0;
				while (stivaAux) {
					void*aux = PopStiva(&stivaAux);
					cont2++;
					AfiInt(out, aux, cont, cont2);	//afisez elementele
					//folosesc cont si cont2 la fel ca la afisarile celor 2 cozi
					PushStiva(&(*memorie)->procesInExecutie->stivaMemorie, aux);//reintroduc elementele in stiva procesului
				}
			}
		}
		else {
			while ((*memorie)->coadaDeAsteptare) {	//daca procesul nu este in rulare si se afla in coada de asteptare
				void* element = PopCoada(&(*memorie)->coadaDeAsteptare);
				if ((*(Procese)element).PID == pid) {
					ok = 1;
					if ((*(Procese)element).stivaMemorie == NULL) {
						fprintf(out, "Empty stack PID %d.\n", (*(Procese)element).PID);
					}
					else {
						int cont = 0;
						while ((*(Procese)element).stivaMemorie){
							void* aux = PopStiva(&(*(Procese)element).stivaMemorie);
							PushStiva(&stivaAux, aux);
							cont++;
						}
						fprintf(out, "Stack of PID %d: ", (*(Procese)element).PID);
						int cont2 = 0;
						while (stivaAux) {	
							void*aux = PopStiva(&stivaAux);
							cont2++;
							AfiInt(out, aux, cont, cont2);
							PushStiva(&(*(Procese)element).stivaMemorie, aux); //reintroduc elementele in stiva procesului
						}
					}
				}
				PushCoada(&coadaAux, element);
			}
			while (coadaAux) {	//reintrudoc elementele in coada de astepare
				void* aux = PopCoada(&coadaAux);
				PushCoada(&(*memorie)->coadaDeAsteptare, aux);
			}
		}
		if (ok == 0) {	//canf flag-ul este 0 inseamna ca nu am niciun proces cu pid-ul respectiv
			fprintf(out, "PID %d not found.\n", pid);
		}
	}
}

void get(FILE*in, FILE* out, Memorie** memorie) {

	int pid;
	fscanf(in, "%d", &pid);
	TLDI coadaAux=NULL;
	int ok = 0;//flag
	if ((*memorie)->procesInExecutie != NULL && (*memorie)->procesInExecutie->PID == pid) {	//verifi daca exista acel proces in executie
		ok = 1;
		fprintf(out, "Process %d is %s (remaining_time: %d).\n", 
			(*memorie)->procesInExecutie->PID, (*memorie)->procesInExecutie->stare, (*memorie)->procesInExecutie->timpDeExecutie);
	}
	else {
		while ((*memorie)->coadaDeAsteptare) {
			void* element = PopCoada(&(*memorie)->coadaDeAsteptare);
			if ((*(Procese)element).PID == pid) {	//au acelasi pid si este in coada de asteptare
				fprintf(out, "Process %d is %s (remaining_time: %d).\n",
					(*(Procese)element).PID, (*(Procese)element).stare, (*(Procese)element).timpDeExecutie);
				ok = 1;
			}
			PushCoada(&coadaAux, element);
		}

		while (coadaAux) {
			void* element = PopCoada(&coadaAux);	//reintroduc elementele in coada de procese in asteptare
			PushCoada(&(*memorie)->coadaDeAsteptare, element);
		}
	}	
	coadaAux = NULL;
	while ((*memorie)->coadaProceseTerminate) {
		void* element = PopCoada(&(*memorie)->coadaProceseTerminate);
		if ((*(Procese)element).PID == pid) {	//au acelasi pid si este in coada de procese terminate
			fprintf(out, "Process %d is %s.\n", (*(Procese)element).PID, (*(Procese)element).stare);
			ok = 1;
		}
		PushCoada(&coadaAux, element);
	}
	
	while (coadaAux) {
		void* element = PopCoada(&coadaAux);	//reintroduc elementele in coada de procese terminate
		PushCoada(&(*memorie)->coadaProceseTerminate, element);
	}

	if (ok == 0) { //cand flag-ul nu este setat,nu am niciun proces cu pid-ul respectiv
		fprintf(out, "Process %d not found.\n", pid);
	}
}

void push(FILE* in, FILE* out, Memorie** memorie) {

	int pid, valoare;
	fscanf(in, "%d%d", &pid, &valoare);
	int* valoareAux = (int*)malloc(sizeof(int));
	*valoareAux = valoare;

	int ok = 0; //flag
	TLDI coadaAux = NULL;

	if ((*memorie)->procesInExecutie != NULL && (*memorie)->procesInExecutie->PID == pid) {
		ok = 1;
		if ((*memorie)->procesInExecutie->dimensiuneMemorie >= 4) {
			(*memorie)->procesInExecutie->dimensiuneMemorie -= 4; //dimensiunea memoriei scade cu 4 octeti fiind adaugat un element
			PushStiva(&(*memorie)->procesInExecutie->stivaMemorie, valoareAux); // pun elementul in stiva procesului
		}
		else {
			fprintf(out, "Stack overflow PID %d.\n", (*memorie)->procesInExecutie->PID); //nu mai am spatiu in stiva procesului
		}
	}
	else {
		while ((*memorie)->coadaDeAsteptare) {
			void* element = PopCoada(&(*memorie)->coadaDeAsteptare);

			if ((*(Procese)element).PID == pid) {
				if ((*(Procese)element).dimensiuneMemorie >= 4) {
					(*(Procese)element).dimensiuneMemorie -= 4; //dimensiunea memoriei scade cu 4 octeti fiind adaugat un element
					PushStiva(&(*(Procese)element).stivaMemorie, valoareAux); // pun elementul in stiva procesului
				}
				else {
					fprintf(out, "Stack overflow PID %d.\n", (*(Procese)element).PID); //nu mai am spatiu in stiva procesului
				}
				ok = 1;
			}
			PushCoada(&coadaAux, element);
		}

		while (coadaAux) {		//reintroduc elementele in coada de asteptare
			void* aux = PopCoada(&coadaAux);
			PushCoada(&(*memorie)->coadaDeAsteptare, aux);
		}
	}
	

	if (ok == 0) { //daca falg-ul nu este setat nu exista acel proces
		fprintf(out, "PID %d not found.\n", pid);
	}
}

void pop(FILE* in, FILE* out, Memorie** memorie) {

	int pid;
	fscanf(in, "%d", &pid);
	TLDI coadaAux=NULL;
	int ok = 0;//flag

	if ((*memorie)->procesInExecutie != NULL && (*memorie)->procesInExecutie->PID == pid) {
		ok = 1;
		if ((*memorie)->procesInExecutie->stivaMemorie == NULL) {	//verific daca procesul care ruleaza are stiva goala
			fprintf(out, "Empty stack PID %d.\n", (*memorie)->procesInExecutie->PID);
		}
		else {
			void* remove = PopStiva(&(*memorie)->procesInExecutie->stivaMemorie); //elimin elementul din stiva
			(*memorie)->procesInExecutie->dimensiuneMemorie += 4; // creste memoria stivei procesului cu 4 octeti
			free(remove);
		}
	}
	else {
		while ((*memorie)->coadaDeAsteptare) {	// acelasi lucru ca la procesul care ruleaza fac si pentru coada de astepare
			void* element = PopCoada(&(*memorie)->coadaDeAsteptare);

			if ((*(Procese)element).PID == pid) {
				if ((*(Procese)element).stivaMemorie == NULL) {
					fprintf(out, "Empty stack PID %d.\n", (*(Procese)element).PID);
				}
				else {
					void* remove = PopStiva(&(*(Procese)element).stivaMemorie);
					(*(Procese)element).dimensiuneMemorie += 4;
					free(remove);
				}
				ok = 1;
			}
			PushCoada(&coadaAux, element);
		}

		while (coadaAux) {	//reintroduc elementele inapoi in coada de asteptare
			void* aux = PopCoada(&coadaAux);
			PushCoada(&(*memorie)->coadaDeAsteptare, aux);
		}
	}
	
	if (ok == 0) { //nu am flag-ul setat deci nu exista procesul
		fprintf(out, "PID %d not found.\n", pid);
	}
}

void run_remake(FILE* in, FILE* out, Memorie** memorie, int cuanta) {
	int time_for_execution;
	fscanf(in, "%d", &time_for_execution);	//citesc cat timp are comanda run de executat
	int cuantaAux;

	if ((*memorie)->procesInExecutie != NULL) { //trebuie sa am un proces care sa ruleze
		while (time_for_execution > 0) {
			if(time_for_execution < (*memorie)->procesInExecutie->back_in_time) {
				cuantaAux = time_for_execution;
			}
			else {
				cuantaAux = (*memorie)->procesInExecutie->back_in_time;
			}
			//retin in cuanta auxiliara cat mai are de executat procesul pana isi atinge cuanta
			//un proces isi atinge cuanta cand campul back_in_time din structura lui este 0

			if ((*memorie)->procesInExecutie->timpDeExecutie <= cuantaAux) { //procesul are acelasi timp sau mai putin de executat decat cuanta

				(*memorie)->procesInExecutie->executed_time += (*memorie)->procesInExecutie->timpDeExecutie;
				//adaug la timpul total executat de proces cat a mai ramas la timpul de executie pana sa devina null

				time_for_execution -= (*memorie)->procesInExecutie->timpDeExecutie;	//actualize cand timp trebuie sa mai execut
				cuantaAux -= (*memorie)->procesInExecutie->timpDeExecutie;

				(*memorie)->procesInExecutie->timpDeExecutie = 0;	//procesul se termina
				(*memorie)->procesInExecutie->stare = "finished";
				int* pid = (int*)malloc(sizeof(int));
				*pid = (*memorie)->procesInExecutie->PID;

				int check = eliberareMemorieProces(&(*memorie), *pid);//eliberez memoria procesului respectiv din stiva de memorie
				if (check == 0) {
					printf("Nu a gasit sa elimine procesul cautat");
				}

				rearanjareMemorie(&(*memorie)); //rearanjez memoria stivei astfel:
				//daca am mai multe locuri libere de memorii cu pid -1 le adun si formez un singur spatiu liber de memorie 
				//avand dimensiunea maxima in care poate intra un proces, suma tuturor dimensiunilor ramase ale locurilor libere

				(*memorie)->listaPidDisponibil = InserareOrdonataLista((*memorie)->listaPidDisponibil, pid, compararePID);				
				//adaug pid-ul acum disponibil in lista de pid-uri disponibile

				PushCoada(&(*memorie)->coadaProceseTerminate, (*memorie)->procesInExecutie); //pun procesul in coada celor terminate
				(*memorie)->procesInExecutie = NULL; // nu se mai executa niciun proces

				if ((*memorie)->coadaDeAsteptare == NULL) {					
					break;//nu mai am ce procese sa execut in continuare			
				}
				else{
					(*memorie)->procesInExecutie = (Procese)PopCoada(&(*memorie)->coadaDeAsteptare); //iau urmatorul proces din coada
					(*memorie)->procesInExecutie->stare = "running";	//intra direct in rulare
					(*memorie)->procesInExecutie->timpDeExecutie -= cuantaAux;	//execut pe el cat a mai ramas din time
					(*memorie)->procesInExecutie->executed_time += cuantaAux;
					(*memorie)->procesInExecutie->back_in_time -= cuantaAux;	//retin aici cat mai am executat
					time_for_execution -= cuantaAux;
				}
			}
			else { //procesul are mai mult timp de executat decat cuanta
				(*memorie)->procesInExecutie->timpDeExecutie -= cuantaAux;	//actualizez timpii de executie si cat mai are procesul de executat
				(*memorie)->procesInExecutie->executed_time += cuantaAux;
				(*memorie)->procesInExecutie->back_in_time -= cuantaAux;
				if ((*memorie)->procesInExecutie->back_in_time == 0) {	//daca back_in_time este 0 inseamna ca procesul si-a executat cuanta
					(*memorie)->procesInExecutie->back_in_time = cuanta;//reinitializez campul cu cuanta

					if ((*memorie)->coadaDeAsteptare == NULL){ //procesul care ruleaza este singurul proces din memorie
						time_for_execution -= cuantaAux;//actualizez timpul de rulare
						continue;
					}

					else {
						(*memorie)->procesInExecutie->stare = "waiting";	//bag procesul in starea waiting
						Procese procesAux = (*memorie)->procesInExecutie;	
						(*memorie)->procesInExecutie = (Procese)PopCoada(&(*memorie)->coadaDeAsteptare);
						(*memorie)->procesInExecutie->stare = "running";	//iau primul proces din coada si il trec in running
						InserareOrdonataCoada(&(*memorie)->coadaDeAsteptare, procesAux, comparareProces);//reintroduc ordonat in coada procesul anterior
					}

				}
				//daca back_in time nu este 0 procesul ramane in continuare in starea running pana isi executa cuanta
				time_for_execution -= cuantaAux;
			}
		}
	}
}

void finish(FILE* in, FILE* out, Memorie** memorie) {

	TLDI coadaAux = NULL;
	int need_time = 0;
	while ((*memorie)->coadaDeAsteptare) {	//adun in variabila need_time tot timpul proceselor care mai au timp de executie
		void* element = PopCoada(&(*memorie)->coadaDeAsteptare);
		need_time += (*(Procese)element).timpDeExecutie;
		PushCoada(&coadaAux, element);
	}

	while (coadaAux) {	//reintroduc elementele in coada
		void* aux = PopCoada(&coadaAux);
		PushCoada(&(*memorie)->coadaDeAsteptare, aux);
	}
	need_time += (*memorie)->procesInExecutie->timpDeExecutie; //aduc si timpul procesului de executie daca exista
	fprintf(out,"Total time: %d\n", need_time);
}