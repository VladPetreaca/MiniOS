# MiniOS


## 1. Introducere

Să ​ne imaginăm că vrem să ne creăm propriul nostru sistem de operare numit MiniOS. Pentru
aceasta vom implementa următoarele două componente:

1. modul pentru gestionarea memoriei
2. modul pentru gestionarea proceselor

## 1.1 Modulul pentru gestionarea memoriei

Fiecare proces care este creat în cadrul unui sistem de operare are alocată o zonă de memorie
de dimensiune fixă și care este stabilită la crearea procesului. La crearea unui proces, acesta specifică
dimensiunea necesară a memoriei, iar acest sistem îi rezervă procesului, dacă este posibil, o zonă de
memorie de dimensiunea cerută.

## 1.2 Modulul pentru gestionarea proceselor

Sistemul nostru de operare va rula pe un procesor cu un singur nucleu, iar pentru a putea rula
mai multe procese în cadrul sistemului nostru de operare, avem nevoie de un planificator. Acest
modul se va ocupa de crearea și planificarea pentru rulare a proceselor.

## 3. Cerință

```
Scopul temei este de a implementa cele două module componente ale sistemului nostru de
operare. Vom caracteriza un proces prin următoarele atribute:
● **PID** ​, identificatorul unic al procesului în cadrul sistemului de operare:
○ este atribuit automat la crearea procesului de către modulul pentru gestiunea
proceselor. Valoare PID-ului este un număr natural cuprins între [1 - 32767]
○ PID-ul este atribuit ca fiind cel mai mic număr disponibil
○ dacă un proces a avut PID-ul X iar acesta și-a terminat execuția, PID-ul X devine
disponibil
● **prioritate** ​, este un ​număr natural cuprins intre [0 - 127]
○ trimisă ca parametru la crearea procesului

● timp de execuție ​, număr natural pozitiv reprezentabil pe 32 de biți
○ trimis ca parametru la crearea procesului
● începutul zonei de memorie
○ este alocată automat de către modulul pentru gestionarea memoriei
● dimensiunea zonei de memorie ​, număr natural pozitiv <= 3 * (1024 ^ 2)
○ trimis ca parametru la crearea procesului
```
## 3. Implementare

### 3.1 Sistemul de gestiune a memoriei

Din cauza alocărilor și dealocărilor repetate, are loc fragmentarea memoriei, caz ce poate duce
la imposibilitatea sistemului de operare de a mai crea procese noi. Astfel, modulul de gestiune a
memoriei va avea implementată o funcționalitate pentru defragmentarea memoriei. Aceasta
funcționalitate va fi folosită automat de fiecare dată când modulul nu reușește să aloce memorie
pentru un proces nou. Defragmentarea se va efectua prin mutarea memoriei proceselor în ordinea
crescătoare a PID-ului începând de la adresa 0. Astfel, memoria procesul cu PID-ul cel mai mic va fi
mutată la adresa 0, memoria procesului cu al doilea cel mai mic PID va fi mutată la adresa 0 +
dimensiunea memoriei procesului cu PID-ul cel mai mic, ș.a.m.d. Sistemul nostru de operare va
adresa o memorie totală de 4MiB, zona din intervalul [3MiB, 4MiB] fiind rezervată de către sistem,
restul memoriei de la [0MiB la 3MiB) fiind folosită pentru memoria proceselor.
[0MiB....................................................................................3MiB).................4MiB]

Sistemul va funcționa după următoarele reguli:
```
● va aloca pentru procesul nou creat o zonă de memorie aflată la cea mai mica adresă
disponibilă
● în cazul în care nu se poate aloca memorie, se rulează o defragmentare și se reia procesul;
dacă nici după defragmentare nu se poate aloca memorie pentru proces, sistemul de gestiune a
memoriei va afișa mesajul ​"Cannot reserve memory for PID <PID>.\n"​.
● toată memoria fiecărui proces va fi de tip stivă
● fiecare proces va avea dreptul de a face push / pop pe stiva proprie
● dacă procesul are stiva plină și face push, sistemul va afișa mesajul ​"Stack overflow PID <PID>.\n"
● dacă procesul are stiva goală și face pop, sistemul va afișa mesajul ​ " ​Empty stack PID <PID>.\n"

```

### 3.2 Sistemul de gestiune a proceselor

Acest sistem are o coadă de așteptare în care sunt ținute procesele care așteaptă eliberarea
procesorului pentru a putea rula. Un proces se poate afla în una din următoarele trei stări:
**_● waiting_**

