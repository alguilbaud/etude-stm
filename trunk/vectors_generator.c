#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

int main(int arc, char * argv[])
{
	FILE *fichier = NULL;
	int i = 0;    
    int nombre_aleatoire = 0;
    char tab[2];
    int taille_vecteur;
    char nom_fichier[50] = {'s','a','m','p','l','e','_'};
    char tmp[15];

    taille_vecteur = atoi(argv[1]);

    
    sprintf(tmp,"%d",taille_vecteur);
    strcat(nom_fichier,tmp);
    strcat(nom_fichier,".txt");
	fichier = fopen(nom_fichier,"w");

	if (fichier != NULL)
    {
        fprintf(fichier, "%s\n", tmp);
    	
    	srand(time(NULL));

    	for(i = 0; i < taille_vecteur; i++)
    	{
        	nombre_aleatoire = rand()%100;
        	if( i == taille_vecteur - 1)
        	{
        		sprintf(tab,"%d",nombre_aleatoire);
        		fprintf(fichier, "%s", tab);
        		fprintf(fichier,"%s","\n");
        	}
        	else
        	{
        		sprintf(tab, "%d", nombre_aleatoire);
        		fprintf(fichier, "%s ", tab);

        	}
    	}

    	for(i = 0; i < taille_vecteur; i++)
    	{
        	nombre_aleatoire = rand()%100;
        	if( i == taille_vecteur - 1)
        	{
        		sprintf(tab,"%d",nombre_aleatoire);
        		fprintf(fichier, "%s", tab);
        		fprintf(fichier,"%s","\n");
        	}
        	else
        	{
        		sprintf(tab, "%d", nombre_aleatoire);
        		fprintf(fichier, "%s ", tab);

        	}
    	}
    }
    else
    {
    	printf("Impossible d'ouvrir le fichier");
    }
    fclose(fichier);
    return EXIT_SUCCESS;
}