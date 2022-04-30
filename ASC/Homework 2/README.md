# Tema 2 - Matrix Multipication
Nume: Mateescu Viorel-Cristian

Grupa: 332CB

## **Implementare**
Tema va contine urmatoarele implementari:

- **blas** varianta cea mai optima utilizand functii din biblioteca "blas.h";
- **neopt** implementarea utilizand cunostiintele din liceu, tinand cont de faptul ca matricea A este superior triunghiulara;
- **opt** implementare optimizata a variantei **neopt**, folosind cunostiintele dobandite in cadrul _Laboratorului 5_ (Arhitectura Sistemelor de Calcul);

## **blas**
Sunt folosite functiile din biblioteca "blas.h" pentru a implementa varianta **blas**.
* folosim functia **dgemm** din biblioteca "blas.h" pentru a calcula matricea BtB si matricea AAt, deoarece functionalitatea lor este similara;
* adunarea finala dintre matricele **(B * AAt)** si **BtB** se face prin adunarea elementelor de pe aceeasi pozitie i si j;

### Timpi de rulare
* N = 200: 0.006657
* N = 400: 0.03916
* N = 800: 0.282491
* N = 1000: 0.550026
* N = 1200: 0.923991

## **neopt**
* Am implementat varianta **neopt**, utilizand cunostiintele din liceu, tinand cont de faptul ca matricea A este superior triunghiulara, astfel:
* Matricea B este inversata;
* Matricea A este inversata;
* Toate matricile sunt initializate cu 0;
* Matricea BtB este calculata;
* Matricea AAt este calculata;
* Matricea BtB si AAt sunt adunate;
* Complexitatea este O(N^3);

### Bt * B

* Singura optimizare realizata a fost in cazul lui Bt (B transpus) prin care am parcurs linie-coloana, si coloana-linie pe matricea B, si am inlocuit elementele de pe aceeasi linie si coloana cu elementele de pe aceeasi coloana din matricea Bt.

* Algoritmul de calculare a matricei BtB este:

```
for i = 1:N
	for j = 1:N
		for k = 1:N
			BtB(i, j) += B(k, i) * B(k, j);
		end
	end
end
```

### A * At

* Trebuie sa tinem cont de elementele din ambele matrici A_tr si A, pentru a putea inmulti elementele corespunzatoare. Astfel parcurgerea trebuie sa fie de la elementul 0 la N, iar j de la 0 la N.

* Algoritmul de calculare a matricei AAt este:

```
for i = 1:N
	for j = 1:N
		for k = 1:N
			AAt(i, j) += A(i, k) * A(j, k);
		end
	end
end
```

### BAAt (B * A * A transpus)

* Algoritmul de calculare a matricei BAAt este:

```
for i = 1:N
	for j = 1:N
		for k = 1:N
			BAAt(i, j) += B(i, k) * A(k, j);
		end
	end
end
```

### Timpi de rulare
* N = 200: 0.196897
* N = 400: 1.705507
* N = 800: 15.038904
* N = 1000: 26.075464
* N = 1200: 50.211803

## **opt_m**
* Am implementat varianta **opt**, utilizand cunostiintele din laboratorul 5, astfel:
* Algoritmul de calculare este idem ca cel de la varianta **neopt**
* Complexitatea este O(N^3);
* Singura diferenta apare la declararea de registrii;
* Tehnica folosita pentru a nu se accesa continuu memoria din matrice, ci doar o data la N operatii;

### Optimizare Bucle For
* Sunt declarati registrii pentru matricile BtB, AAt, BAAt;
* Declararea de _acumulator_ folosita pentru adunarea elementelor de pe aceeasi pozitie i si j;
* Declararea de _i_, _j_, _k_ folosita pentru parcurgerea matricilor;
* La final, acumulatorul este adunat la matricea calculata;

### Registrii de stocare
* Definire de variabile in Registrii procesorului pentru a reduce timpul de acces la memorie, prin adaugarea de registrii de stocare si in functia de calcul.

### Timpi de rulare
* N = 200: 0.033653
* N = 400: 0.620598
* N = 800: 6.921369
* N = 1000: 9.111224
* N = 1200: 20.340525

* Se observa ca timpul de rulare a **opt_m** este mai mic decat **neopt**, dar mai mult decat **blas**.
* Diferenta de rulare intre **neopt** si **opt_m** este mai mica decat pe baza algoritmului de calculare.
* Se observa o diferenta de ~2% intre **neopt** si **opt_m**.
* Graficele timpilor de rulare sunt mai bune pentru fiecare varianta.

## Resurse:
Laborator 5: https://ocw.cs.pub.ro/courses/asc/laboratoare/05

Cblas Wiki: https://www.netlib.org/lapack/explore-html/de/da0/cblas_8h_source.html
