# -*- coding: utf-8 -*-
''' Check that control variables read with readContolVars are displayed
    correctly.

    There was an error in the get_element_data_dict function of the 
    control_vars module. This test checks that the same error does not occur
    again.
'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026 AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
import os
import xc
from materials.ec3 import EC3_materials
from materials.ec3 import EC3_limit_state_checking
from model import predefined_spaces
import shutil

#Materials
## Steel material
steel= EC3_materials.S355JR
steel.gammaM= 1.00
## Profile geometry
shape= EC3_materials.MicropileTubeShape(steel= steel, name= 'MP_177.8x10.5')
shape.sectionClass= 1

## Problem geometry
height= 1.0 # column height.

# Compute critical load.
Ncr= shape.getNcr(LeqY= height, LeqZ= height)
refNcr= math.pi**2*shape.EIz()/height**2
ratio1= abs(Ncr-refNcr)/refNcr

# Non-dimensional slenderness.
lambdaZ= shape.getAdimensionalSlendernessZ(Leq= height)
# Bow imperfection
McRdz= shape.getMcRdz()
NcRd= shape.getNcRd()
e0= 0.34*(lambdaZ-0.2)*McRdz/NcRd

# Buckling reduction factor (equal for both axis).
chiN= shape.getBucklingReductionFactorZ(Leq= height)

# Ultimate load:
Fult= -chiN*NcRd

# Finite element model.
## Problem type
steelColumn= xc.FEProblem()
steelColumn.title= 'Benchmark 4'
preprocessor= steelColumn.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Model geometry

### Points.
p0= modelSpace.newKPoint(0.0,0.0,0.0)
p1= modelSpace.newKPoint(0.0,0.0,height)

### Lines
l1= modelSpace.newLine(p0,p1)
l1.nDiv= 6

### Material.
xcSection= shape.defElasticShearSection3d(preprocessor)
### Mesh generation

### Geometric transformations
cooTrf= modelSpace.newCorotCrdTransf("corot", xzVector= xc.Vector([0,1,0])) # Corotational transformation.

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= cooTrf.name
elem= seedElemHandler.newElement("ElasticBeam3d")
xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

### Constraints
modelSpace.fixNode('000_FF0', p0.getNode().tag)
modelSpace.fixNode('00F_FFF', p1.getNode().tag)

# Set the limit state to display.
limitState= EC3_limit_state_checking.normalStressesResistance
# Get the file containing the values of the control vars.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
auxModulePath= pth+"/../../../../aux/internal_forces/"
controlVarsFileName= auxModulePath+'test_uls_checking_ec3_03_normal_stresses_control_vars.json'
modelSpace.readControlVars(controlVarsFileName)

# Check results.
fname= os.path.basename(__file__)

## Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
outputFileName= '/tmp/'+fname.replace('.py', '_cf.jpeg')


oh.displayBeamResult(attributeName= limitState.label, itemToDisp='CF', beamSetDispRes= xcTotalSet, setToDisplay= xcTotalSet, fileName= outputFileName)

testOK= os.path.isfile(outputFileName)

from misc_utils import log_messages as lmsg
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.remove(outputFileName) # Clean after yourself.
