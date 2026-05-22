#!/bin/bash
#To copy the crash file into current dir

core_path="/var/lib/apport/coredump/"
latest_file=$(ls -t /var/lib/apport/coredump/ | head -1)
core_path+=$latest_file;
echo $core_path;
current_path=$(pwd);
current_path+=/build;
echo $current_path ;
scp $core_path $current_path;

