'''
GEM replay script
Author : Siyu Jian
'''

import logging
import datetime
import os
import json
import sys
import random
import csv
from multiprocessing import Pool
class runErrorList:
    FileNotFoundList=[]
    RuntimeErrorList = []
    def __init__(self):
        pass
    def MissingRawFile(self,runID):
        self.FileNotFoundList.append(runID)
    def Save(self):
        pass

runErrorList = runErrorList()

class gemJobs:
    runType = ""
    runID = 0
    SavePedestal = ""
    LoadPedestal = ""
    Mapping=""
    defaultSavePath=""
    defaultRawPath =""
    defaultPedestalPath = ""
    HitRootFile =""
    rawFileName=""
    nfiles = 3
    runConfigFile = ""
    rawDataStri=""
    def __init__(self,runType="",runID=0,Mapping="",SavePedestal="",LoadPedestal="",nevent=0,HitRootFile ="",Nfile=0):
        self.runID = runID
        self.runType = runType
        self.Mapping = Mapping
        self.SavePedestal = SavePedestal
        self.LoadPedestal = LoadPedestal
        self.HitRootFile = HitRootFile
    def Validate(self):
        if self.runID == 0 :
            return False
        if not self.Mapping :
            return  False
        if self.runType == "GetRootFile" and not self.LoadPedestal:
            return False
        return  True

    def getrunConfig(self,JobSavePath=""):
        if not self.Validate():
            return False
        if not self.SavePedestal:
            fname = "pedestal_LHRS_{runID}.root".format(runID = self.runID)
            if self.runID > 20000:
                fname = "pedestal_RHRS_{runID}.root".format(runID =self.runID)
            self.SavePedestal = os.path.join(self.defaultPedestalPath,fname)

        if not self.HitRootFile:
            rawfname = "prexLHRS_{}.root".format(self.runID)
            if self.runID > 20000:
                rawfname = "prexRHRS_UVaGEM_{}.root".format(self.runID)
            self.HitRootFile = os.path.join(self.defaultSavePath,rawfname)

        if not self.rawDataStri:
            rawfname = "prexLHRS_{}.dat".format(self.runID)
            if self.runID > 20000:
                rawfname = "prexRHRS_{}.dat".format(self.runID)
            self.rawDataStri = os.path.join(self.defaultRawPath, rawfname) + ", 0, {}".format(self.nfiles)

        if self.runType == "CalPedestal" and not self.LoadPedestal:
            self.LoadPedestal = self.SavePedestal

        configJob = runConfigGenerator(runType=self.runType,Mapping=self.Mapping,SavedPed=self.SavePedestal,LoadPed=self.LoadPedestal,\
                                       Nevent=self.nfiles*1000000,HitRootFile=self.HitRootFile,NFIle=self.nfiles,InputString=self.rawDataStri)
        self.runConfigFile = configJob.SaveConfig(fname=JobSavePath)

    def getRawfilename(self):
        if not self.rawFileName:
            filename = "prexLHRS_{}.dat".format(self.runID)
            if self.runID > 20000:
                filename = "prexRHRS_{}.dat".format(self.runID)

            self.rawFileName = os.path.join(self.defaultRawPath,filename)
        return  self.rawFileName

    def getRunType(self):
        pass

    def getSubmitTime(self):
        pass

    def getThreadID(self):
        pass

    def IsReplayed(self):
        pass


