# compila_ambos: compilaseq compilaparalelo exec_sequencial
# 	./apriori_paralelo >> comparativo.txt

# compilaparalelo: 
# 	g++ -lm -fopenmp main_paralelo.cpp -o apriori_paralelo 
exec_sequencial: compilaseq
	./apriori_sequencial 5 basedados_short.data

compilaseq:
	g++ -lm main_sequencial.cpp -o apriori_sequencial -Wall -Werror -pedantic


	 # > comparativo.txt
