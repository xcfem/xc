# -*- coding: utf-8 -*-
''' Define the same RC section in two different FE problems.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

import math
import xc
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from materials.ec2 import EC2_materials

concrete= EC2_materials.C35
reinfSteel= EC2_materials.S500B

## Pile concrete section.
pileDiameter= 1.80
pileCover= 0.075 # concrete cover for the piles.
stirrupsDiameter= 16e-3 # diameter of the stirrups
stirrupsSpacing= 0.15 # spacing of stirrups
### RC section of the piles.
pileRCSection= def_column_RC_section.RCCircularSection(name= 'pileRCSection', sectionDescr= 'Pile RC section', concrType= concrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
#### Pile main reinforcement.
pileRCSection.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 32e-3, nRebars= 24, width= math.pi*(pileDiameter-2*(pileCover+stirrupsDiameter)), nominalCover= pileCover+stirrupsDiameter)])# in circular sections 'width' is the perimeter of the circumference surrounds the reinforcing bars.
#### Pile shear reinforcement.
pileRCSection.shReinf= def_simple_RC_section.ShearReinforcement(familyName= "pile_sh", nShReinfBranches= 2, areaShReinfBranch= math.pi*(stirrupsDiameter)**2/4., shReinfSpacing= stirrupsSpacing, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= math.pi/4.0)

# FE problem A definition.
feProblemA= xc.FEProblem()
preprocessorA= feProblemA.getPreprocessor

pileMaterialA= pileRCSection.defRCSection(preprocessorA, matDiagType= 'd')
area_a= pileMaterialA.getArea()

# FE problem B definition.
feProblemB= xc.FEProblem()
preprocessorB= feProblemB.getPreprocessor

pileMaterialB= pileRCSection.defRCSection(preprocessorB, matDiagType= 'd')
area_b= pileMaterialB.getArea()

error= (area_a-area_b)/area_a

'''
print(pileMaterialA.tipo())
print('Area a: ', area_a)
print('Area b: ', area_b)
print('error: ', error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
