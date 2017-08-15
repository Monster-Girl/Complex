#########################################################################
# File Name: shellsort.sh
# Author: monster
# mail: 1104306242@qq.com
# Created Time: Tue 15 Aug 2017 06:23:44 PM CST
#########################################################################
#!/bin/bash
read -a arr
function shellsort()
{
	arr=$1
	local gap=$2
	while [ $gap -gt 1 ]
	do
		((gap=gap/3+1))
		for (( i=0;i<$2-gap;i++))
		do 
			local end=$i;
			local tmp=${arr[$gap +$end]}
			while [ $end -ge 0 ]
			do
				if [ ${arr[$end]} -gt $tmp ];then
					((arr[$end+$gap]=arr[$end]))
					((end-=gap))
				else
					break;
				fi
			done
			arr[$end+$gap]=$tmp
		done
	done
	echo ${arr[*]}
}

shellsort ${arr} ${#arr[@]}

