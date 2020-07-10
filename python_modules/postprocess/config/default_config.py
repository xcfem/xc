# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

import os
import xc_base
import geom
import xc
from postprocess import output_styles
from postprocess import limit_state_data as lsd

def findWorkingDirectory(fNameMark= 'env_config.py'):
    '''Search upwards to find the directory where the file
       argument is.

    :param fNameMark: name of the file that marks the working directory
                      of the project.
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

class ProjectDirTree(object):
    ''' Paths to project directories.

       :ivar intForcPath: relative path from the model path of the directory where results 
                     of internal forces are placed.
       :ivar verifPath: relative path from the model path of the directory where results of 
                     limit state  verifications are placed
       :ivar reportPath: relative path of the directory where calculation report 
                     files are placed
       :ivar resultsPath : relative path from reportPath of the directory where to place  
                     graphic and text files to be included in the report
    '''
    def __init__(self,intForcPath,verifPath,reportPath,resultsPath, fNameMark= 'env_config.py'):
        '''
        Constructor.

       :ivar intForcPath: relative path from the model path of the directory where results 
                     of internal forces are placed.
       :ivar verifPath: relative path from the model path of the directory where results of 
                     limit state  verifications are placed
       :ivar reportPath: relative path of the directory where calculation report 
                     files are placed
       :ivar resultsPath : relative path from reportPath of the directory where to place  
                     graphic and text files to be included in the report
                            
        '''
        #default names of files with data for FE model generation, results of
        #limit state verifications, ..
        self.workingDirectory= findWorkingDirectory(fNameMark)
        self.intForcPath= intForcPath
        self.verifPath= verifPath
        self.reportPath= reportPath
        self.resultsPath=resultsPath

    def getInternalForcesResultsPath(self):
        ''' Return the path for the files that contains
            the internal forces.'''
        return self.workingDirectory+'/'+self.intForcPath
    
    def getFullVerifPath(self):
        return self.workingDirectory+'/'+self.verifPath
    
    def getFullReportPath(self):
        return self.workingDirectory+'/'+self.reportPath+self.resultsPath+'text/'
    
    def getRltvReportPath(self):
        return self.resultsPath+'text/'
    
    def getFullGraphicsPath(self):
        return self.getFullReportPath()+'graphics/'

    def getRltvGraphicsPath(self):
        return self.getRltvReportPath()+'graphics/'

    def getReportSectionsFile(self):
        return self.getFullReportPath()+'sectReport.tex'
    
    def getReportSectionsGrPath(self):
        return self.getFullReportPath()+'sections/'

    def getRltvReportSectionsGrPath(self):
        return self.getRltvReportPath()+'sections/'

    def getReportLoadsFile(self):
        return self.getFullReportPath()+'report_loads.tex'
    
    def getReportLoadsGrPath(self):
        return self.getFullGraphicsPath()+'loads/'
    
    def getRltvReportLoadsGrPath(self):
        return self.getRltvGraphicsPath()+'loads/'
    
    def getVerifNormStrFile(self):
        ''' Return the path of the normal stresses verification
            results file.'''
        return self.getFullVerifPath()+'verifRsl_normStrsULS.py'
    
    def getReportNormStrFile(self):
        ''' Return the path of the normal stresses verification
            report file.'''
        return self.getFullReportPath()+'report_normStrsULS.tex'
    
    def getReportNormStrGrPath(self):
        ''' Return the path for the normal stresses verification
            graphics files.'''
        return self.getFullGraphicsPath()+'normStrsULS/'
        
    def getRltvReportNormStrGrPath(self):
        ''' Return the path for the normal stresses verification
            graphics files.'''
        return self.getRltvGraphicsPath()+'normStrsULS/'

    def getVerifShearFile(self):
        ''' Return the path of the shear verification
            results file.'''
        return self.getFullVerifPath()+'verifRsl_shearULS.py'
    
    def getReportShearFile(self):
        ''' Return the path of the shear verification
            report file.'''
        return self.getFullReportPath()+'report_shearULS.tex'
    
    def getReportShearGrPath(self):
        ''' Return the path for the shear verification
            graphics files.'''
        return self.getFullGraphicsPath()+'shearULS/'

    def getRltvReportShearGrPath(self):
        ''' Return the path for the shear verification
            graphics files.'''
        return self.getRltvGraphicsPath()+'shearULS/'

    def getVerifCrackFreqFile(self):
        ''' Return the path of the crack verification
            results file (frequent loads).'''
        return self.getFullVerifPath()+'verifRsl_crackingSLS_freq.py'
    
    def getReportCrackFreqFile(self):
        ''' Return the path of the crack verification
            report file (frequent loads).'''
        return self.getFullReportPath()+'report_crackingSLS_freq.tex'
    
    def getReportCrackFreqGrPath(self):
        ''' Return the path for the crack verification
            graphics files (frequent loads).'''
        return self.getFullGraphicsPath()+'crackingSLS_freq/' 
        
    def getRltvReportCrackFreqGrPath(self):
        ''' Return the path for the crack verification
            graphics files (frequent loads).'''
        return self.getRltvGraphicsPath()+'crackingSLS_freq/' 
        
    def getVerifCrackQpermFile(self):
        ''' Return the path of the crack verification
            results file (quasi-permanent loads).'''
        return self.getFullVerifPath()+'verifRsl_crackingSLS_qperm.py'
    
    def getReportCrackQpermFile(self):
        ''' Return the path of the crack verification
            report file (quasi-permanent loads).'''
        return self.getFullReportPath()+'report_crackingSLS_qperm.tex'
    
    def getReportCrackQpermGrPath(self):
        ''' Return the path for the crack verification
            graphics files (quasi-permanent loads).'''
        return self.getFullGraphicsPath()+'crackingSLS_qperm/' 
        
    def getRltvReportCrackQpermGrPath(self):
        ''' Return the path for the crack verification
            graphics files (quasi-permanent loads).'''
        return self.getRltvGraphicsPath()+'crackingSLS_qperm/' 
        
    def getVerifFatigueFile(self):
        ''' Return the path of the fatigue verification
            results file.'''
        return self.getFullVerifPath()+'verifRsl_fatigueULS.py'
    
    def getReportFatigueFile(self):
        ''' Return the path of the fatigue verification
            report file.'''
        return self.getFullReportPath()+'report_fatigueStrsULS.tex'
    
    def getReportFatigueGrPath(self):
        ''' Return the path for the fatigue verification
            graphics files.'''
        return self.getFullGraphicsPath()+'fatigueStrsULS/'

    def getRltvReportFatigueGrPath(self):
        ''' Return the path for the fatigue verification
            graphics files.'''
        return self.getRltvGraphicsPath()+'fatigueStrsULS/'

    def getVerifFile(self, limitStateLabel):
        '''Return the path of the verification results file
           for the limit state argument.

           :param limitStateLabel: label identifying the limit state.
        '''
        if(limitStateLabel=='ULS_normalStressesResistance'):
            return self.getVerifNormStrFile()
        elif(limitStateLabel=='ULS_shearResistance'):
            return self.getVerifShearFile()
        elif(limitStateLabel=='SLS_frequentLoadsCrackControl'):
            return self.getVerifCrackFreqFile()
        elif(limitStateLabel=='SLS_quasiPermanentLoadsLoadsCrackControl'):
            return self.getVerifCrackQpermFile()
        elif(limitStateLabel=='ULS_fatigueResistance'):
            return self.getVerifFatigueFile()
        else:
            lmsg.error('Label: '+limitStateLabel+' unknown.')
            return None
        
    def getReportFile(self, limitStateLabel):
        ''' Return the path of the verification report file
            for the limit state argument.

           :param limitStateLabel: label identifying the limit state.
        '''
        if(limitStateLabel=='ULS_normalStressesResistance'):
            return self.getReportNormStrFile()
        elif(limitStateLabel=='ULS_shearResistance'):
            return self.getReportShearFile()
        elif(limitStateLabel=='SLS_frequentLoadsCrackControl'):
            return self.getReportCrackFreqFile()
        elif(limitStateLabel=='SLS_quasiPermanentLoadsLoadsCrackControl'):
            return self.getReportCrackQpermFile()
        elif(limitStateLabel=='ULS_fatigueResistance'):
            return self.getReportFatigueFile()
        else:
            lmsg.error('Label: '+limitStateLabel+' unknown.')
            return None
        
    def getReportGrPath(self, limitStateLabel):
        ''' Return the full path for the verification graphics files
            for the limit state argument.

           :param limitStateLabel: label identifying the limit state.
        '''
        if(limitStateLabel=='ULS_normalStressesResistance'):
            return self.getReportNormStrGrPath()
        elif(limitStateLabel=='ULS_shearResistance'):
            return self.getReportShearGrPath()
        elif(limitStateLabel=='SLS_frequentLoadsCrackControl'):
            return self.getReportCrackFreqGrPath()
        elif(limitStateLabel=='SLS_quasiPermanentLoadsLoadsCrackControl'):
            return self.getReportCrackQpermGrPath()
        elif(limitStateLabel=='ULS_fatigueResistance'):
            return self.getReportFatigueGrPath()
        else:
            lmsg.error('Label: '+limitStateLabel+' unknown.')
            return None
    
    def getReportRltvGrPath(self, limitStateLabel):
        ''' Return the relative path for the verification graphics files
            for the limit state argument.

           :param limitStateLabel: label identifying the limit state.
        '''
        if(limitStateLabel=='ULS_normalStressesResistance'):
            return self.getRltvReportNormStrGrPath()
        elif(limitStateLabel=='ULS_shearResistance'):
            return self.getRltvReportShearGrPath()
        elif(limitStateLabel=='SLS_frequentLoadsCrackControl'):
            return self.getRltvReportCrackFreqGrPath()
        elif(limitStateLabel=='SLS_quasiPermanentLoadsLoadsCrackControl'):
            return self.getRltvReportCrackQpermGrPath()
        elif(limitStateLabel=='ULS_fatigueResistance'):
            return self.getRltvReportFatigueGrPath()
        else:
            lmsg.error('Label: '+limitStateLabel+' unknown.')
            return None
    
    def getReportSimplLCFile(self):
        return self.getFullReportPath()+'report_resSimplLC.tex'
    
    def getReportSimplLCGrPath(self):
        return self.getFullGraphicsPath()+'resSimplLC/'

    def getRltvReportSimplLCGrPath(self):
        return self.getRltvGraphicsPath()+'resSimplLC/'

    def getPathList(self):
        ''' Create the project directory tree.'''
        retval= list()
        retval.append(self.getInternalForcesResultsPath())
        retval.append(self.getFullVerifPath())
        retval.append(self.getFullReportPath())
        retval.append(self.getFullGraphicsPath())
#        retval.append(self.getReportSectionsFile())
        retval.append(self.getReportSectionsGrPath())
#        retval.append(self.getReportLoadsFile())
        retval.append(self.getReportLoadsGrPath())
        retval.append(self.getReportNormStrGrPath())
        retval.append(self.getReportShearGrPath())
        retval.append(self.getReportCrackFreqGrPath())
        retval.append(self.getReportCrackQpermGrPath())
        retval.append(self.getReportFatigueGrPath())
        retval.append(self.getReportSimplLCGrPath())
        return retval
        
    def createTree(self):
        ''' Create the project directory tree.'''
        paths= self.getPathList()
        for pth in paths:
            if(not os.path.exists(pth)):
                os.makedirs(pth)



class EnvConfig(output_styles.OutputStyle):
    '''Generic configuration of XC environment variables.

       :ivar grWidth: size of the graphics to be included in the annex          
    '''
    def __init__(self,language= 'english',intForcPath= 'results/internalForces/',verifPath= 'results/verifications/',reportPath='./',resultsPath= 'annex/', grWidth='\linewith', fNameMark= 'env_config.py'):
        '''
        Constructor.

        :param language: english, spanish, french 
        :param intForcPath: full path of the directory where results of 
                      internal forces are placed.
        :param verifPath: full path of the directory where results of 
                      limit state  verifications are placed
        :param reportPath: relative path of the directory where calculation report 
                     files are placed
        :param resultsPath : relative path from reportPath of the directory where to place  
                     graphic and text files to be included in the report
        :param grWidth: size of the graphics to be included in the annex
        :param fNameMark: name of the file that marks the working directory
               of the project.                            
        '''
        super(EnvConfig,self).__init__(language= language)
        #default names of files with data for FE model generation, results of
        #limit state verifications, ..
        self.projectDirTree= ProjectDirTree(intForcPath= intForcPath,verifPath= verifPath,reportPath=reportPath,resultsPath=resultsPath)

        lsd.LimitStateData.internal_forces_results_directory= intForcPath
        lsd.LimitStateData.check_results_directory= verifPath

        # self.intForcPath=intForcPath
        # self.verifPath=verifPath


        self.capTexts= self.getCaptionTextsDict()
        self.colors=setBasicColors
        self.grWidth=grWidth



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
