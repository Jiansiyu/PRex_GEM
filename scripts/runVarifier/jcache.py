'''
* jget scipt used for get the file from the mss 
*     
*      Siyu Jian (sj9va@virginia.edu)
*
* By defaul, the looking path is located at  /mss/halla/happexsp/raw
* full name of the file need to be specified
* it also can take input / multi input 
*
* input(it can take any of those):
*       run list file [filename, save path[optinal]]
*       run number
*       run name 
*       run name with path
*       save path [optinal, if not specified, it will save to the current path]
*
* othertings:
*             It will check the existance of the file in the MSS, if not exist, skip it and write an log
*
* 
* 
* Function List 
*  number      1
*  runname without runID   1
*  runname with runID
*  load file txt
'''

import os
import glob
import sys
class mssget(object):
    '''
    '''
    def __init__(self):
        self.mssPath="/mss/halla/happexsp/raw/" 
        self.SavePath="./"
        self.MssFileQauryList=[]
        self.MssSuccessList=[]
        self.MssFailList=[]
        self.CurrentFile=""
        pass

    def GetFiles(self):
        #start get the file and save the file into the folder 
        commandList=[]
        for file in self.MssFileQauryList:
            if len(file) > 3:
                jgetCMD="jcache get {0} ".format(file)
                commandList.append(jgetCMD)
        commandList=list(set(commandList))
        commandList.sort()
        for item in commandList:
            print("Get Item: {}".format(item))
            os.system(item)

    def setSavePath(self, SavePath="."):
        self.SavePath=SavePath
    
    def ReadListFile(self,RunListFileName="runList.txt"):
        '''
        read the file list from the file 
        add support for multi format support

        formate can be 

        run nmae (or run number) start run ID (optional) end run ID(optional)

        prexRHRS_1111.dat,     0 , 1
        or
        prexRHRS_1111.dat
        or 
        1111     0 , 1
        '''

        # parser the file 
        # only take txt file
        runListArray=[]
        FileListArray=[]
        if ".txt" in RunListFileName:
            with open(RunListFileName) as fileio:
                for line in fileio:
                    if '#' not in line:
                        runListArray.append([item for item in line.strip().split(',')])
                        
                    else:
                        runListArray.append([item for item in line.strip().split('#')[0].strip().split(',')])

        for line in runListArray:
            if len(line) == 2 and len(line[0]) > 2:
                for item in self.ReadList(avg=line[0],startRunNumber=int(line[1])):
                    if len(item) !=0:
                        FileListArray.append(item[0])
            elif len(line) == 3 and len(line[0]) > 2:
                for item in self.ReadList(avg=line[0],startRunNumber=int(line[1]),endRunNumber=int(line[2])):
                    if len(item) !=0:
                        FileListArray.append(item[0])
            elif len(line) ==1 and len(line[0]) > 2:
                for item in self.ReadList(avg=line[0]):
                    if len(item) !=0:    
                        FileListArray.append(item[0])
        return FileListArray
    
    def ReadList(self, avg="",startRunNumber=-1, endRunNumber=-1):
        '''
        need multi format support
        input number? input filename? input filename with path? or combined
        '''
        fileList=[]
        
        if os.path.isfile(avg) and ".txt" in avg:
            for item in self.ReadListFile(RunListFileName=avg):
                if len(item) != 0:
                    fileList.append(item)
        elif avg.isnumeric():
            # this is the number that want to convert into an file name
            # and get all the files in the mss
            if startRunNumber != -1 :
                if endRunNumber > startRunNumber:
                    for splitID in range(startRunNumber,endRunNumber+1):
                        for item in self.ReadListWNumber(runNumber=int(avg),FileSplitID=splitID):
                            fileList.append(item)
                else:
                    for item in self.ReadListWNumber(runNumber=int(avg),FileSplitID=startRunNumber):
                        fileList.append(item)
            else:
                filename=self.ReadListWNumber(runNumber=int(avg))
                for item in filename:
                    fileList.append(item)
        else:
            for item in self.ReadListWname(filename=avg):
                fileList.append(item)
        
        for item in fileList:
            self.MssFileQauryList.append(item)
        return fileList


    def ReadListWNumber(self,runNumber=123, FileSplitID=0):
        '''
        read the file list from the input number
        '''
        filename=""
        if runNumber < 20000:
            filename="{0}prexLHRS_{1}.dat".format(self.mssPath,runNumber)
            if FileSplitID != -1:
                filename="{0}.{1}".format(filename,FileSplitID)
        else:
            filename="{0}prexRHRS_{1}.dat".format(self.mssPath,runNumber)
            if FileSplitID != -1:
                filename="{0}.{1}".format(filename,FileSplitID)
        return self.ReadListWname(filename=filename)

    def ReadListWname(self, filename="aa.dat.0", FileSplitID=-1):
        '''
        Get all file list
        if the file end with run number, it will only read out this one 
        if the file end with '.dat', it will return the list all the runs ith different runID
        
        '''
        # form full filename

        if not self._isFileWpath(filename=filename):
            filename="{0}{1}".format(self.mssPath,filename)
        filenamewithoutNumber=filename
        
        if filename[-1].isdigit():
            return glob.glob("{}".format(filenamewithoutNumber))
        elif FileSplitID ==-1 :
            return glob.glob("{}.*".format(filenamewithoutNumber))
        else:
            return glob.glob("{0}.{}".format(filenamewithoutNumber,FileSplitID))
    
    def _isFileWpath(self, filename=""):
        '''
        Check the input number wether have path
        '''
        return '/' in filename

    def _IsFileExist(self,filename="text.txt"):
        '''
        Check file exist or not
        '''
        return os.path.isfile(filename)
    

    def _WriteToLog(self,log="", logLevel="1",LogType=""):
        pass

    def test(self):
        #self.ReadListFile()        
        for i in self.ReadList(avg="runList.txt"):
            print(i)
    
if __name__ == "__main__":
    '''
    if no input 
    if input numbe (or array of number )
    if input is a filename(with or with out path)
    '''
    test=mssget()
    for item in sys.argv[1:]:
        if os.path.isdir(item):
            test.setSavePath(SavePath=item)
    for item in sys.argv[1:]:
        if item.isnumeric():
            test.ReadList(avg=item)
        elif '-' in item:
            RunRange=item.split('-')
            if RunRange[0].isnumeric() and RunRange[1].isnumeric():
                print(" ----->  Getting Run Range  {}-{} /{}".format(RunRange[0],RunRange[1],int(RunRange[1])-int(RunRange[0])+1))
                for runID in range(int(RunRange[0]),int(RunRange[1])+1):
                    print("\t --> Get {} : {}/{}".format(runID,runID-int(RunRange[0]),int(RunRange[1])-int(RunRange[0])+1))
                    test.ReadList(avg=str(runID))
    test.GetFiles()

