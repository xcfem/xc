# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE F.11C SINGLE-ANGLE FLEXURAL MEMBER WITH VERTICAL AND HORIZONTAL LOADING

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import xc_base
import geom
import xc
from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from solution import predefined_solutions
#from postprocess import output_handler

from misc_utils import units_utils


MPa2ksi= 0.145038
kN2kips= 0.2248
kip2kN= 1.0/kN2kips

m2Toin2= 1.0/units_utils.inchToMeter**2

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Example F.11C'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials

## Steel material
steel= ASTM_materials.A36
steel.gammaM= 1.00

## Profile geometry
# *************************************************************************
# AISC single angle profiles.
# *************************************************************************
#         Section axis:         *                *
#      AISC            XC       *    Points:     *    Principal axes:
#                               *                *
#         ^ Y           ^ y     *                *   
#         |             |       *                *    Z-axis
#                               *     A          *       \  |       W-axis
#       |             |         *      |         *        \ |     /
#       | -> X        | -> z    *      |         *         \|   /
#       |             |         *      |         *          | +
#       -------       -------   *     B-------C  *          ----------
#
# *************************************************************************

# The axis used in AISC catalogue are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.
shape= ASTM_materials.LShape(steel,'L4X4X1/4')
xcSection= shape.defElasticShearSection3d(preprocessor)

# Model geometry

## Points.
span= 6*units_utils.footToMeter
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPoint(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPoint(geom.Pos3d(span,0.0,0.0))

## Lines
lineHandler= preprocessor.getMultiBlockTopology.getLines
l1= lineHandler.newLine(p0.tag,p1.tag)
l1.nDiv= 10

# Mesh
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
trfs= preprocessor.getTransfCooHandler
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,-1,0])
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= lin.name
elem= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))

xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Constraints
modelSpace.fixNode("000_0FF",p0.getNode().tag)
modelSpace.fixNode("F00_FFF",p1.getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['deadLoad','liveLoad', 'windLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Dead load.
deadLoad= xc.Vector([0.0,0.0,-0.05e3*kip2kN/units_utils.footToMeter])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
for e in xcTotalSet.elements:
  e.vector3dUniformLoadGlobal(deadLoad)
  
## Live load.
liveLoad= xc.Vector([0.0,0.0,-0.15e3*kip2kN/units_utils.footToMeter])
cLC= loadCaseManager.setCurrentLoadCase('liveLoad')
for e in xcTotalSet.elements:
  e.vector3dUniformLoadGlobal(liveLoad)

## Wind load.
windLoad= xc.Vector([0.0,0.12e3*kip2kN/units_utils.footToMeter,0.0])
cLC= loadCaseManager.setCurrentLoadCase('windLoad')
for e in xcTotalSet.elements:
  e.vector3dUniformLoadGlobal(windLoad)

## Load combinations
combContainer= combs.CombContainer()

### Ultimate limit state.
combContainer.ULS.perm.add('combULS01','1.2*deadLoad+1.0*liveLoad+1.0*windLoad')

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
MMaxZ= -1e23
MMinY= -MMaxZ
for e in xcTotalSet.elements:
    MMaxZ= max(MMaxZ,max(e.getMz1, e.getMz2))
    MMinY= min(MMinY,min(e.getMy1, e.getMy2))
MMaxZRef= -(1.2*deadLoad[2]+1.0*liveLoad[2])*span**2/8.0
MMinYRef= -1.0*windLoad[1]*span**2/8.0
ratio1= abs((MMaxZ-MMaxZRef)/MMaxZRef)
ratio2= abs((MMinY-MMinYRef)/MMinYRef)

beam= aisc.Member(l1.name, shape, unbracedLengthX= span, unbracedLengthY= span, unbracedLengthZ= span, lstLines= [l1])
CF= beam.getBiaxialBendingEfficiency(Nd= 0.0, Myd=MMinY,Mzd= MMaxZ)[0]
ratio3= abs(CF-0.37675810756810074)/0.37675810756810074

'''
print('MMaxZ= ',MMaxZ/1e3,' kN m (',MMaxZ/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', MMaxZ/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio1= ',ratio1)
print('MMinY= ',MMinY/1e3,' kN m (',MMinY/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', MMinY/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio2= ',ratio2)
print('CF= ',CF)
print('ratio3= ',ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Graphic output
# oh= output_handler.OutputHandler(modelSpace)
#oh.displayLoads()
#oh.displayLocalAxes()
#oh.displayDispRot('uY')
#oh.displayDispRot('uZ')
#oh.displayStrongWeakAxis()
#oh.displayIntForcDiag('My')
#oh.displayIntForcDiag('Mz')
