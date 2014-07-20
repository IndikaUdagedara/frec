#!/bin/bash

cd $1
echo -n "" > names.list;
echo -n "" > names.db;
let k=0;


for i in $(ls); do 

	cd $i; 
	if [ $? -eq 0 ] 
		then
		echo -n "" > files.list;
		for j in $(ls | sort |grep jpg); do 
			echo "$j, $k" >> files.list; 
		done; 
		cwd=`pwd`;
		cd ..; 
		echo "RawImgDir_$k = \"$cwd\"" >> names.list;
		echo "$cwd, $k" >> names.db;
		let k=k+1; 
	fi
done
