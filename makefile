compila_ambos: compilaseq compilaparalelo exec_sequencial
	./apriori_paralelo >> comparativo.txt

compilaparalelo: 
	g++ -lm -fopenmp main_paralelo.cpp -o apriori_paralelo 

compilaseq:
	g++ -lm main_sequencial.cpp -o apriori_sequencial

exec_sequencial:
	./apriori_sequencial > comparativo.txt


#compile: 
#	g++ -lm -W -Wall -ansi  -Wno-long-long -pedantic -Werror main.cpp -o exec 