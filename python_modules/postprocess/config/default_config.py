# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2018, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import ast
import errno
from pathlib import Path
import os
import sys
import tempfile
import xc
from postprocess import output_styles
from postprocess import limit_state_data as lsd
from postprocess.config import file_names as fn
from misc_utils import log_messages as lmsg
from misc_utils import compat

def findDirectoryUpwards(fNameMark: str):
    '''Search upwards to find the directory where the file
       argument is.

    :param fNameMark: name of the file that marks the directory
                      we are searching for.
    '''
    current_working_dir= os.getcwd()
    working_dir= current_working_dir
    while True:
        file_list = os.listdir(working_dir)
        parent_dir = os.path.dirname(working_dir)
        if fNameMark in file_list:
            break
        else:
            if working_dir == parent_dir: #if dir is root dir
                working_dir= None
                break
            else:
                working_dir = parent_dir
    if(not working_dir):
        working_dir= current_working_dir
    return working_dir

def findWorkingDirectory(fNameMark= 'env_config.py'):
    '''Search upwards to find the directory where the file
       argument is.

    :param fNameMark: name of the file that marks the working directory.
    '''
    return findDirectoryUpwards(fNameMark)

def setWorkingDirectory(fNameMark= 'env_config.py'):
    '''Search upwards to find the directory where the file
       argument is.

    :param fNameMark: name of the file that marks the working directory.
    '''
    retval= findWorkingDirectory(fNameMark= fNameMark)
    if(retval[-1]!='/'):
        retval= retval+'/'
    if(not retval in sys.path):
        sys.path.append(retval)
    return retval

def findProjectDirectory(fNameMark= '.git'):
    '''Search upwards to find the directory where the file
       argument is.

    :param fNameMark: name of the file that marks the working directory.
    '''
    return findDirectoryUpwards(fNameMark)


def find_files(fileName, searchPath):
    ''' Return the occurrences of filename in search_path

    :param fileName: name of the file to search.
    :param searchPath: root directory to start the search.
    '''
    result = []

    # Walking top-down from the root
    for root, dirs, files in os.walk(searchPath):
        if fileName in files:
            result.append(os.path.join(root, fileName))
    return result

def find_directories(dirName, searchPath):
    ''' Return the occurrences of filename in search_path

    :param dirName: name of the directory to search.
    :param searchPath: root directory to start the search.
    '''
    result = []

    # Walking top-down from the root
    for root, dirs, files in os.walk(searchPath):
        if dirName in dirs:
            result.append(os.path.join(root, dirName))
    return result

