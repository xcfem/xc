# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

# DEPRECATED module 17/11/2018. Use postprocess/config/default_config.py 

class ProjectDirectories:
    '''Project directoy structure.'''
    check_results_dir= 'results/verifications/' #Path to results_verif* files.
    internal_forces_results_dir= 'results/internalForces/' #Path to esf_el* files.
    graphic_output_dir= 'annex/graphiques/'
    text_output_dir= 'annex/tex/'
    def __init__(self,rootDir):
        '''Project directories constructor

        :param rootDir:      root directory of the project.
        '''
        self.rootDir= rootDir
    def getInternalForcesResultsDirectory(self):
        '''Return the path to esf_el* files.'''
        return self.rootDir+'/'+self.internal_forces_results_dir
    def getCheckResultsDirectory(self):
        '''Return the path to results_verif* files.'''
        return self.rootDir+'/'+self.check_results_dir
    def getGraphicOutputDir(self):
        '''Return the path to store graphic output files.'''
        return self.rootDir+'/'+self.graphic_output_dir
    def getTextOutputDir(self):
        '''Return the path to store text output files.'''
        return self.rootDir+'/'+self.text_output_dir
    def getCrackingSLSFreqFileName(self):
        '''Return the file name for the results of crack control
           checking under frequent loads.'''
        return self.getCheckResultsDirectory()+'verifRsl_crackingSLS_freq.py'
    def getCrackingSLSQPermFileName(self):
        '''Return the file name for the results of crack control
           checking under frequent loads.'''
        return self.getCheckResultsDirectory()+'verifRsl_crackingSLS_qperm.py'
    def getFatigueULSFileName(self):
        '''Return the file name for the results of fatigue checking.'''
        return self.getCheckResultsDirectory()+'verifRsl_fatigueULS.py'
    def getShearULSFileName(self):
        '''Return the file name for the results of shear strength checking.'''
        return self.getCheckResultsDirectory()+'verifRsl_shearULS.py'
    def getNormalStressesULSFileName(self):
        '''Return the file name for the results of normal stresses checking.'''
        return self.getCheckResultsDirectory()+'verifRsl_normStrsULS.py'
    def getSectionDataInputFile(self):
        '''Return the reinforced concrete sections file name.'''
        return self.rootDir+'/sectionsDef.py'
    def getSectionOutputDir(self):
        '''Return the paht to store section related output.'''
        return self.rootDir+'/sectionsDef.py'