class runConfigGenerator:
    template = "#---------------------------------------------------------------\n" \
               "# This is an Automatic Generated Run File\n" \
               "#---------------------------------------------------------------\n" \
               "#GEM analysis configure file\n" \
               "#---------------------------------------------------------------\n" \
               "# runType\n" \
               "#---------------------------------------------------------------\n" \
               "RUNTYPE: {runType},0 ,0\n" \
               "#---------------------------------------------------------------\n" \
               "# Mapping file\n" \
               "#---------------------------------------------------------------\n" \
               "MAPPING: {MappingFile}\n" \
               "\n" \
               "#---------------------------------------------------------------\n" \
               "# Pedestal\n" \
               "#---------------------------------------------------------------\n" \
               "SAVEPED: {SavePedestalName} \n" \
               "\n" \
               "#---------------------------------------------------------------\n" \
               "# Path to load from Pedestal file\n" \
               "#---------------------------------------------------------------\n" \
               "LOADPED: {PedestalFile}\n" \
               "\n" \
               "#---------------------------------------------------------------\n" \
               "# N Event\n" \
               "#---------------------------------------------------------------\n" \
               "NEVENT: {nevent}\n" \
               "#Reult directory\n" \
               "HitRootFile: {SaveRootFname}\n" \
               "\n" \
               "###################################################################\n" \
               "# Input File for physics analysis; NOT FOR OTHER TYPE ANALYSIS\n" \
               "###################################################################\n" \
               "NFILE: {nfiles}\n" \
               "INPUTFILE:   {inputfile}\n" \
               "\n" \
               "\n"

    def __init__(self, runType="", Mapping="", SavedPed="", LoadPed="", Nevent=0, HitRootFile="", NFIle=0,
                 InputString=""):
        if "pedestal" in runType.lower():
            runType = "CalPedestal"
        elif "root" in runType.lower():
            runType = "GetRootFile"
        else:
            logging.error("Unsupport runType {}".format(runType))
            return False

        if runType == "CalPedestal" and not SavedPed:
            logging.error("In Pedestal Mode, no SavedPed parameter detected!!")
            return False
        if runType == "GetRootFile" and not (HitRootFile and LoadPed and InputString):
            logging.error("In root mode, HitRootfile or loadPed or InputString not detected!!")

        self.ConfigStr = self.template.format(runType=runType, MappingFile=Mapping, SavePedestalName=SavedPed,
                                              PedestalFile=LoadPed, \
                                              nevent=Nevent, SaveRootFname=HitRootFile, nfiles=NFIle,
                                              inputfile=InputString)

    def SaveConfig(self, fname=""):
        if not fname:
            logging.warning("Did not find the fname !!")
            fname = "tempConfig_{}.cfg".format(random.random())
        text_file = open(fname, "w")
        text_file.write(self.ConfigStr)
        text_file.close()
        return  fname

class runListAPI:
    '''
    Google and Local Run List API
    used for read the run informations
    '''
    def __init__(self):
        pass

class JobSubmitter:
    RawFileSourceFolder = ""
    server = "local"
    curDir = ""
    jsonConfigFile  = ''
    replayCommand = ""
    replayMappingFile = ""
    replayNevent = 0
    JobConfigSavePath = ""
    defaultRawPath = ""
    defaultPedestalPath =""
    defaultSavePath = ""
    def __init__(self, server="local"):
        self.server = server
        if not server:
            logging.warning("Server Not Defined!!, set it to local by default")
            self.server = "local"
        self.curDir = os.path.dirname(os.path.realpath(__file__))
        if self.server == "local":
            self.jsonConfigFile = os.path.join(self.curDir,"config/local.json")
        else:
            self.jsonConfigFile = os.path.join(self.curDir,"config/ifarm.json")
        self.readConfig(Config=self.jsonConfigFile)

    def isfolderexist(self, folder=""):
        return os.path.isdir(folder)

    def isfileexist(self, filename=""):
        return os.path.isfile(filename)

    def readConfig(self,Config=""):
        if not self.isfileexist(Config):
            logging.warning("Json Config file did not find!! Use with Caution!!")
            return
        try:
            with open(Config) as jsonfile:
                data  = json.load(jsonfile)
                self.replayCommand = data["replayCommand"]
                self.replayMappingFile = data["MAPPING"]
                self.replayNevent = int(data["NEVENT"])
                self.JobConfigSavePath = data["ConfigSavePath"]
                self.defaultRawPath = data["defaultRawPath"]
                self.defaultPedestalPath = data["defaultPedestalPath"]
                self.defaultSavePath = data["defaultSavePath"]
        except:
            logging.warning("Can not decode json file {}, Use with Caution!!".format(Config))

    def SearchRawRange(self,fpath=""):
        if not fpath:
            return None
        if not self.isfileexist(filename="{}.{}".format(fpath,0)):
            return None
        for i in range(0,100):
            if not self.isfileexist(filename="{}.{}".format(fpath,i)):
                break

        return [0,i]

    def ParserCSV(self,runfname=""):
        if not runfname or not self.isfileexist(runfname):
            logging.error("Can not find csv run List {}".format(runfname))
            return
        runList = []
        try:
            with open(runfname,newline='') as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    runID = int(row["runID"])
                    runType = row["runType"]
                    Mapping = row["Mapping"]
                    SavePedestal = row["savepedestal"]
                    LoadPedestal = row["pedestal"]
                    if "pedestal" in runType.lower():
                        runType = "CalPedestal"
                    elif "root" in runType.lower():
                        runType = "GetRootFile"
                    else:
                        logging.error("Unsupport runType {}".format(runType))
                    runList.append(gemJobs(runType=runType,runID=runID,Mapping=Mapping,SavePedestal=SavePedestal,LoadPedestal=LoadPedestal))
        except:
            logging.error("Error when parser CSV {}".format(runfname))
        return  runList

    def submitJob(self):
        pass

