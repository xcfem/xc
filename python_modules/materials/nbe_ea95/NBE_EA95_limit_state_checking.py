# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to NBE-EA95.'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@gmail.com"

# import sys # Not needed yet.
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd

class VonMisesStressController(lsc.LimitStateControllerBase):
    '''Object that controls Von Mises stress limit state.'''

    ControlVars= cv.VonMisesControlVars
    def __init__(self, limitStateLabel, vonMisesStressId= 'max_von_mises_stress'):
        ''' Constructor.

        :param limitStateLabel: limit state identifier.
        :param vonMisesStressId: identifier of the Von Mises stress to read
                                (see NDMaterial and MembranePlateFiberSection).
        '''
        super(VonMisesStressController,self).__init__(limitStateLabel)

    def initControlVars(self,setCalc):
        '''Initialize control variables over elements.

        :param setCalc: set of elements to which define control variables
        '''
        for e in setCalc.elements:
            e.setProp(self.limitStateLabel,self.ControlVars())
            
    def readInternalForces(self,intForcCombFileName, setCalc= None):
        '''Launch checking.

        :param intForcCombFileName: Name of the file containing the internal 
                                    forces on the element sections.
        '''
        # Read internal forces results.
        intForcItems= lsd.readIntForcesFile(intForcCombFileName, setCalc, self.vonMisesStressId)
        return intForcItems[2]
            
    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element material
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get factored yield stress.
        factoredYieldStress= 0.9*elem.getProp('yieldStress')
        # Check each element section.
        for lf in elementInternalForces:
            # Compute efficiency (both sections will have the
            # same Von Mises stress so this is redundant)
            CFtmp= lf.vonMisesStress/factoredYieldStress
            # Update efficiency.
            if(CFtmp>elem.getProp(self.limitStateLabel).CF):
                elem.setProp(self.limitStateLabel,self.ControlVars(lf.idComb,CFtmp,lf.vonMisesStress))
