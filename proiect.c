
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

#define TIPURI_DATE 5
#define ALGORITMI 8
#define SETURI_MAX 10
#define LIMITA_MEMORIE_TOTALA 900000000LL

typedef int (*SortFunc)(int[], int);
typedef void (*GenFunc)(int[], int, unsigned int);

typedef enum {
    FARA_SKIP,
    SKIP_TIMP,
    SKIP_MEMORIE
} TipSkip;

typedef struct {
    int dimensiune;
    int repetari;
} TestConfig;

typedef struct {
    const char *nume;
    GenFunc genereaza;
} GenInfo;

typedef struct {
    const char *nume;
    SortFunc sorteaza;
    int limitaTimp;
} SortInfo;

typedef struct {
    int rulari;
    int esuat;
    TipSkip skip;
    double total;
    double minim;
    double maxim;
} Rezultat;

void *incearcaAlocare(size_t bytes) {
    if (bytes == 0) {
        return NULL;
    }
    return malloc(bytes);
}

void schimba(int *primul, int *alDoilea) {
    int auxiliar = *primul;
    *primul = *alDoilea;
    *alDoilea = auxiliar;
}

int bubbleSort(int vector[], int dimensiune) {
    for (int pas = 0; pas < dimensiune - 1; pas++) {
        for (int index = 0; index < dimensiune - pas - 1; index++) {
            if (vector[index] > vector[index + 1]) {
                schimba(&vector[index], &vector[index + 1]);
            }
        }
    }
    return 1;
}

int selectionSort(int vector[], int dimensiune) {
    for (int pozitie = 0; pozitie < dimensiune - 1; pozitie++) {
        int pozitieMinima = pozitie;
        for (int index = pozitie + 1; index < dimensiune; index++) {
            if (vector[index] < vector[pozitieMinima]) {
                pozitieMinima = index;
            }
        }
        schimba(&vector[pozitie], &vector[pozitieMinima]);
    }
    return 1;
}

int insertionSort(int vector[], int dimensiune) {
    for (int pozitie = 1; pozitie < dimensiune; pozitie++) {
        int valoare = vector[pozitie];
        int index = pozitie - 1;
        while (index >= 0 && vector[index] > valoare) {
            vector[index + 1] = vector[index];
            index--;
        }
        vector[index + 1] = valoare;
    }
    return 1;
}

void quickSortRec(int vector[], int stanga, int dreapta) {
    int indexStanga = stanga;
    int indexDreapta = dreapta;
    int pivot = vector[(stanga + dreapta) / 2];

    while (indexStanga <= indexDreapta) {
        while (vector[indexStanga] < pivot) {
            indexStanga++;
        }
        while (vector[indexDreapta] > pivot) {
            indexDreapta--;
        }
        if (indexStanga <= indexDreapta) {
            schimba(&vector[indexStanga], &vector[indexDreapta]);
            indexStanga++;
            indexDreapta--;
        }
    }

    if (stanga < indexDreapta) {
        quickSortRec(vector, stanga, indexDreapta);
    }
    if (indexStanga < dreapta) {
        quickSortRec(vector, indexStanga, dreapta);
    }
}

int quickSort(int vector[], int dimensiune) {
    if (dimensiune > 1) {
        quickSortRec(vector, 0, dimensiune - 1);
    }
    return 1;
}

void interclaseaza(int vector[], int auxiliar[], int stanga, int mijloc, int dreapta) {
    int indexStanga = stanga;
    int indexDreapta = mijloc + 1;
    int indexAuxiliar = stanga;

    while (indexStanga <= mijloc && indexDreapta <= dreapta) {
        if (vector[indexStanga] <= vector[indexDreapta]) {
            auxiliar[indexAuxiliar++] = vector[indexStanga++];
        } else {
            auxiliar[indexAuxiliar++] = vector[indexDreapta++];
        }
    }

    while (indexStanga <= mijloc) {
        auxiliar[indexAuxiliar++] = vector[indexStanga++];
    }

    while (indexDreapta <= dreapta) {
        auxiliar[indexAuxiliar++] = vector[indexDreapta++];
    }

    for (int index = stanga; index <= dreapta; index++) {
        vector[index] = auxiliar[index];
    }
}

