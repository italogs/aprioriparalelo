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

#define RETIRAR <<"(Retirar mensagem)"

using namespace std;




struct timeval start, end;
double delta = 0.0;
void inicia_relogio() {
	gettimeofday(&start, NULL);
}

void finaliza_relogio() {
	gettimeofday(&end, NULL);
	delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
	end.tv_usec - start.tv_usec) / 1.e6;
	printf("Tempo Total de execucao = %12.10f\n",delta);	
}	

string IntToString (int a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}
string subStrPrincipal(string str){
	return str.substr(0,str.find_last_of(" "));
}

string getUltimoItem(string str){
	return str.substr(str.find_last_of(" "));
}
bool compstr(string &s1, string &s2) {
	if(s1.compare(s2)>0){return 1;}
	if(s1.compare(s2)<0){return -1;}
	return 0;
}
string combinaRotulos(string s1,string s2, int itemset) {
	int *vet =  new int[itemset];
	char *dup = strdup(s1.c_str());
	char *token = strtok(dup," ");
	int i = 0, j = 0;
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


int getItemPorPos(string str,unsigned pos){
	if(pos == 0) {
		for (unsigned i = 0; i < str.length(); i++) {
			if(str[i] == ' ')
				return atoi(  str.substr(0,i).c_str()  );
		}
	}
	unsigned cont = 0 ;
	for (unsigned i = 0; i < str.length(); i++) {
		if(str[i] == ' ') cont++;
		if(cont == pos) {
			unsigned j = i;
			while(str[j] != ' ') j++;
			return atoi(   str.substr(i+1,j).c_str()    );
		}
	}
	return -1;
}

void escreveConjuntoArquivo(map<string,int> *candidatos, ofstream &arquivo_saida,int tamanhoItemset) {
	arquivo_saida << "Items sets (L"<<tamanhoItemset<<")"<<endl;
	arquivo_saida <<"Quantidade novos Conjuntos: "<< candidatos->size() <<endl;
	arquivo_saida << "{ ";
	for (map<string,int>::iterator it = candidatos->begin(); it != candidatos->end(); ++it) {
		arquivo_saida << "{"<<it->first<<"} ";
	}
	arquivo_saida << "}"<<endl;
}
int naoAchouSubConjunto = 0;
void subset(int *arr, int *data, int start, int end, int index, int r,map<string,int> *candidatos){
    int j, i;

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

map<string,int> *eliminarSubconjuntosNaoFrequentes(map<string,int> *novosCandidatos,map<string,int> *candidatos,int tamanhoItemset){
	char *dup,*token;
	int *vetNovosCandidatos = new int[tamanhoItemset],*vetSubSet = new int[tamanhoItemset-1], posCandidato = 0;
	for(map<string,int>::iterator it = novosCandidatos->begin() ; it != novosCandidatos->end() ; ) {
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
	}

	return novosCandidatos;
}


int main(int argc,char *argv[]){

	if(argc != 3) {
		printf("Parametros incompletos \n");
		return 0;
	}

	bool habilitar_log = false;
	bool habilitar_arquivo_saida = true;

	int suporte_minimo = atoi(argv[1]);
	fstream arquivo_entrada(argv[2]);
	ofstream arquivo_saida("saida_sequencial.data");
	ofstream arquivo_log("log_sequencial.data");
	string transacao;

	unsigned tamanhoItemset = 1;
 	map<string,int> *candidatos = new map<string,int>;
 	map<string,int>::iterator it;

	int frequencia = 0 , it_presente = 0,it2_presente = 0, pos =0 , item;
	char *dup = NULL, *token = NULL;

 	inicia_relogio();

	while(getline(arquivo_entrada, transacao)){
		char *dup = strdup(transacao.c_str());
		char *token = strtok(dup," ");
		while(token) {
			if(candidatos->find(token) == candidatos->end()) {
				string itemset(token);
				candidatos->insert(candidatos->begin(),pair<string,int>(itemset,1));
			} else {
				it = candidatos->find(token);
				it->second++;
			}
			token = strtok(NULL," ");
		}
		free(dup);
	}
	for(map<string,int>::iterator it = candidatos->begin() ; it != candidatos->end();  ) {
		if(it->second < suporte_minimo) {
			candidatos->erase(it++);
		} else{
			++it;
		}
	}
	cout<<"Suporte:"<<suporte_minimo<<endl;
	cout<<"Itemset tamanho: "<<tamanhoItemset<<endl;
	cout<<"Quantidade de novos conjuntos: "<<candidatos->size()<<endl;
	if(habilitar_log) {
		arquivo_log << "Suporte:"<<suporte_minimo<<endl;
		arquivo_log << "Itemset tamanho: "<<tamanhoItemset<<endl;
		arquivo_log << "Quantidade de novos conjuntos: "<<candidatos->size()<<endl;
	}
	if(habilitar_arquivo_saida){
		arquivo_saida << "Suporte minimo: " << suporte_minimo<<endl;
	}
	map<string,int> *novosCandidatos;


	while(candidatos->size() > 0) {
		if(habilitar_arquivo_saida){
			escreveConjuntoArquivo(candidatos,arquivo_saida,tamanhoItemset);
		}
		novosCandidatos = new map<string,int>;
		tamanhoItemset++;
		//lacos para gerar as combinacoes
		cout<<"Quantidade novos Conjuntos: "<< candidatos->size() <<endl;
		cout<<"Itemset tamanho: "<<tamanhoItemset<<endl;
		for(map<string,int>::iterator it = candidatos->begin() ; it != candidatos->end() ; ++it) {
			map<string,int>::iterator itTemp = it;
			itTemp++;
			for(map<string,int>::iterator it2 = itTemp ; it2 != candidatos->end() ; ++it2) {
				string conj1(it->first); 
				string conj2(it2->first);

				arquivo_entrada.clear();
				arquivo_entrada.seekg(0);

				frequencia = 0;
				string novoCandidato;
				if (tamanhoItemset == 2) {

					while(getline(arquivo_entrada, transacao)){
						dup = strdup(transacao.c_str());
						token = strtok(dup," ");
						it_presente = it2_presente = 0;
						while(token) {
							if(!it_presente && strcmp(it->first.c_str(),token) == 0) 
								it_presente = 1;
							if(!it2_presente && strcmp(it2->first.c_str(),token) == 0)
								it2_presente = 1;
							if(it_presente == 1 && it2_presente == 1){
								frequencia++;
								break;
							}
							token = strtok(NULL," ");
						}
						free(dup);
					}

					novoCandidato = it->first + " " + it2->first;
				} else {//tamanhoItemset > 2
					if(subStrPrincipal(it->first) == subStrPrincipal(it2->first)) {
						novoCandidato = combinaRotulos(it->first,it2->first,tamanhoItemset);
						//frequencia deste conjunto						
						while(getline(arquivo_entrada, transacao)){
							dup = strdup(transacao.c_str());
							token = strtok(dup," ");
							pos = 0;
							item = getItemPorPos(novoCandidato,pos);
							unsigned qtd_items = 0;
							while(token) {
								if(item == atoi(token)){
									qtd_items++;
									item = getItemPorPos(novoCandidato,++pos);
									if(item == -1)
										break;
									dup = strdup(transacao.c_str());
									token = strtok(dup," ");
								} else {
									token = strtok(NULL," ");
								}
							}
							free(dup);
							if(qtd_items == tamanhoItemset) {
								frequencia++;
							}
						}
					}
				}
				if(frequencia >= suporte_minimo) {
					novosCandidatos->insert(novosCandidatos->begin(),pair<string,int>(novoCandidato,frequencia));
				}
			}
		}
		if(tamanhoItemset > 2)
			novosCandidatos = eliminarSubconjuntosNaoFrequentes(novosCandidatos, candidatos, tamanhoItemset);



		delete candidatos;
		candidatos = novosCandidatos;




		novosCandidatos = NULL;

		cout<<"Quantidade de conjuntos frequentes: "<<candidatos->size()<<endl<<endl;
	}

	finaliza_relogio();	
	if(habilitar_log){
		arquivo_log << "Tempo de execucao: "<<delta << " segundos" << endl;
	}
	
	arquivo_entrada.close();
	arquivo_log.close();
	arquivo_saida.close();
	return 0;
}

