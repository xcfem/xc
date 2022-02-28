# -*- coding: utf-8 -*-
''' Example E2.4 Single Wood Screw Lateral Design Value - Double Shear
    Wood-to-wood Connection from "STRUCTURAL WOOD DESIGN SOLVED EXAMPLE
    PROBLEMS" of the American Wood Council.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
from materials.awc_nds import specific_gravity as sg
from materials.awc_nds import dowel_type_fasteners as fasteners
from materials.awc_nds import AWCNDS_materials as mat

fastener= fasteners.WoodScrew(diameter= 0.19*mat.in2meter, length= 6.0*mat.in2meter, headDiameter= 0.363*mat.in2meter, rootDiameter= 0.152*mat.in2meter, bendingYieldStrength= 80e3*mat.psi2Pa) 

# There is an error in the example the Fe values correspond
# to a G= 0.55 NOT to a G= 0.5
mainMemberWood= mat.Wood('Fake Douglas Fir-Larch', specificGravity= 0.55)
sideMemberWood= mat.Wood('Fake Structural Composite Lumber', specificGravity= 0.55)

Fem= mainMemberWood.getDowelBearingStrength(fastener.D, theta= 0.0)
Fes= sideMemberWood.getDowelBearingStrength(fastener.D, theta= 0.0)
Fyb= fastener.Fyb

mainMemberThickness= 3.0*mat.in2meter
sideMemberThickness= 1.5*mat.in2meter

mainMemberDowelBearingLength= mainMemberThickness
sideMemberDowelBearingLength= fastener.L-mainMemberDowelBearingLength-sideMemberThickness-fastener.tip/2.0
pMin= fastener.getMinPenetration()
Rd= fastener.getReductionTerm(theta= 0.0, yieldMode= 'Im')
k3= fastener.getK3(mainMemberWood, sideMemberWood, mainMemberDowelBearingLength, sideMemberDowelBearingLength, theta_m= 0.0, theta_s= 0.0)
Z= fastener.getYieldLimit(mainMemberWood, sideMemberWood, mainMemberDowelBearingLength, sideMemberDowelBearingLength, theta_m= 0.0, theta_s= 0.0, doubleShear= True)

ratio= abs(Z-1.03959600276e3)/1.03959600276e3

'''
print('Main member Fem= ', Fem/1e6, ' MPa (', Fem/mat.psi2Pa,' psi)')
print('Side members Fes= ', Fes/1e6, ' MPa (', Fes/mat.psi2Pa,' psi)')
print('fastener bending yield strengt Fyb= ', Fyb/1e6, ' MPa (', Fyb/mat.psi2Pa,' psi)')
print('min. penetration p_min= ', pMin*1e3, ' mm (', pMin/mat.in2meter,' in)')
print('reduction term Rd= ', Rd)
print('k3= ', k3)
print('Z= ', Z/1e3,' kN (',Z/mat.pound2N,' lbs)')
print('ratio= ',ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio)<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