void mergeSortRec(int vector[], int auxiliar[], int stanga, int dreapta) {
    if (stanga >= dreapta) {
        return;
    }

    int mijloc = (stanga + dreapta) / 2;
    mergeSortRec(vector, auxiliar, stanga, mijloc);
    mergeSortRec(vector, auxiliar, mijloc + 1, dreapta);
    interclaseaza(vector, auxiliar, stanga, mijloc, dreapta);
}

int mergeSort(int vector[], int dimensiune) {
    if (dimensiune <= 1) {
        return 1;
    }

    int *auxiliar = incearcaAlocare((size_t)dimensiune * sizeof(int));
    if (auxiliar == NULL) {
        return 0;
    }

    mergeSortRec(vector, auxiliar, 0, dimensiune - 1);
    free(auxiliar);
    return 1;
}

void heapify(int vector[], int dimensiune, int pozitie) {
    while (1) {
        int pozitieMare = pozitie;
        int copilStanga = 2 * pozitie + 1;
        int copilDreapta = 2 * pozitie + 2;

        if (copilStanga < dimensiune && vector[copilStanga] > vector[pozitieMare]) {
            pozitieMare = copilStanga;
        }
        if (copilDreapta < dimensiune && vector[copilDreapta] > vector[pozitieMare]) {
            pozitieMare = copilDreapta;
        }
        if (pozitieMare == pozitie) {
            break;
        }

        schimba(&vector[pozitie], &vector[pozitieMare]);
        pozitie = pozitieMare;
    }
}

int heapSort(int vector[], int dimensiune) {
    if (dimensiune <= 1) {
        return 1;
    }

    for (int pozitie = dimensiune / 2 - 1; pozitie >= 0; pozitie--) {
        heapify(vector, dimensiune, pozitie);
        if (pozitie == 0) {
            break;
        }
    }

    for (int pozitie = dimensiune - 1; pozitie > 0; pozitie--) {
        schimba(&vector[0], &vector[pozitie]);
        heapify(vector, pozitie, 0);
    }

    return 1;
}

int countingSort(int vector[], int dimensiune) {
    if (dimensiune <= 1) {
        return 1;
    }

    int valoareMaxima = vector[0];
    for (int index = 1; index < dimensiune; index++) {
        if (vector[index] > valoareMaxima) {
            valoareMaxima = vector[index];
        }
    }

    int *frecventa = calloc((size_t)valoareMaxima + 1, sizeof(int));
    int *rezultat = incearcaAlocare((size_t)dimensiune * sizeof(int));

    if (frecventa == NULL || rezultat == NULL) {
        free(frecventa);
        free(rezultat);
        return 0;
    }

    for (int index = 0; index < dimensiune; index++) {
        frecventa[vector[index]]++;
    }

    for (int index = 1; index <= valoareMaxima; index++) {
        frecventa[index] += frecventa[index - 1];
    }

    for (int index = dimensiune - 1; index >= 0; index--) {
        rezultat[frecventa[vector[index]] - 1] = vector[index];
        frecventa[vector[index]]--;
    }

    memcpy(vector, rezultat, (size_t)dimensiune * sizeof(int));
    free(frecventa);
    free(rezultat);
    return 1;
}

