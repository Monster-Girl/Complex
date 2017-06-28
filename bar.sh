#########################################################################
# File Name: bar.sh
# Author: monster
# mail: 1104306242@qq.com
# Created Time: Wed 28 Jun 2017 03:36:45 PM CST
#########################################################################
#!/bin/bash
i=0
index=0
bar=''
lable=('|' '/' '-' '\\')
while [ $i -le 100 ]
do
	printf "[%-100s][%d%%][%c]\r" "$bar" "$i" "${lable[$index]}"
	bar='#'$bar	
	let i++
	let index++
	let index%=4
	usleep 30000	
done
	printf "\n"

