#########################################################################
# File Name: maxmin.sh
# Author: monster
# mail: 1104306242@qq.com
# Created Time: Wed 28 Jun 2017 04:38:06 PM CST
#########################################################################
#!/bin/bash
function mymaxmin(){
    max=$1
	min=$1
	for i in $@
do
	if [ $max -le $i ];then
		max=$i
	fi
	if [ $min -gt $i ];then
		min=$i
	fi
done
	echo "max=$max,min=$min"
}
																							
mymaxmin "$@"