int radixSort(int vector[], int dimensiune) {
    if (dimensiune <= 1) {
        return 1;
    }

    int valoareMaxima = vector[0];
    for (int index = 1; index < dimensiune; index++) {
        if (vector[index] > valoareMaxima) {
            valoareMaxima = vector[index];
        }
    }

    int *auxiliar = incearcaAlocare((size_t)dimensiune * sizeof(int));
    if (auxiliar == NULL) {
        return 0;
    }

    for (long long putere = 1; valoareMaxima / putere > 0; putere *= 10) {
        int frecventa[10] = {0};

        for (int index = 0; index < dimensiune; index++) {
            int cifra = (int)((vector[index] / putere) % 10);
            frecventa[cifra]++;
        }

        for (int index = 1; index < 10; index++) {
            frecventa[index] += frecventa[index - 1];
        }

        for (int index = dimensiune - 1; index >= 0; index--) {
            int cifra = (int)((vector[index] / putere) % 10);
            auxiliar[frecventa[cifra] - 1] = vector[index];
            frecventa[cifra]--;
        }

        memcpy(vector, auxiliar, (size_t)dimensiune * sizeof(int));
    }

    free(auxiliar);
    return 1;
}

unsigned int pasRandom(unsigned int *seed) {
    *seed = (*seed * 1664525u) + 1013904223u;
    return *seed;
}

void randomArray(int vector[], int dimensiune, unsigned int seed) {
    for (int index = 0; index < dimensiune; index++) {
        vector[index] = (int)(pasRandom(&seed) % (unsigned int)dimensiune);
    }
}

void sortedArray(int vector[], int dimensiune, unsigned int seed) {
    (void)seed;
    for (int index = 0; index < dimensiune; index++) {
        vector[index] = index;
    }
}

void reverseArray(int vector[], int dimensiune, unsigned int seed) {
    (void)seed;
    for (int index = 0; index < dimensiune; index++) {
        vector[index] = dimensiune - index - 1;
    }
}

void almostSortedArray(int vector[], int dimensiune, unsigned int seed) {
    sortedArray(vector, dimensiune, seed);

    int schimbari = dimensiune / 20;
    if (schimbari < 1) {
        schimbari = 1;
    }

    for (int pas = 0; pas < schimbari; pas++) {
        int primaPozitie = (int)(pasRandom(&seed) % (unsigned int)dimensiune);
        int aDouaPozitie = (int)(pasRandom(&seed) % (unsigned int)dimensiune);
        schimba(&vector[primaPozitie], &vector[aDouaPozitie]);
    }
}

void flatArray(int vector[], int dimensiune, unsigned int seed) {
    for (int index = 0; index < dimensiune; index++) {
        vector[index] = (int)(pasRandom(&seed) % 5u);
    }
}

int esteSortat(const int vector[], int dimensiune) {
    for (int index = 1; index < dimensiune; index++) {
        if (vector[index] < vector[index - 1]) {
            return 0;
        }
    }
    return 1;
}

void initRezultat(Rezultat *rezultat) {
    rezultat->rulari = 0;
    rezultat->esuat = 0;
    rezultat->skip = FARA_SKIP;
    rezultat->total = 0.0;
    rezultat->minim = -1.0;
    rezultat->maxim = 0.0;
}

void seteazaSkip(Rezultat *rezultat, TipSkip tipSkip) {
    rezultat->rulari = 0;
    rezultat->esuat = 0;
    rezultat->skip = tipSkip;
    rezultat->total = 0.0;
    rezultat->minim = -1.0;
    rezultat->maxim = 0.0;
}

void adaugaTimp(Rezultat *rezultat, double timp) {
    rezultat->rulari++;
    rezultat->total += timp;
    if (rezultat->minim < 0.0 || timp < rezultat->minim) {
        rezultat->minim = timp;
    }
    if (timp > rezultat->maxim) {
        rezultat->maxim = timp;
    }
}

double medie(Rezultat rezultat) {
    if (rezultat.rulari == 0) {
        return 0.0;
    }
    return rezultat.total / rezultat.rulari;
}

