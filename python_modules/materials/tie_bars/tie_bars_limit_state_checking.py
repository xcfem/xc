# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 3. '''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@gmail.com"

from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd
from materials import steel_member_base
from materials import limit_state_checking_base as lsc

class TieBarMember(steel_member_base.Member):
    ''' Tie bar members.'''
    
    def installULSControlRecorder(self, recorderType, calcSet= None):
        '''Install recorder for verification of ULS criterion.

        :param recorderType: type of the recorder to install.
        :param calcSet: set of elements to be checked (defaults to 'None' which 
                        means that this set will be created elsewhere). In not
                        'None' the member elements will be appended to this set.
        '''
        recorder= self.createRecorder(recorderType, calcSet)
        self.shape.setupULSControlVars(self.elemSet)
        nodHndlr= self.getPreprocessor().getNodeHandler        
        if(nodHndlr.numDOFs==3):
            recorder.callbackRecord= controlULSCriterion2D()
        else:
            recorder.callbackRecord= controlULSCriterion()
        return recorder
    
class BiaxialBendingControlVars(cv.BiaxialBendingControlVars):
    '''Control variables for biaxial bending normal stresses.
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        '''
        super(BiaxialBendingControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
        
#       _    _       _ _        _        _       
#      | |  (_)_ __ (_) |_   __| |_ __ _| |_ ___ 
#      | |__| | '  \| |  _| (_-<  _/ _` |  _/ -_)
#      |____|_|_|_|_|_|\__| /__/\__\__,_|\__\___|
#       __ ___ _ _| |_ _ _ ___| | |___ _ _ ___   
#      / _/ _ \ ' \  _| '_/ _ \ | / -_) '_(_-<   
#      \__\___/_||_\__|_| \___/_|_\___|_| /__/   
# Limit state controllers.
    
class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls normal stresses limit state.'''
    ControlVars= BiaxialBendingControlVars
    
    def __init__(self, limitStateLabel, solutionProcedureType= lsc.defaultStaticLinearSolutionProcedure):
        ''' Constructor.

        :param limitStateLabel: label that identifies the limit state.
        '''
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= lsc.defaultStaticLinearSolutionProcedure)
        
    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the material of the element
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose material will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get section properties.
        crossSection= elem.getProp('crossSection')
        # Check each element section.
        for lf in elementInternalForces:
            # Compute efficiency.
            CFtmp= crossSection.getBiaxialBendingEfficiency(lf.N,lf.My,lf.Mz)
            sectionLabel= self.getSectionLabel(lf.idSection)
            label= self.limitStateLabel+sectionLabel
            # Update efficiency.
            if(CFtmp>elem.getProp(label).CF):
                elem.setProp(label,self.ControlVars(sectionLabel,lf.idComb,CFtmp,lf.N,lf.My,lf.Mz))
                
class NormalStressesLimitStateData(lsd.NormalStressesSteelLimitStateData):
    ''' Reinforced concrete normal stresses data for limit state checking.'''
    
    def getController(self, biaxialBending= True):
        ''' Return a controller corresponding to this limit state.

        :param biaxialBending: if True use a controller that checks bending
                               around both cross-section axes.
        '''
        retval= None
        if(biaxialBending):
            retval= BiaxialBendingNormalStressController(self.label)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; not implemented yet for uniaxial bending.") 
            retval= None
        return retval
    
normalStressesResistance= NormalStressesLimitStateData()        
        
def controlULSCriterion():
    return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkBiaxialBendingForElement(self,nmbComb)'''

def controlULSCriterion2D():
    return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkUniaxialBendingForElement(self,nmbComb)'''
