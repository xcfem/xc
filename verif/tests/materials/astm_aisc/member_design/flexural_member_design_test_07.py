# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE F.5 I-SHAPED FLEXURAL MEMBER IN MINOR-AXIS BENDING.

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
steelBeam.title= 'Example F.5'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.WShape(steel,'W12X58')
xcSection= shape.defElasticShearSection2d(preprocessor, majorAxis= False)

# Model geometry

## Points.
span= 15.0*units_utils.footToMeter
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
deadLoad= xc.Vector([0.0,-0.667e3*kip2kN/units_utils.footToMeter, 0.0])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
for e in xcTotalSet.elements:
  e.vector2dUniformLoadGlobal(deadLoad)
  
## Live load.
liveLoad= xc.Vector([0.0,-2e3*kip2kN/units_utils.footToMeter, 0.0])
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
E= shape.get('E')
Iy= shape.get('Iy') # minor axis
refD1= (5.0*liveLoad[1]*span**4/384.0)/E/Iy
ratio1= abs((refD1-d1)/refD1)
deflection= d1/span # Deflection

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
MMaxRef= -1.2*deadLoad[1]*span**2/8.0-1.6*liveLoad[1]*span**2/8.0
ratio2= abs((MMax-MMaxRef)/MMaxRef)

beam=  aisc.Member(l1.name, shape, unbracedLengthX= l1.getLength(), unbracedLengthY= l1.getLength(), unbracedLengthZ= l1.getLength(), lstLines= [l1])
Mu= beam.getDesignFlexuralStrength(majorAxis= False)
MuRef= 165.4965e3
MuRefText= 0.9*136e3*kip2kN*units_utils.footToMeter
ratio3= abs((Mu-MuRef)/MuRef)
ratio4= abs((Mu-MuRefText)/MuRefText)

'''
print('span= ', span, ' m(',span/units_utils.footToMeter,' ft)')
print('Iy= ', Iy, ' m4(',Iy/units_utils.inchToMeter**4,' in4)')
print('E= ', E/1e9, ' GPa(',E/1e6*MPa2ksi,' ksi)')
print('refD1= ', refD1*1e3, ' mm(',refD1/units_utils.inchToMeter,' in)')
print('d1= ', d1*1e3, ' mm(',d1/units_utils.inchToMeter,' in)')
print('ratio1= ',ratio1)
print('dY= ',d1*1e3,' mm/', d1/units_utils.inchToMeter,' in; ratio= L/', 1/deflection, 'L= ', span, ' m')
print('MMaxRef= ',MMaxRef/1e3,' kN m')
print('MMax= ',MMax/1e3,' kN m')
print('ratio2= ',ratio2)
print('Mu= ',Mu/1e3,' kN m(',Mu/1e3*kN2kips/units_utils.footToMeter,' kip-ft)')
print('MuRef= ',MuRef/1e3,' kN m(',MuRef/1e3*kN2kips/units_utils.footToMeter,' kip-ft)')
print('MuRefText= ',MuRefText/1e3,' kN m(',MuRefText/1e3*kN2kips/units_utils.footToMeter,' kip-ft)')
print('ratio3= ',ratio3)
print('ratio4= ',ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<6e-2 and ratio2<1e-7 and ratio3<1e-5 and ratio4<.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
