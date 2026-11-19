# -*- coding: utf-8 -*-
''' Limit state checking according to EN 1992-1-1:2004. Annex F. Tension 
reinforcement expressions for in-plane stress conditions.
'''

from __future__ import print_function

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

import sys
import math
from postprocess import limit_state_data as lsd
from postprocess.config import file_names as fn
from materials import limit_state_checking_base as lscb
from postprocess import control_vars as cv

#       ___ ___ ___    _     _                       ___ 
#      | __/ __|_  )__/ |   /_\  _ _  _ _  _____ __ | __|
#      | _| (__ / /___| |  / _ \| ' \| ' \/ -_) \ / | _| 
#      |___\___/___|  |_| /_/ \_\_||_|_||_\___/_\_\ |_|  
# EN 1992-1-1:2004. Annex F.
# Tension reinforcement expressions for in-plane stress conditions
class Ec2InPlaneStressLimitStateData(lsd.ULS_LimitStateData):
    ''' In-plane stress conditions limit state data.

    '''
    def __init__(self, designSituations= lsd.default_uls_design_situations):
        '''Constructor.

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_earthquake, etc.
        '''
        super(Ec2InPlaneStressLimitStateData,self).__init__(limitStateLabel= 'ULS_Ec2InPlaneStressResistance', outputDataBaseFileName= fn.ec2InPlaneStressVerificationResultsFile, designSituations= designSituations)
        
    def getStressesDict(self, nmbComb, elems):
        '''Creates a dictionary with the element's stresses.

        :param nmbComb: combination name.
        :param elems: element set.
        '''
        return eif.get_stresses_dict(nmbComb, elems)
    
    def readInternalForces(self, setCalc):
        ''' Read the stresses for the elements in the set argument.

        :param setCalc: elements to read stresses for.
        '''
        return lsd.read_stresses_dict(self.getStressesFileName(), setCalc)

    def getController(self):
        ''' Return a controller corresponding to this limit state.

        :param code_limit_state_checking: code used to check the limit state.
        '''
        return Ec2InPlaneStressController(limitStateLabel= self.label)
        
    def checkElements(self, elementsToCheck, outputCfg= lsd.VerifOutVars()):
        '''Checking of stresses in ultimate limit states
            (see self.dumpCombinations).

        :param elementsToCheck: elements to check.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
        '''
        return elementsToCheck.check(limitStateData= self, outputCfg= outputCfg, threeDim= True)
    
    def runChecking(self ,outputCfg):
        '''This method reads, for the elements in setCalc, the stresses 
        previously calculated and saved in the corresponding file.
        Using the 'initControlVars' and 'updateEfficiencyForSet' methods of 
        the controller, the appropriate attributes are assigned to the 
        elements and the associated limit state verification is run.
        The results are written to a file in order to be displayed or listed.

        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to the 
               result file [defatults to False], generation or not
               of list file [defatults to False, ...)
        '''
        retval= super(Ec2InPlaneStressLimitStateData,self).runChecking(outputCfg, sections= [''])
        return retval
    
    def check(self, setCalc, controller, appendToResFile=False, listFile=False, calcMeanCF=False):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  True if results are appended to 
               existing file of results (defaults to False)
        :param listFile: True if latex listing file of results 
               is desired to be generated (defaults to False)
        :param calcMeanCF: True if average capacity factor is
               meant to be calculated (defaults to False)
        '''
        if appendToResFile not in [True,False]:
            lmsg.error("Argument 'appendToResFile' must be True or False")
            exit(1)
        if listFile not in [True,False]:
            lmsg.error("Argument 'listFile' must be True or False")
            exit(1)
        if calcMeanCF not in [True,False]:
            lmsg.error("Argument 'calcMeanCF' must be True or False")
            exit(1)
        outputCfg= lsd.VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return self.runChecking(outputCfg= outputCfg)

ec2InPlaneStressResistance= Ec2InPlaneStressLimitStateData()

class Ec2InPlaneStressController(lscb.LimitStateControllerBase):
    '''Object that controls reinforced concrete ULS using the
       expressions given in Annex F of EN 1992-1-1:2004.'''

    ControlVars= cv.Ec2InPlaneStressControlVars
    def __init__(self, limitStateLabel, solutionProcedureType= lscb.defaultStaticLinearSolutionProcedure):
        ''' Constructor.

        :param limitStateLabel: limit state identifier.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(Ec2InPlaneStressController, self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= solutionProcedureType)

    def initControlVars(self, elements):
        '''Initialize control variables on the given elements.

        :param elements: elements in which the control variables 
                         will be defined.
        '''
        for e in elements:
            e.setProp(self.limitStateLabel, self.ControlVars())
            
    def readStresses(self, stressesCombFileName, setCalc= None):
        '''Launch checking.

        :param stressesCombFileName: Name of the file containing the stresses 
                                    on the element gauss points.
        '''
        # Read internal forces results.
        intForcItems= lsd.read_internal_forces_file(stressesCombFileName, setCalc)
        return intForcItems[2]
            
    def updateEfficiency(self, elem, elementStresses):
        ''' Compute the efficiency of the element material
            subjected to the stresses argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementStresses: stresses on the element.
        '''
        # Get available strengths.
        ftd1_avail= elem.getProp('ftd1_avail')
        ftd2_avail= elem.getProp('ftd2_avail')
        sigma_c_avail= elem.getProp('sigma_c_avail')
        if(ftd1_avail and ftd2_avail and sigma_c_avail):
            # Check each element section.
            for stresses in elementStresses:
                sigma_11= -stresses.sigma_11
                sigma_12= stresses.sigma_12
                sigma_22= -stresses.sigma_22
                if(sigma_11>0.0 and sigma_22>0.0 and (sigma_11*sigma_22>sigma_12**2)): # no reinforcement needed.
                   ftd1_req= 0.0
                   ftd2_req= 0.0
                   sigma_c_req= 0.0
                   CFtmp= 0.0
                else:
                    
                    abs_sigma_12= abs(sigma_12)
                    if(sigma_11<abs_sigma_12):
                        ftd1_req= abs_sigma_12-sigma_11
                        ftd2_req= max(abs_sigma_12-sigma_22, 0)
                        sigma_c_req= 2*abs_sigma_12
                    else:
                        ftd1_req= 0.0
                        ftd2_req= sigma_12**2/sigma_11-sigma_22
                        sigma_c_req= sigma_11*(1+(sigma_12/sigma_11)**2)
                if(ftd1_req<0 or ftd2_req<0 or sigma_c_req<0):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    errMsg= className+'.'+methodName+"; somethig when wrong.\n"
                    if(ftd1_req<0):
                        errMsg+= ' negative required strenght on axis 1: '+str(ftd1_req)+'\n'
                    if(ftd2_req<0):
                        errMsg+= ' negative required strenght on axis 2: '+str(ftd2_req)+'\n'
                    if(sigma_c_req<0):
                        errMsg+= ' negative required strenght for concrete: '+str(sigma_c_req)+'\n'
                    lmsg.error(errMsg)
                    exit(1)
                ftd1_cf= ftd1_req/ftd1_avail
                ftd2_cf= ftd2_req/ftd2_avail
                sigma_c_cf= sigma_c_req/sigma_c_avail
                CFtmp= max(ftd1_cf, ftd2_cf, sigma_c_cf)
                CF= elem.getProp(self.limitStateLabel).CF
                if(CFtmp>CF):
                    cv= self.ControlVars(combName= stresses.idComb, CF= CFtmp, sigma_11= sigma_11, sigma_12= sigma_12, sigma_22= sigma_22, ftd1_req= ftd1_req, ftd2_req= ftd2_req, sigma_c_req= sigma_c_req, ftd1_cf= ftd1_cf, ftd2_cf= ftd2_cf, sigma_c_cf= sigma_c_cf)
                    elem.setProp(self.limitStateLabel, cv)

