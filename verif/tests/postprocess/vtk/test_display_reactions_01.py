# -*- coding: utf-8 -*-
''' Check reactions graphic output. The test does not verify the output 
contents, only that the method runs and the file is  created.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties (IPE-80)
A= 7.64e-4 # Cross section area (m2)
Iz= 8.49e-8 # Cross section moment of inertia (m4)

# Geometry
L= 1 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Define nodes.
### Half simply-suppoted beam (symmetry at n2)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(L,0)
### Whole simply-suppoted beam.
n11= nodes.newNodeXY(0,1)
n12= nodes.newNodeXY(L,1)
n13= nodes.newNodeXY(2*L,1)

## Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin")

## Define material.
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)

## Define elements.
# Define element.
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
halfBeam= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
wholeBeamLeft= elements.newElement("ElasticBeam2d",xc.ID([n11.tag,n12.tag]))
wholeBeamRight= elements.newElement("ElasticBeam2d",xc.ID([n12.tag,n13.tag]))

# Constraints
## Half beam constraints.
modelSpace.fixNode00F(n1.tag)
vertLine= geom.Line2d(geom.Pos2d(L,0), geom.Pos2d(L,10.0)) # Vertical line at beam end.
# Normally, the symmetry constraints will not be aligned with the axes, but
# this case is an exception to make the checking of the results easier.
symmetry= modelSpace.constraints.newSymmetryConstraint(n2.tag, vertLine)
## Whole beam constraints.
modelSpace.fixNode00F(n11.tag)
modelSpace.fixNode00F(n13.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
loadVector= xc.Vector([0,-F,0])
lp0.newNodalLoad(n2.tag, loadVector)
lp0.newNodalLoad(n12.tag, 2*loadVector)
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solve.
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
if(result!=0):
  errMsg= 'Solution algorithm failed.'
  lmsg.error(errMsg)
  exit(1)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.displayReactions()

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayReactions(fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