class ProjectDirTree(object):
    ''' Paths to project directories.

       :ivar intForcPath: relative path from the model path of the directory where results 
                     of internal forces are placed.
       :ivar verifPath: relative path from the model path of the directory where results of 
                     limit state  verifications are placed
       :ivar reportPath: relative path of the directory where calculation report 
                     files are placed
       :ivar reportResultsPath : relative path from reportPath of the directory where to place  
                     graphic and text files to be included in the report
    '''
    def __init__(self, resultsPath, intForcPath, verifPath, reportPath, reportResultsPath, fNameMark= 'env_config.py'):
        '''
        Constructor.

       :param resultsPath: relative path from the model path of the directory where results 
                     are placed.
       :param intForcPath: relative path from the results path of the directory where results 
                     of internal forces are placed.
       :param verifPath: relative path from the results path of the directory where results of 
                     limit state  verifications are placed
       :param reportPath: relative path of the directory where calculation report 
                     files are placed
       :param reportResultsPath : relative path from reportPath of the directory where to place  
                     graphic and text files to be included in the report
                            
        '''
        #default names of files with data for FE model generation, results of
        #limit state verifications, ..
        self.workingDirectory= findWorkingDirectory(fNameMark)
        self.resultsPath= resultsPath
        self.intForcPath= intForcPath
        self.verifPath= verifPath
        self.reportPath= reportPath
        self.reportResultsPath= reportResultsPath

    def getMainPaths(self):
        ''' Return the main paths used in the project.'''
        retval= list()
        retval.append(self.getFullResultsPath())
        retval.append(self.getFullReportPath())
        return retval
                      
    def cleanWorkingDirectory(self):
        ''' Clean all the files in the working directory.'''
        fullPaths= self.getMainPaths()
        for pth in fullPaths:
            os.system("rm -f -r "+pth)

    def getRltvResultsPath(self):
        ''' Return the relative path for the files that contains
            results.'''
        return self.resultsPath

    def getFullResultsPath(self):
        ''' Return the full path for the files that contains
            results.'''
        return self.workingDirectory+'/'+self.getRltvResultsPath()

    def newResultsPath(self, folderName:str):
        ''' Creates a new folder inside the results directory.

        :param folderName: name for the new folder.
        '''
        pth= self.getFullResultsPath()
        newFolderPath= pth+folderName
        if os.path.isfile(newFolderPath): # Name already in use for a file.
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; '"+str(folderName) + "' is already a file name inside: '"+str(pth)+"' can't create folder.")
            newFolderPath= None
        elif not os.path.isdir(newFolderPath): # Directory doesn't exist yet.
            os.mkdir(newFolderPath)
        return newFolderPath

    def findResultsPath(self, folderName:str):
        ''' Search for a folder inside the results directory.

        :param folderName: name for the folder to find.
        '''
        retval= None
        pth= self.getFullResultsPath()
        tmp= find_directories(dirName= folderName, searchPath= pth)
        sz= len(tmp)
        if(sz>0):
            retval= tmp[0]
            if(sz>1): # Many directories with the same name.
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+"; ther are"+str(sz)+ " directories named: '"+str(folderName) + "' inside: '"+str(pth)+"' returning the first one.")
        return retval

    def findResultsFile(self, fileName:str):
        ''' Search for a folder inside the results directory.

        :param fileName: name for the folder to find.
        '''
        retval= None
        pth= self.getFullResultsPath()
        tmp= find_files(fileName= fileName, searchPath= pth)
        sz= len(tmp)
        if(sz>0):
            retval= tmp[0]
            if(sz>1): # Many directories with the same name.
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+"; ther are"+str(sz)+ " files named: '"+str(fileName) + "' inside: '"+str(pth)+"' returning the first one.")
        return retval

    def getInternalForcesResultsPath(self):
        ''' Return the path for the files that contains
            the internal forces.'''
        return self.getFullResultsPath()+'/'+self.intForcPath
    
    def getReactionsResultsPath(self):
        ''' Return the path for the files that contains
            the reactions.'''
        return self.getFullResultsPath()+'reactions/'
    
    def getFullVerifPath(self):
        ''' Return the full path for the limit state checking files.'''
        return self.getFullResultsPath()+'/'+self.verifPath
    
    def getRltvConnectionsResultsPath(self):
        ''' Return the relative path for the fatigue verification
            graphics files.'''
        return self.getRltvResultsPath()+'/connections/'

    def getFullConnectionsResultsPath(self):
        ''' Return the relative path for the fatigue verification
            graphics files.'''
        return self.getFullResultsPath()+'connections/'
        
    def getRltvReactionsResultsPath(self):
        ''' Return the relative path for the fatigue verification
            graphics files.'''
        return self.reportResultsPath+'reactions/'

    def getFullReactionsResultsPath(self):
        ''' Return the relative path for the fatigue verification
            graphics files.'''
        return self.getFullResultsPath()+'/'+self.getRltvReactionsResultsPath()
        
    def getRltvReportPath(self):
        ''' Return the relative path for the report files.'''  
        return self.reportResultsPath
    
    def getRltvTextReportPath(self):
        ''' Return the relative path for the text report files.'''  
        return self.getRltvReportPath()+'text/'
    
    def getFullReportPath(self):
        ''' Return the full path for the report files.'''
        retval= self.workingDirectory+'/'+self.reportPath
        rltvReportPath= self.getRltvReportPath()
        if(rltvReportPath!=self.reportPath): # There is some bug here.
            retval+= rltvReportPath # Don't repeat the same directory.
        return retval
    
    def getFullTextReportPath(self):
        ''' Return the full path for the text report files.'''
        retval= self.workingDirectory+'/'+self.reportPath
        rltvTextReportPath= self.getRltvTextReportPath()
        if(rltvTextReportPath.startswith(self.reportPath)): # There is some bug here.
            if sys.version_info < (3,9):
                rltvTextReportPath= compat.removeprefix(rltvTextReportPath, self.reportPath)
            else:
                rltvTextReportPath= rltvTextReportPath.removeprefix(self.reportPath)
        retval+= rltvTextReportPath # Don't repeat the same directory.
        return retval

    def getFullGraphicsPath(self):
        ''' Return the full path for the graphic files.'''        
        return self.getFullReportPath()+'graphics/'

    def getRltvGraphicsPath(self):
        ''' Return the relative path for the graphic files.'''        
        return self.getRltvReportPath()+'graphics/'

    def getReportSectionsFile(self):
        ''' Return the full path for the sections report file.'''        
        return self.getFullTextReportPath()+'report_RCsections.tex'
    
    def getReportSectionsGrPath(self):
        ''' Return the full path for the sections graphic files.'''        
        return self.getFullGraphicsPath()+'sections/'

    def getRltvReportSectionsGrPath(self):
        ''' Return the relative path for the sections graphic files.'''        
        return self.getRltvGraphicsPath()+'sections/'

    def getReportLoadsFile(self):
        ''' Return the path for the load report file.'''        
        return self.getFullTextReportPath()+'report_loads.tex'
    
    def getReportLoadsGrPath(self):
        ''' Return the path for the load graphic files.'''        
        return self.getFullGraphicsPath()+'loads/'
    
    def getRltvReportLoadsGrPath(self):
        ''' Return the relative path for the load graphic files.'''        
        return self.getRltvGraphicsPath()+'loads/'

    # Normal stresses limit state results.
    def getVerifNormStrFile(self):
        ''' Return the path of the normal stresses verification
            results file.'''
        return self.getFullVerifPath()+fn.normalStressesVerificationResultsFile+'.json'
    
    def getReportNormStrFile(self):
        ''' Return the path of the normal stresses verification
            report file.'''
        return self.getFullTextReportPath()+fn.normalStressesVerificationReportFile+'.tex'
    
    def getReportNormStrGrPath(self):
        ''' Return the path for the normal stresses verification
            graphics files.'''
        return self.getFullGraphicsPath()+fn.normalStressesDirectory
        
    def getRltvReportNormStrGrPath(self):
        ''' Return the path for the normal stresses verification
            graphics files.'''
        return self.getRltvGraphicsPath()+fn.normalStressesDirectory

    # Buckling limit state results.
    def getVerifBucklingFile(self):
        ''' Return the path of the buckling verification
            results file.'''
        return self.getFullVerifPath()+fn.bucklingVerificationResultsFile+'.json'

    def getReportBucklingFile(self):
        ''' Return the path of the buckling verification
            report file.'''
        return self.getFullTextReportPath()+fn.bucklingVerificationReportFile+'.tex'
    
    def getReportBucklingGrPath(self):
        ''' Return the path for the buckling verification
            graphics files.'''
        return self.getFullGraphicsPath()+fn.bucklingDirectory
        
    def getRltvReportBucklingGrPath(self):
        ''' Return the path for the buckling verification
            graphics files.'''
        return self.getRltvGraphicsPath()+fn.bucklingDirectory
    
    # Shear limit state results.
    def getVerifShearFile(self):
        ''' Return the path of the shear verification
            results file.'''
        return self.getFullVerifPath()+fn.shearVerificationResultsFile+'.json'
    
    def getReportShearFile(self):
        ''' Return the path of the shear verification
            report file.'''
        return self.getFullTextReportPath()+fn.shearVerificationReportFile+'.tex'
    
    def getReportShearGrPath(self):
        ''' Return the path for the shear verification
            graphics files.'''
        return self.getFullGraphicsPath()+fn.shearDirectory

    def getRltvReportShearGrPath(self):
        ''' Return the path for the shear verification
            graphics files.'''
        return self.getRltvGraphicsPath()+fn.shearDirectory
    
    # Torsion limit state results.
    def getVerifTorsionFile(self):
        ''' Return the path of the torsion verification
            results file.'''
        return self.getFullVerifPath()+fn.torsionVerificationResultsFile+'.json'
    
    def getReportTorsionFile(self):
        ''' Return the path of the torsion verification
            report file.'''
        return self.getFullTextReportPath()+fn.torsionVerificationReportFile+'.tex'
    
    def getReportTorsionGrPath(self):
        ''' Return the path for the torsion verification
            graphics files.'''
        return self.getFullGraphicsPath()+fn.torsionDirectory

    def getRltvReportTorsionGrPath(self):
        ''' Return the path for the torsion verification
            graphics files.'''
        return self.getRltvGraphicsPath()+fn.torsionDirectory

    # Von Mises stresses.
    def getVerifVonMisesStressFile(self):
        ''' Return the path of the VonMisesStress verification
            results file.'''
        return self.getFullVerifPath()+fn.vonMisesStressesVerificationResultsFile+'.json'
    
    def getReportVonMisesStressFile(self):
        ''' Return the path of the VonMisesStress verification
            report file.'''
        return self.getFullTextReportPath()+fn.vonMisesStressesVerificationReportFile+'.tex'
    
    def getReportVonMisesStressGrPath(self):
        ''' Return the path for the VonMisesStress verification
            graphics files.'''
        return self.getFullGraphicsPath()+fn.vonMisesStressesDirectory

    def getRltvReportVonMisesStressGrPath(self):
        ''' Return the path for the VonMisesStress verification
            graphics files.'''
        return self.getRltvGraphicsPath()+fn.vonMisesStressesDirectory

    # Cracking. Rare loads.
    def getVerifCrackRareFile(self):
        ''' Return the path of the crack verification
            results file (rare loads).'''
        return self.getFullVerifPath()+fn.crackControlRareVerificationResultsFile+'.json'
    
    def getReportCrackRareFile(self):
        ''' Return the path of the crack verification
            report file (rare loads).'''
        return self.getFullTextReportPath()+fn.crackControlRareVerificationReportFile+'.tex'
    
    def getReportCrackRareGrPath(self):
        ''' Return the path for the crack verification
            graphics files (rare loads).'''
        return self.getFullGraphicsPath()+fn.crackControlRareDirectory
        
    def getRltvReportCrackRareGrPath(self):
        ''' Return the path for the crack verification
            graphics files (rare loads).'''
        return self.getRltvGraphicsPath()+fn.crackControlRareDirectory

    # Crack control. Frequent loads.
    def getVerifCrackFreqFile(self):
        ''' Return the path of the crack verification
            results file (frequent loads).'''
        return self.getFullVerifPath()+fn.crackControlFreqVerificationResultsFile+'.json'
        
    def getReportCrackFreqFile(self):
        ''' Return the path of the crack verification
            report file (frequent loads).'''
        return self.getFullTextReportPath()+fn.crackControlFreqVerificationReportFile+'.tex'
    
    def getReportCrackFreqGrPath(self):
        ''' Return the path for the crack verification
            graphics files (frequent loads).'''
        return self.getFullGraphicsPath()+fn.crackControlFreqDirectory
        
    def getRltvReportCrackFreqGrPath(self):
        ''' Return the path for the crack verification
            graphics files (frequent loads).'''
        return self.getRltvGraphicsPath()+fn.crackControlFreqDirectory
        
    # Crack control. Quasi-permanent loads.
    def getVerifCrackQpermFile(self):
        ''' Return the path of the crack verification
            results file (quasi-permanent loads).'''
        return self.getFullVerifPath()+fn.crackControlQpermVerificationResultsFile+'.json'
    
    def getReportCrackQpermFile(self):
        ''' Return the path of the crack verification
            report file (quasi-permanent loads).'''
        return self.getFullTextReportPath()+fn.crackControlQpermVerificationReportFile+'.tex'
    
    def getReportCrackQpermGrPath(self):
        ''' Return the path for the crack verification
            graphics files (quasi-permanent loads).'''
        return self.getFullGraphicsPath()+fn.crackControlQpermDirectory
        
    def getRltvReportCrackQpermGrPath(self):
        ''' Return the path for the crack verification
            graphics files (quasi-permanent loads).'''
        return self.getRltvGraphicsPath()+fn.crackControlQpermDirectory

    # Fatigue
    def getVerifFatigueFile(self):
        ''' Return the path of the fatigue verification
            results file.'''
        return self.getFullVerifPath()+fn.fatigueVerificationResultsFile+'.json'
    
    def getReportFatigueFile(self):
        ''' Return the path of the fatigue verification
            report file.'''
        return self.getFullTextReportPath()+fn.fatigueVerificationReportFile+'.tex'
    
    def getReportFatigueGrPath(self):
        ''' Return the path for the fatigue verification
            graphics files.'''
        return self.getFullGraphicsPath()+fn.fatigueDirectory

    def getRltvReportFatigueGrPath(self):
        ''' Return the path for the fatigue verification
            graphics files.'''
        return self.getRltvGraphicsPath()+fn.fatigueDirectory

    def getVerifFile(self, limitStateLabel):
        '''Return the path of the verification results file
           for the limit state argument.

        :param limitStateLabel: label identifying the limit state.
        '''
        if(limitStateLabel=='ULS_normalStressesResistance'):
            return self.getVerifNormStrFile()
        elif(limitStateLabel=='ULS_bucklingResistance'):
            return self.getVerifBucklingFile()
        elif(limitStateLabel=='ULS_shearResistance'):
            return self.getVerifShearFile()
        elif(limitStateLabel=='ULS_torsionResistance'):
            return self.getVerifTorsionFile()
        elif(limitStateLabel=='SLS_rareLoadsCrackControl'):
            return self.getVerifCrackRareFile()
        elif(limitStateLabel=='SLS_frequentLoadsCrackControl'):
            return self.getVerifCrackFreqFile()
        elif(limitStateLabel=='SLS_quasiPermanentLoadsCrackControl'):
            return self.getVerifCrackQpermFile()
        elif(limitStateLabel=='ULS_fatigueResistance'):
            return self.getVerifFatigueFile()
        elif(limitStateLabel=='ULS_VonMisesStressResistance'):
            return self.getVerifVonMisesStressFile()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; label: '"+str(limitStateLabel) + "' unknown.")
            return None
        
    def getReportFile(self, limitStateLabel):
        ''' Return the path of the verification report file
            for the limit state argument.

           :param limitStateLabel: label identifying the limit state.
        '''
        if(limitStateLabel=='ULS_normalStressesResistance'):
            return self.getReportNormStrFile()
        elif(limitStateLabel=='ULS_bucklingResistance'):
            return self.getReportBucklingFile()
        elif(limitStateLabel=='ULS_shearResistance'):
            return self.getReportShearFile()
        elif(limitStateLabel=='ULS_torsionResistance'):
            return self.getReportTorsionFile()
        elif(limitStateLabel=='SLS_rareLoadsCrackControl'):
            return self.getReportCrackRareFile()
        elif(limitStateLabel=='SLS_frequentLoadsCrackControl'):
            return self.getReportCrackFreqFile()
        elif(limitStateLabel=='SLS_quasiPermanentLoadsCrackControl'):
            return self.getReportCrackQpermFile()
        elif(limitStateLabel=='ULS_fatigueResistance'):
            return self.getReportFatigueFile()
        elif(limitStateLabel=='ULS_VonMisesStressResistance'):
            return self.getReportVonMisesStressFile()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; label: '"+str(limitStateLabel) + "' unknown.")
            return None
        
    def getReportGrPath(self, limitStateLabel):
        ''' Return the full path for the verification graphics files
            for the limit state argument.

           :param limitStateLabel: label identifying the limit state.
        '''
        if(limitStateLabel=='ULS_normalStressesResistance'):
            return self.getReportNormStrGrPath()
        elif(limitStateLabel=='ULS_bucklingResistance'):
            return self.getReportBucklingGrPath()
        elif(limitStateLabel=='ULS_shearResistance'):
            return self.getReportShearGrPath()
        elif(limitStateLabel=='ULS_torsionResistance'):
            return self.getReportTorsionGrPath()
        elif(limitStateLabel=='SLS_rareLoadsCrackControl'):
            return self.getReportCrackRareGrPath()
        elif(limitStateLabel=='SLS_frequentLoadsCrackControl'):
            return self.getReportCrackFreqGrPath()
        elif(limitStateLabel=='SLS_quasiPermanentLoadsCrackControl'):
            return self.getReportCrackQpermGrPath()
        elif(limitStateLabel=='ULS_fatigueResistance'):
            return self.getReportFatigueGrPath()
        elif(limitStateLabel=='ULS_VonMisesStressResistance'):
            return self.getReportVonMisesStressGrPath()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; label: '"+str(limitStateLabel) + "' unknown.")
            return None
    
    def getReportRltvGrPath(self, limitStateLabel):
        ''' Return the relative path for the verification graphics files
            for the limit state argument.

           :param limitStateLabel: label identifying the limit state.
        '''
        if(limitStateLabel=='ULS_normalStressesResistance'):
            return self.getRltvReportNormStrGrPath()
        elif(limitStateLabel=='ULS_bucklingResistance'):
            return self.getRltvReportBucklingGrPath()
        elif(limitStateLabel=='ULS_shearResistance'):
            return self.getRltvReportShearGrPath()
        elif(limitStateLabel=='ULS_torsionResistance'):
            return self.getRltvReportTorsionGrPath()
        elif(limitStateLabel=='SLS_rareLoadsCrackControl'):
            return self.getRltvReportCrackRareGrPath()
        elif(limitStateLabel=='SLS_frequentLoadsCrackControl'):
            return self.getRltvReportCrackFreqGrPath()
        elif(limitStateLabel=='SLS_quasiPermanentLoadsCrackControl'):
            return self.getRltvReportCrackQpermGrPath()
        elif(limitStateLabel=='ULS_fatigueResistance'):
            return self.getRltvReportFatigueGrPath()
        elif(limitStateLabel=='ULS_VonMisesStressResistanc'):
            return self.getRltvReportVonMisesStressResistancGrPath()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; label: '"+str(limitStateLabel) + "' unknown.")
            return None
    
    def getReportSimplLCFile(self):
        return self.getFullTextReportPath()+fn.simpleLoadCaseReportFile+'.tex'
    
    def getReportSimplLCGrPath(self):
        return self.getFullGraphicsPath()+fn.simpleLoadCaseResultsDirectory

    def getRltvReportSimplLCGrPath(self):
        return self.getRltvGraphicsPath()+fn.simpleLoadCaseResultsDirectory

    def getPathList(self):
        ''' Return the project directory tree.'''
        retval= list()
        retval.append(self.getInternalForcesResultsPath())
        retval.append(self.getReactionsResultsPath())
        retval.append(self.getFullVerifPath())
        retval.append(self.getFullTextReportPath())
        retval.append(self.getFullGraphicsPath())
