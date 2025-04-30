# -*- coding: utf-8 -*-
'''Check representation of inertia loads on ElasticBeam3d elements.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
#from postprocess import output_handler
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Create nodes.
l= 10.0
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(l,0,0)

# Create elements.
## Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))
modelSpace.setDefaultCoordTransf(lin)

## Material
structureConcrete= EHE_materials.HA30
reinfSteel= EHE_materials.B500S
stiffnessReductionFactor= 1
lintelsRCSection= def_simple_RC_section.RCRectangularSection(name= 'lintelsRCSection', sectionDescr= 'lintels section', concrType= structureConcrete, reinfSteelType= reinfSteel, width= 1.5, depth= 1.5)
lintelsMaterial= lintelsRCSection.defElasticShearSection3d(preprocessor, reductionFactor= stiffnessReductionFactor)
modelSpace.setDefaultMaterial(lintelsMaterial)

## Element definition.
elements= preprocessor.getElementHandler
beam= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))

## Whole model mass data.
xcTotalSet= modelSpace.getTotalSet()
massZ= xcTotalSet.getTotalMassComponent(2)
massRefZ= 2500*1.5*1.5*l
ratio1= abs(massZ-massRefZ)/massRefZ

# Constraints.
# Zero movement for node 1.
modelSpace.fixNode000_0FF(n1.tag)
# Partial constraint for node 2.
modelSpace.fixNode000_FFF(n2.tag)


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
gravity= 9.81
accel= xc.Vector([0,0,gravity])
xcTotalSet= modelSpace.getTotalSet()
modelSpace.createSelfWeightLoad(xcTotalSet, accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.displayLoads()

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
