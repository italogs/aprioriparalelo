
make
sudo sh enableHyperthreading.sh

for i in 1 2 3 4 5
do
	echo "Paralelo paralelo_hp_on_ $i"
	./apriori_paralelo 1 1 1 400 basedados.data 4 > paralelo_hp_on_16_$i.txt
done

sudo sh disableHyperthreading.sh

for i in 1 2 3 4 5
do
	echo "Paralelo paralelo_hp_off_ $i "
	./apriori_paralelo 1 1 1 400 basedados.data 4 > paralelo_hp_off_16_$i.txt
done

	# for i in 1 2 3 4 5
	# do
	# 	echo "Sequencial $i"
	# 	./apriori_sequencial 1 1 1 800 basedados.data > sequencial_$i.txt
	# done



