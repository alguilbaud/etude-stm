// Programme caculant le produit scalaire de 2 vecteurs en utilisant pthread et la librairie tiny_stm

#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <unistd.h> 
#include <math.h>
#include <string.h>
#include "stm.h"
#include "heure.h"


// macro pour la taille maximale du tableau de thread ( 63 = 64 - le thread maitre (main)) 
#define NB_THREAD_MAX 63

#define START(tid, ro) { stm_tx_attr_t _a = {{.id = tid,.read_only = ro}}; sigjmp_buf *_e = stm_start(_a); if (_e != NULL)sigsetjmp(*_e, 0)

#define LOAD(addr) stm_load((stm_word_t *)addr)

#define STORE(addr, value) stm_store((stm_word_t *)addr,(stm_word_t)value)

#define COMMIT stm_commit(); }

// barriere de démarrage pour tous les thread
pthread_barrier_t barriere;

// variable contenant le resultat final, protegee par la stm
long final_sum = 0;

// vecteurs de valeurs codés en dur
long taille_tab;
long *U;
long *V;

long nb_threads = 0;
double duree_boucle = 0;

void* calcul(void *id_thread)
{
	int rc;

	long i;

	//on recupere le numero du thread
	long id = (long) id_thread;
	
	long part_sum = 0;
	long index_min = 0;
	long index_max = 0;
	
	long sum_stm;
	
	double date_avantBoucle; // variable pour la boucle vide
	
	//calcul partiel servant a determiner index_min et index_max
	long partial_calcul = (long) floor((float) (taille_tab/nb_threads));
	
	// on cacule l'index de depart
	index_min = partial_calcul*id;
	
	if(id == (nb_threads - 1)){
		// si c'est le dernier thread, index_max est egal a la derniere case du tableau
		index_max = taille_tab - 1;
	}
	else{
		// sinon index_max est egal a la case precedent l'index_min du thread suivant
		index_max = partial_calcul*(id+1) - 1;
	} 
	
	// on cacule la somme partielle, sans modifier final_sum	
	for (i = index_min; i <= index_max; i++){
	     part_sum += U[i]*V[i];
    }
	
	// point de synchronisation
    rc = pthread_barrier_wait(&barriere);

    if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
    {
        printf("imopssible d'attendre sur la barriere\n");
        exit(-1);
    }

	// utilisation de la stm pour acceder a final_sum
    stm_init_thread();
    
    //demarrage de la STM en mode read/write
    START(0, 0);
    
    //recuperation de la somme
    sum_stm = LOAD(&final_sum);
    
    date_avantBoucle=give_time();
    
    // boucle while vide pour pouvoir rallonger le temps passe en section critique
    while ( (give_time()-date_avantBoucle)<duree_boucle ){}
    
    //modification de la somme en local
    sum_stm += part_sum;
    
    STORE(&final_sum, sum_stm);
    
    //vérification de la transaction
    COMMIT;
    
    //fin de la transaction
    stm_exit_thread();
   	return NULL;
}

main(int argc, char * argv[])
{
	if(argc != 4){
		printf("Nombre de paramètres incorrects ! Entrez dans l'ordre :\n");
		printf("- le nombre de threads à utiliser \n");
		printf("- le nom du fichier contenant les valeurs à calculer \n");
		printf("- le temps en secondes à passer en section critique \n");
		exit(0);
	}

	long i;
	long current_number;
	
	char output_file[50] = {'r','e','s','u','l','t','a','t','s','/','r','e','s','u','l','t','a','t','s','S','T','M','_'}; 
	
	char string_time[20];
	char final_line[20];

	FILE *fichier = fopen(argv[2],"r");
	
	strcat(output_file,argv[2]);
	
	FILE *output = fopen(output_file,"a");
	
	fscanf(fichier,"%ld", &taille_tab);
	
	U = malloc(sizeof(long) * taille_tab);
	V = malloc(sizeof(long) * taille_tab);

	for(i = 0; i < taille_tab; i++)
	{
		fscanf(fichier,"%ld",&current_number);
		U[i] = current_number;
	}

	for(i = 0; i < taille_tab; i++)
	{
		fscanf(fichier,"%ld",&current_number);
		V[i] = current_number;
	}

	fclose(fichier);

	// variables utilisee pour calculer le temps d'execution
	double temps_debut;
	double temps_fin;

	// tableau contenant les threads, on fixe arbitrairement le maximum à 64
	pthread_t tab_thread[NB_THREAD_MAX];
	
	//initialisation de la STM
	stm_init();
	
	nb_threads = atoi(argv[1]);
	
	if( nb_threads > NB_THREAD_MAX)
	{
		nb_threads = NB_THREAD_MAX;
	}
	
	sscanf(argv[3],"%lf", &duree_boucle);

	if (nb_threads > taille_tab)
		nb_threads = taille_tab;

	// initialisation du seuil de déblocage de la barriere avec le nombre de thread en parametre
	pthread_barrier_init(&barriere, NULL, nb_threads);

	// on cree chaque thread
	for( i = 0; i < nb_threads; i++)
	{
		if (pthread_create(&tab_thread[i], NULL, calcul, (void *)i))
		{ 
			perror("pthread_create"); 	
			exit(EXIT_FAILURE);
	    }
	}
	temps_debut = give_time();
	
	for(i = 0; i < nb_threads; i++){
	     	if (pthread_join(tab_thread[i], NULL)) 
	     		perror("pthread_join");
	}	

	// destruction de la barriere apres destruction de tous les threads
	pthread_barrier_destroy(&barriere);

	// fin du chronometre
	temps_fin = give_time();

	//nettoyage de la STM
	stm_exit();

	sprintf(final_line,"%ld",nb_threads);
	
	strcat(final_line,"\t");

	sprintf(string_time,"%f",temps_fin - temps_debut);
	
	strcat(final_line,string_time);
	
	fprintf(output, "%s\n", final_line);
	
	fclose(output);
	
	return (EXIT_SUCCESS);
}
