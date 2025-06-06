# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import os
import sys
from postprocess import output_handler as oh
from postprocess.config import default_config
from misc_utils import log_messages as lmsg

class ReportGenerator(oh.OutputHandler):
    '''Class intended to generate reports of data, loads and  
    calculation results.
    '''
    
    def __init__(self, modelSpace= None, envConfig= default_config.EnvConfig()):
        super(ReportGenerator,self).__init__(modelSpace,outputStyle= envConfig)

    def getEnvConfig(self):
        ''' Return the instance of EnvConfig class passed as parameter
        '''
        return self.outputStyle

    def getReportFileName(self, limitStateLabel):
        ''' Return the name of the file where the report will be written.

        :param limitStateLabel: label that identifies the limit state.
        '''
        cfg= self.getEnvConfig()
        return cfg.projectDirTree.getReportFile(limitStateLabel)

    def checksReport(self, limitStateLabel, setsShEl, argsShEl, setsBmEl=[], argsBmEl=[], rgMinMax=None, defaultDiagramDirection= 'J'):
        '''Create a LaTeX report including the desired graphical results 
        obtained in the verification of a limit state.

        :param limitStateLabel: label that identifies the limit state.
        :param setsShEl: Ordered list of sets of 2D elements to be
                         included in the report.
        :param argsShEl: Ordered list of arguments to be included in the 
                         report for 2D elements
        :param setsBmView: Ordered list of set of 1D elements to be included 
                           in the report. 
        :param argsBmElScale: Ordered list of arguments to be included in the 
                              report for 1D elements
        :param defaultDiagramDirection: default direction of the diagrams in
                                        1D elements (J: element local j vector 
                                        or K: element local K vector).
        '''
        retval= list()
        cfg= self.getEnvConfig()
        texReportFileName= self.getReportFileName(limitStateLabel= limitStateLabel)
        report= cfg.projectDirTree.open(fileName= texReportFileName, mode= 'w') # report latex file
        fullPath= cfg.projectDirTree.getReportGrPath(limitStateLabel)
        cfg.makedirs(fullPath) # crate the directory if needed.
        rltvPath= cfg.projectDirTree.getReportRltvGrPath(limitStateLabel)
        for st in setsShEl:
            for arg in argsShEl:
                for idSection in [1, 2]:
                    capt=cfg.capTexts[limitStateLabel] + '. '+ st.description.capitalize() + ', ' + cfg.capTexts[arg] + ', ' + 'dir. '+str(idSection)
                    sectionName= 'Sect'+str(idSection)
                    suffix= st.name+arg+sectionName # set name + argument identifier + section name
                    bitmapFileName= suffix+'.jpg'
                    retval.append(bitmapFileName)
                    fullgrFileNm= fullPath+suffix
                    rltvgrFileNm= rltvPath+suffix
                    fullgrFileNmAndExt= fullPath+bitmapFileName
                    self.displayField(limitStateLabel, section= idSection,argument=arg, component=None, setToDisplay=st, fileName=fullgrFileNmAndExt, rgMinMax=rgMinMax)
                    if not os.path.exists(fullgrFileNmAndExt):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.error(className+'.'+methodName+'; something went wrong, file: '+str(fullgrFileNmAndExt) + ' doesn\'t exist.')
                    label= limitStateLabel+suffix
                    oh.append_graphic_to_tex_file(texFile=report,  graphicFileName= rltvgrFileNm, graphicWidth= cfg.grWidth, captionText= capt, label= label)
                
        for stV in setsBmEl:
            for argS in argsBmEl:
                capt= cfg.capTexts[limitStateLabel]  + '. '+ stV.description.capitalize() + ', ' + cfg.capTexts[argS]
                suffix= stV.name+argS # set name + argument identifier.
                bitmapFileName= suffix+'.jpg'
                fullgrFileNm= fullPath+suffix
                rltvgrFileNm= rltvPath+suffix
                fullgrFileNmAndExt= fullPath+bitmapFileName
                self.displayBeamResult(attributeName=limitStateLabel,itemToDisp=argS,beamSetDispRes=stV,setToDisplay=stV,caption=capt,fileName= fullgrFileNmAndExt, defaultDirection= defaultDiagramDirection)
                label= limitStateLabel+suffix
                oh.append_graphic_to_tex_file(texFile=report, graphicFileName= rltvgrFileNm, graphicWidth= cfg.grWidth, captionText= capt, label= label)
        report.close()
        return retval

    def loadsReport(self, loadCasesAndSets:dict):
        ''' Create a report of the given loads.

        :param loadCasesAndSets: dictionary whose keys are the names of the
                                 load cases to represent and the associated
                                 values are lists of sets to represent the
                                 load case on.
        '''
        cfg= self.getEnvConfig()
        texReportFileName= cfg.projectDirTree.getReportLoadsFile() # laTex file where the graphics will be included.
        outputPath= cfg.projectDirTree.getReportLoadsGrPath() # directory to place the figures.
        cfg.projectDirTree.makedirs(outputPath) # Create the directory if doesn't exists.
        retval= list()
        with cfg.projectDirTree.open(fileName= texReportFileName, mode= 'w') as latexOutputFile:
            for lcName in loadCasesAndSets:
                setsToDisplay= loadCasesAndSets[lcName]
                self.modelSpace.addLoadCaseToDomain(lcName)
                for xcSet in setsToDisplay:
                    if(xcSet is None):
                        xcSet= self.modelSpace.getTotalSet()
                    caption= lcName+' on set: '+xcSet.name
                    fLabel= lcName+xcSet.name
                    bitmapFileName= fLabel+'.png'
                    outputFileName= outputPath+bitmapFileName
                    self.displayLoads(setToDisplay= xcSet, fileName= outputFileName, caption= caption)
                    oh.append_graphic_to_tex_file(texFile= latexOutputFile,  graphicFileName=  outputFileName, graphicWidth= cfg.grWidth, captionText= caption, label= fLabel)
                    retval.append(bitmapFileName)
                self.modelSpace.removeLoadCaseFromDomain(lcName)
        return retval
    
  
            
