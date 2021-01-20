#include <stdio.h>
#include <stdlib.h>

int* init_int_feld(unsigned int anzahl_bytes) {

	int* int_feld_zeiger = malloc(anzahl_bytes * (sizeof(int)));

	if (int_feld_zeiger != NULL) {
	
		for (unsigned int i = 0; i < anzahl_bytes; i++) {
			int_feld_zeiger[i] = 0; // mit 0 initialisieren
		}

	}
	return int_feld_zeiger;
}

int main(void) {

	unsigned int val = 0;
	int scanf_result;
	printf("Anzahl der Elemente im Feld: ");
	scanf_result = scanf_s("%u", &val);
	if (scanf_result != 1) {
		printf("Eingabefehler. Programm wird abgebrochen\n");
		return 1;
	}

	int* feld_zeiger = init_int_feld(val);

	if (feld_zeiger == NULL) {
		printf("Speicherreservierung nicht erfolgreich!\n");
		return 1;
	}

	printf("Elemente mit Werten ausgeben\n");
	for (unsigned i = 0; i < val; i++) {
		printf("arr[%u] = %u\n", i, feld_zeiger[i]);
	}

	printf("NEUE Anzahl der Elemente im Feld: ");
	scanf_result = scanf_s("%u", &val);
	if (scanf_result != 1) {
		printf("Eingabefehler. Programm wird abgebrochen\n");
		return 1;
	}

	feld_zeiger = realloc(feld_zeiger, val * sizeof(int));

	if (feld_zeiger == NULL) {
		printf("Speicherreservierung nicht erfolgreich!\n");
		return 1;
	}

	printf("Elemente mit Werten nach Änderung ausgeben\n");
	for (unsigned i = 0; i < val; i++) {
		printf("arr[%u] = %u\n", i, feld_zeiger[i]);
	}

	if (feld_zeiger != NULL) {
		free(feld_zeiger);
	}

	return 0;
}