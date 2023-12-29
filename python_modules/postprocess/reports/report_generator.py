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

    :ivar modelSpace: FE model data.
    :ivar outputStyle: style of the output.
    '''
    
    def __init__(self,modelSpace= None, envConfig= default_config.EnvConfig()):
        super(ReportGenerator,self).__init__(modelSpace,outputStyle= envConfig)

    def getEnvConfig(self):
        ''' Return the instance of EnvConfig class passed as parameter
        '''
        return self.outputStyle

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
        cfg= self.getEnvConfig()
        texReportFile= cfg.projectDirTree.getReportFile(limitStateLabel)
        report= open(texReportFile,'w') # report latex file
        fullPath= cfg.projectDirTree.getReportGrPath(limitStateLabel)
        cfg.makedirs(fullPath)
        rltvPath= cfg.projectDirTree.getReportRltvGrPath(limitStateLabel)
        for st in setsShEl:
            for arg in argsShEl:
                for idSection in [1, 2]:
                    capt=cfg.capTexts[limitStateLabel] + '. '+ st.description.capitalize() + ', ' + cfg.capTexts[arg] + ', ' + 'dir. '+str(idSection)
                    sectionName= 'Sect'+str(idSection)
                    suffix= st.name+arg+sectionName # set name + argument identifier + section name
                    fullgrFileNm=fullPath+suffix
                    rltvgrFileNm=rltvPath+suffix
                    fullgrFileNmAndExt= fullgrFileNm+'.jpg'
                    self.displayField(limitStateLabel, section= idSection,argument=arg, component=None, setToDisplay=st, fileName=fullgrFileNmAndExt,rgMinMax=rgMinMax)
                    if not os.path.exists(fullgrFileNmAndExt):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.error(className+'.'+methodName+'; something went wrong, file: '+str(fullgrFileNmAndExt) + ' doesn\'t exist.')
                    label= limitStateLabel+suffix
                    oh.insertGrInTex(texFile=report, grFileNm=rltvgrFileNm, grWdt= cfg.grWidth, capText= capt, labl= label)
                
        for stV in setsBmEl:
            for argS in argsBmEl:
                capt= cfg.capTexts[limitStateLabel]  + '. '+ stV.description.capitalize() + ', ' + cfg.capTexts[argS]
                suffix= stV.name+argS # set name + argument identifier.
                fullgrFileNm=fullPath+suffix
                rltvgrFileNm=rltvPath+suffix
                self.displayBeamResult(attributeName=limitStateLabel,itemToDisp=argS,beamSetDispRes=stV,setToDisplay=stV,caption=capt,fileName=fullgrFileNm+'.jpg', defaultDirection= defaultDiagramDirection)
                label= limitStateLabel+suffix
                oh.insertGrInTex(texFile=report,grFileNm=rltvgrFileNm,grWdt=cfg.grWidth,capText=capt, labl= label)
        report.close()

    
  
            
