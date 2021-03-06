'''

'''

import os
import sys
from pathlib import Path
import csv

localPath = "/home/newdriver/PRex/PRex_Data/Raw/"
ifarmPath = "/cache/halla/happexsp/raw/"
RawPath = localPath
if __name__ == '__main__':
    if "jlab" in os.uname()[1] or "aonl" in os.uname()[1]:
        RawPath = ifarmPath
    else:
        RawPath = localPath

    runCheckedList = [["runID","Status"],]
    for item in sys.argv[1:]:
        if '-' in item:
            RunRange=item.split('-')
            if RunRange[0].isnumeric() and RunRange[1].isnumeric():
                print(" ----->  Getting Run Range  {}-{} /{}".format(RunRange[0],RunRange[1],int(RunRange[1])-int(RunRange[0])+1))
                for runID in range(int(RunRange[0]),int(RunRange[1])+1):
                    print("\t --> Get {} : {}/{}".format(runID,runID-int(RunRange[0]),int(RunRange[1])-int(RunRange[0])+1))

                    filenameStr='{}prexLHRS_{}.dat.0'.format(RawPath,runID)
                    if int(runID) > 20000:
                        filenameStr='{}prexRHRS_{}.dat.0'.format(RawPath,runID)

                    print(filenameStr)
                    if os.path.isfile(filenameStr):
                        if Path(filenameStr).stat().st_size > 198432768:
                            cmd = './runVerifier {}'.format(runID)
                            os.system(cmd)
                            runCheckedList.append([runID,'Checked'])
                        else:
                            runCheckedList.append([runID,'SizeError'])
                    else:
                        runCheckedList.append([runID,'FileMissing'])

    with open("runLog.csv","w",newline='') as file:
        writer = csv.writer(file, delimiter='|')
        writer.writerows(runCheckedList)