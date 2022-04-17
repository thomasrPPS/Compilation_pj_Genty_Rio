#! /bin/bash

rm results_ref.txt

for i in 1 2 3 4 5 6 7 
do  
	echo -n "S_KO $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc_ref tests/Syntaxe/KO/test$i.c -s &> buffer.txt
	TEMP=$(echo $(cat buffer.txt)  | cut -d ":" -f 1)
	echo $BUFFER$TEMP >> results_ref.txt
done

for i in 1 2 3 4 5 6 7
do 
	echo -n "S_OK $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc_ref tests/Syntaxe/OK/test$i.c -s &> buffer.txt
	RESULT=$(cat buffer.txt)
	if [ -z "$RESULT" ]
	then
		echo -n "Test okay" > buffer.txt
		RESULT=$(cat buffer.txt) 
	else
		RESULT=$(cat buffer.txt)
	fi
	echo $BUFFER$RESULT >> results_ref.txt
done

for i in 1 2 3 4 5 6 7 8 9 10 11
do 
	echo -n "V_KO $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc_ref tests/Verif/KO/test$i.c  &> buffer.txt
	TEMP=$(echo $(cat buffer.txt)  | cut -d ":" -f 1)
	echo $BUFFER$TEMP >> results_ref.txt
done

for i in 1 2 3 4 5 6 7 8 9 10 11
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
	echo $BUFFER$RESULT >> results_ref.txt
done

for i in 1 2 3 4 5
do 
	echo -n "G_KO $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc_ref tests/Gencode/KO/test$i.c &> buffer.txt
	java -jar tools/Mars_4_2.jar  out.s &> buffer.txt
	head -n 3 buffer.txt | tail -1 > buff.txt
	TEMP=$(cat buff.txt)
	echo $BUFFER$TEMP >> results_ref.txt
done

for i in 1 2 3 4 5 6
do 
	echo -n "G_OK $i :" > buffer.txt
	BUFFER=$(cat buffer.txt) 
	./minicc_ref tests/Gencode/OK/test$i.c -s &> buffer.txt
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
	echo $BUFFER$RESULT >> results_ref.txt
done
rm buff.txt
