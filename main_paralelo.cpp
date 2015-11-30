#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdbool.h>
#include <unistd.h>
#include <cmath>
#include <string.h>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sys/time.h>
#include <sstream>
#include <omp.h>
// #include <mpi.h>

#define RETIRAR <<"(Retirar mensagem)"

using namespace std;

struct timeval start, end;
double delta = 0.0;

static void inicia_relogio() {
	gettimeofday(&start, NULL);
}

static void finaliza_relogio() {
	gettimeofday(&end, NULL);
	delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
	end.tv_usec - start.tv_usec) / 1.e6;
	printf("Tempo Total de execucao = %12.10f\n\n\n",delta);	
}	

static string IntToString (int a) {
    ostringstream temp;
    temp << a;
    return temp.str();
}

static string subStrPrincipal(string str){
	return str.substr(0,str.find_last_of(" "));
}

static string combinaRotulos(string s1,string s2, int itemset) {
	int *vet =  new int[itemset];
	register char *dup = strdup(s1.c_str()),*token = strtok(dup," ");
	register int i = 0, j = 0;
	while(token) {
		vet[i] = atoi(token);
		i++;
		token = strtok(NULL," ");
	}
	free(dup);
	dup = strdup(s2.c_str());
	token = strtok(dup," ");
	for ( j = 0; j < itemset-2 ; j++) { 
		token = strtok(NULL," ");
	}
	vet[i] = atoi(token);
	free(dup);

	sort(vet,vet+itemset);
	string novo_rotulo = IntToString(vet[0]);
	
	for(i = 1;i<itemset;i++)
		novo_rotulo += " "+ IntToString(vet[i]);
	delete[] vet;
	return novo_rotulo;
}


static int getItemPorPos(string str,unsigned pos){
	if(pos == 0) {
		for (unsigned i = 0; i < str.length(); i++) {
			if(str[i] == ' ')
				return atoi(  str.substr(0,i).c_str()  );
		}
	}
	unsigned cont = 0 , j;
	for (unsigned i = 0; i < str.length(); i++) {
		if(str[i] == ' ') cont++;
		if(cont == pos) {
			j = i;
			while(str[j] != ' ') j++;
			return atoi(   str.substr(i+1,j).c_str()    );
		}
	}
	return -1;
}

static void escreveConjuntoArquivo(map<string,unsigned int> *candidatos, ofstream &arquivo_saida,int tamanhoItemset) {
	arquivo_saida << "Items sets (L"<<tamanhoItemset<<")"<<endl;
	arquivo_saida <<"Quantidade novos Conjuntos: "<< candidatos->size() <<endl;
	arquivo_saida << "{ ";
	map<string,unsigned int>::iterator it_fim = candidatos->end() ;
	for (map<string,unsigned int>::iterator it = candidatos->begin(); it != candidatos->end(); ++it) {
		arquivo_saida << "{"<<it->first<<"} ";
	}
	arquivo_saida << "}"<<endl;
}

bool naoAchouSubConjunto = 0;
static void subset(int *arr, int *data, int start, int end, int index, int r,map<string,unsigned int> *candidatos){
    register int j, i;
    if (index == r) {
    	string subconjunto = IntToString(data[0]);
        for (j = 1; j < r; j++)
        	subconjunto += " "+IntToString(data[j]);
        if(candidatos->find(subconjunto) == candidatos->end())
        	naoAchouSubConjunto = 1;
        return;
    }
    for (i = start; i <= end && end - i + 1 >= r - index; i++) {
        data[index] = arr[i];
        subset(arr, data, i+1, end, index+1, r,candidatos);
    }
}

