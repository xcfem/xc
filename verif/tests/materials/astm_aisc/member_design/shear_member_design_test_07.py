# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE G.3 ANGLE IN SHEAR

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from solution import predefined_solutions

from misc_utils import units_utils


MPa2ksi= 0.145038
kN2kips= 0.2248
kip2kN= 1.0/kN2kips

m2Toin2= 1.0/units_utils.inchToMeter**2

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Example G.1A'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A36
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.LShape(steel,'L5X3X1/4')
xcSection= shape.defElasticShearSection2d(preprocessor)

# Required shear strength
VMax= 21*kip2kN*1000.0 # N

## Shear strength

member= aisc.Member('member', shape, unbracedLengthX= 100.0)

Aw= member.shape.getAw()
AwRef= 5*units_utils.inchToMeter*0.25*units_utils.inchToMeter
ratio1= abs((Aw-AwRef)/AwRef)
Vu= member.getDesignShearStrength()
VuRef= 0.9*27e3*kip2kN
ratio2= abs((Vu-VuRef)/VuRef)


'''
print('t= ', shape.get('t')/units_utils.inchToMeter, 'in')
print('Aw= ',Aw*1e4,' cm2')
print('AwRef= ',AwRef*1e4,' cm2')
print('ratio1= ',ratio1)
print('Vu= ',Vu/1e3,' kN m')
print('VuRef= ',VuRef/1e3,' kN m')
print('ratio2= ',ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<5e-4 and ratio2<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
