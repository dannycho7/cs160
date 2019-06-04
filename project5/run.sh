#!/bin/bash
for i in {1..84}
do
	CMD="./lang < ../project6/tests/${i}.good.lang"
	echo $CMD
	eval $CMD
done
