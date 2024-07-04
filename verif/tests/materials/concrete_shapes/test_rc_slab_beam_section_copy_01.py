# -*- coding: utf-8 -*-
''' Check shear verification for very small sollicitations. Home made test.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.sections.fiber_section import def_simple_RC_section
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from postprocess import element_section_map

structureConcrete= EC2_materials.C30
reinfSteel= EC2_materials.S500B

testRCSection= element_section_map.RCSlabBeamSection(name= 'testRCSection', sectionDescr= 'deck slab RC section; transition zone 1', concrType= structureConcrete, reinfSteelType= reinfSteel, depth= 0.25)
### regular zone deck slab reinforcement.
testTransverseRebarsDiam= 16e-3
testTransverseSpacing= 0.15/2.0
testLongitudinalRebarsDiam= 25e-3
testLongitudinalRebarsSpacing= 0.20
testNominalBottomCover= 0.035
testNominalTopCover= 0.035
#### Transversal reinforcement.
testRCSection.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= testTransverseRebarsDiam, rebarsSpacing= testTransverseSpacing, nominalCover= testNominalTopCover)])
testRCSection.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= testTransverseRebarsDiam, rebarsSpacing= testTransverseSpacing, nominalCover= testNominalBottomCover)])
#### Longitudinal reinforcement.
testRCSection.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= testLongitudinalRebarsDiam, rebarsSpacing= testLongitudinalRebarsSpacing, nominalCover=testNominalTopCover+testTransverseRebarsDiam)])
testRCSection.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= testLongitudinalRebarsDiam, rebarsSpacing= testLongitudinalRebarsSpacing, nominalCover= testNominalBottomCover+testTransverseRebarsDiam)])

shearReinforcedRCSection= testRCSection.getCopy()
equalBefore= (shearReinforcedRCSection==testRCSection)

### Shear reinforcement.
shReinfSpacing= 0.2
shearReinforcedRCSection.dir2ShReinfY= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= 2, areaShReinfBranch= math.pi*(12e-3)**2/4.,shReinfSpacing= shReinfSpacing, angAlphaShReinf= math.pi/2.0)
equalAfter= (shearReinforcedRCSection==testRCSection)

'''
print(equalBefore)
print(equalAfter)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (equalBefore and not equalAfter):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Matplotlib output
# import xc
# problem=xc.FEProblem()
# preprocessor=problem.getPreprocessor
# testRCSection.createSections()
# testRCSection.plot(preprocessor= preprocessor)
    
