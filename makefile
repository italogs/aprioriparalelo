run: compile 
	./exec 

compile: 
	g++ -lm main.cpp -o exec 


#compile: 
#	g++ -lm -W -Wall -ansi  -Wno-long-long -pedantic -Werror main.cpp -o exec 


# delete:
	# rm main teste.data treinamento.data