#########################################################################
# File Name: ping.sh
# Author: monster
# mail: 1104306242@qq.com
# Created Time: Sat 05 Aug 2017 08:54:45 AM CST
#########################################################################
#!/bin/bash

IP="www.baidu.com"

./ping $IP

var=$(cat out.txt)  #将out.txt的数据进行读取
loss=${var%% *}     #获取空格前的丢包数据
avg=${var##* }      #获取平均抵达时间

if [ $(echo "${avg}>100" | bc)==1 ];then
	#浮点数的比较需要借助其他工具
	#if [ $loss -gt 15 ];then
	#	echo "网络严重有误，请重新连接网络" | mail -s "网络连接有误" 1104306242@qq.com
	#else
		echo "网络存在延迟，不适合游戏" | mail -s "网络存在延迟" 1104306242@qq.com
	#fi
else
	echo "网络一切正常" | mail -s "网络正常" 1104306242@qq.com
fi
