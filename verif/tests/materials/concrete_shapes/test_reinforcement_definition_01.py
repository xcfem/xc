# -*- coding: utf-8 -*-
''' Test convenience functions that define reinforcement of rectangular sections.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
import geom
import xc
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from misc_utils import log_messages as lmsg

foundationConcrete= EHE_materials.HA35
reinfSteel= EHE_materials.B500S
pileCapCover= 0.06 # concrete cover for the pile cap.
fiStirr= 16e-3 # diameter of the stirrups.

# Section geometry.
depth= 1.5
width= 1.5

testRCSection= def_simple_RC_section.RCRectangularSection(name= 'testRCSection', sectionDescr= 'test RC section', concrType= foundationConcrete, reinfSteelType= reinfSteel, width= width, depth= depth)

### Main reinforcement.
firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= testRCSection.defineMainReinforcement(nominalCover= pileCapCover, fiStirr= fiStirr, topLayersDiameters= [32e-3, 32e-3], bottomLayersDiameters= [32e-3, 32e-3], lateralLayersDiameters= [32e-3, 32e-3], nRebarsHoriz= 10, nRebarsVert= 9)

testRCSection.defineShearReinforcementYZ(nShReinfBranchesY= 2, fiStirrY= 8e-3, spacingY= 0.15, nShReinfBranchesZ= 4, fiStirrZ= 8e-3, spacingZ= 0.15)

# XC material definition
problem=xc.FEProblem()
preprocessor=problem.getPreprocessor
testRCSection.defRCSection(preprocessor= preprocessor, matDiagType= 'k')

# Check results.
## Main reinforcement area.
mainReinforcementArea= testRCSection.getMainReinforcementArea()
refMainReinforcementArea= (40+2*18)*math.pi*(32e-3/2.0)**2
ratio1= abs(mainReinforcementArea-refMainReinforcementArea)/refMainReinforcementArea

tangConcr= testRCSection.getConcreteDiagram(preprocessor).getTangent()
## Center of gravity of the homogenized section.
GH= testRCSection.geomSection.getCenterOfMassHomogenizedSection(tangConcr)
refGH= geom.Pos2d(0.0, -0.00101841)
ratio2= geom.Pos2d(GH[0], GH[1]).dist(refGH)

## Inertia tensor of homogenized section.
IyH= testRCSection.geomSection.getIyHomogenizedSection(tangConcr)
refIyH= 0.5093909086169506
ratio3= abs(IyH-refIyH)/refIyH
IzH=  testRCSection.geomSection.getIzHomogenizedSection(tangConcr)
refIzH= 0.5140827713956208
ratio4= abs(IzH-refIzH)/refIzH
PyzH= testRCSection.geomSection.getPyzHomogenizedSection(tangConcr)
ratio5= abs(PyzH)

'''
print(mainReinforcementArea*1e4, refMainReinforcementArea*1e4, ratio1)
print(GH, refGH, ratio2)
print(IyH, refIyH, ratio3)
print(IzH, refIzH, ratio4)
print(PyzH, 0.0, ratio5)
'''

testOK= (abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-6) and (abs(ratio4)<1e-6) and (abs(ratio5)<1e-6)

import os
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Matplotlib output
# testRCSection.plot(preprocessor= preprocessor)
    
# # DXF output
# import ezdxf
# doc = ezdxf.new("R2000")
# msp = doc.modelspace()
# testRCSection.writeDXF(modelSpace= msp)
# dxfFileName= fname.removesuffix('.py')
# doc.saveas(dxfFileName+'.dxf')
