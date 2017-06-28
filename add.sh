#########################################################################
# File Name: add.sh
# Author: monster
# mail: 1104306242@qq.com
# Created Time: Wed 28 Jun 2017 04:15:29 PM CST
#########################################################################
#!/bin/bash
i=1
sum=0
str=''
while [ $i -le 100 ]
do
	let sum+=i
	[ $i -eq 1 ]&&{
	str=$i
	let i++
	continue
}
	str=$str'+'$i
	let i++
done
						
echo "$str=$sum"				
