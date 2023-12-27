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

structureConcrete= EHE_materials.HA30
reinfSteel= EHE_materials.B500S
columnNominalCover= 0.04
columnStirrupsDiameter= 12e-3

# Section geometry.
width= 0.45
depth= 1.4

testRCSection= def_simple_RC_section.RCRectangularSection(name= 'testRCSection', sectionDescr= 'test RC section', concrType= structureConcrete, reinfSteelType= reinfSteel, width= width, depth= depth, swapReinforcementAxes= False)

### Main reinforcement.
firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= testRCSection.defineMainReinforcement(nominalCover= columnNominalCover, fiStirr= columnStirrupsDiameter, topLayersDiameters= [32e-3, 32e-3], bottomLayersDiameters= [32e-3, 32e-3], lateralLayersDiameters= [25e-3, None], nRebarsHoriz= 6, nRebarsVert= 11)

testRCSection.defineShearReinforcementYZ(nShReinfBranchesY= 4, fiStirrY= columnStirrupsDiameter, spacingY= 0.15, nShReinfBranchesZ= 8, fiStirrZ= columnStirrupsDiameter, spacingZ= 0.15)

# XC material definition
problem=xc.FEProblem()
preprocessor=problem.getPreprocessor
testRCSection.defRCSection(preprocessor= preprocessor, matDiagType= 'k')

# Check results.
## Main reinforcement area.
mainReinforcementArea= testRCSection.getMainReinforcementArea()
refMainReinforcementArea= 24*math.pi*(32e-3/2.0)**2+22*math.pi*(25e-3/2.0)**2
ratio1= abs(mainReinforcementArea-refMainReinforcementArea)/refMainReinforcementArea

tangConcr= testRCSection.getConcreteDiagram(preprocessor).getTangent()
## Center of gravity of the homogenized section.
GH= testRCSection.geomSection.getCenterOfMassHomogenizedSection(tangConcr)
refGH= geom.Pos2d(0.0, -0.00108338)
ratio2= geom.Pos2d(GH[0], GH[1]).dist(refGH)

## Inertia tensor of homogenized section.
IyH= testRCSection.geomSection.getIyHomogenizedSection(tangConcr)
refIyH= 0.155685196997532
ratio3= abs(IyH-refIyH)/refIyH
IzH=  testRCSection.geomSection.getIzHomogenizedSection(tangConcr)
refIzH= 0.013977087341801567
ratio4= abs(IzH-refIzH)/refIzH
PyzH= testRCSection.geomSection.getPyzHomogenizedSection(tangConcr)
ratio5= abs(PyzH)

ratio6= abs(testRCSection.getShearReinfY().nShReinfBranches-4)+abs(testRCSection.getShearReinfZ().nShReinfBranches-8)

'''
print('main reinforcement area: ', mainReinforcementArea*1e4, refMainReinforcementArea*1e4, ratio1)
print(GH, refGH, ratio2)
print(IyH, refIyH, ratio3)
print(IzH, refIzH, ratio4)
print(PyzH, 0.0, ratio5)
print(ratio6)
'''

testOK= (abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-6) and (abs(ratio4)<1e-6) and (abs(ratio5)<1e-6) and (ratio6==0)

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
