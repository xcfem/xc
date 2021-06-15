# -*- coding: utf-8 -*-
# Home made test.

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import xc_base
import geom
import xc
from materials.astm_aisc import ASTM_materials
from model import predefined_spaces
from actions import load_cases
from solution import predefined_solutions

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Self weight test'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.WShape(steel,'W18X50')
xcSection= shape.defElasticShearSection2d(preprocessor)

# Model geometry

## Points.
span= 10.0
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPntFromPos3d(geom.Pos3d(span,0.0,0.0))

## Lines
lineHandler= preprocessor.getMultiBlockTopology.getLines
l1= lineHandler.newLine(p0.tag,p1.tag)
l1.nDiv= 2

totalMass= shape.get('P')*span
totalWeight= totalMass*9.81

# Mesh
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
trfs= preprocessor.getTransfCooHandler
lin= trfs.newLinearCrdTransf2d("lin")
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= lin.name
elem= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))

xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Constraints
modelSpace.fixNode00F(p0.getNode().tag)
modelSpace.fixNodeF0F(p1.getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['deadLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Dead load.
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
gravityVector= xc.Vector([0.0,9.81])
modelSpace.createSelfWeightLoad(xcTotalSet,gravityVector)

# Solution
## Linear static analysis.
modelSpace.analysis= predefined_solutions.simple_static_linear(steelBeam)

## Deflection linit
preprocessor.getLoadHandler.addToDomain('deadLoad')
result= modelSpace.analyze(calculateNodalReactions= True)

R0= p0.getNode().getReaction[1]
R1= p1.getNode().getReaction[1]
ratio0= abs(R0-totalWeight/2.0)/(totalWeight/2.0)
ratio1= abs(R1-totalWeight/2.0)/(totalWeight/2.0)

'''
print('R0= ',R0)
print('ratio0= ', ratio0)
print('R1= ',R1)
print('ratio1= ', ratio1)
print('totalMass= ', totalMass, 'kg')
print('totalWeight= ', totalWeight/1e3, 'kN')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio0<0.01 and ratio1<0.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
