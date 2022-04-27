Nume: MATEESCU Viorel-Cristian
Grupa: 332CB

# Tema 3 SO - Loader de executabile

#### search_for_segment(uintptr_t address)
* Parcurgem segmentul curent cu un iterator, pana gasim segmentul care contine adresa pe care o cautam.
* Functia returneaza segmentul in care se afla adresa data.

#### sigsegv_check (int, siginfo_t*, void*)
* Parametrii:
int signum - numarul de semnal;
siginfo_t* info - informatii despre semnal;
void* sigtext - contextul de executie.

* Implementare:
Folosim functia sigaction pentru a seta handler-ul pentru semnalul SIGSEGV.
* In cazul in care semnalul SIGSEGV este primit, se apeleaza functia sigsegv_check.
* Se verifica daca adresa se afla in segmentul curent, daca nu se verifica daca exista un segment.
* Se cauta segmentul din care face parte adresa respectiva.
* Se verifica daca pagina din care face parte adresa respectiva este mapata.
* Daca pagina nu este mapata, se verifica daca exista un segment de tip SEG_DATA(char *data) care sa contina pagina.
* Pagina este mapata cu "MAP_ANONYMOUS", deoarece file_size si pagina curenta
se afla intr-o zona din afara memoriei rezervate.
* Daca pagina este deja mapata, se ruleaza un handler de eroare pentru a nu accesa memorie intr-o zona neautorizata.
* La final se marcheaza pagina ca mapata.

#### so_init_loader
* Initializeaza biblioteca generala.
* Inregistreaza "pagefault handler" sub forma unei rutine pentru tratarea semnalului SIGSEGV (sau un handler de eroare).
* Se pastreaza o referinta catre rutina default, atunci cand este urmarit semnalul SIGSEGV.

#### so_execute
* Parseaza binarul care este specificat in "path".
* Initializeaza "Entry Point" din structura executabilului.
* Deschide fisierul in mod RDONLY, pentru a pastra un file descriptor.
* Bytes sunt setati pe 0, iar pentru fiecare segment se calculeaza numarul de pagini, alocandu-se memorie.