class iFarmSubmitter(JobSubmitter):

    def __init__(self):
        super().__init__(server='ifarm')

class localSubmitter(JobSubmitter):
    JobArray=[]
    runList = ""

    def __init__(self):
        super().__init__(server='local')
        print(self.defaultRawPath)
        print(self.defaultSavePath)

    def _run(self,runConfigfile = ""):
        if self.isfileexist(runConfigfile):
            logging.info("Working on {}".format(runConfigfile))
            # print(runConfigfile)
            os.system("{} {}".format(self.replayCommand,runConfigfile))

    def runAll(self,ncores = 8):
        threadPool = Pool(ncores)
        threadPool.map(self._run,[job.runConfigFile for job in self.JobArray])

    def replaySingleRun(self,job=gemJobs()):
        if job.Validate():
            job.getrunConfig(JobSavePath=os.path.join(self.JobConfigSavePath,"runConfig_{}.cfg".format(job.runID)))
        else:
            logging.warning("Error run {}".format(job.runID))

    def replayCSV(self,fname="replayList.csv"):
        for job in self.ParserCSV(runfname=fname):

            if not job.Mapping:
                job.Mapping = self.replayMappingFile

            job.defaultRawPath = self.defaultRawPath
            job.defaultSavePath = self.defaultSavePath
            job.defaultPedestalPath = self.defaultPedestalPath
            if self.SearchRawRange(job.getRawfilename()):
                job.nfiles = self.SearchRawRange(job.getRawfilename())[1]
            self.replaySingleRun(job=job)
            self.JobArray.append(job)

    def replay(self,runID=-999,runType ="",csvList=''):
        if runID != -999:
            self.replaySingleRun(runID=runID,runType = runType)

class TemplateDecoder:
    pass

class tester:
    def __init__(self):
        pass

    def runConfigTester(self):
        a = runConfigGenerator(runType="CalPedestal", Mapping="text.mapping", SavedPed="save.root", LoadPed="pesd.root",
                               Nevent=11000, HitRootFile="asasa.root", NFIle=2, InputString="sasasa.dat, 1 ,2")
        a.SaveConfig("text.cfg")
    def jobsubmitter(self):
        a = JobSubmitter(        )
        a.ParserCSV(runfname="replayList.csv")

if __name__ == '__main__':
    # if len(sys.argv) >1:
    #     for item in sys.argv:
    #         if item.endswith(".csv"):
    #             if "ifarm" not  in os.uname().nodename:
    runner = localSubmitter()
    runner.replayCSV()
    runner.runAll()

    # a = tester()
    # a.jobsubmitter()
    # # print(os.path.dirname(os.path.realpath(__file__)))
    # run locally
    # if "siyu" not in os.uname().nodename:
    #     a=JobSubmitter()
