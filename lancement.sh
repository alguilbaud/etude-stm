#!/bin/bash
for ((i = 10000; i <= 10000000; i=i*10)); do
	./vectors_generator $i
	for ((j = 0; j < 10; j++)); do
		echo "Lancement de variation_nb_thread avec 63 threads, taille des vecteurs de $i et temps dans la section critique de 0.00$j sec"
		./variation_nb_thread 63 sample_$i.txt 0.00$j
	done
done
