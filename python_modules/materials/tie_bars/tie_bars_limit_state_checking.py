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
from materials import steel_member_base
from materials import limit_state_checking_base as lsc
from postprocess import limit_state_data as lsd
import xc

class TieBarMember(steel_member_base.Member):
    ''' Tie bar members.'''
    
    def installULSControlRecorder(self, recorderType, prep):
        '''Install recorder for verification of ULS criterion.

        :param recorderType: type of the recorder to install.
        :param prep: pre-processor for the finite element problem.
        '''
        self.prep= prep
        nodes= prep.getNodeHandler
        domain= prep.getDomain
        recorder= domain.newRecorder(recorderType,None)
        if(not self.elemSet):
            self.createElementSet()
        if(len(self.elemSet)==0):
            lmsg.warning('Element set is empty.')
        eleTags= list()
        for e in self.elemSet:
            eleTags.append(e.tag)
            e.setProp('ULSControlRecorder',recorder)
        idEleTags= xc.ID(eleTags)
        recorder.setElements(idEleTags)
        self.shape.setupULSControlVars(self.elemSet)
        if(nodes.numDOFs==3):
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
    
class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls normal stresses limit state.'''
    ControlVars= BiaxialBendingControlVars
    def __init__(self,limitStateLabel):
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)
        
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
