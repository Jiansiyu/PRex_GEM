
#
# $1 Database filename 
# $2 database name sting
# $3 database that need to replaced by
# $4 backup folder
FILENAME=$1
PARAMETERNAME=$2
PARAMETER=$3
BACKUPFNAME=$4

echo "in file :${FILENAME} ${PARAMETERNAME} will be changed to  ${PARAMETER}" 

echo "original db will be backuped to ${BACKUPFNAME}"

cp ${FILENAME} ${BACKUPFNAME}

sed -i -e "s/\($PARAMETERNAME\).*/\1 = $PARAMETER/" $FILENAME

echo "data base changed during this process::"

diff  $FILENAME $BACKUPFNAME

