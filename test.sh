#!/bin/sh 
make;
testDirectory="./testCmds/";
./server &
#Récupération du pid du serveur
pid=$!;

sleep 0.5;
# Teste tout les fichiers tst du répertoire de test
for TEST in ${testDirectory}*.tst;do
      ./client < $TEST;  
done
 
sleep 0.5;
pkill server
