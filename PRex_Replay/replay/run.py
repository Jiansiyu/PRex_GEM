import os
import sys
from  multiprocessing import  Pool
import multiprocessing

def run(runID=0):
    # print(runID)
    os.system("./run.sh {}".format(runID))

if __name__ == '__main__':
    threadPool = Pool(max(multiprocessing.cpu_count(),8))

    runIDs = [int(x) for x in sys.argv[1:] if unicode(x, 'utf-8').isnumeric()]
    threadPool.map(run,runIDs)