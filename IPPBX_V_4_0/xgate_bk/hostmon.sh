#!/bin/bash 
#sh script for restart process
#Host monitor checking from xgate

a=0
ip_remote="10.22.7.76"
ip_local="10.22.7.75"


while [ 1 ]
do
   dt=`date`
   nmap $ip_remote -p 5060 | grep 5060 > hostmonitor.txt   #Redirect the out put to mon_xgate.txt

   if grep -q open  hostmonitor.txt; then   #Find the string xgate_monitor in mon_xgate.txt
      echo "xgate already running in $ip_remote"
   else
     echo "xgate not running in $ip_remote"
     echo "checking in $ip_local"
     sleep 2s
     ps -ef | grep xg > mon_xgate.txt
     if grep -q xgate mon_xgate.txt; then
         echo "xgate already running in $ip_local"
     else
         echo "$dt  Not Running... Restarting" >> xg_lookup.txt #Appending to xg_lookup.txt line by line
         echo "Started xgate in $ip_local"
         xgate                     #restarting script
     fi
  fi
 echo "sleeping for 5 sec"
   sleep 5s
done
~

