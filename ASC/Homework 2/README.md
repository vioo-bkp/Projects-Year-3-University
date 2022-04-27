# Tema 2 - Matrix Multipication
Nume: Mateescu Viorel-Cristian

Grupa: 332CB

## Implementare
Tema va contine urmatoarele implementari:

- **blas** varianta cea mai optima utilizand functii din biblioteca "blas.h";
- **neopt** implementarea utilizand cunostiintele din liceu, tinand cont de faptul ca matricea A este superior triunghiulara;
- **opt** implementare optimizata a variantei **neopt**, folosind cunostiintele dobandite in cadrul _Laboratorului 5_ (Arhitectura Sistemelor de Calcul);

## **blas**
TODO

## **neopt**
Am implementat varianta **neopt**, utilizand cunostiintele din liceu, tinand cont de faptul ca matricea A este superior triunghiulara, astfel:

### Bt * B

Singura optimizare realizata a fost in cazul lui Bt (B transpus) prin care am parcurs linie-coloana, si coloana-linie pe matricea B, si am inlocuit elementele de pe aceeasi linie si coloana cu elementele de pe aceeasi coloana din matricea Bt.

### BAAt (B * A * A transpus)

Se fac inmultirile relevante petru o matrice superior triunghiulara;
Am parcurs ulimul for de la i la N.

### A * At

Trebuie sa tinem cont de elementele din ambele matrici A_tr si A, pentru a putea inmulti elementele corespunzatoare. Astfel parcurgerea trebuie sa fie de la elementul 0 la Minimul dintre Linii si Coloane.

## **opt**
Am implementat varianta **opt**, utilizand cunostiintele din laboratorul 5, astfel:

### Optimizare Bucle For

Am folosit pointer la matrice prin eliminare operatiilor de forma _i * N + j_, scazand numarul de operatii floting point

### Registrii de stocare
Definire de variabile in Registrii procesorului pentru a reduce timpul de acces la memorie.

## Resurse:
Laborator 5: https://ocw.cs.pub.ro/courses/asc/laboratoare/05

Cblas Wiki: https://www.netlib.org/lapack/explore-html/de/da0/cblas_8h_source.html
