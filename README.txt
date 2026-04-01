Compilare:
cc -O2 -std=c11 proiect.c -o proiect

Ce face codul:
- testeaza dimensiunile 10, 20, 50, 100, 1000, 10000, 100000, 1000000, 10000000 si 100000000
- foloseste liste aleatoare, sortate, invers sortate, aproape sortate si plate
- genereaza datele de test pe loc la fiecare rulare
- ruleaza Bubble Sort, Selection Sort, Insertion Sort, Quick Sort, Merge Sort, Heap Sort, Counting Sort si Radix Sort
- verifica dupa fiecare rulare daca lista este sortata corect
- salveaza doar rezultatele in rezultate/rezultate.csv
- salveaza analiza in rezultate/analiza.txt

Observatii:
- Bubble Sort, Selection Sort si Insertion Sort sunt sarite automat la dimensiuni mari
- daca un algoritm ar cere prea multa memorie, apare SKIP_MEMORIE in rezultate