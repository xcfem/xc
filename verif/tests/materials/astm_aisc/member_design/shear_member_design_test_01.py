# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE G.1A W-SHAPE IN STRONG AXIS SHEAR

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import geom
import xc
from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from solution import predefined_solutions

from misc_utils import units_utils


MPa2ksi= 0.145038
kN2kips= 0.2248
kip2kN= 1.0/kN2kips

m2Toin2= 1.0/units_utils.inchToMeter**2

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Example G.1A'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.WShape(steel,'W24X62')
xcSection= shape.defElasticShearSection2d(preprocessor)

# Model geometry

## Points.
span= 35.0*units_utils.footToMeter
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPoint(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPoint(geom.Pos3d(span,0.0,0.0))

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
seedElemHandler.defaultTransformation= lin.name
elem= seedElemHandler.newElement("ElasticBeam2d")

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
deadLoad= -48.0e3*kip2kN/span*2
deadLoadVector= xc.Vector([0.0,deadLoad])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
for e in xcTotalSet.elements:
    e.vector2dUniformLoadGlobal(deadLoadVector)
  
## Live load.
liveLoad= -145.0e3*kip2kN/span*2
liveLoadVector= xc.Vector([0.0,liveLoad])
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

member= aisc.Member('member', shape, unbracedLengthX= span)

Aw= member.shape.getAw()
AwRef= 10.2*units_utils.inchToMeter**2
ratio2= abs((Aw-AwRef)/AwRef)
Vu= member.getDesignShearStrength()
VuRef= 306e3*kip2kN
ratio3= abs((Vu-VuRef)/VuRef)


'''
print('VMax= ',VMax/1e3,' kN /', VMax*kN2kips/1e3, 'kips')
print('VMaxRef= ',VMaxRef/1e3,' kN /', VMaxRef*kN2kips/1e3, 'kips')
print('ratio1= ',ratio1)
print('tw= ', shape.get('tw')/units_utils.inchToMeter, 'in')
print('Aw= ',Aw*1e4,' cm2')
print('AwRef= ',AwRef*1e4,' cm2')
print('ratio2= ',ratio2)
print('Vu= ',Vu/1e3,' kN m')
print('VuRef= ',VuRef/1e3,' kN m')
print('ratio3= ',ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<5e-4 and ratio2<5e-3 and ratio3<5e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
