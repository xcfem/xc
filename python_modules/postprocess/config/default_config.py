# -*- coding: utf-8 -*-

import os
import xc_base
import geom
import xc
from postprocess import output_styles
from postprocess import limit_state_data as lsd

def findWorkingDirectory():
    '''Search upwards to find the directory where the file
       env_config.py is.'''
    working_dir= os.getcwd()
    file_name= 'env_config.py'
    while True:
        file_list = os.listdir(working_dir)
        parent_dir = os.path.dirname(working_dir)
        if file_name in file_list:
            break
        else:
            if working_dir == parent_dir: #if dir is root dir
                working_dir= None
                break
            else:
                working_dir = parent_dir
    return working_dir

class envConfig(output_styles.OutputStyle):
    '''Generic configuration of XC environment variables.

       :ivar intForcPath: full path of the directory where results of 
                     internal forces are placed.
       :ivar verifPath: full path of the directory where results of 
                     limit state  verifications are placed
       :ivar annexPath: full path of the directory where to place graphic and 
                     text files for the generation of the annex
       :ivar grWidth: size of the graphics to be included in the annex          
    '''
    def __init__(self,language,intForcPath,verifPath,annexPath,grWidth='120mm'):
        '''
        :param language: english, spanish, french 
        :param intForcPath: full path of the directory where results of 
                      internal forces are placed.
        :param verifPath: full path of the directory where results of 
                      limit state  verifications are placed
        :param annexPath: full path of the directory where to place graphic and 
                      text files for the generation of the annex
        :param grWidth: size of the graphics to be included in the annex
                            
        '''
        super(envConfig,self).__init__(language= language)
        #default names of files with data for FE model generation, results of
        #limit state verifications, ..

        lsd.LimitStateData.internal_forces_results_directory= intForcPath
        lsd.LimitStateData.check_results_directory= verifPath

        self.intForcPath=intForcPath
        self.verifPath=verifPath
        
        self.verifNormStrFile=verifPath+'verifRsl_normStrsULS.py'
        self.reportNormStrFile=annexPath+'text/report_normStrsULS.tex'
        self.reportNormStrGrPath=annexPath+'text/graphics/normStrsULS/'
        
        self.verifShearFile=verifPath+'verifRsl_shearULS.py'
        self.reportShearFile=annexPath+'text/graphics/report_shearULS.tex'
        self.reportShearGrPath=annexPath+'text/graphics/shearULS/'
        
        self.verifCrackFreqFile=verifPath+'verifRsl_crackingSLS_freq.py'
        self.reportCrackFreqFile=annexPath+'text/report_crackingSLS_freq.tex'
        self.reportCrackFreqGrPath=annexPath+'text/graphics/crackingSLS_freq/' 
        
        self.verifCrackQpermFile=verifPath+'verifRsl_crackingSLS_qperm.py'
        self.reportCrackQpermFile=annexPath+'text/report_crackingSLS_qperm.tex'
        self.reportCrackQpermGrPath=annexPath+'text/graphics/crackingSLS_qperm/' 
        
        self.verifFatigueFile=verifPath+'verifRsl_fatigueULS.py'
        self.reportFatigueFile=annexPath+'text/report_fatigueStrsULS.tex' 
        self.reportFatigueGrPath=annexPath+'text/graphics/fatigueStrsULS/'

        self.reportSimplLCFile=annexPath+'text/report_resSimplLC.tex'
        self.reportSimplLCGrPath=annexPath+'text/graphics/resSimplLC/'

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
