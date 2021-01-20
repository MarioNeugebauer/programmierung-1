#include <stdio.h>

struct person {
	char name[20];
	char vorname[20];
	char ort[20];
	struct person* naechste_person;
};

typedef struct person person_t;

person_t* neue_person_erzeugen(char name[], char vorname[], char ort[]) {
//person_t* neue_person_erzeugen(person_t neue_personendaten) {
		person_t* neue_person;
	neue_person = malloc(sizeof(person_t));
	strcpy(*neue_person->name, name);
	strcpy(*neue_person->vorname, vorname);
	strcpy(*neue_person->ort, ort);
	(*neue_person).naechste_person = NULL;
	return neue_person;
}

void person_einfuegen(person_t *eine_person, person_t *neue_person) {
	neue_person->naechste_person = eine_person->naechste_person;
	eine_person->naechste_person = neue_person;
}

void naechste_person_loeschen(person_t* vorgaenger_person) {
	person_t* temp_person;
	temp_person = vorgaenger_person->naechste_person;
	vorgaenger_person->naechste_person = temp_person->naechste_person;
	free(temp_person);
}

void alle_personen_ausgeben(person_t *erste_person) {
	person_t* aktuelle_person = erste_person;
	if (erste_person == NULL) {
		printf("Keine Daten vorhanden.\n");
		return;
	}
	do {
		printf("%s, %s, %s\n", aktuelle_person->name, aktuelle_person->vorname, aktuelle_person->ort);
		aktuelle_person = aktuelle_person->naechste_person;
	} while (aktuelle_person != NULL);
}

int main(void) {


	person_t* listenanfang = NULL;

	person_t daten1 = { "Adamski", "Anton", "Adorf", NULL };
	person_t* p1 = neue_person_erzeugen("Adamski", "Anton", "Adorf");

	listenanfang = p1;

	printf("\nListenanfang wurde festgelegt.\n");

	alle_personen_ausgeben(listenanfang);

	person_t daten2 = { "Beispiel", "Bertha", "Beispielstadt", NULL };
	person_t* p2 = neue_person_erzeugen(daten2);

	person_einfuegen(listenanfang, p2);

	printf("\nEine Person wurde hinzugefuegt.\n");

	alle_personen_ausgeben(listenanfang);

	person_t daten3 = { "Charakter", "Carl", "Chemnitz", NULL };
	person_t* p3 = neue_person_erzeugen(daten3);

	person_einfuegen(listenanfang, p3);

	printf("\nEine weitere Person wurde hinzugefuegt.\n");

	alle_personen_ausgeben(listenanfang);

	naechste_person_loeschen(listenanfang);

	printf("\nEine Person nach listenanfang wurde geloescht.\n");

	alle_personen_ausgeben(listenanfang);


	naechste_person_loeschen(listenanfang);

	printf("\nEine weitere Person nach listenanfang wurde geloescht.\n");

	alle_personen_ausgeben(listenanfang);

	free(listenanfang);

}