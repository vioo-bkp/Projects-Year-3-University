# Tema 3 SO - Loader de executabile

# Author
- Name: Mateescu Viorel-Cristian
- Group: 323CB

### Implementare:
- Implementarea functiilor necesare pentru rezolvarea temei.
- Utilizarea laboratoarelor 4 si 5 din cursul de SO (Sisteme de Operare).

#### search_for_segment(uintptr_t address)
- Parcurgerea segmentului curent pentru a gasi segmentul care contine adresa cautata.
- Functia returneaza segmentul in care se afla adresa data.

#### sigsegv_check (int, siginfo_t*, void*)
- Implementarea handler-ului pentru semnalul SIGSEGV.
- Verificarea daca adresa se afla in segmentul curent si, daca nu, cautarea segmentului corespunzator.
- Verificarea daca pagina este mapata si, daca nu, cautarea unui segment de tip SEG_DATA care sa contina pagina.
- Maparea paginii si marcarea acesteia ca mapata.

#### so_init_loader
- Initializarea bibliotecii generale.
- Inregistrarea unui handler pentru semnalul SIGSEGV.
- Salvarea referintei catre handler-ul default.

#### so_execute
- Parsarea binarului specificat in "path".
- Initializarea "Entry Point" din structura executabilului.
- Deschiderea fisierului in mod RDONLY si calcularea numarului de pagini pentru fiecare segment.

### Referinte:
- [Laborator 4 SO](https://ocw.cs.pub.ro/courses/so/laboratoare/laborator-04)
- [Laborator 5 SO](https://ocw.cs.pub.ro/courses/so/laboratoare/laborator-05)
