app=${1}

if [ ${app} = "wordcount" ]; then
	mpirun --hostfile hostfile -np ${2} ./wordcount
fi
