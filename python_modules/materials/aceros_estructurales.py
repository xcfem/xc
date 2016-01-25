# -*- coding: utf-8 -*-
# Características mecánicas de distintos tipos de acero.

import math
from materials import typical_materials

class ASTMSteel(object):
  """ASTM structural steel."""
  fy= 250e6 # Yield stress.
  fu= 400e6
  gammaM= 1.0
  E= 200e9 #Young modulus

  def __init__(self, fy, fu, gammaM):
    self.fy= fy
    self.fu= fu
    self.gammaM= gammaM
 
  def fyd(self):
    return self.fy/self.gammaM

ASTM_A36= ASTMSteel(250e6,400e6,1.0)
ASTM_A529= ASTMSteel(290e6,414e6,1.0)
ASTM_A53= ASTMSteel(240e6,414e6,1.0)
ASTM_A992= ASTMSteel(345e6,450e6,1.0)
ASTM_A500= ASTMSteel(315e6,400e6,1.0)
ASTM_A307= ASTMSteel(245e6,390e6,1.0)

class EC3Steel(object):
  '''Eurocode 3 structural steel.
    fy ->  Yield stress.
    fy16 -> 0<t<16mm
    fy40 -> 16<t<40mm
    fy63 -> 40<t<63mm
    fy80 -> 63<t<80mm
    fy100 -> 80<t<100mm
    fy125 -> 80<t<125mm
    fu -> ultimate stress
    gammaM '''

  def __init__(self, fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM):
    self.fy= fy
    self.fy16= fy16
    self.fy40= fy40
    self.fy63= fy63
    self.fy80= fy80
    self.fy100= fy100
    self.fy125= fy125
    self.fu= fu
    self.gammaM= gammaM
    self.E= 210000e6 #Young modulus

  def fyd(self):
    return self.fy/self.gammaM
  def fydV(self):
    return self.fyd()/math.sqrt(3)

  def getDesignElasticPerfectlyPlasticMaterial(self,preprocessor,name):
    return typical_materials.defElasticPPMaterial(preprocessor, name,self.E,self.fyd(),-self.fyd())

# Norma europea EN 10025-2:2004
S235JR= EC3Steel(fy= 235e6, fy16= 235e6, fy40= 225e6, fy63= 215e6, fy80= 215e6, fy100= 215e6, fy125= 195e6,fu= 360e6,gammaM= 1.1)

S275JR= EC3Steel(fy= 275e6, fy16= 275e6, fy40= 265e6, fy63= 255e6, fy80= 245e6, fy100= 235e6, fy125= 225e6,fu= 360e6,gammaM= 1.1)

S355JR= EC3Steel(fy= 355e6, fy16= 355e6, fy40= 345e6, fy63= 335e6, fy80= 325e6, fy100= 315e6, fy125= 295e6, fu= 360e6, gammaM= 1.1)

S450J0= EC3Steel(fy= 450e6, fy16= 450e6, fy40= 430e6, fy63= 410e6, fy80= 390e6, fy100= 380e6, fy125= 380e6, fu= 360e6, gammaM= 1.1)