#### ○ procesul se află în ​ coada de așteptare

**_● running_**
○ rulează activ pe procesor
**_● finished_**
○ procesul se află în ​ **_coada de procese terminate_**
Sistemul funcționează după următoarele reguli:
● are definită o cuantă de timp reprezentând timpul maxim de rulare continuă a unui proces
○ dimensiunea unei cuante este de T milisecunde (ms), T va fi citit din fișierul de
intrare.

#### ● procesele din ​ coada de a ș teptare ​ sunt ordonate astfel:

```
○ descrescător după prioritate
○ crescător după timp de execuție rămas, în caz de priorități egale
○ crescător după PID, în caz de prioritate și timp de execuție rămas egale.
```
#### ● procesele din ​ coada finished ​sunt ordonate astfel:

```
○ procesul care și-a terminat primul execuția este primul în coadă
○ procesul care și-a terminat ultimul execuția este ultimul în coadă
○ se alege pentru rulare primul proces din coada de așteptare
○ după expirarea cuantei de timp a procesului curent:
○ dacă nu mai sunt alte procese în sistem, acesta își continuă rularea
```
####● altfel, procesul este trecut din starea ​ running ​ în starea ​ waiting

```
○ primul proces din coada de așteptare este trecut din starea ​ waiting în starea ​ running,
urmând ca apoi să fie scos din coadă și să înceapă execuția
○ este adăugat în coada de așteptare procesul care a trecut în starea ​ waiting ​.
```
####● dacă un proces își termina execuția:

```
○ i se eliberează memoria, urmând ca aceasta să devină disponibilă pentru procese noi
○ PID-ul asociat procesului devine disponibil și poate fi atribuit proceselor create
ulterior
○ acesta este mutat în ​ coada de procese terminate
○ primul proces din coada de așteptare este trecut în starea ​ running
```
#### ● dacă nu mai sunt procese în sistem, va trece spre rulare procesul ​ Idle (PID = 0) ​care este un

```
○ proces implicit al sistemului de operare. Acest proces rulează până când apare un proces în
sistem, caz în care este înlocuit la rulare imediat de acesta.
```
#### ● la crearea unui proces acesta este inserat în ​ coada de așteptare sau este pus în direct în starea

#### running ​în cazul în care în sistem există doar procesul ​ Idle ​.

#### ● procesul ​ Idle ​nu are stivă


## 4. Descrierea opera ț iilor ș i a datelor de intrare

```
Rezolvarea temei va consta în efectuarea unui set de operații descrise în fișierul de intrare.
```
#### A. Adăugare proces în coada de așteptare

#### Sintaxă ​ : ​ add <mem_size_in_bytes> <exec_time_in_ms> <priority>

#### Mod de func ț ionare: Creează un proces nou cu atributele date și îl inserează în coada de

așteptare după următoarele criterii:
○ descrescător după prioritate
○ crescător după timp de execuție rămas, în caz de priorități egale
○ crescător după PID, în caz de prioritate și timp de execuție rămas egale.
În caz de succes afișează un mesaj de forma:
Process created successfully: PID: <PID>, Memory starts at
<hex_memory_address>.\n

#### Exemplu: ​add 1048576 3000 7​ /* este creat un proces cu o memorie de 1MB, timp de rulare

#### 3s ș i prioritate 7 */

Process created successfully: PID: 1, Memory starts at 0x3c0.
Se garantează că la orice moment de timp va fi disponibil cel puțin un PID pentru un proces nou. Dacă

#### în sistem există doar procesul ​ Idle ​, procesul nou creat va fi trecut direct în starea ​ running ​, fără a mai fi

adăugat în coada de așteptare.

#### B. Determinarea stării unui proces

```
Sintaxă: ​ ​get <PID>
```
#### Exemplu: ​ ​get 1023

#### Mod de funcționare ​: Comanda cauta toate procesele cu identificatorul <PID> și le afișează informatul de mai jos:

#### ● proces în starea ​ running ​:

```
Process <PID> is running (remaining_time =
timp_ramas).\n
```
#### ● proces în starea ​ waiting ​:

```
Process <PID> is waiting (remaining_time =
timp_ramas).\n
```
#### ● proces în starea ​ finished ​:

```
Process <PID> is finished.\n
● În cazul în care in sistem nu a existat niciun proces cu PID-ul dat, se va afișa mesajul:
Process <PID> not found.\n
```
#### Prima data se verifica procesul din starea ​ running ​, apoi procesele din ​ coada de așteptare ​, iar apoi