static map<string,unsigned int> *eliminarSubconjuntosNaoFrequentes(map<string,unsigned int> *novosCandidatos,map<string,unsigned int> *candidatos,int tamanhoItemset){
	register char *dup,*token;
	int *vetNovosCandidatos = new int[tamanhoItemset],*vetSubSet = new int[tamanhoItemset-1];
	register unsigned int posCandidato = 0;
	map<string,unsigned int>::iterator it_fim = novosCandidatos->end() ;
	for(map<string,unsigned int>::iterator it = novosCandidatos->begin() ; it != it_fim ; ) {
		posCandidato = 0;
		dup = strdup((it->first).c_str());
		token = strtok(dup," ");

		while(token) {
			vetNovosCandidatos[posCandidato++] = atoi(token);
			token = strtok(NULL," ");
		}
		naoAchouSubConjunto = 0;
		subset(vetNovosCandidatos, vetSubSet, 0, tamanhoItemset-1, 0, tamanhoItemset-1,candidatos);
		if(naoAchouSubConjunto)
			novosCandidatos->erase(it++);
		else
			++it;
		free(dup);
	}
	return novosCandidatos;
}

int main(int argc,char *argv[]){

	if(argc != 7) {
		cout<<"Parametros incompletos"<<endl;
		return 0;
	}
	bool habilitar_log = atoi(argv[1]);
	bool habilitar_impressao = atoi(argv[2]);
	bool habilitar_arquivo_saida = atoi(argv[3]);
	register unsigned int suporte_minimo = atoi(argv[4]);

	unsigned int n_threads = atoi(argv[6]);
	fstream *arquivo_entrada = new fstream[n_threads];
	for (unsigned int i = 0; i < n_threads; ++i) {
		arquivo_entrada[i].open(argv[5]);
	}
	
	ofstream arquivo_saida("saida_paralelo.data");
	ofstream arquivo_log("log_paralelo.data");
	string transacao;	
	register unsigned tamanhoItemset = 1;
 	map<string,unsigned int> *candidatos = new map<string,unsigned int>;
 	map<string,unsigned int>::iterator it, it_fim;
	register unsigned int frequencia = 0 , it_presente = 0,it2_presente = 0, pos =0, qtd_items;
	int item;
	register char *dup = NULL, *token = NULL;
	omp_set_num_threads(n_threads);
 	inicia_relogio();

 	while(getline(arquivo_entrada[0], transacao)){
		dup = strdup(transacao.c_str());
		token = strtok(dup," ");
		while(token) {
			if(candidatos->find(token) == candidatos->end()) {
				string itemset(token);
				candidatos->insert(candidatos->begin(),pair<string,unsigned int>(itemset,1));
			} else {
				it = candidatos->find(token);
				it->second++;
			}
			token = strtok(NULL," ");
		}
		free(dup);
	}

	
	unsigned int max_candidatos = candidatos->size();
	it_fim = candidatos->end();
	it = candidatos->begin();

	#pragma omp parallel
	{
		#pragma omp single
		{
			for(unsigned int i = 0  ; i < max_candidatos; i++  ) {
				// #pragma omp task
				// {
					if(it->second < suporte_minimo) {
						candidatos->erase(it++);
					} else{
						++it;
					}
				// }
			}
		}
		#pragma omp single
		{
			cout<<"Programa Paralelo"<<endl;
			if(habilitar_impressao){
				cout<<"Suporte:"<<suporte_minimo<<endl;
				cout<<"n_Threads:"<<n_threads<<endl;
				cout<<"Itemset tamanho:"<<tamanhoItemset<<endl;
				cout<<"Quantidade de novos conjuntos:"<<candidatos->size()<<endl;
			}
		}
		#pragma omp single
		{
			if(habilitar_log) {
				arquivo_log << "Suporte:"<<suporte_minimo<<endl;
				arquivo_log << "n_Threads:"<<n_threads<<endl;
				arquivo_log << "Itemset tamanho:"<<tamanhoItemset<<endl;
				arquivo_log << "Quantidade de novos conjuntos: "<<candidatos->size()<<endl;
			}
		}
		#pragma omp single
		{
			if(habilitar_arquivo_saida){
				arquivo_saida << "Suporte minimo:"<<suporte_minimo<<endl;
			}
		}
	}
	map<string,unsigned int> *novosCandidatos;
	string conj1, conj2,novoCandidato;
	vector<string> v;
	while(candidatos->size() > 0) {
		if(habilitar_arquivo_saida)
			escreveConjuntoArquivo(candidatos,arquivo_saida,tamanhoItemset);
		
		novosCandidatos = new map<string,unsigned int>;
		tamanhoItemset++;
		
		if(habilitar_impressao){
			cout<<"Quantidade novos Conjuntos: "<< candidatos->size() <<endl;
			cout<<"Itemset tamanho: "<<tamanhoItemset<<endl;
		}

		
		for( it = candidatos->begin(); it != candidatos->end(); ++it ) {
	    	v.push_back( it->first );
	    }
	    
	    unsigned int v_size = v.size();
	    cout<<"V_size:"<<v_size<<endl;
	   
	    for(unsigned int i = 0 ; i < v_size ; i++) {
	    	conj1 = v[i];
	    	#pragma omp parallel for private(novoCandidato,qtd_items,item,pos,conj2,transacao,token,dup,it_presente,it2_presente,frequencia)
	    	for (unsigned int j = i+1; j < v_size; j++) {
	    		
				conj2 = v[j];

				int thread_id = omp_get_thread_num();
				arquivo_entrada[thread_id].clear();
				arquivo_entrada[thread_id].seekg(0);
				
				frequencia = 0;
				
				if (tamanhoItemset == 2) {
					//criar um ponteiro de arquivo para cada thread e executar
					while(getline(arquivo_entrada[thread_id], transacao)) {
						dup = strdup(transacao.c_str());
						token = strtok(dup," ");
						it_presente = it2_presente = 0;
						while(token) {
							if(!it_presente && strcmp(v[i].c_str(),token) == 0) 
								it_presente = 1;
							if(!it2_presente && strcmp(v[j].c_str(),token) == 0)
								it2_presente = 1;
							if(it_presente == 1 && it2_presente == 1){
								frequencia++;
								break;
							}
							token = strtok(NULL," ");
						}
						free(dup);
					}

					novoCandidato = v[i] + " " + v[j];
				} else {//tamanhoItemset > 2
					if(subStrPrincipal(v[i]) == subStrPrincipal(v[j])) {
						novoCandidato = combinaRotulos(v[i],v[j],tamanhoItemset);
						//frequencia deste conjunto						
						while(getline(arquivo_entrada[thread_id], transacao)){
							dup = strdup(transacao.c_str());
							token = strtok(dup," ");
							pos = 0;
							item = getItemPorPos(novoCandidato,pos);
							qtd_items = 0;
							while(token) {
								if(item == atoi(token)){
									qtd_items++;
									item = getItemPorPos(novoCandidato,++pos);
									if(item == -1)
										break;
									dup = strdup(transacao.c_str());
									token = strtok(dup," ");
								} else
									token = strtok(NULL," ");
							}
							free(dup);
							if(qtd_items == tamanhoItemset)
								frequencia++;
						}
					}
				}
				#pragma omp critical
				{
					if(frequencia >= suporte_minimo)
						novosCandidatos->insert(novosCandidatos->begin(),pair<string,unsigned int>(novoCandidato,frequencia));
				}
   			}// fim 2 for

   			if(i == 50)break;
   			// break;
   			// cout<<"VSF:"<<novosCandidatos->size()<<endl;

   			// #pragma omp master			
   			// for( map<string,unsigned int>::iterator it = novosCandidatos->begin(); it != novosCandidatos->end(); ++it ) {
		    // 	cout<<it->first<<endl;
		    // }
		    // #pragma omp barrier
   			
   			// exit(0);
   		
	    }
	    v.clear();
		if(tamanhoItemset > 2)
			novosCandidatos = eliminarSubconjuntosNaoFrequentes(novosCandidatos, candidatos, tamanhoItemset);

		delete candidatos;
		candidatos = novosCandidatos;

		novosCandidatos = NULL;
		if(habilitar_impressao)
			cout<<"Quantidade de conjuntos frequentes: "<<candidatos->size()<<endl<<endl;
	}

	finaliza_relogio();	
	if(habilitar_log)
		arquivo_log << "Tempo de execucao: "<<delta << " segundos" << endl;
	
	arquivo_entrada[0].close();
	arquivo_log.close();
	arquivo_saida.close();
	return 0;
}

