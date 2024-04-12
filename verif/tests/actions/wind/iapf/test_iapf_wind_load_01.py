# -*- coding: utf-8 -*-
''' Miscellaneous test of wind loads according to IAPF-10.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2023, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from actions.wind import IAPF_wind

# Site data
v_ref= 24 # reference speed of the wind.
terrainCategory= 'II'

# Bridge data
deckHeight= 12.85
deckWidth= 25+1.5
h_eq= 4.25

# Compute wind pressure.
c_risk= IAPF_wind.getRiskFactor(T= 100.0)
ratio1= abs(c_risk-1.038776575154488)/1.038776575154488
c_fx= IAPF_wind.getSolidWebBridgeDragForceCoefficient(B= deckWidth, h_eq= h_eq)
ratio2= abs(c_fx-1.3)/1.3
designVelocity= IAPF_wind.getDesignVelocity(terrainCategory= terrainCategory, v_ref= v_ref, z= deckHeight)
ratio3= abs(designVelocity-39.52785121880105)/39.52785121880105
unitWindLoadDeck= IAPF_wind.get_unit_wind_load(terrainCategory= terrainCategory, v_ref= v_ref, z= deckHeight, Cd= c_fx)
ratio4= abs(unitWindLoadDeck-1.269491455355233e3)/1.269491455355233e3

'''
print('risk factor: ', c_risk, ratio1)
print('drag force coefficient: ', c_fx, ratio2)
print('design velocity: ', designVelocity, 'm/s', ratio3)
print('unit wind load: ', unitWindLoadDeck/1e3, 'kN/m2', ratio4)
print('force per unit length: ', unitWindLoadDeck*h_eq/1e3, 'kN/m')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-6) and (abs(ratio4)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