#### procesele din ​ coada de procese terminate ​. Cele doua cozi se vor parcurge de la primul element spreultimul.

#### C. Salvarea datelor pe stivă

#### Sintaxă ​ : ​ ​push <PID> <4_signed_bytes_data>

#### Exemple ​ : ​push 1023 ​ 2147483647

#### ​push 1024 -

#### Mod de func ț ionare ​: Pune pe stiva procesului 4 octeți. În caz de stivă plină afișează mesajul:

```
"Stack overflow PID <PID>.\n"​ și nu va adaugă nimic în stivă. În cazul în care
nu există niciun proces cu PID-ul dat se va afișa mesajul: ​"PID <PID> not
found.\n"​.
```
#### D. Ș tergerea datelor de pe stivă

#### Sintaxă ​ : ​ ​pop <PID>

#### Exemplu ​ : ​pop 1023

#### Mod de func ț ionare ​: Elimină primii 4 octeți din vârful stivei procesului. În caz de stivă goală

#### afișează mesajul: ​ " ​Empty stack PID <PID>.\n". ​În cazul în care nu există niciun

```
proces cu PID-ul dat se va afișa mesajul: ​"PID <PID> not found.\n"​.
```
#### E. Afi ș area stivei unui proces

#### Sintaxă ​: ​print stack <PID>

#### Mod de func ț ionare ​: Afișează doar conținutul plin al stivei în felul următor:

```
Stack of PID <PID>: 4_byte1 4_byte2 ... 4_byteN.\n​,
unde:
4_byte1 sunt primii 4 octeți de la baza stivei, iar 4_byteN sunt ultimii 4 octeți de la vârful
stivei.
```
#### Exemplu ​:

```
Stack of PID 1023: 5353 4324122 ​ 2147483647 ​ ​-2147483648.
```
#### În caz de stivă goală afișează mesajul: ​ " ​Empty stack PID <PID>.\n".

#### F. Afi ș area cozii de a ș teptare

#### Sintaxă ​: ​print waiting

#### Mod de funcționare ​: Afișează coada de așteptare a planificatorului la momentul curent în

```
felul următor:
Waiting queue:\n
[(<PID1>: priority = <prioritate1>, remaining_time =
<timp_execuție_rămas1>),\n
...,
(<PIDN>: priority = <prioritateN>, remaining_time =
<timp_execuție_rămasN>)]\n​ ,
unde PID1 corespunde primului proces aflat în coada de așteptare a planificatorului, iar PIDN
corespunde ultimului proces din coada de așteptare.
```
#### Exemplu ​:

```
Waiting queue:
[(1452: priority = 72, remaining_time = 100),
(1521: priority = 55, remaining_time = 250),
```

```
(1245: priority = 55, remaining_time = 320),
(1352: priority = 23, remaining_time = 500)]
```
#### G. Afi ș area cozii de procese terminate

#### Sintaxă ​: ​print finished

#### Mod de func ț ionare ​: Afișează coada de procese terminate astfel:

```
Finished queue:\n
```
#### [(<PID1>: priority = <prioritate1>, ​executed_time =

```
<timp_execuție1>​ )\n,
...,
```
#### (<PIDN>: priority = <prioritateN>, ​executed_time =

#### <timp_execuțieN>​ )]\n ​,

```
unde PID1 corespunde primului proces aflat în coada de așteptare a planificatorului, iar PIDN
corespunde ultimului proces din coada de așteptare.
```
#### Exemplu ​:

```
Finished queue:
[(1452: priority = 72, executed_time = 134),
(1521: priority = 55, executed_time = 14),
```
#### (1245: priority = 55, executed_time = 156)]

#### H. Executarea unui număr de unită ț i de timp

#### Sintaxă ​:​ ​run <număr_unități_timp>

#### Mod de func ț ionare ​:

```
Execută următoarele ​număr_unități_timp​ de timp pe procesor.
```
#### În cazul în care în sistem nu există niciun proces, se va rula procesul ​ Idle ​ ​care nu face

```
nimic.
```
#### I. Terminarea execu ț iei tuturor proceselor

#### Sintaxă ​: ​finish

#### Mod de func ț ionare ​: Se continuă execuția până când toate procesele ajung în coada de

```
procese terminate.
La finalul execuției, se afișează timpul total necesar pentru terminarea proceselor neterminate
```
#### în momentul apelării comenzii ​ finish ​:

```
Total time: <total_execution_time>\n
```
#### În datele de intrare va exista maxim un apel al comenzii ​ finish ​, iar după acesta nu vor mai

```
exista alte comenzi.
```
