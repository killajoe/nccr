#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
	int i,k;
	float price;
	char code[128];
	char desc[128];
	char deposit[9];
	char tax[9];
	char typ[9];

	FILE *fp;
	
	while(1) {
		printf("Eingabe eines neuen Produktes oder Abbruch mit '^c'\n");
		printf("Barcode: ");
		if(scanf("%s", code) == EOF) {
			printf("Fehler bei Eingabe des Barcodes\n");
			fflush(stdout);
			return 1;
		}
		if(strlen(code) != 13) {
			printf("Kein gueltiger Barcode (13 Stellen)!\n... Dieser Eintrag wird uebersprungen\n\n");
			fflush(stdout);
			continue;
		}
		fgets(desc, 127, stdin); /* hier ist noch was in stdin was ich nicht brauche */
		
		printf("Beschreibung: ");
		fgets(desc, 127, stdin);
		printf("Preis (incl. MwSt.): ");
		if(scanf("%f", &price) == EOF) {
			printf("Fehler bei Eingabe der Beschreibung\n");
			fflush(stdout);
			return 1;
		}
		getchar();
		printf("Pfand j/n: ");
		switch(getchar()) {
			case 'j':
				getchar();
				printf("Pfand 1=8 2=15 3=25: ");
				switch(getchar()) {
					case '1':
						strcpy(deposit, "PF1");
						break;

					case '2':
						strcpy(deposit, "PF2");
						break;

					case '3':
						strcpy(deposit, "PF3");
						break;

					default:
						printf("Keine gueltige Eingabe (1, 2 oder 3)!\n... Dieser Eintrag wird uebersprungen\n\n");
						fflush(stdout);
						continue;
				}
				break;

			case 'n':
				strcpy(deposit, "PF0");
				break;

			default:
				printf("Keine gueltige Eingabe (ja oder nein)!\n... Dieser Eintrag wird uebersprungen\n\n");
				fflush(stdout);
				continue;
		}
		
		getchar();
		printf("MwSt 1=7%% oder 2=19%%: ");
		switch(getchar()) {
			case '1':
				strcpy(tax, "TX1");
				getchar();
				break;

			case '2':
				strcpy(tax, "TX2");
				getchar();
				break;

			default:
				printf("Keine Angabe, waehle 7%% MwSt.\n");
				strcpy(tax, "TX1");
		}
		printf("Typ 1=ST 2=BE 3=FL 4=KG: ");
		switch(getchar()) {
			case '1':
				strcpy(typ, "ST");
				break;

			case '2':
				strcpy(typ, "BE");
				break;

			case '3':
				strcpy(typ, "FL");
				break;

			case '4':
				strcpy(typ, "KG");
				break;

			default:
				printf("Keine Angabe, waehle Typ 'ST'\n");
				strcpy(typ, "ST");
		}
		printf("\n  Zusammenfassung:\n  %s;%.2f;%s;%s;%s;%s\n",code,price,deposit,tax,typ,desc);
		fp = fopen("database1.csv","a");
		if(fp == NULL) {
			printf("\nProblem beim Oeffnen der Datenbank\n");
			fflush(stdout);
			exit(1);
		}
		fprintf(fp, "%s;%.2f;%s;%s;%s;%s",code,price,deposit,tax,typ,desc);
		fclose(fp);
	}

	return 0;
}
