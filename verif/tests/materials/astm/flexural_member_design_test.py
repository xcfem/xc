# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE F.1-1A W-SHAPE FLEXURAL MEMBER DESIGN IN MAJOR AXIS BENDING,
# CONTINUOUSLY BRACEDhome made test

from __future__ import division
from __future__ import print_function

import xc_base
import geom
import xc
from materials.astm import ASTM_materials
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from solution import predefined_solutions

inch2meter= 0.0254
MPa2ksi= 0.145038
kN2kips= 0.2248
kip2kN= 1.0/kN2kips
foot2meter= 0.3048
m2Toin2= 1.0/inch2meter**2

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Example F.1-1A'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A992
hss= ASTM_materials.WShape(steel,'W18X50')
steel.gammaM= 1.00
## Profile geometry
profile= ASTM_materials.WShape(steel,'W18X50')
xcSection= profile.defElasticShearSection2d(preprocessor,steel)

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
deadLoad= xc.Vector([0.0,-0.45e3*kip2kN/foot2meter, 0.0])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
for e in xcTotalSet.elements:
  e.vector2dUniformLoadGlobal(deadLoad)
  
## Live load.
liveLoad= xc.Vector([0.0,-0.75e3*kip2kN/foot2meter, 0.0])
cLC= loadCaseManager.setCurrentLoadCase('liveLoad')
for e in xcTotalSet.elements:
  e.vector2dUniformLoadGlobal(liveLoad)

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

## Deflection linit
preprocessor.getLoadHandler.addToDomain('combSLS01')
result= analysis.analyze(1)
midSpan1= span/2
midPos1= geom.Pos3d(midSpan1,0.0,0.0)
n1= l1.getNearestNode(geom.Pos3d(midSpan1,0.0,0.0))
d1= n1.getDisp[1]
refD1= -1.17*746/800*inch2meter
ratio1= abs((refD1-d1)/refD1)
deflection= d1/span # Deflection

## Flexural strength
preprocessor.resetLoadCase()
preprocessor.getLoadHandler.addToDomain('combULS01')
result= analysis.analyze(1)
nodes.calculateNodalReactions(True,1e-7)
VMax= -1e23
VMin= -VMax
MMax= -1e23
MMin= -MMax
for e in xcTotalSet.elements:
  VMax= max(VMax,max(e.getV1, e.getV2))
  VMin= min(VMin,min(e.getV1, e.getV2))
  MMax= max(MMax,max(e.getM1, e.getM2))
  MMin= min(MMin,min(e.getM1, e.getM2))
MMaxRef= -(1.2*deadLoad[1]+1.6*liveLoad[1])*span**2/8.0
ratio2= abs((MMax-MMaxRef)/MMaxRef)

# Because the beam is continuously braced and compact, only the
# yielding limit state applies.
Phi_b= 0.90 # LRFD
Mu= Phi_b*profile.getWz()*profile.steelType.fy
MuRef= Phi_b*421e3*kip2kN*foot2meter
ratio3= abs((Mu-MuRef)/MuRef)

'''
print(refD1)
print('ratio1= ',ratio1)
print('dY= ',d1*1e3,' mm/', d1/inch2meter,' in; ratio= L/', 1/deflection, 'L= ', span, ' m')
print('MMaxRef= ',MMaxRef/1e3,' kN m')
print('MMax= ',MMax/1e3,' kN m')
print('ratio2= ',ratio2)
print('Mu= ',Mu/1e3,' kN m')
print('MuRef= ',MuRef/1e3,' kN m')
print('ratio3= ',ratio3)
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(ratio1<5e-4 and ratio2<1e-7and ratio2<5e-3):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
