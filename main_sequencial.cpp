#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdbool.h>
#include <unistd.h>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sys/time.h>

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

string subStrPrincipal(string str){
	return str.substr(0,str.find_last_of(" "));
}

string getUltimoItem(string str){
	return str.substr(str.find_last_of(" "));
}

int getItemPorPos(string str,int pos){

	if(pos == 0) {
		for (int i = 0; i < str.length(); i++) {
			if(str[i] == ' ')
				return atoi(  str.substr(0,i).c_str()  );
		}
	}
	int cont = 0 ;
	for (int i = 0; i < str.length(); i++) {
		if(str[i] == ' ') cont++;
		if(cont == pos) {
			int j = i;
			while(str[j] != ' ') j++;
			return atoi(   str.substr(i+1,j).c_str()    );
		}
	}
	cout<<"Erro na funcao"<<endl;
	return -1;
}

int main(int argc,char *argv[]){

	fstream arquivo_entrada("entrada_sequencial.data");

	ofstream arquivo_saida("saida_sequencial.data");
	ofstream arquivo_log("log_sequencial.data");
	string linha;

	arquivo_entrada >> linha;
	int nItemsPorTransacao = atoi(linha.c_str());
	arquivo_entrada >> linha;
	int nTransacoes = atoi(linha.c_str());
	arquivo_entrada >> linha;
	int suporte_minimo = atoi(linha.c_str());
	arquivo_log << "nItemsPorTransacao: " << nItemsPorTransacao << endl<<  "nTransacoes: " << nTransacoes<<endl<<"Suporte Minimo: "<<suporte_minimo<<endl;
	arquivo_log << "Matriz Entrada"<<endl;
	int matriz[nTransacoes][nItemsPorTransacao];
	int vet1[nItemsPorTransacao],vet2[nItemsPorTransacao];

	for(int i = 0 ; i < nTransacoes ; i++){
		for(int j = 0 ; j < nItemsPorTransacao ; j++) {
			arquivo_entrada >> linha;
			matriz[i][j] = atoi(linha.c_str());	
		}
	}

	for (int i = 0; i < nTransacoes; i++) {
		arquivo_log << "#"<<i<<"\t";
	}
	arquivo_log << endl;
	for(int i = 0 ; i < nTransacoes ; i++){
		for(int j = 0 ; j < nItemsPorTransacao ; j++) {
			arquivo_log << matriz[i][j] <<"\t";
		}
		arquivo_log << endl;
	}

	int tamanhoItemset = 1;
 	map<string,int> candidatos;
 	// bool tem_candidato = true;
 	int frequencia = 0;
 	inicia_relogio();
    for(int i = 0 ; i < nItemsPorTransacao ; i++){
    	frequencia = 0;

		for(int j = 0 ; j < nTransacoes  ; j++) {
			if (matriz[j][i] > 0 ) 
				frequencia+=1;// matriz[j][i];
		}
		char temp[2] = {i+48};
		temp[1]='\0';
		string itemset(temp);
		candidatos.insert(candidatos.begin(),pair<string,int>(itemset,frequencia));
	}

	cout<<"Itemset tamanho: "<<tamanhoItemset<<endl;
	cout<<"Quantidade de novos conjuntos: "<<candidatos.size()<<endl;
	arquivo_saida << "Items sets (L"<<tamanhoItemset<<")"<<endl;
	arquivo_saida << "{ ";
	for (map<string,int>::iterator it = candidatos.begin(); it != candidatos.end(); ++it) {
		if(it->second >= suporte_minimo) {
			arquivo_saida << "{"<<it->first<<"} ";
		} else {
			candidatos.erase(it->first);
		}
	}
	arquivo_saida << "}"<<endl;
	
	cout<<"Quantidade de conjuntos frequentes: "<<candidatos.size()<<endl<<endl;
	map<string,int> novosCandidatos;

	while(candidatos.size() > 0 ) {

		tamanhoItemset++;
		//lacos para gerar as combinacoes
		cout<<"Itemset tamanho: "<<tamanhoItemset<<endl;
		for(map<string,int>::iterator it = candidatos.begin() ; it != candidatos.end() ; ++it) {
			map<string,int>::iterator itTemp = it;
			itTemp++;
			for(map<string,int>::iterator it2 = itTemp ; it2 != candidatos.end() ; ++it2) {
				string conj1(it->first); 
				string conj2(it2->first);
				
				int  frequencia = 0;
				if (tamanhoItemset == 2) {

					for (int i = 0; i < nTransacoes; i++) {
						if(matriz[i][atoi(it->first.c_str())]  > 0  && matriz[i][atoi(it2->first.c_str())]  > 0)
							frequencia++;
					}
					novosCandidatos.insert(novosCandidatos.begin(),pair<string,int>(it->first + " " + it2->first,frequencia));
				} else {//tamanhoItemset > 2
					if(subStrPrincipal(it->first) == subStrPrincipal(it2->first)){
						string novoCandidato = subStrPrincipal(it->first) + getUltimoItem(it->first) + getUltimoItem(it2->first);
						//frequencia deste conjunto
						for (int i = 0; i < nTransacoes; i++) {
							vet1[i] = matriz[i][getItemPorPos(novoCandidato,0)];
						}

						for (int k = 1; k < tamanhoItemset; k++) {
							int item = getItemPorPos(novoCandidato,k);
							for (int i = 0; i < nTransacoes; i++) {
								if(vet1[i] > 0 && matriz[i][item] > 0) vet1[i] = 1;
								else vet1[i] = 0;
							}
						}

						for (int i = 0; i < nTransacoes; i++) {
							if(vet1[i] > 0 ) frequencia++;
						}
						novosCandidatos.insert(novosCandidatos.begin(),pair<string,int>(novoCandidato,frequencia));
					}
				}
			}
		}
		cout<<"Quantidade novos candidatos: "<<novosCandidatos.size()<<endl;
		// selecionando candidatos
		arquivo_saida << "Items sets (L"<<tamanhoItemset<<")"<<endl;
		arquivo_saida << "{ ";
		candidatos.clear();

		for (map<string,int>::iterator it = novosCandidatos.begin(); it != novosCandidatos.end(); ++it) {
			if(it->second >= suporte_minimo) {
				candidatos.insert(candidatos.begin(),pair<string,int>(it->first,it->second));
				arquivo_saida << "{"<<it->first<<"} ";
			} else {
				cout<<"Eliminado -> itemset "<<it->first<<" frequencia: "<<it->second<<endl;
			}
		}
		arquivo_saida << "}"<<endl;
		novosCandidatos.clear();
		cout<<"Quantidade de conjuntos frequentes: "<<candidatos.size()<<endl<<endl<<endl;
	}
	finaliza_relogio();	
	arquivo_log << "Tempo de execucao: "<<delta << " segundos" << endl;	
	return 0;
}

