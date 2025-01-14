# -*- coding: utf-8 -*-
''' Test distribution of wind vertical pressures in a horizontal (or almost 
    horizontal) surface.'''

from __future__ import division
from __future__ import print_function

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from actions.wind import base_wind
from actions.wind import IAPF_wind

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Problem geometry
## Big quads.
origin= geom.Pos3d(-2.5,0,0)
iVector= geom.Vector3d(1,0,0)
jVector= geom.Vector3d(0,1,0)
bottomPoints= list()
topPoints= list()
for i in range(0,6):
    v= i*iVector
    bp= origin+v
    bottomPoints.append(modelSpace.newKPoint(bp.x, bp.y, bp.z))
    tp= origin+jVector+v
    topPoints.append(modelSpace.newKPoint(tp.x, tp.y, tp.z))

quads= list()
bottomPoint0= bottomPoints[0]
topPoint0= topPoints[0]
for bottomPoint1, topPoint1 in zip(bottomPoints[1:], topPoints[1:]):
    quad= modelSpace.newQuadSurface(bottomPoint0, bottomPoint1, topPoint1, topPoint0)
    quad.setElemSizeIJ(0.1, 0.1)
    quads.append(quad)
    topPoint0= topPoint1
    bottomPoint0= bottomPoint1
    
### Define set.
quadsSet= modelSpace.defSet('quadsSet')
for bq in quads:
    quadsSet.surfaces.append(bq)
quadsSet.fillDownwards()

### Generate mesh.
#### Material definition
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "matPrb",E= 30e6,nu= 0.3,rho= 0.0, h= 0.25)
modelSpace.setDefaultMaterial(mat)
modelSpace.newSeedElement("ShellMITC4")
quadsSet.genMesh(xc.meshDir.I)
quadsSet.fillDownwards()

#### Vertical pressure due to transverse wind.
verticalWindPressure= 0.49e3 # See wind.py
verticalWindDir= xc.Vector([0, 0, 1])
transverseWindDir= xc.Vector([0, 1, 0])
verticalPressureDistribution= IAPF_wind.getTrapezoidalPressureDistribution(h= 1.0, heightFraction= 0.25, averagePressure= verticalWindPressure)
angleThreshold= math.radians(60) # minimum angle to consider that the wind is
                                 # normal to a face border.

lc= modelSpace.newLoadPattern(name= 'lc')
modelSpace.setCurrentLoadPattern(lc.name)
loadVectors= base_wind.def_vertical_pressure_load(surfaces= quadsSet.surfaces, pressureDistribution= verticalPressureDistribution, horizontalWindDirection= transverseWindDir, verticalWindDirection= verticalWindDir, angleThreshold= angleThreshold)

## Check total load value.
totalLoad= geom.SlidingVectorsSystem3d()
totalLoadNorm= 0.0
for loadVector in loadVectors:
    totalLoad+= loadVector
    
totalLoadRef= geom.SlidingVectorsSystem3d(geom.Pos3d(0,0,0), geom.Vector3d(0, 0, 2450), geom.Vector3d(618.625,0,0))

test= totalLoad-totalLoadRef
R= test.getResultant()
M= test.getMoment()
errR= R.getModulus()/totalLoadRef.getResultant().getModulus() # Check the load values.
errM= M.getModulus()/totalLoadRef.getMoment().getModulus() # Check the correct placement of the loads.

'''
print('total load: ', totalLoad)
print('R= ', R)
print('M= ', M)
print('errR: ', errR)
print('errM: ', errM)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(errR<1e-6 and errM<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks()
# oh.displayFEMesh()
# modelSpace.addLoadCaseToDomain(lc.name)
# oh.displayLoads()
