== Sisteme de Operare ==
Tema 2 - Bibilioteca stdio

Nume: Mateescu Viorel-Cristian
Grupa: 332CB

Implementare:

so_fopen:
	Deschide fisierul aflat la pathname, folosind permisiunile de deschidere mode.
	Aceasta aloca o structura pe care o intoarce.
	Cand fisierul este deschis, pozitia cursorului este setata la inceputul fisierului.

so_fclose:
	Inchide fisierul primit ca parametru si elibereaza memoria din structura alocata.

so_popen:
	Deschide un proces alocat din structura.
	Creeaza un proces cu comanda primita ca parametru.
	In cazul in care comanda nu este valida, se intoarce NULL.
	Procesul copil redirectioneaza la stdin (daca avem write) si stdout (daca avem read)
	corespunzator file_descriptor si executa comanda primita.
	Procesul parinte creeaza o structura asociata procesului copil si o returneaza.

so_fileno:
	Returneaza descriptorul de fisier al streamului.

so_fflush:
	Daca bufferul nu este gol, se scrie continutul bufferului in fisierul cu descriptorul file_d,
	iar pozitia bufferului este setata pe 0.
	Se returneaza 0 in caz de succes, -1 in caz de eroare.

so_ftell:
	Returneaza pozitia curenta a streamului.

so_ferror:
	Returneaza 0 daca nu s-a produs nici o eroare, altfel returneaza 1.

so_fgetc:
	Returneaza caracterul de pe pozitia curenta a streamului,
	iar pozitia streamului este incrementata cu +1.
	Sterge datele in cazul unei scrieri anterioare, si stabileste READ ca ultima operatie.

so_fputc:
	Returneaza caracterul de pe pozitia curenta a streamului,
	iar pozitia streamului este incrementata cu +1.
	Sterge datele in cazul unei scrieri anterioare, si stabileste WRITE ca ultima operatie.
	Apelul de scriere este efectuat in cazul unui buffer gol.

so_feof:
	Returneaza 0 daca nu s-a ajuns la finalul fisierului, altfel returneaza 1.

so_fseek:
	Muta cursorul de pe pozitia specificata. Face flush pe buffer daca au fost efectuate scrieri,
	si resetaza indexul la buffer in cazul unui citiri.
	Mutarea pozitiei cursorului poate fi fata de inceputul fisierului sau de sfarsitul fisierului, prin lseek.

so_fwrite:
	bytes_to_write - numarul de bytes ce trebuie scrisi in fisier.
	bytes_written - numarul de bytes scrisi in fisier.
	While loop folosit pentru scrierea de bytes.
	Resetam indexul bufferului in cazul unui citiri.
	Copiem bytes in buffer cu memcpy. Setam sfarsitul bufferului ca fiind numarul de bytes copiati.
	Incrementam bytes_written cu numarul de bytes copiati si returnam bytes_written.

so_fread:
	Idem ca so_fwrite, dar invers.
