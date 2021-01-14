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
    Type = ""
    runID = 0
    filePath = ""
    fileNameTemplate = ""
    ThreadID = 0
    Mapping=""
    FLag = False
    def __init__(self,runType="",runID=0,Mapping="",SavePedestal="",Pedestal="",nevent=0,HitRootFile ="",Nfile=0):
        pass

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
               "RUNTYPE: {runType}\n" \
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
        except:
            logging.warning("Can not decode json file {}, Use with Caution!!".format(Config))

class iFarmSubmitter(JobSubmitter):

    def __init__(self):
        super().__init__(server='ifarm')


class localSubmitter(JobSubmitter):
    JobArray=[]
    runList = ""

    def __init__(self):
        super().__init__(server='local')

    def replaySingleRun(self,runID,runType="",nevent=1000000):
        fileNameTemplate = "prexLHRS_{runID}.dat"
        if runID > 20000:
            fileNameTemplate = "prexRHRS_{runID}.dat"
        fileNameTemplate = fileNameTemplate.format(runID = runID)

        if "pedestal" in runType.lower():
            runType = "CalPedestal"
        elif "root" in runType.lower():
            runType = "GetRootFile"
        else:
            logging.error("Unsupport runType {}".format(runType))
            return False

        # create the configure file
        Mapping = ""
        

    def _replayCSV(self):
        pass

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


if __name__ == '__main__':
    a = tester()
    a.runConfigTester()
    print(os.path.dirname(os.path.realpath(__file__)))
    # run locally
    # if "siyu" not in os.uname().nodename:
    #     a=JobSubmitter()
