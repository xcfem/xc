# -*- coding: utf-8 -*-
from __future__ import division
''' Structural steel as specified in ASTM standard.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
from materials import structural_steel
from materials import typical_materials

class EC3Steel(structural_steel.BasicSteel):
  '''Eurocode 3 structural steel.

    :ivar fy16: 0<t<16mm
    :ivar fy40: 16<t<40mm
    :ivar fy63: 40<t<63mm
    :ivar fy80: 63<t<80mm
    :ivar fy100: 80<t<100mm
    :ivar fy125: 80<t<125mm
    :ivar gammaM1: Partial factor for buckling resistance.
    :ivar gammaM2: Partial factor for cross-sections in tension to fracture.
  '''

  def __init__(self, fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM, gammaM1= 1.0, gammaM2= 1.1):
    super(EC3Steel,self).__init__(210000e6,0.3,fy,fu,gammaM)
    self.fy16= fy16
    self.fy40= fy40
    self.fy63= fy63
    self.fy80= fy80
    self.fy100= fy100
    self.fy125= fy125
    self.gammaM1= gammaM1 #Partial factor for buckling resistance.
    self.gammaM2= gammaM2 #Partial factor for cross-sections in tension to fracture.

  def gammaM0(self):
    return self.gammaM

  def fyd(self):
    return self.fy/self.gammaM
  def fydV(self):
    return self.fyd()/math.sqrt(3)
  def getLambda1(self):
    '''
       :returns: lambda_1 value as specified in EC3 part 1 5.5.1.2
    '''
    return math.pi*math.sqrt(self.E/self.fy)
  def getDesignElasticPerfectlyPlasticMaterial(self,preprocessor,name):
    return typical_materials.defElasticPPMaterial(preprocessor, name,self.E,self.fyd(),-self.fyd())

# European norm EN 10025-2:2004
S235JR= EC3Steel(fy= 235e6, fy16= 235e6, fy40= 225e6, fy63= 215e6, fy80= 215e6, fy100= 215e6, fy125= 195e6,fu= 360e6,gammaM= 1.1)

S275JR= EC3Steel(fy= 275e6, fy16= 275e6, fy40= 265e6, fy63= 255e6, fy80= 245e6, fy100= 235e6, fy125= 225e6,fu= 360e6,gammaM= 1.1)

S355JR= EC3Steel(fy= 355e6, fy16= 355e6, fy40= 345e6, fy63= 335e6, fy80= 325e6, fy100= 315e6, fy125= 295e6, fu= 360e6, gammaM= 1.1)

S450J0= EC3Steel(fy= 450e6, fy16= 450e6, fy40= 430e6, fy63= 410e6, fy80= 390e6, fy100= 380e6, fy125= 380e6, fu= 360e6, gammaM= 1.1)
