Nume: Mateescu Viorel-Cristian
Grupă: 332CB"

# Tema 2 Marketplaces

Organizare
-
Implementarea temei foloseste dictionarele din Python:
- Operatiile pe lista si dictionare sunt in general Thread-safe;
- Pentru producatori si produsele pe care acestia le publica;
- Pentru produsele adaugate/sterse din cos de catre cumparatori;
- Este evitata problema accesului comun a mai multor producatori (concurenta);
- Deoarece fiecare producator acceseaza lista sa de produse;
- Pentru fiecare producator se va folosi o lista de produse.

Producator
-
- Se indentifica printr-un ID unic;
- Produce fiecare tip de produs pe rand, si publica acel produs;
- Daca marketplace-ul este plin, asteapta un anumit timp si verifica;
- Daca poate produce/publica alte produse;
- Publicarea si producerea de produse sunt realizate cat timp programul ruleaza.

Consumer
-
- Este indentificat printr-un ID unic (fiecare nou consumator detine un cos unic);
- Adauga produse in cos, numai daca se gasesc in marketplace;
- Daca produsul nu este accesibil, asteapta o perioada de timp si cauta din nou;
- Plaseaza comanda si afiseaza produsele cumparate.

Marketplace
-
- Implementarea temei este preponderenta in fisierul "Marketplace.py";
- Marketplace-ul (considerat si broker), prezinta doua tipuri de produse (ceai si cafea);
- Produse ce vor fi comercializate de catre producatori si cumparate de consumatori;
- Marketplace se asigura ca la final returneaza o lista cu produsele existente in magazin;
- La finalul programului, se va afisa cosul de cumparatori si produsele cumparate;

Pe langa constructorii prezenti, am adaugat urmatoarele definitii:
- "producer" - dictionar pentru (producator, produs), ce se produce/ce se vinde;
- "consumer" - dictionar pentru (cart, produs), ce se adauga in cos/ce se cumpara;
- "unique_cart" - dictionar pentru stergerea unui id producator.
- Dar si un "buffer reminder" pentru producator, cand readauga produsul in cos.
