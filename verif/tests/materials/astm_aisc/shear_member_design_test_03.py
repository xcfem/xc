# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE G.4 RECTANGULAR HSS IN SHEAR

from __future__ import division
from __future__ import print_function

import math
import xc_base
import geom
import xc
from materials.astm_aisc import ASTM_materials
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from solution import predefined_solutions

inch2meter= 0.0254
MPa2ksi= 0.145038
kN2kips= 0.2248
kip2kN= 1.0/kN2kips
kip2N= kip2kN*1e3
foot2meter= 0.3048
m2Toin2= 1.0/inch2meter**2

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Example G.4'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A500
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.HSSShape(steel,'HSS6X4X3/8')
xcSection= shape.defElasticShearSection2d(preprocessor,steel)

# Model geometry

## Points.
span= 35.0*foot2meter
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPntFromPos3d(geom.Pos3d(span,0.0,0.0))

## Lines
lineHandler= preprocessor.getMultiBlockTopology.getLines
l1= lineHandler.newLine(p0.tag,p1.tag)
l1.nDiv= 10

# Mesh
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
trfs= preprocessor.getTransfCooHandler
lin= trfs.newLinearCrdTransf2d("lin")
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= "lin"
elem= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))

xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Constraints
modelSpace.fixNode00F(p0.getNode().tag)
modelSpace.fixNodeF0F(p1.getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['deadLoad','liveLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Dead load.
deadLoad= -11.0*kip2N/span*2.0
deadLoadVector= xc.Vector([0.0,deadLoad, 0.0])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
for e in xcTotalSet.elements:
    e.vector2dUniformLoadGlobal(deadLoadVector)
  
## Live load.
liveLoad= -33.0*kip2N/span*2.0
liveLoadVector= xc.Vector([0.0,liveLoad, 0.0])
cLC= loadCaseManager.setCurrentLoadCase('liveLoad')
for e in xcTotalSet.elements:
    e.vector2dUniformLoadGlobal(liveLoadVector)

## Load combinations
combContainer= combs.CombContainer()

### Serviceability limit states.
combContainer.SLS.qp.add('combSLS01', '1.0*liveLoad')

### Ultimate limit state.
combContainer.ULS.perm.add('combULS01','1.2*deadLoad+1.6*liveLoad')

### Dump combination definition into XC.
combContainer.dumpCombinations(preprocessor)

# Solution
## Linear static analysis.
analysis= predefined_solutions.simple_static_linear(steelBeam)

## Shear strength
preprocessor.resetLoadCase()
preprocessor.getLoadHandler.addToDomain('combULS01')
result= analysis.analyze(1)
nodes.calculateNodalReactions(True,1e-7)
VMax= -1e23
VMin= -VMax
for e in xcTotalSet.elements:
  VMax= max(VMax,max(e.getV1, e.getV2))
  VMin= min(VMin,min(e.getV1, e.getV2))
VMaxRef= -(1.2*deadLoad+1.6*liveLoad)*span/2.0
ratio1= abs((VMax-VMaxRef)/VMaxRef)

Aw= shape.getAw()
AwRef= 3.46*inch2meter**2
ratio2= abs((Aw-AwRef)/AwRef)
Vu= shape.getDesignShearStrength()
VuRef= 0.9*0.6*shape.steelType.fy*Aw*1.0
VuRefText= 0.9*95.5e3*kip2kN
ratio3= abs((Vu-VuRefText)/VuRefText)


'''
print('VMax= ',VMax/1e3,' kN (', VMax*kN2kips/1e3, 'kips)')
print('VMaxRef= ',VMaxRef/1e3,' kN (', VMaxRef*kN2kips/1e3, 'kips)')
print('ratio1= ',ratio1)
print('t= ', shape.get('t')/inch2meter, 'in')
print('Aw= ',Aw*1e4,' cm2')
print('AwRef= ',AwRef*1e4,' cm2')
print('ratio2= ',ratio2)
print('Vu= ',Vu/1e3,' kN', Vu*kN2kips/1e3, 'kips)')
print('VuRef= ',VuRef/1e3,' kN', VuRef*kN2kips/1e3, 'kips)')
print('VuRefText= ',VuRefText/1e3,' kN', VuRefText*kN2kips/1e3, 'kips)')
print('ratio3= ',ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<5e-4 and ratio2<1e-3 and ratio3<1e-2):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