#        retval.append(self.getReportSectionsFile())
        retval.append(self.getReportSectionsGrPath())
#        retval.append(self.getReportLoadsFile())
        retval.append(self.getReportLoadsGrPath())
        retval.append(self.getReportNormStrGrPath())
        retval.append(self.getReportBucklingGrPath())
        retval.append(self.getReportShearGrPath())
        retval.append(self.getReportTorsionGrPath())
        retval.append(self.getReportCrackFreqGrPath())
        retval.append(self.getReportCrackQpermGrPath())
        retval.append(self.getReportFatigueGrPath())
        retval.append(self.getReportSimplLCGrPath())
        retval.append(self.getFullConnectionsResultsPath())
        retval.append(self.getFullReactionsResultsPath())
        return retval
        
    def createTree(self):
        ''' Create the project directory tree.'''
        paths= self.getPathList()
        for pth in paths:
            if(not os.path.exists(pth)):
                os.makedirs(pth)

    def makedirs(self, pth):
        ''' Recursive directory creation function. Like mkdir(), but 
            makes all intermediate-level directories needed to contain 
           the leaf directory.

        :param path: the path to create.
        '''
        if(not os.path.exists(pth)):
            try:
                os.makedirs(pth)
            except OSError as exc: # Guard against race condition
                if exc.errno != errno.EEXIST:
                    raise

    def open(self, fileName, mode='r', buffering=-1, encoding=None, errors=None, newline=None, closefd=True, opener=None):
        ''' Open file and return a corresponding file object..

        :param fileName: the path and name of the file.
        :param mode: see Python documentation for open built-in function.
        :param bufferting: see Python documentation for open built-in function.
        :param encoding: see Python documentation for open built-in function.
        :param errors: see Python documentation for open built-in function.
        :param newline: see Python documentation for open built-in function.
        :param closefd: see Python documentation for open built-in function.
        :param opener: see Python documentation for open built-in function.
        '''
        pth= os.path.dirname(fileName)
        self.makedirs(pth)
        return open(fileName, mode, buffering, encoding, errors, newline, closefd, opener)

