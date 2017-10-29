# -*- coding: utf-8 -*-
from __future__ import division
''' Structural steel as specified in ASTM standard.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials import steel_base

class ASTMSteel(steel_base.BasicSteel):
  '''ASTM structural steel.'''

  def __init__(self, fy= 250e6, fu= 400e6, gammaM= 1.0):
    super(ASTMSteel,self).__init__(200e9,0.3,fy,fu,gammaM)

ASTM_A36= ASTMSteel(250e6,400e6,1.0)
ASTM_A529= ASTMSteel(290e6,414e6,1.0)
ASTM_A53= ASTMSteel(240e6,414e6,1.0)
ASTM_A992= ASTMSteel(345e6,450e6,1.0)
ASTM_A500= ASTMSteel(315e6,400e6,1.0)
ASTM_A307= ASTMSteel(245e6,390e6,1.0)
