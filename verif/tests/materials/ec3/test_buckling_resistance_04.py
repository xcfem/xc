''' Check the computation of critical axial loads based on the results of an eigenvalue analysis. Two-dimensional model. See also test_buckling_resistance_02.py

Inspired on the problem described in: https://sdcverifier.com/benchmarks/eurocode-3-buckling-resistance-of-a-compression-member/
'''

import sys
import geom
import xc

import math
from model import predefined_spaces
from solution import predefined_solutions
from materials.ec3 import EC3_materials
from materials.ec3 import EC3_limit_state_checking

# Material
steel= EC3_materials.S355JR
gammaM0= 1.00
steel.gammaM= gammaM0 
steelShape= EC3_materials.CHSShape(steel,'CHS_244.5_10.0')
## Section class.
steelShape.sectionClass= steelShape.getClassInCompression()

# Finite element model.
## Problem type
feProblem= xc.FEProblem()
feProblem.title= 'Building columns effective lengths.'
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Problem geometry
H= 4
p2= modelSpace.newKPoint(0,H,0)
p1= modelSpace.newKPoint(0,0,0)
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 6

## Geometric transformations
corot= modelSpace.newCorotCrdTransf("corot")
modelSpace.setDefaultCoordTransf(corot)

## Finite element material
xcSection= steelShape.defElasticShearSection2d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)

## Finite element type.
modelSpace.newSeedElement("ElasticBeam2d")

## Mesh generation.
l1.genMesh(xc.meshDir.I)

## Constraints.
bottomNode= p1.getNode()
topNode= p2.getNode()
modelSpace.fixNode('00F',bottomNode.tag)
modelSpace.fixNode('0FF',topNode.tag)

## Loads
lp0= modelSpace.newLoadPattern(name= 'lp0')
NEd= 1630e3
lp0.newNodalLoad(topNode.tag, xc.Vector([0,-NEd,0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Perform linear buckling analysis.
linearBucklingAnalysis= predefined_solutions.SpectraLinearBucklingAnalysis(prb= feProblem, numModes= 3)
linearBucklingAnalysis.setup()
analOk= linearBucklingAnalysis.solve()

# Get results.
eulerBucklingLoadFactor1= linearBucklingAnalysis.analysis.getEigenvalue(1)
eulerBucklingLoadFactor2= linearBucklingAnalysis.analysis.getEigenvalue(2)
bucklingLoadFactors= [eulerBucklingLoadFactor1, eulerBucklingLoadFactor2]

xcTotalSet= modelSpace.getTotalSet()

avgLeff= 0.0
avgNbRd= 0.0
# Compute critical axial loads.
for e in xcTotalSet.elements:
    Leffi, mechLambdai, Xi, NbRdi= EC3_limit_state_checking.get_buckling_parameters(element= e, bucklingLoadFactors= bucklingLoadFactors, steelShape= steelShape)
    avgLeff+= Leffi[0]
    avgNbRd+= NbRdi[0]

sz= len(xcTotalSet.elements)
avgLeff/=sz
ratio1= abs(avgLeff-3.9407414627361965)/3.9407414627361965
avgNbRd/=sz
ratio2= abs(avgNbRd-2306.398883222727e3)/2306.398883222727e3

'''
print('first euler buckling load factor: ', eulerBucklingLoadFactor1)
print('second euler buckling load factor: ', eulerBucklingLoadFactor2)
print('average effective length: ', avgLeff, 'm', ratio1)
print('average buckling resistance: ', avgNbRd/1e3, 'kN', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-3) and (ratio2<1e-3)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayEigenvectors(1)
# # oh.displayEigenResult(1, defFScale= 10.0)
# # oh.displayLocalAxes()
# # oh.displayReactions()