class EnvConfig(output_styles.OutputStyle):
    '''Generic configuration of XC environment variables.

       :ivar grWidth: size of the graphics to be included in the annex          
    '''
    def __init__(self,language= 'english', resultsPath='tmp_results/', intForcPath= 'internalForces/',verifPath= 'verifications/', reportPath='annex/',reportResultsPath= 'annex/', grWidth='\\linewidth', fNameMark= 'env_config.py'):
        '''
        Constructor.

        :param language: english, spanish, french 
        :param intForcPath: full path of the directory where results of 
                      internal forces are placed.
        :param verifPath: full path of the directory where results of 
                      limit state  verifications are placed
        :param reportPath: relative path of the directory where calculation report 
                     files are placed
        :param reportResultsPath : relative path from reportPath of the directory where to place  
                     graphic and text files to be included in the report
        :param grWidth: size of the graphics to be included in the annex
        :param fNameMark: name of the file that marks the working directory
               of the project.                            
        '''
        super(EnvConfig,self).__init__(language= language)
        #default names of files with data for FE model generation, results of
        #limit state verifications, ..
        self.projectDirTree= ProjectDirTree(resultsPath= resultsPath, intForcPath= intForcPath,verifPath= verifPath,reportPath= reportPath,reportResultsPath= reportResultsPath, fNameMark= fNameMark)

        lsd.LimitStateData.internal_forces_results_directory= intForcPath
        lsd.LimitStateData.check_results_directory= verifPath

        # self.intForcPath=intForcPath
        # self.verifPath=verifPath


        self.capTexts= self.getCaptionTextsDict()
        self.colors= setBasicColors
        self.grWidth= grWidth

    def setWorkingDirectory(self, pth):
        ''' Assign the working directory.

        :param pth: path to use as working directory.
        '''
        self.projectDirTree.workingDirectory= pth
        
    def getWorkingDirectory(self):
        ''' Return the current working directory.'''
        return self.projectDirTree.workingDirectory
    
    def makedirs(self, pth):
        ''' Recursive directory creation function. Like mkdir(), but 
            makes all intermediate-level directories needed to contain 
           the leaf directory.

        :param path: the path to create.
        '''
        return self.projectDirTree.makedirs(pth)
    
    def cleandirs(self):
        ''' Clean all the files in the working directory.'''
        self.projectDirTree.cleanWorkingDirectory()

    def open(self, fileName, mode='r', buffering=-1, encoding=None, errors=None, newline=None, closefd=True, opener=None):
        ''' Open file and return a corresponding file object..

        :param fileName: the path and name of the file.
        :param mode: see Python documentation for open built-in function.
        :param bufferting: see Python documentation for open built-in function.
        :param encoding: see Python documentation for open built-in function.
        :param errors: see Python documentation for open built-in function.
        :param newline: see Python documentation for open built-in function.
        :param closefd: see Python documentation for open built-in function.
        :param opener: see Python documentation for open built-in function.
        '''
        return self.projectDirTree.open(fileName, mode, buffering, encoding, errors, newline, closefd, opener)

    def compileCode(self,filename):
        ''' Execute the code in the file argument.

        :param filename: name of the file to execute.
        '''
        return compileCode(self.getWorkingDirectory()+'/'+filename)