unsigned int calculeazaSeed(int dimensiune, int tip, int repetare) {
    unsigned long long baza = 123456789ULL;
    baza += (unsigned long long)dimensiune * 97ULL;
    baza += (unsigned long long)tip * 1000003ULL;
    baza += (unsigned long long)(repetare + 1) * 65537ULL;
    return (unsigned int)baza;
}

void creeazaDirector(const char *nume) {
    if (mkdir(nume, 0777) == -1 && errno != EEXIST) {
        printf("Nu am putut crea directorul %s.\n", nume);
        exit(1);
    }
}

int valoareMaximaPosibila(int tipDate, int dimensiune) {
    if (tipDate == 4) {
        return 4;
    }
    if (dimensiune <= 0) {
        return 0;
    }
    return dimensiune - 1;
}

long long memorieAuxiliara(int algoritm, int dimensiune, int valoareMaxima) {
    switch (algoritm) {
        case 4:
            return (long long)dimensiune * (long long)sizeof(int);
        case 6:
            return ((long long)dimensiune + (long long)valoareMaxima + 1LL) * (long long)sizeof(int);
        case 7:
            return ((long long)dimensiune + 10LL) * (long long)sizeof(int);
        default:
            return 0;
    }
}

long long memorieTotalaEstimata(int algoritm, int dimensiune, int valoareMaxima) {
    return (long long)dimensiune * (long long)sizeof(int) + memorieAuxiliara(algoritm, dimensiune, valoareMaxima);
}

TipSkip skipInitial(int indexAlgoritm, SortInfo algoritm, int dimensiune, int valoareMaxima) {
    if (algoritm.limitaTimp > 0 && dimensiune > algoritm.limitaTimp) {
        return SKIP_TIMP;
    }
    if (memorieTotalaEstimata(indexAlgoritm, dimensiune, valoareMaxima) > LIMITA_MEMORIE_TOTALA) {
        return SKIP_MEMORIE;
    }
    return FARA_SKIP;
}

const char *statusRezultat(Rezultat rezultat) {
    if (rezultat.esuat) {
        return "FAIL";
    }
    if (rezultat.skip == SKIP_TIMP) {
        return "SKIP_TIMP";
    }
    if (rezultat.skip == SKIP_MEMORIE) {
        return "SKIP_MEMORIE";
    }
    return "OK";
}

int gasesteSet(TestConfig seturi[], int numarSeturi, int dimensiune) {
    for (int index = 0; index < numarSeturi; index++) {
        if (seturi[index].dimensiune == dimensiune) {
            return index;
        }
    }
    return -1;
}

