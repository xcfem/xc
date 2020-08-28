# -*- coding: utf-8 -*-
# AISC 360-16 BENCHMARK PROBLEM CASE 1 (figure C-C2.2).
# ElasticBeam3d with corotational formulation.

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
from materials.astm_aisc import AISC_limit_state_checking as aisc
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from solution import predefined_solutions
# from postprocess import output_handler

# Problem type
steelColumn= xc.FEProblem()
steelColumn.title= 'AISC 360-16 benchmark problem Case 2 (3D P-Delta)'
preprocessor= steelColumn.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.WShape(steel,'W14X48')
xcSection= shape.defElasticShearSection3d(preprocessor)

# Model geometry

## Points.
height= 8.53
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPntFromPos3d(geom.Pos3d(0.0,0.0,height))

## Lines
lineHandler= preprocessor.getMultiBlockTopology.getLines
l1= lineHandler.newLine(p0.tag,p1.tag)
l1.nDiv= 6

# Mesh
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Geometric transformations
cooTrf= modelSpace.newPDeltaCrdTransf("corot",xc.Vector([0,1,0]))    

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= cooTrf.name
elem= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))
xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Constraints
modelSpace.fixNode("000_000",p0.getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['p0', 'p1', 'p2', 'p3']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Point load 0.
cLC= loadCaseManager.setCurrentLoadCase('p0')
pLoad= pLoad= xc.Vector([4.45e3, 0.0, 0.0, 0.0, 0.0, 0.0])
p1.getNode().newLoad(pLoad)

## Point load 1.
cLC= loadCaseManager.setCurrentLoadCase('p1')
pLoad= pLoad= xc.Vector([4.45e3, 0.0, -445e3, 0.0, 0.0, 0.0])
p1.getNode().newLoad(pLoad)
  
## Point load 2.
cLC= loadCaseManager.setCurrentLoadCase('p2')
pLoad= pLoad= xc.Vector([4.45e3, 0.0, -667e3,0.0, 0.0, 0.0])
p1.getNode().newLoad(pLoad)

## Point load 3.
cLC= loadCaseManager.setCurrentLoadCase('p3')
pLoad= pLoad= xc.Vector([4.45e3, 0.0, -890e3, 0.0, 0.0, 0.0])
p1.getNode().newLoad(pLoad)

## Control sections
topNode= p1.getNode()
bottomNode= p0.getNode()
bottomElement= preprocessor.getSets.defSet('bottomElement')
bottomElement.nodes.append(bottomNode)
steelColumn.errFileName= "/tmp/erase.log" # Don't print error messages.
bottomElement.fillUpwards()
steelColumn.errFileName= "cerr" # Display errors if any.
  
## Load combinations
combContainer= combs.CombContainer()

combContainer.SLS.qp.add('c0', '1.0*p0')
combContainer.SLS.qp.add('c1', '1.0*p1')
combContainer.SLS.qp.add('c2', '1.0*p2')
combContainer.SLS.qp.add('c3', '1.0*p3')

### Dump combination definition into XC.
combContainer.dumpCombinations(preprocessor)

# Reference values
refMom= [38.0e3,53.1e3,67.7e3,96.2e3]
refDisp= [22.9e-3, 33.9e-3, 44.6e-3, 65.4e-3]

# Solution
## Solution procedure
# SIMPLE NEWTON-RAPHSON segfaults when computing more
# than a combination. See SuperLU.setSize()
# solution= predefined_solutions.SolutionProcedure()
# solution.convergenceTestTol= 1e-5
# analysis= solution.plainNewtonRaphson(steelColumn)

analysis= predefined_solutions.penalty_newton_raphson(steelColumn)

disp= list()
bendingMoments= list()
## Compute values
for c in ['c0', 'c1', 'c2', 'c3']:
    preprocessor.resetLoadCase()
    preprocessor.getLoadHandler.addToDomain(c)
    result= analysis.analyze(1)
    disp.append(topNode.getDisp[0])
    M= 0.0
    for e in bottomElement.elements:
        if(e.getNodes[0].tag==bottomNode.tag):
            M= e.getMz1
        else:
            M= e.getMz2
    bendingMoments.append(M)

ratio1= 0.0
for d,dRef in zip(disp,refDisp):
    ratio1= max(ratio1,abs((dRef-d)/dRef))
    
ratio2= 0.0
for m,mRef in zip(bendingMoments,refMom):
    ratio2= max(ratio2,abs((mRef-m)/mRef))

'''
print('ratio1= ',ratio1)
print('dX= ',disp,' m')
print('ref_dX=',refDisp,' m')
print('ratio2= ',ratio2)
print('m= ',bendingMoments,' N m')
print('mRef=',refMom,' N m')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<.015 and ratio2<0.01):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

#########################################################
# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)

## Uncomment to display blocks
#oh.displayBlocks()

## Uncomment to display the mesh
#oh.displayFEMesh()

## Uncomment to display the loads
#oh.displayLoads()

## Uncomment to display the vertical displacement
#oh.displayDispRot(itemToDisp='uY')
#oh.displayNodeValueDiagram(itemToDisp='uX')

## Uncomment to display the reactions
#oh.displayReactions()

## Uncomment to display the internal force
#oh.displayIntForcDiag('Mz')

