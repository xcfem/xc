# -*- coding: utf-8 -*-
'''Calculation of the lateral subgrade reaction using the Broms' formula.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from scipy.constants import g
from geotechnics.foundations import pile



# Data
Emat=2.8e6    # elastic modulus of pile material [Pa]
pile= pile.CircularPile(E= Emat, pileType= 'friction', diameter= 1.5, soilLayers= [], pileSet= None)

Esoil= 26000*1e3*g
kh= pile.getBromsLateralSubgradeReactionModulus(E= Esoil)
khRef= 13000*1e3*g

ratio1= abs(kh-khRef)/khRef

'''
print('kh= ', kh/1e6)
print('khRef= ', khRef/1e6)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<0.1:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

