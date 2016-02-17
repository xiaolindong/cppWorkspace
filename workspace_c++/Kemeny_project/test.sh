#!/bin/bash
#cd ./src
dir="./data/test"
result="./results.txt"
for f in "$dir"/*;do 
 # cat $f
	name=$f
	echo $name >> $result
	./kemeny "$name" >> $result
done
