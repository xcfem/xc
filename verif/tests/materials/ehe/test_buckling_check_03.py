# -*- coding: utf-8 -*-
''' Test calculation of fictitious eccentricity according to clause 43.5.1 of EHE-08.

Inspired in the test found in: https://web.archive.org/web/20241008065525/http://calculocivil.com/es/ehe08/pandeo/calc
'''

from __future__ import division
from __future__ import print_function

import math
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
import os
from misc_utils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

diameter= 0.3 # Section diameter expressed in meters.
cover= 0.03 # Concrete cover expressed in meters.
l0= 5 # Buckling length.

# Materials definition
concr= EHE_materials.HA25
steel= EHE_materials.B500S
## Reinforced concrete section.
section= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)

## Reinforcement.
rebarDiam= 20e-3
rebarArea= math.pi*(rebarDiam/2.0)**2
numOfRebars= 12
shearRebarsDiam= 8e-3
mechCover= cover-shearRebarsDiam-rebarDiam/2.0
mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*mechCover), nominalCover= cover+shearRebarsDiam)])
## Put the reinforcement in the section.
section.mainReinf= mainReinf

# Loads.
Nd= -700e3
Md= 15e3
e2= 2.14e-2
e1= e2

# Slenderness
## Compute mechanical slenderness
iz= section.izHomogenizedSection()
mechanicalSlenderness= l0/iz
## Compute lower slenderness limit
### Non-dimensional axial force.
nu= section.getNonDimensionalAxialForce(Nd)
refNu= 0.5941784542097426
ratio1= abs(nu-refNu)/refNu
lowerSlendernessLimit= EHE_limit_state_checking.get_lower_slenderness_limit(C= 0.2, nonDimensionalAxialForce= nu, e1= e1, e2= e2, sectionDepth= diameter)
refLowerSlendernessLimit= 42.42200041009507
ratio2= abs(lowerSlendernessLimit-refLowerSlendernessLimit)/refLowerSlendernessLimit
## Compute fictitious eccentricity
ee= e2 # Equivalent first order design eccentricity (sway supports).
reinforcementFactor= 2
ef= EHE_limit_state_checking.get_fictitious_eccentricity(sectionDepth= diameter, firstOrderEccentricity= ee, reinforcementFactor= reinforcementFactor, epsilon_y= steel.eyd(), radiusOfGyration= iz, bucklingLength= l0)

## Total eccentricity.
et= max(ee+ef, e2) # Clause 43.5.1 of EHE-08
refEt= 8.853897919631322e-2
ratio3= abs(et-refEt)/refEt
## Bending moment
bucklingBendingMoment= Nd*et

'''
print('radius of gyration: ', iz*1e2, 'cm')
print('mechanical slenderness: ', mechanicalSlenderness)
print('Non-dimensional axial force nu= ', nu, ratio1)
print('lower slenderness limit lambda_inf= ', lowerSlendernessLimit, ratio2)
print('fictitious eccentricity ef= ', ef*100, 'cm')
print('total eccentricity ef= ', et*100, 'cm', ratio3)
print('bending moment Md= ', bucklingBendingMoment/1e3, 'kN.m')
'''

fname= os.path.basename(__file__)
if ((abs(ratio1)<1e-4) and abs(ratio2<1e-4) and abs(ratio3<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
