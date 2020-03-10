# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from model.geometry import truss_generators
from materials import typical_materials
from materials.sections import section_properties
from solution import predefined_solutions

inchToMeter= 2.54/100.0
feetToMeter= 0.3048
psfTokNm2= 0.04788026
    
#########################################################
# Problem definition.
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
modelSpace= predefined_spaces.StructuralMechanics3D(preprocessor.getNodeHandler)

#########################################################
# Material definition.
lumber4x2= section_properties.RectangularSection("lumber4x2",b=3.5*inchToMeter,h=1.5*inchToMeter)
wood= typical_materials.MaterialData(name= 'wood',E=12.4e9,nu=0.33,rho=500)
chordSectionsGeometry= lumber4x2
chordsSection= chordSectionsGeometry.defElasticShearSection3d(preprocessor,wood)
diagonalSectionsGeometry= lumber4x2
diagonalsMaterial= typical_materials.defElasticMaterial(preprocessor,"diagonalsMaterial",wood.E)
postsSection= chordsSection

depth= 22*inchToMeter-chordSectionsGeometry.h
panelSize= 60*inchToMeter
span= feetToMeter*31.0+5*inchToMeter

#########################################################
# Mesh generation.
lowerChordAxis= geom.Segment3d(geom.Pos3d(0.0,0.0,0.0),geom.Pos3d(span,0.0,0.0))
upperChordAxis= geom.Segment3d(geom.Pos3d(0.0,0.0,depth),geom.Pos3d(span,0.0,depth))
truss= truss_generators.FanTruss(lowerChordAxis, upperChordAxis, trussModule= panelSize)
truss.lowerChordMaterial= chordsSection
truss.upperChordMaterial= chordsSection
truss.diagonalMaterial= diagonalsMaterial
truss.diagonalArea= diagonalSectionsGeometry.A()
truss.postsMaterial= postsSection

truss.genMesh(feProblem)

#########################################################
# Boundary conditions.
ptA= truss.upperChordPoints[0]
ptB= truss.upperChordPoints[-1]
modelSpace.fixNode000_FFF(ptA.getNode().tag)  # Fix all the 3 displacement DOFs of the node
                                              # at point A.
modelSpace.fixNode000_FFF(ptB.getNode().tag)  # Fix all the 3 displacement DOFs of the node
                                              # at point B.
ptC= truss.lowerChordPoints[0]
ptD= truss.lowerChordPoints[-1]
modelSpace.fixNode('F0F_FFF',ptC.getNode().tag)  # Fix the y displacement DOFs of the node
                                                 # at point C.
modelSpace.fixNode('F0F_FFF',ptD.getNode().tag)  # Fix the y displacement DOFs of the node
                                                 # at point D.

#########################################################
# Load
lPatterns= preprocessor.getLoadHandler.getLoadPatterns  # Load pattern container.
# Variation of load with time.
ts= lPatterns.newTimeSeries("constant_ts","ts")  # Constant load, no variation.
lPatterns.currentTimeSeries= "ts"  # Time series to use for the new load patterns.
# Load pattern definition
lp0= lPatterns.newLoadPattern("default","0") #New load pattern named 0
lPatterns.currentLoadPattern= lp0.name

centerSpacing= 16.0*inchToMeter
creepFactorDeadLoad= 1.0
creepFactorLiveLoad= 1.0
#Upper chord loads
uniformLiveLoad= (creepFactorDeadLoad*10.0+creepFactorLiveLoad*40.0)*centerSpacing*psfTokNm2*1e3
loadVector=xc.Vector([0,0,-uniformLiveLoad,0,0,0])
for e in truss.upperChordSet.getElements:
    e.vector3dUniformLoadGlobal(loadVector)

#Lower chord loads
uniformLiveLoad= (creepFactorDeadLoad*5.0)*centerSpacing*psfTokNm2*1e3
loadVector=xc.Vector([0,0,-uniformLiveLoad,0,0,0])
for e in truss.lowerChordSet.getElements:
    e.vector3dUniformLoadGlobal(loadVector)

# We add the load case to domain.
lPatterns.addToDomain(lp0.getName())

xcTotalSet= preprocessor.getSets.getSet("total")

#########################################################
# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

lowerChordCentroid= truss.lowerChordSet.getNodes.getCentroid(1.0)
lowerChordCenterNode= truss.lowerChordSet.getNodes.getNearestNode(lowerChordCentroid)
globalCreepFactor= 1.5
deltaY= globalCreepFactor*lowerChordCenterNode.getDisp[2]  # y displacement of node at point pt1.
deltaYTeor= -span/480.0
diff= (deltaYTeor-deltaY)
ratio= abs(diff/deltaYTeor)
ratioForTest= abs(deltaY+20.8929830547/1e3)/20.8929830547

'''
print('deltaY= ', deltaY*1e3, ' mm')
print('deltaYTeor= ', deltaYTeor*1e3, ' mm')
print('diff= ', diff*1e3, ' mm')
print('ratio= ', ratio)
print('ratioForTest= ', ratioForTest)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratioForTest<=1e-12) :
  print ("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