void scrieAnaliza(FILE *fisier,
                  TestConfig seturi[],
                  int numarSeturi,
                  GenInfo tipuriDate[],
                  SortInfo algoritmi[],
                  Rezultat rezultate[SETURI_MAX][TIPURI_DATE][ALGORITMI]) {
    fprintf(fisier, "Analiza automata pentru toate testele\n\n");
    fprintf(fisier, "Datele de test sunt generate pe loc la fiecare rulare.\n");
    fprintf(fisier, "Nu se salveaza un fisier separat cu toate valorile generate, pentru ca se ajunge pana la 100000000 de elemente.\n\n");
    fprintf(fisier, "Algoritmi folositi:\n");
    fprintf(fisier, "Bubble Sort\nSelection Sort\nInsertion Sort\nQuick Sort\nMerge Sort\nHeap Sort\nCounting Sort\nRadix Sort\n\n");
    fprintf(fisier, "Complexitati teoretice:\n");
    fprintf(fisier, "Bubble Sort: O(n^2), memorie O(1)\n");
    fprintf(fisier, "Selection Sort: O(n^2), memorie O(1)\n");
    fprintf(fisier, "Insertion Sort: O(n^2), dar merge bine pe liste aproape sortate, memorie O(1)\n");
    fprintf(fisier, "Quick Sort: O(n log n) in medie\n");
    fprintf(fisier, "Merge Sort: O(n log n), memorie O(n)\n");
    fprintf(fisier, "Heap Sort: O(n log n), memorie O(1)\n");
    fprintf(fisier, "Counting Sort: O(n + k), bun cand valorile nu sunt foarte mari\n");
    fprintf(fisier, "Radix Sort: O(numar_cifre * n), bun pentru numere intregi nenegative\n\n");
    fprintf(fisier, "Cei mai rapizi algoritmi pe fiecare caz:\n");

    for (int indexSet = 0; indexSet < numarSeturi; indexSet++) {
        for (int indexTip = 0; indexTip < TIPURI_DATE; indexTip++) {
            int indexBest = -1;
            double timpBest = 0.0;

            for (int indexAlgoritm = 0; indexAlgoritm < ALGORITMI; indexAlgoritm++) {
                Rezultat rezultatCurent = rezultate[indexSet][indexTip][indexAlgoritm];
                if (rezultatCurent.skip != FARA_SKIP || rezultatCurent.esuat || rezultatCurent.rulari == 0) {
                    continue;
                }
                if (indexBest == -1 || medie(rezultatCurent) < timpBest) {
                    indexBest = indexAlgoritm;
                    timpBest = medie(rezultatCurent);
                }
            }

            if (indexBest != -1) {
                fprintf(fisier,
                        "n=%d, tip=%s -> %s cu media %.6f sec\n",
                        seturi[indexSet].dimensiune,
                        tipuriDate[indexTip].nume,
                        algoritmi[indexBest].nume,
                        timpBest);
            }
        }
    }

    fprintf(fisier, "\nCazuri sarite:\n");
    for (int indexSet = 0; indexSet < numarSeturi; indexSet++) {
        for (int indexTip = 0; indexTip < TIPURI_DATE; indexTip++) {
            for (int indexAlgoritm = 0; indexAlgoritm < ALGORITMI; indexAlgoritm++) {
                Rezultat rezultatCurent = rezultate[indexSet][indexTip][indexAlgoritm];
                if (rezultatCurent.skip != FARA_SKIP) {
                    fprintf(fisier,
                            "n=%d, tip=%s, algoritm=%s -> %s\n",
                            seturi[indexSet].dimensiune,
                            tipuriDate[indexTip].nume,
                            algoritmi[indexAlgoritm].nume,
                            statusRezultat(rezultatCurent));
                }
            }
        }
    }

    fprintf(fisier, "\nObservatii automate:\n");

    int index1000 = gasesteSet(seturi, numarSeturi, 1000);
    if (index1000 != -1) {
        double insertionAleator = medie(rezultate[index1000][0][2]);
        double insertionAproape = medie(rezultate[index1000][3][2]);
        if (insertionAleator > 0.0 && insertionAproape > 0.0) {
            fprintf(fisier,
                    "La n=1000, Insertion Sort pe liste aproape sortate a avut %.6f sec, iar pe liste aleatoare %.6f sec.\n",
                    insertionAproape,
                    insertionAleator);
        }
    }

    int index100000 = gasesteSet(seturi, numarSeturi, 100000);
    if (index100000 != -1) {
        double quickAleator = medie(rezultate[index100000][0][3]);
        double mergeAleator = medie(rezultate[index100000][0][4]);
        double heapAleator = medie(rezultate[index100000][0][5]);
        if (quickAleator > 0.0) {
            fprintf(fisier, "La n=100000 pe liste aleatoare, Quick Sort a avut %.6f sec.\n", quickAleator);
        }
        if (mergeAleator > 0.0) {
            fprintf(fisier, "La n=100000 pe liste aleatoare, Merge Sort a avut %.6f sec.\n", mergeAleator);
        }
        if (heapAleator > 0.0) {
            fprintf(fisier, "La n=100000 pe liste aleatoare, Heap Sort a avut %.6f sec.\n", heapAleator);
        }
    }

    int index100000000 = gasesteSet(seturi, numarSeturi, 100000000);
    if (index100000000 != -1) {
        double quickMare = medie(rezultate[index100000000][0][3]);
        double mergeMare = medie(rezultate[index100000000][0][4]);
        double heapMare = medie(rezultate[index100000000][0][5]);
        if (quickMare > 0.0) {
            fprintf(fisier, "La n=100000000 pe liste aleatoare, Quick Sort a avut %.6f sec.\n", quickMare);
        }
        if (mergeMare > 0.0) {
            fprintf(fisier, "La n=100000000 pe liste aleatoare, Merge Sort a avut %.6f sec.\n", mergeMare);
        }
        if (heapMare > 0.0) {
            fprintf(fisier, "La n=100000000 pe liste aleatoare, Heap Sort a avut %.6f sec.\n", heapMare);
        }
    }

    fprintf(fisier, "Algoritmii O(n^2) sunt buni pe liste mici, dar devin nepractice pe liste mari.\n");
    fprintf(fisier, "Daca apare SKIP_TIMP, inseamna ca acel algoritm ar dura prea mult pentru dimensiunea respectiva.\n");
    fprintf(fisier, "Daca apare SKIP_MEMORIE, inseamna ca acel algoritm ar cere prea multa memorie pentru acel caz.\n");
}

