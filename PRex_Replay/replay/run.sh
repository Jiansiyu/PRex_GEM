#!/bin/bash
runID=$1
source ./env/setenv.sh
echo ${DB_DIR}
sleep 3

if [ $1 -gt 20000  ]; then
        echo "Working on Raster RHRS"
        sleep 2
#        analyzer -b -q 'setupR.C('${1}')'
else
        echo "Working on Raster LHRS"
        sleep 2
#        analyzer -b -q 'setupL.C('${1}')'
fi

#analyzer -b -q 'replay.C('${1}','30000')'
analyzer -b -q 'gemrun.C('${1}')'

