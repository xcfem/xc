# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking of strut-and-tie models. '''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

import sys
import math
import geom
from materials import limit_state_checking_base as lscb
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd
from postprocess.config import file_names as fn
from misc_utils import log_messages as lmsg

#       _    _       _ _        _        _       
#      | |  (_)_ __ (_) |_   __| |_ __ _| |_ ___ 
#      | |__| | '  \| |  _| (_-<  _/ _` |  _/ -_)
#      |____|_|_|_|_|_|\__| /__/\__\__,_|\__\___|
#       __ ___ _ _| |_ _ _ ___| | |___ _ _ ___   
#      / _/ _ \ ' \  _| '_/ _ \ | / -_) '_(_-<   
#      \__\___/_||_\__|_| \___/_|_\___|_| /__/   
# Limit state controllers.

class StrutAndTieStressController(lscb.LimitStateControllerBase):
    '''Object that controls normal stresses limit state on strut and tie
       models.

    :ivar concrete: strut concrete material.
    :ivar steel: tie steel material.
    '''

    ControlVars= cv.StrutAndTieControlVars
    
    def __init__(self, limitStateLabel, concrete, steel, solutionProcedureType= lscb.defaultStaticLinearSolutionProcedure):
        ''' Constructor.

        :param limitStateLabel: label that identifies the limit state.
        :param concrete: strut concrete material.
        :param steel: tie steel material.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(StrutAndTieStressController,self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= solutionProcedureType)
        self.concrete= concrete
        self.steel= steel

    @staticmethod
    def _get_maximum_and_minimum_elastic_modulus_(elements):
        ''' Return the maximum and minimum elastic moduli of the given elements
            materials.

        :param elements: elements to process.
        '''
        maxValue= -6.023e23
        minValue= -maxValue
        for e in elements:
            E= e.getMaterial().E
            if(E>maxValue):
                maxValue= E
            if(E<minValue):
                minValue= E
        return maxValue, minValue

    def _classify_strut_and_tie_elements_(self, elements):
        ''' Classify the given elements as ties (made of steel) or struts
            (made of concrete) and extract.

        :param elements: elements to classify.
        '''
        Es, Ec= StrutAndTieStressController._get_maximum_and_minimum_elastic_modulus_(elements)
        self.strutTags= set()
        self.tieTags= set()
        for e in elements:
            E= e.getMaterial().E
            if(E>Ec): # is steel.
                self.tieTags.add(e.tag)
            else: # is concrete. 
                self.strutTags.add(e.tag)
                
    def computeEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the material of the element
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose material will be checked.
        :param elementInternalForces: internal forces acting on the 
                                      cross-section.
        '''
        fyd= self.steel.fyd()
        fctm= self.concrete.fctm()
        fcd= self.concrete.fcd()
        inverted= False
        N= elementInternalForces.N
        if(elem.tag in self.tieTags):
            typo= 'tie'
            stress= N/elem.sectionArea
            if(stress<0.0): # tie in compression
                retval= stress/fcd
                inverted= True
            else: # tie in tension.
                retval= stress/fyd
        else:
            typo= 'strut'
            stress= N/elem.sectionArea
            if(stress>0.0): # strut in tension
                retval= stress/fctm
                inverted= True
            else: # strut in compression.
                retval= stress/fcd
        return retval, typo, inverted  

    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the material of the element
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose material will be checked.
        :param elementInternalForces: internal forces acting on the 
                                      cross-section.
        '''
        # Check each element section.
        for lf in elementInternalForces:
            # Compute efficiency.
            CFtmp, typo, inverted= self.computeEfficiency(elem, elementInternalForces= lf)
            label= self.limitStateLabel
            # Element type.
            # Update efficiency.
            if(CFtmp>elem.getProp(label).CF):
                elem.setProp(label, self.ControlVars(combName= lf.idComb, CF= CFtmp, N= lf.N, typo= typo, inverted= inverted))
          
class StrutAndTieStressesLimitStateData(lsd.ULS_LimitStateData):
    ''' Strut and tie normal stresses data for limit state checking.'''
    
    def __init__(self, designSituations= lsd.default_uls_design_situations):
        '''Constructor 

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, sls_quasi-permanent,
                                 sls_frequent, sls_rare, uls_earthquake, etc. 
        '''
        super(StrutAndTieStressesLimitStateData,self).__init__(limitStateLabel= 'StrutAndTie', outputDataBaseFileName= fn.strutAndTieVerificationResultsFile, designSituations= designSituations)
    
    def getController(self, concrete, steel):
        ''' Return a controller corresponding to this limit state.

        :param concrete: strut concrete material.
        :param steel: tie steel material.
        :param biaxialBending: if True use a controller that checks bending
                               around both cross-section axes.
        '''
        return StrutAndTieStressController(limitStateLabel= self.label, concrete= concrete, steel= steel)
    
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        :returns: number of properties read.
        '''
        return modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifStrutAndTieFile())
    
    def check(self, setCalc, concrete, steel, appendToResFile='N', listFile='N', calcMeanCF='N'):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param concrete: strut concrete material.
        :param steel: tie steel material.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        '''
        controller= self.getController(concrete= concrete, steel= steel)
        controller._classify_strut_and_tie_elements_(setCalc.elements)
        outputCfg= lsd.VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return self.runChecking(outputCfg= outputCfg, sections= [''])
    
# StrutAndTie limit state.
strutAndTieLimitState= StrutAndTieStressesLimitStateData()

