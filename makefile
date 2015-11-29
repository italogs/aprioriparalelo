suporte = 5
base_dados = basedados_short.data
n_threads = 3
flags = -lm -Wall -Werror -pedantic -O3
entrada_sequencial = ${suporte} ${base_dados}
entrada_paralelo = ${entrada_sequencial} ${n_threads}
executa_ambos: exec_sequencial exec_paralelo
	echo "FIM"

exec_paralelo:compilaparalelo
	./apriori_paralelo ${entrada_paralelo} >> comparativo.txt
exec_sequencial: compilasequencial 
	./apriori_sequencial ${entrada_sequencial} > comparativo.txt

compilasequencial:
	g++ main_sequencial.cpp -o apriori_sequencial ${flags}

compilaparalelo: 
	g++  main_paralelo.cpp -o apriori_paralelo -fopenmp ${flags} 
