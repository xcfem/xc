# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE F.2-2B COMPACT CHANNEL FLEXURAL MEMBER WITH BRACING AT ENDS AND
# FIFTH POINTS.

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
steelBeam.title= 'Example F.2-2B'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A36
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.CShape(steel,'C15X33.9')
xcSection= shape.defElasticShearSection2d(preprocessor)

# Model geometry

## Points.
span= 25.0*units_utils.footToMeter
unbracedLength= span/5.0 # braced at fifth points
pointHandler= preprocessor.getMultiBlockTopology.getPoints
points= list()
for i in range(0,6):
    x= i*unbracedLength
    points.append(pointHandler.newPoint(geom.Pos3d(x,0.0,0.0)))

## Lines
lines= list()
lineHandler= preprocessor.getMultiBlockTopology.getLines
pA= points[0]
for p in points[1:]:
    pB= p
    lines.append(lineHandler.newLine(pA.tag,pB.tag))
    pA= pB


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
modelSpace.fixNode00F(points[0].getNode().tag)
modelSpace.fixNodeF0F(points[-1].getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['deadLoad','liveLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Dead load.
DL= -0.23e3*kip2kN/units_utils.footToMeter
deadLoad= xc.Vector([0.0, DL])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
for e in xcTotalSet.elements:
    e.vector2dUniformLoadGlobal(deadLoad)
  
## Live load.
LL= -0.69e3*kip2kN/units_utils.footToMeter
liveLoad= xc.Vector([0.0, LL])
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

## Flexural strength
preprocessor.resetLoadCase()
preprocessor.getLoadHandler.addToDomain('combULS01')
result= analysis.analyze(1)
nodes.calculateNodalReactions(True,1e-7)
MMax= -1e23
MMin= -MMax
for e in xcTotalSet.elements:
    MMax= max(MMax,max(e.getM1, e.getM2))
    MMin= min(MMin,min(e.getM1, e.getM2))
MMaxRef= -(1.2*deadLoad[1]+1.6*liveLoad[1])*span**2/8.0
ratio1= abs((MMax-MMaxRef)/MMaxRef)

Mp= shape.getPlasticMoment()
Mu= Mp
worstBeam= None
for l in lines:
    beam=  aisc.Member(l.name, shape, unbracedLengthX= l.getLength(), unbracedLengthY= span, unbracedLengthZ= span, lstLines= [l])
    Mui= beam.getDesignFlexuralStrength()
    if(Mui<Mu):
        Mu= min(Mu, Mui)
        worstBeam= beam
MuRef= 0.9*146e3*kip2kN*units_utils.footToMeter
ratio2= abs((Mu-MuRef)/MuRef)

'''
print('MMaxRef= ',MMaxRef/1e3,' kN m')
print('MMax= ',MMax/1e3,' kN m')
print('ratio2= ',ratio1)
print('Mu= ',Mu/1e3,' kN m(',Mu/1e3*kN2kips/units_utils.footToMeter,' kip-ft)')
print('MuRef= ',MuRef/1e3,' kN m(',MuRef/1e3*kN2kips/units_utils.footToMeter,' kip-ft)')
print('ratio3= ',ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-7 and ratio2<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
