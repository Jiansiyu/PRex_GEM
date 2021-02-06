import os
import sys
from  multiprocessing import  Pool
import multiprocessing
from subprocess import call

def generateRunJobs(runID=0):
    if os.path.isfile("job/"+str(runID)+".txt"):
        return True
    with open("job/"+str(runID)+".txt", "w") as txt:
        txt.write("PROJECT: PRex\n")
        if len(sys.argv) < 9:
            txt.write("TRACK: debug\n")
        else:
            txt.write("TRACK: analysis\n")

        txt.write("COMMAND: /w/halla-scifs17exp/parity/disk1/siyu/prex_replay/gemreplay/run.csh {}\n".format(runID))
        txt.write("JOBNAME: counting_replay\n")
        txt.write("MEMORY: 4 GB\n")
        txt.write("DISK_SPACE: 20 GB\n")
    return True

def run(runID=0):
    os.system("./run.sh {}".format(runID))

if __name__ == '__main__':
    if 'ifarm' in os.uname().nodename:
        runIDs = [str(x) for x in sys.argv[1:] if str(x).isnumeric()]
        for runID in runIDs:
            generateRunJobs(runID=runID)
            ss = "job/" + str(runID) + ".txt"
            if os.path.isfile(ss):
                call(["jsub", ss])

    else:
        threadPool = Pool(min(multiprocessing.cpu_count(),4))
        runIDs = [int(x) for x in sys.argv[1:] if str(x).isnumeric()]
        threadPool.map(run,runIDs)
