#!/bin/bash

while getopts "slh" opt; do
    case "$opt" in
	s)
	    testlen=${testlen}short
	    ;;
	l)
	    testlen=${testlen}long
	    ;;
	h)
	    echo "Usage: $0 [-s|l] [h]"
	    echo
	    echo "-s    Run short version of test (more than 100x quicker)"
	    echo "-l    Run a very long version of test"
	    echo "-h    Display this message and exit"
	    exit
	    ;;
	*)
	    exit
	    ;;
    esac
done

echo $0 | grep "^./"
if [ "$?" -eq "0" ]; then
    echo "Refusing to run in the directory holding the script... This"
    echo "does delete data with rm -rf *, you know."
    exit
fi

if [ -z "$testlen" ]; then
    Clist="-C1 -C4 -C16"
    Plist="-P1 -P4 -P16"
    Ilist="-I1 -I50 -I100"
    Ulist="1024 2048 4096 8192 16384"
    maptype="Y"
    echo Running normal test
elif [ "${testlen}" == "short" ]; then
    Clist="-C4"
    Plist="-P4"
    Ilist="-I50 -I100"
    Ulist="1024"
    maptype="Y"
    echo Running short test
elif [ "${testlen}" == "long" ]; then
    Clist="-C1 -C4 -C8 -C16"
    Plist="-P1 -P4 -P8 -P16"
    Ilist="-I1 -I25 -I50 -I99 -I100"
    Ulist="1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576"
    maptype="Z"
else
    echo "What would you say a ${testlen} test is?"
    exit
fi

slist="-s -0"
#Don't use -mg, multiversion has bug that segfaults on deadlock
mglist="-m -g -0"

count=0
runs=0

echo Begun at $(date)

for dorun in "" "y"; do
    for C in $Clist; do
	for P in $Plist; do
	    for I in $Ilist; do
		for s in $slist; do
		    for U in $Ulist; do
			for o in "-o" "-${maptype}$((U*16))" "-0"; do
			    for mg in $mglist; do
				for exec in aeidb idb; do
				    for vg in ""; do # "valgrind -q"; do
					flags="$C $P $I $s -U$U $o $mg"
					cmd="$vg ../$exec $flags"
					if [ -z $dorun ]; then
					    count=$((count+1))
					else
					    rm -rf *
					    runs=$((runs+1))
					    /usr/bin/time -o timing -f"%e" $cmd
					    rc=$?
					    echo \[${runs}/${count}\] \[$(date)\] $(cat timing) : $cmd
					    if [ "$rc" -ne "0" ]; then
						echo "Process terminated with $rc"
						exit 1
					    fi
					fi
				    done
				done
			    done
			done
		    done
		done
	    done
	done
    done
done

if [ -z $short ]; then
    /bin/true
else
    echo "Ended at $(date)"
    exit
fi

echo Scaling - Single thread
for i in $(seq 10 20); do
    rm -rf *
    cmd="../idb -U $((2**i))"
    echo $(/usr/bin/time -f"%e" $cmd 2>&1) seconds : $cmd
done

echo Scaling - Single pbranch
for i in $(seq 10 20); do
    rm -rf *
    cmd="../aeidb -U $((2**i))"
    echo $(/usr/bin/time -f"%e" $cmd 2>&1) seconds : $cmd
done

echo Strong scaling - Multithread, Single Table
for i in $(seq 1 6); do
    rm -rf *
    cmd="../idb -U $((256*1024)) -P $((2**i)) -g -s"
    echo $(/usr/bin/time -f"%e" $cmd 2>&1) seconds : $cmd
done

echo Strong scaling - Multithread, Single Table Double Context
for i in $(seq 1 6); do
    rm -rf *
    cmd="../idb -U $((256*1024)) -P $((2**i)) -g -C2 -s"
    echo $(/usr/bin/time -f"%e" $cmd 2>&1) seconds : $cmd
done

echo Strong scaling - Multithread, Multi Table
for i in $(seq 1 8); do
    rm -rf *
    cmd="../idb -U $((256*1024)) -P $((2**i)) -s"
    echo $(/usr/bin/time -f"%e" $cmd 2>&1) seconds : $cmd
done

echo Strong scaling - Multi-pbranch, Single Table
for i in $(seq 1 8); do
    rm -rf *
    cmd="../aeidb -U $((256*1024)) -P $((2**i)) -g -s"
    echo $(/usr/bin/time -f"%e" $cmd 2>&1) seconds : $cmd
done

echo Strong scaling - Multi-pbranch, Single Table Double Context
for i in $(seq 1 8); do
    rm -rf *
    cmd="../aeidb -U $((256*1024)) -P $((2**i)) -g -C2 -s"
    echo $(/usr/bin/time -f"%e" $cmd 2>&1) seconds : $cmd
done

echo Strong scaling - Multi-pbranch, Multi Table
for i in $(seq 1 8); do
    rm -rf *
    cmd="../aeidb -U $((256*1024)) -P $((2**i)) -s"
    echo $(/usr/bin/time -f"%e" $cmd 2>&1) seconds : $cmd
done

echo "Ended at $(date)"
