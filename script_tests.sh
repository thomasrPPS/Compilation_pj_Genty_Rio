#! /bin/bash

rm log.txt

for i in 1 2 3 4 5 6 7 8 
do  
	echo -n "S_KO $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc tests/Syntaxe/KO/test$i.c -s &> buffer.txt
	echo $BUFFER$(cat buffer.txt) >> log.txt
done

for i in 1 2 3 4 5
do 
	echo -n "S_OK $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc tests/Syntaxe/OK/test$i.c -s &> buffer.txt
	RESULT=$(cat buffer.txt)
	if [ -z "$RESULT" ]
	then
		echo -n "Test okay" > buffer.txt
		RESULT=$(cat buffer.txt) 
	else
		RESULT=$(cat buffer.txt)
	fi
	echo $BUFFER$RESULT >> log.txt
done

for i in 1 2 3 4 5 6 7 8
do 
	echo -n "V_KO $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc tests/Verif/KO/test$i.c  &> buffer.txt
	echo $BUFFER$(cat buffer.txt) >> log.txt
done

for i in 1 2 3 4 5
do 
	echo -n "V_OK $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc tests/Verif/OK/test$i.c -s &> buffer.txt
	RESULT=$(cat buffer.txt) 
	if [ -z "$RESULT" ]
	then
		echo -n "Test okay" > buffer.txt
		RESULT=$(cat buffer.txt) 
	else
		RESULT=$(cat buffer.txt)
	fi
	echo $BUFFER$RESULT >> log.txt
done

for i in 1 2 3 4 5
do 
	echo -n "G_KO $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc tests/Gencode/KO/test$i.c &> buffer.txt
	java -jar tools/Mars_4_2.jar  out.s &> buffer.txt
	head -n 3 buffer.txt | tail -1 > buff.txt
	echo $BUFFER$(cat buff.txt) >> log.txt
done

for i in 1 2 3 4 5
do 
	echo -n "G_OK $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc tests/Gencode/OK/test$i.c -s &> buffer.txt
	java -jar tools/Mars_4_2.jar  out.s &> buffer.txt
	head -n 3 buffer.txt | tail -1 > buff.txt
	RESULT=$(cat buff.txt) 
	if [ -z "$RESULT" ]
	then
		echo -n "Test okay" > buff.txt
		RESULT=$(cat buff.txt) 
	else
		RESULT=$(cat buff.txt)
	fi
	echo $BUFFER$RESULT >> log.txt
done
rm buff.txt

rm buffer.txt

diff -a log.txt  results_ref.txt > buffer.txt
BUFFER=$(cat buffer.txt) 
if [ -z "$BUFFER" ]
then
	echo -n "Il n'y a pas d'erreur qui ont été trouvées dans le compilateur \n"
else
	diff -a log.txt  results_ref.txt
fi
