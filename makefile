suporte = 5
base_dados = basedados_short.data
n_threads = 4
habilitar_log = 1
habilitar_impressao = 1
habilitar_arquivo_saida = 1
arquivo_comparacao = comparativo.txt


flags_compilacao = -lm -Wall -Werror -pedantic -O3

# flags_compilacao = -lm -O3
entrada_sequencial = ${habilitar_log} ${habilitar_impressao} ${habilitar_arquivo_saida} ${suporte} ${base_dados}
entrada_paralelo = ${entrada_sequencial} ${n_threads}

executa_ambos: stack_size desabilitar_HT compilaparalelo compilasequencial
	echo "Compilado"

desabilitar_HT:
	sudo sh disableHyperthreading.sh

# exec_paralelo:compilaparalelo
# 	./apriori_paralelo ${entrada_paralelo} >> ${arquivo_comparacao}

# exec_sequencial: compilasequencial 
# 	./apriori_sequencial ${entrada_sequencial} > ${arquivo_comparacao}

compilasequencial:
	g++ main_sequencial.cpp -o apriori_sequencial ${flags_compilacao}

compilaparalelo: 
	g++ main_paralelo.cpp -o apriori_paralelo -fopenmp ${flags_compilacao} 

stack_size: 
	# stack_size_pc
	export OMP_STACKSIZE=10GB

# stack_size_pc:	
# 	ulimit -s unlimited