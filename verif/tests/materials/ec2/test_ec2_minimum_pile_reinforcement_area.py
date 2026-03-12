# -*- coding: utf-8 -*-
''' Computation of the minimumn reinforcement area for a bored pile, according
    to paragraph (3) of clause 9.8.5 of EC2:2004.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section

pileDiameter= 1.5-.05 # EN 1992-1-1 clause 2.3.4.2 (2) 
kf= 1.25 # EN 1992-1-1 clause 2.4.2.5 (2) 
concrete= EC2_materials.EC2Concrete('pileC30', fck= -30e6, gammaC= 1.5*kf)
reinfSteel= EC2_materials.S500B
nomCover= 75e-3 # nominal cover 

# Pile reinforcement.
## Shear reinforcement
shearReinfDiam= 12e-3
shearReinfArea= math.pi*(shearReinfDiam/2.0)**2
nBranches= 2
shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= nBranches, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.20, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

## Longitudinal reinforcement
rebarDiam= 25e-3
rebarArea= math.pi*(rebarDiam/2.0)**2
numOfRebars= 28
mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(pileDiameter-2*(nomCover+shearReinfDiam)), nominalCover= nomCover)])
# Section geometry
pileSection= def_column_RC_section.RCCircularSection(name='test', sectionDescr= 'circular section', Rext= pileDiameter/2.0, concrType=concrete, reinfSteelType= reinfSteel)
pileSection.mainReinf= mainReinf
pileSection.shReinf= shearReinf

# Check minimum longitudinal reinforcement.
eff, as_min= EC2_limit_state_checking.check_bored_piles_minimum_longitudinal_reinforcement(pileSection, nationalAnnex= 'Spain')

# Reference value.
Ac= math.pi*(pileDiameter/2)**2
fyd= 500e6/1.15
fcd= 30e6/(1.5*kf)
As_req= max(0.1*Ac*fcd/fyd, 0.0025*Ac)
ratio1= abs(As_req-as_min)/As_req
ratio2= abs(eff-0.4421257142857143)

# print('Minimum required reinforcement area: ', as_min*1e6, 'mm2')
# print('Reinforcement area: ', pileSection.getMainReinforcementArea()*1e6, 'mm2')
# print('Efficiency: ', eff)
# print('ratio1= ', ratio1)
# print('ratio2= ', ratio2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
