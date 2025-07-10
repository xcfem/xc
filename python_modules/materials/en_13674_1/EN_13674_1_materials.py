# -*- coding: utf-8 -*-
''' Steel material as specified in the EN13674−1 standard.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
import sys
from materials import steel_base
from materials import typical_materials
from misc_utils import log_messages as lmsg

class EN13674_1_Steel(steel_base.BasicSteel):
    '''Eurocode 3 structural steel.

      :ivar gammaM: partial safety factor for steel strength.
      :ivar gammaM1: partial factor for buckling resistance.
      :ivar gammaM2: partial factor for cross-sections in tension to fracture.
    '''
    def __init__(self, fy, fu, gammaM, gammaM1= 1.0, gammaM2= 1.1):
        ''' Constructor.
          :param fy:  yield stress.
          :param fu: ultimate stress.
          :param gammaM: partial safety factor for steel strength.
          :param gammaM1: partial factor for buckling resistance.
          :param gammaM2: partial factor for cross-sections in tension to fracture.
        '''

        super().__init__(210000e6,0.3,fy,fu,gammaM)
        self.gammaM1= gammaM1 #Partial factor for buckling resistance.
        self.gammaM2= gammaM2 #Partial factor for cross-sections in tension to fracture.

    def gammaM0(self):
        ''' Return the value of the partial safety factor for steel strength.'''
        return self.gammaM

    def setGammaM0(self, gammaM0:float):
        ''' Assign the value of the partial safety factor for steel strength.

        :param gammaM0: Partial safety factor for steel strength.
        '''
        self.gammaM= gammaM0

    def fydV(self):
        ''' Return the design value of the shear strength.'''
        return self.fyd()/math.sqrt(3)

    def getLambda1(self):
        '''return lambda_1 value as specified in EC3 part 1 5.5.1.2
        '''
        return math.pi*math.sqrt(self.E/self.fy)

    def getDesignElasticPerfectlyPlasticMaterial(self,preprocessor,name):
        return typical_materials.defElasticPPMaterial(preprocessor, name,self.E,self.fyd(),-self.fyd())


# European norm EN 10025-2:2004
R260= EN13674_1_Steel(fy= 260e6, fu= 880e6, gammaM= 1.1)
R350HT= EN13674_1_Steel(fy= 840e6, fu= 1175e6, gammaM= 1.1)


