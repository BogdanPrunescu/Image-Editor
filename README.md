Prunescu Bogdan-Andrei 311CA
	
Programul se foloseste de 3 fisiere pentru a rula: image_editor.c, care
citeste de la tastatura comenziile si le interpreteaza, header.h, care este
un fisier header si functions.c in care sunt scrise functiile necesare rularii
programului.
	
In main, programul citeste linii de comanda pana cand numele comenzii
este "EXIT", caz in care programul s-a incheiat si memoria folosita este
eliberata. Daca comanda nu este "EXIT" se verifica daca comanda este valida si
se executa comanda corespunzatoare:
	
Cazul "LOAD":
Se cauta tipul fisierului pentru a stii cum trebuie citite
valorile matricei, se elibereaza memoria matricei vechi daca exista una si se
citesc datele din fisier.
	Complexitate timp: O(n * m), unde n si m sunt dimensiunile matricei.
	
Cazul "SELECT":
Se gaseste urmatorul cuvant dupa SELECT, iar daca acesta este "ALL"se
va selecta toata matrice, altfel se citesc coordonatele si se verifica daca
sunt valide(sunt numere sau sirul de caractere asociat nu este NULL). La final
se verifica daca coordonatele se afla in interiorul matricei.
	Complexitate timp: O(n), unde n este lungimea coordonatelor.
	
Cazul "ROTATE":
Se cauta mai intai unghiul, dupa care se verifica daca unghiul si
coordonatele sunt valide. Apoi unghiul este prelucrat pentru a se folosii doar
rotirea la stanga a unei matrici (daca unghiul este pozitiv, se scade cu 360).
Cat timp unghiul este mai mic decat 0 matricea se roteste prin transpunerea si
inversarea coloanelor ei, dupa care se creste unghiul cu 90. In plus,
operatiile se executa pe o matrice temporara care este alocata dinamic.
	Complexitate timp: O(n * m), unde n = x2 - x1 si m = y2 - y1.
	
Cazul "CROP"
Pentru cazul "CROP" se aloca dinamic o matrice temporara in care se
retin elementele din interiorul selectiei, dupa care se da free la matricea
veche si ii se atribuie un pointer la noua matrice.
	Complexitate timp: O(n * m), unde n = x2 - x1 si m = y2 - y1.
	
Cazul "APPLY"
La cazul "APPLY" am folosit 4 matrici cu valorile specifice fiecarui
tip de filtru si se afla parametrul comenzii. Se verifica daca parametrul
exista si este valid si daca imaginea pe care se va aplica comanda este
colorata, si, in functie de parametrul introdus se va apela functia
apply_filter() cu parametrii corespunzatori. Aceasta functie se foloseste de
doi vectori de directie pentru a trece prin toti vecinii unui pixel pentru a
aplica nucleul de imagine si a calcula valoarea noua a pixelului, pentru toate
canalele imaginii.
	Complexitate timp: O(n * m), unde n = x2 - x1 si m = y2 - y1.