int main(void) {
    TestConfig seturi[SETURI_MAX] = {
        {10, 10000},
        {20, 10000},
        {50, 5000},
        {100, 2000},
        {1000, 100},
        {10000, 10},
        {100000, 3},
        {1000000, 1},
        {10000000, 1},
        {100000000, 1}
    };

    GenInfo tipuriDate[TIPURI_DATE] = {
        {"aleator", randomArray},
        {"sortat", sortedArray},
        {"invers_sortat", reverseArray},
        {"aproape_sortat", almostSortedArray},
        {"plat", flatArray}
    };

    SortInfo algoritmi[ALGORITMI] = {
        {"Bubble Sort", bubbleSort, 2000},
        {"Selection Sort", selectionSort, 2000},
        {"Insertion Sort", insertionSort, 5000},
        {"Quick Sort", quickSort, 0},
        {"Merge Sort", mergeSort, 0},
        {"Heap Sort", heapSort, 0},
        {"Counting Sort", countingSort, 0},
        {"Radix Sort", radixSort, 0}
    };

    Rezultat rezultate[SETURI_MAX][TIPURI_DATE][ALGORITMI];
    int numarSeturi = SETURI_MAX;

    creeazaDirector("rezultate");

    FILE *fisierRezultate = fopen("rezultate/rezultate.csv", "w");
    FILE *fisierAnaliza = fopen("rezultate/analiza.txt", "w");

    if (fisierRezultate == NULL || fisierAnaliza == NULL) {
        printf("Nu am putut deschide fisierele de iesire.\n");
        if (fisierRezultate != NULL) {
            fclose(fisierRezultate);
        }
        if (fisierAnaliza != NULL) {
            fclose(fisierAnaliza);
        }
        return 1;
    }

    fprintf(fisierRezultate,
            "n,tip,repetari,algoritm,status,rulari,timp_total_sec,timp_mediu_sec,timp_min_sec,timp_max_sec,memorie_auxiliara_estimata_bytes,memorie_totala_estimata_bytes\n");

    printf("Programul ruleaza toate testele pana la n=100000000.\n");
    printf("Datele de test sunt generate pe loc, nu se salveaza separat.\n\n");

    for (int indexSet = 0; indexSet < numarSeturi; indexSet++) {
        int dimensiune = seturi[indexSet].dimensiune;
        int repetari = seturi[indexSet].repetari;

        for (int indexTip = 0; indexTip < TIPURI_DATE; indexTip++) {
            int valoareMaxima = valoareMaximaPosibila(indexTip, dimensiune);

            printf("n=%d, tip=%s, repetari=%d\n", dimensiune, tipuriDate[indexTip].nume, repetari);

            for (int indexAlgoritm = 0; indexAlgoritm < ALGORITMI; indexAlgoritm++) {
                initRezultat(&rezultate[indexSet][indexTip][indexAlgoritm]);
                TipSkip tipSkip = skipInitial(indexAlgoritm, algoritmi[indexAlgoritm], dimensiune, valoareMaxima);
                if (tipSkip != FARA_SKIP) {
                    seteazaSkip(&rezultate[indexSet][indexTip][indexAlgoritm], tipSkip);
                }
            }

            for (int repetare = 0; repetare < repetari; repetare++) {
                unsigned int seed = calculeazaSeed(dimensiune, indexTip, repetare);

                for (int indexAlgoritm = 0; indexAlgoritm < ALGORITMI; indexAlgoritm++) {
                    Rezultat *rezultatCurent = &rezultate[indexSet][indexTip][indexAlgoritm];

                    if (rezultatCurent->skip != FARA_SKIP) {
                        continue;
                    }

                    int *vector = incearcaAlocare((size_t)dimensiune * sizeof(int));
                    if (vector == NULL) {
                        seteazaSkip(rezultatCurent, SKIP_MEMORIE);
                        continue;
                    }

                    tipuriDate[indexTip].genereaza(vector, dimensiune, seed);

                    clock_t start = clock();
                    int succes = algoritmi[indexAlgoritm].sorteaza(vector, dimensiune);
                    clock_t stop = clock();

                    if (!succes) {
                        free(vector);
                        seteazaSkip(rezultatCurent, SKIP_MEMORIE);
                        continue;
                    }

                    if (!esteSortat(vector, dimensiune)) {
                        rezultatCurent->esuat = 1;
                        free(vector);
                        fclose(fisierRezultate);
                        fclose(fisierAnaliza);
                        printf("Sortare gresita la %s, n=%d, tip=%s\n",
                               algoritmi[indexAlgoritm].nume,
                               dimensiune,
                               tipuriDate[indexTip].nume);
                        return 1;
                    }

                    adaugaTimp(rezultatCurent, (double)(stop - start) / CLOCKS_PER_SEC);
                    free(vector);
                }
            }

            for (int indexAlgoritm = 0; indexAlgoritm < ALGORITMI; indexAlgoritm++) {
                Rezultat rezultatCurent = rezultate[indexSet][indexTip][indexAlgoritm];

                fprintf(fisierRezultate,
                        "%d,%s,%d,%s,%s,%d,%.6f,%.6f,%.6f,%.6f,%lld,%lld\n",
                        dimensiune,
                        tipuriDate[indexTip].nume,
                        repetari,
                        algoritmi[indexAlgoritm].nume,
                        statusRezultat(rezultatCurent),
                        rezultatCurent.rulari,
                        rezultatCurent.total,
                        medie(rezultatCurent),
                        rezultatCurent.minim < 0.0 ? 0.0 : rezultatCurent.minim,
                        rezultatCurent.maxim,
                        memorieAuxiliara(indexAlgoritm, dimensiune, valoareMaxima),
                        memorieTotalaEstimata(indexAlgoritm, dimensiune, valoareMaxima));

                if (rezultatCurent.skip != FARA_SKIP || rezultatCurent.esuat) {
                    printf("  %-15s %s\n", algoritmi[indexAlgoritm].nume, statusRezultat(rezultatCurent));
                } else {
                    printf("  %-15s medie=%.6f sec\n", algoritmi[indexAlgoritm].nume, medie(rezultatCurent));
                }
            }

            printf("\n");
        }
    }

    scrieAnaliza(fisierAnaliza, seturi, numarSeturi, tipuriDate, algoritmi, rezultate);

    fclose(fisierRezultate);
    fclose(fisierAnaliza);

    printf("Rezultatele au fost salvate in rezultate/rezultate.csv si rezultate/analiza.txt\n");

    return 0;
}
