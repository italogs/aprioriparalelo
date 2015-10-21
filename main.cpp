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

// #include <omp.h>
// #include <mpi.h>

#define RETIRAR <<"(Retirar mensagem)"


using namespace std;

// struct timeval start, end;

int main(int argc,char *argv[]){
	
	// gettimeofday(&start, NULL);

	fstream arquivo_entrada("entrada.data");

	ofstream arquivo_saida("saida.data");
	ofstream arquivo_log("log.data");
	string linha;

	arquivo_entrada >> linha;
	int nItemsPorTransacao = atoi(linha.c_str());
	arquivo_entrada >> linha;
	int nTransacoes = atoi(linha.c_str());
	arquivo_entrada >> linha;
	int suporte_minimo = atoi(linha.c_str());
	arquivo_log << "nItemsPorTransacao: " << nItemsPorTransacao << endl<<  "nTransacoes: " << nTransacoes<<endl<<"suporte_minimo: "<<suporte_minimo<<endl;
	arquivo_log << "Matriz Entrada"<<endl;
	int matriz[nTransacoes][nItemsPorTransacao];
	int vet1[nItemsPorTransacao],vet2[nItemsPorTransacao];

	for(int i = 0 ; i < nTransacoes ; i++){
		for(int j = 0 ; j < nItemsPorTransacao ; j++) {
			arquivo_entrada >> linha;
			matriz[i][j] = atoi(linha.c_str());	
		}
	}





	for (int i = 0; i < nTransacoes; i++)
	{
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
    for(int i = 0 ; i < nItemsPorTransacao ; i++){
    	int frequencia = 0;
		for(int j = 0 ; j < nTransacoes  ; j++) {
			if (matriz[j][i] > 0 ) 
				frequencia += matriz[j][i];
		}
		char temp[2] = {i+48};
		temp[1]='\0';
		string itemset(temp);
		candidatos.insert(candidatos.begin(),pair<string,int>(itemset,frequencia));
	}

	// candidatos
	arquivo_saida << "Items sets (L"<<tamanhoItemset<<")"<<endl;
	arquivo_saida << "{ ";
	for (map<string,int>::iterator it = candidatos.begin(); it != candidatos.end(); ++it)
	{
		if(it->second >= suporte_minimo) {
			arquivo_saida << "{"<<it->first<<"} ";
		} else {
			cout<<"Eliminado -> itemset "<<it->first<<" frequencia: "<<it->second<<endl;
			candidatos.erase(it->first);
		}
	}
	arquivo_saida << "}"<<endl;
	
	cout<<"Candidatos.size(): "<<candidatos.size()<<endl;
	map<string,int> novosCandidatos;
	cout<<"suporte minimo: "<<suporte_minimo<<endl<<endl<<endl;

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
				

				if (tamanhoItemset == 2) {
					int cont = 0;
					for (int i = 0; i < nTransacoes; i++)
					{
						if(matriz[i][atoi(it->first.c_str())]  > 0  && matriz[i][atoi(it2->first.c_str())]  > 0)
							cont++;
					}
					novosCandidatos.insert(novosCandidatos.begin(),pair<string,int>(it->first + " " + it2->first,cont));
					cout<<"Novo Candidato: {";
					cout<<it->first + "," + it2->first<<"} Freq: "<<novosCandidatos[it->first + " " + it2->first]<<endl;
					
				} else {//tamanhoItemset > 2

				cout<<"<"<<it->first<<">,";
				cout<<"<"<<it2->first<<">"<<endl;
				}

			}
		}
		cout<<"Quantidade novos candidatos: "<<novosCandidatos.size()<<endl;
		// selecionando candidatos
		arquivo_saida << "Items sets (L"<<tamanhoItemset<<")"<<endl;
		arquivo_saida << "{ ";
		candidatos.clear();

		for (map<string,int>::iterator it = novosCandidatos.begin(); it != novosCandidatos.end(); ++it)
		{
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
		
		// break;


		
	}

	// gettimeofday(&end, NULL);
	// double delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
	// end.tv_usec - start.tv_usec) / 1.e6;
	// arquivo_log << "Tempo de execucao: "<<delta << " segundos" << endl;	
	// printf("time=%12.10f\n",delta);	
	// return 0;
}

