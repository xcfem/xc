# -*- coding: utf-8 -*-
''' Check buckling verification test.'''

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

diameter= 1.25 # Section diameter expressed in meters.
cover= 0.05 # Concrete cover expressed in meters.
l0= 20.36 # Buckling length.

# Loads.
Nd= -2990e3 # concomitant Nd= -1400e3
MdTop= 2000e3
MdBottom= -1970e3
if(MdTop>MdBottom):
    e1, e2= EHE_limit_state_checking.get_buckling_e1_e2_eccentricities(Nd= Nd, MdMax= MdTop, MdMin= MdBottom)
else:
    e1, e2= EHE_limit_state_checking.get_buckling_e1_e2_eccentricities(Nd= Nd, MdMax= MdBottom, MdMin= MdTop)

# Materials definition
concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Reinforced concrete section.
section= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)

## Reinforcement.
rebarDiam= 25e-3
rebarArea= math.pi*(rebarDiam/2.0)**2
numOfRebars= 24
shearRebarsDiam= 16e-3
mechCover= cover-shearRebarsDiam-rebarDiam/2.0
mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*mechCover), nominalCover= cover+shearRebarsDiam)])
## Put the reinforcement in the section.
section.mainReinf= mainReinf

# Slenderness
## Compute mechanical slenderness
iz= section.izHomogenizedSection()
mechanicalSlenderness= l0/iz
refMechanicalSlenderness= 64.09311816794691
ratio0= abs(mechanicalSlenderness-refMechanicalSlenderness)/refMechanicalSlenderness
## Compute lower slenderness limit
### Non-dimensional axial force.
nu= section.getNonDimensionalAxialForce(Nd)
lowerSlendernessLimit= EHE_limit_state_checking.get_lower_slenderness_limit(C= 0.2, nonDimensionalAxialForce= nu, e1= e1, e2= e2, sectionDepth= diameter)
ratio1= abs(lowerSlendernessLimit-100)/100

## Compute fictitious eccentricity
ee= e2 # Equivalent first order design eccentricity (sway supports).
reinforcementFactor= 2
ef= EHE_limit_state_checking.get_fictitious_eccentricity(sectionDepth= diameter, firstOrderEccentricity= ee, reinforcementFactor= reinforcementFactor, epsilon_y= steel.eyd(), radiusOfGyration= iz, bucklingLength= l0)
ratio2= abs(ef-33.833155801533366e-2)/33.833155801533366e-2

## Total eccentricity.
et= max(ee+ef, e2) # Clause 43.5.1 of EHE-08
ratio3= abs(et-100.72278790855678e-2)/100.72278790855678e-2

## Bending moment
bucklingBendingMoment= Nd*et

'''
print('radius of gyration: ', iz*1e2, 'cm')
print('mechanical slenderness: ', mechanicalSlenderness, ratio0)
print('Non-dimensional axial force nu= ', nu)
print('lower slenderness limit lambda_inf= ', lowerSlendernessLimit, ratio1)
if(mechanicalSlenderness<lowerSlendernessLimit):
    print('\nMechanical slenderness ('+str(mechanicalSlenderness)+') is less than the lower slenderness limit of the member ('+str(lowerSlendernessLimit)+'); second order effects may be disregarded.\n')
print('equivalent first order design eccentricity ee= ', ee*100, 'cm')
print('fictitious eccentricity ef= ', ef*100, 'cm', ratio2)
print('total eccentricity et= ', et*100, 'cm', ratio3)
print('bending moment Md= ', bucklingBendingMoment/1e3, 'kN.m')
'''

fname= os.path.basename(__file__)
if ((abs(ratio0)<1e-4) and (abs(ratio1)<1e-4) and (abs(ratio2)<1e-4) and (abs(ratio3)<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

