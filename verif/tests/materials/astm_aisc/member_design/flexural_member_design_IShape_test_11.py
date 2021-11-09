# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE F.1-2A W-SHAPE FLEXURAL MEMBER DESIGN IN MAJOR AXIS BENDING,
# BRACED AT THIRD POINTS

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT) and Ebrahim Raeyat (ER)"
__copyright__= "Copyright 2021, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com, ebe79442114@gmail.com"

import xc_base
import geom
import xc
from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
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
steelBeam.title= 'Example F.1-2A'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.IShape(bf=.191, tf=.0145, tw=.00902, hw=.435, steel=steel, name='W18X50')
xcSection= shape.defElasticShearSection2d(preprocessor)

# Model geometry

## Points.
span= 35.0*foot2meter
unbracedLength= span/3.0 # braced at third points
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPoint(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPoint(geom.Pos3d(unbracedLength,0.0,0.0))
p2= pointHandler.newPoint(geom.Pos3d(2.0*unbracedLength,0.0,0.0))
p3= pointHandler.newPoint(geom.Pos3d(span,0.0,0.0))

## Lines
lineHandler= preprocessor.getMultiBlockTopology.getLines
l1= lineHandler.newLine(p0.tag,p1.tag)
l2= lineHandler.newLine(p1.tag,p2.tag)
l3= lineHandler.newLine(p2.tag,p3.tag)

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
modelSpace.fixNodeF0F(p3.getNode().tag)

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
for l in [l1,l2,l3]:
    beam= aisc.Member(l.name, shape, unbracedLengthX= unbracedLength, unbracedLengthY= span, unbracedLengthZ= span, lstLines= [l])
    Mui= beam.getDesignFlexuralStrength()
    if(Mui<Mu):
        Mu= min(Mu, Mui)
        worstBeam= beam
Fy= shape.steelType.fy
Sz= shape.get('Wzel')
Lp= shape.getLp(majorAxis= True)
Lr= shape.getLr(majorAxis= True)
Cb= worstBeam.getLateralTorsionalBucklingModificationFactor()
MuRef= 0.9*Cb*(Mp-(Mp-0.7*Fy*Sz)*((unbracedLength-Lp)/(Lr-Lp)))
ratio2= abs((Mu-MuRef)/MuRef)
MuRefText= 0.9*339e3*kip2kN*foot2meter
ratio3= abs((Mu-MuRefText)/MuRefText)
'''
print('MMaxRef= ',MMaxRef/1e3,' kN m')
print('MMax= ',MMax/1e3,' kN m')
print('ratio1= ',ratio1)
print('Mu= ',Mu/1e3,' kN m(',Mu/1e3*kN2kips/foot2meter,' kip-ft)')
print('MuRef= ',MuRef/1e3,' kN m(',MuRef/1e3*kN2kips/foot2meter,' kip-ft)')
print('ratio2= ',ratio2)
print('ratio3= ',ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-7 and ratio2<1e-4 and ratio3<0.05):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
