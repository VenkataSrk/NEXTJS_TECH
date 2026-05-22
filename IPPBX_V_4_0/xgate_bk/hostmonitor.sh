#!/bin/bash
#sh script for restart process

#Host monitor checking from xgate

a=0

while [ 1 ]
do
   dt=`date`
   nmap 10.22.7.75 -p 5060 | grep 5060 > hostmonitor.txt   #Redirect the out put to mon_xgate.txt

    if grep -q open  hostmonitor.txt; then   #Find the string xgate_monitor in mon_xgate.txt
      echo ""
     else
       ps -ef | grep xgate > mon_xgate.txt
#     if grep -q xgate mon_xgate.txt; then 
     if grep -q xgate mon_xgate.txt; then 
         echo ""
     else 
         echo "$dt  Not Running... Restarting" >> xg_lookup.txt #Appending to xg_lookup.txt line by line
#         ./xgate                     #restarting script
     fi
   fi
   sleep 5s
done

