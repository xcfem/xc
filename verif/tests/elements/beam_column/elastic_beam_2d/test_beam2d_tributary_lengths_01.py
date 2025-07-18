# -*- coding: utf-8 -*-
'''Check the computation of tributary lengths for ElasticBeam2d elements.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model.sets import sets_mng 
from materials import typical_materials

# Create FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Problem geometry
l= 10
### k-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(l,0,0))
### lines
lines= preprocessor.getMultiBlockTopology.getLines
ln= lines.newLine(pt1.tag,pt2.tag)
ln.nDiv= 15 # Number of elements along the line.

## Mesh generation

### Materials definition
# Mechanical properties (irrelevant in this example).
E= 2.0e11 # Young modulus.
A= 31e-4 # Beam cross-section area.
h= 2.0*math.sqrt(A/math.pi) # Beam cross-section depth.
I= 2810e-8 # Inertia of the beam section.
sry= 2.42 # Shear coefficient.
Ay= A/sry
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu)) # Shear modulus.
scc= typical_materials.defElasticShearSection2d(preprocessor, "scc",A,E,G,I,alpha= Ay/A)
modelSpace.setDefaultMaterial(scc)

### Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)

## Seed element
beam2d= modelSpace.newSeedElement("ElasticBeam2d")
beam2d.h= h

## Generate mesh.
ln.genMesh(xc.meshDir.I)

tributaryLengths= sets_mng.get_tributary_lengths(xcSet= ln, initialGeometry= False)

# Check results.
tributaryLength= 0.0
for n in ln.nodes:
    tributaryLength+= tributaryLengths[n.tag]

error= abs(tributaryLength-l)/l


# print(tributaryLength, error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