#Predefined colors for sets (progressing from light to dark)

setBasicColors={
    'yellow01':xc.Vector([1,1,0]),
    'yellow02':xc.Vector([1,0.9,0]),
    'orange01':xc.Vector([1,0.8,0.5]),
    'orange02':xc.Vector([1,0.7,0]),
    'orange03':xc.Vector([1,0.6,0]),
    'pink01':xc.Vector([1,0.6,1.0]),
    'pink02':xc.Vector([1,0.2,1]),
    'pink03':xc.Vector([1,0,0.7]),
    'red01':xc.Vector([1,0.1,0.1]),
    'red02':xc.Vector([1,0.2,1]),
    'red03':xc.Vector([1,0,0]),
    'red04':xc.Vector([0.9,0,0]),
    'green01':xc.Vector([0.8,1,0.4]),
    'green02':xc.Vector([0.7,0.9,0.1]),
    'green03':xc.Vector([0.8,0.8,0.1]),
    'green04':xc.Vector([0.6,0.5,0.1]),
    'brown04':xc.Vector([0.5,0.,0.]),
    'brown03':xc.Vector([0.5,0.1,0.]),
    'brown02':xc.Vector([0.5,0.3,0.1]),
    'brown01':xc.Vector([0.6,0.3,0.]),
    'purple04':xc.Vector([0.6,0.2,1.]),
    'purple03':xc.Vector([0.7,0.2,0.7]),
    'purple02':xc.Vector([0.7,0.4,0.7]),
    'purple01':xc.Vector([0.7,0.7,0.8]),
    'blue01':xc.Vector([0.2,1.,1.]),
    'blue02':xc.Vector([0.,0.8,1]),
    'blue03':xc.Vector([0.,0.4,1]),
    'blue04':xc.Vector([0.,0.,0.9])     
  }

# Configuration for temporary storage of results (normally with testing purposes).

def get_temporary_env_config(subDirName: str= None):
    ''' Return an EnvConvig object suitable for temporary storage of results,
        normally with testing purposes.

        :param subDirName: name of the folder where results will be stored.
    '''
    dirName= None
    if(subDirName is None):
        tempDir= tempfile.TemporaryDirectory()
        dirName= tempDir.name
    else:
        dirName= '/tmp/'+subDirName
    retval= EnvConfig(language='en', resultsPath= 'tmp_results/', intForcPath= 'internalForces/',verifPath= 'verifications/', reportPath='annex/',reportResultsPath= 'annex/',grWidth='120mm')
    retval.projectDirTree.workingDirectory= dirName
    return retval

def compileCode(inputFile):
    fullPath= Path(inputFile).resolve()
    codeSource= open(inputFile).read()
    code= ast.parse(codeSource)
    compiledCode= compile(code, filename=str(fullPath), mode='exec')
    return compiledCode
