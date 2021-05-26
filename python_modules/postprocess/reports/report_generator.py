# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "


from postprocess import output_handler as oh
from postprocess.config import default_config

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

    def checksReport(self,limitStateLabel,setsShEl,argsShEl,setsBmEl=[],argsBmEl=[]):
        '''Create a LaTeX report including the desired graphical results 
        obtained in the verification of a limit state.

        :param limitStateLabel: limit state
        :param setsShEl: Ordered list of sets of shell elements to be included 
        in the report.
        :param argsShEl: Ordered list of arguments to be included in the r   
                       eport for shell elements
        :param setsBmView: Ordered list of set of beam elements to be included 
                       in the report. 
        :param argsBmElScale: Ordered list of arguments to be included in the 
                      report for beam elements
        '''
        cfg= self.getEnvConfig()
        texReportFile= cfg.projectDirTree.getReportFile(limitStateLabel)
        report= open(texReportFile,'w') # report latex file
        fullPath= cfg.projectDirTree.getReportGrPath(limitStateLabel)
        cfg.makedirs(fullPath)
        rltvPath= cfg.projectDirTree.getReportRltvGrPath(limitStateLabel)
        for st in setsShEl:
            for arg in argsShEl:
                capt=cfg.capTexts[limitStateLabel] + '. '+ st.description.capitalize() + ', ' + cfg.capTexts[arg] + ', ' + 'dir. 1'
                fullgrFileNm=fullPath+st.name+arg+'Sect1'
                rltvgrFileNm=rltvPath+st.name+arg+'Sect1'
                self.displayField(limitStateLabel, 1,arg, None, st, fullgrFileNm+'.jpg')
                oh.insertGrInTex(texFile=report,grFileNm=rltvgrFileNm,grWdt=cfg.grWidth,capText=capt)
                capt=cfg.capTexts[limitStateLabel] + '. '+ st.description.capitalize() + ', ' + cfg.capTexts[arg] + ', ' + 'dir. 2'
                fullgrFileNm=fullPath+st.name+arg+'Sect2'
                rltvgrFileNm=rltvPath+st.name+arg+'Sect2'
                self.displayField(limitStateLabel, 2,arg, None, st, fullgrFileNm+'.jpg')
                oh.insertGrInTex(texFile=report,grFileNm=rltvgrFileNm,grWdt=cfg.grWidth,capText=capt)

        for stV in setsBmEl:
            for argS in argsBmEl:
                capt= cfg.capTexts[limitStateLabel]  + '. '+ stV.description.capitalize() + ', ' + cfg.capTexts[argS] 
                fullgrFileNm=fullPath+stV.name+argS
                rltvgrFileNm=rltvPath+stV.name+argS
                self.displayBeamResult(attributeName=limitStateLabel,itemToDisp=argS,beamSetDispRes=stV,setToDisplay=stV,caption=capt,fileName=fullgrFileNm+'.jpg')
                oh.insertGrInTex(texFile=report,grFileNm=rltvgrFileNm,grWdt=cfg.grWidth,capText=capt)
        report.close()

    
  
            
