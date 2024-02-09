''' Check the computation of critical axial loads based on the results of an eigenvalue analysis. Three-dimensional model. See also test_buckling_resistance_04.py

Inspired on the problem described in: https://sdcverifier.com/benchmarks/eurocode-3-buckling-resistance-of-a-compression-member/
'''

import sys
import geom
import xc

import math
from model import predefined_spaces
from solution import predefined_solutions
from materials.ec3 import EC3_materials

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
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry
H= 4
p2= modelSpace.newKPoint(0,0,H)
p1= modelSpace.newKPoint(0,0,0)
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 6

## Geometric transformations
corot= modelSpace.newCorotCrdTransf("corot", xzVector= xc.Vector([1,0,0]))
modelSpace.setDefaultCoordTransf(corot)

## Finite element material
xcSection= steelShape.defElasticShearSection3d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)

## FiniteElement.
modelSpace.newSeedElement("ElasticBeam3d")

## Mesh generation.
l1.genMesh(xc.meshDir.I)

## Constraints.
bottomNode= p1.getNode()
topNode= p2.getNode()
#modelSpace.fixNode('000_FF0',bottomNode.tag)
#modelSpace.fixNode('000_FFF',topNode.tag)
modelSpace.fixNode('000_FF0',bottomNode.tag)
modelSpace.fixNode('00F_FFF',topNode.tag)

## Loads
lp0= modelSpace.newLoadPattern(name= 'lp0')
NEd= 1630e3
lp0.newNodalLoad(topNode.tag, xc.Vector([0,0,-NEd,0,0,0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Perform linear buckling analysis.
linearBucklingAnalysis= predefined_solutions.LinearBucklingAnalysis(prb= feProblem, numModes= 4, constraintHandlerType= 'transformation', numberingMethod= 'rcm', convTestType= "norm_disp_incr_conv_test", convergenceTestTol= 1e-8, maxNumIter= 1000, soeType= "band_gen_lin_soe", solverType= "band_gen_lin_lapack_solver", solutionAlgorithmType= 'krylov_newton_soln_algo', eigenSOEType= "spectra_soe", eigenSolverType= "spectra_solver")
linearBucklingAnalysis.setup()
analOk= linearBucklingAnalysis.solve()

# Get results.
eulerBucklingLoadFactor1= linearBucklingAnalysis.analysis.getEigenvalue(1)
eulerBucklingLoadFactor2= linearBucklingAnalysis.analysis.getEigenvalue(2)
eulerBucklingLoadFactor3= linearBucklingAnalysis.analysis.getEigenvalue(3)

xcTotalSet= modelSpace.getTotalSet()

avgLeff= 0.0
avgNbRd= 0.0
# Compute critical axial loads.
for e in xcTotalSet.elements:
    N= e.getN()
    # Critical axial load.
    Ncri= [eulerBucklingLoadFactor1*N, eulerBucklingLoadFactor2*N, eulerBucklingLoadFactor3*N]
    # Effective length.
    section= e.physicalProperties.getVectorMaterials[0]
    EIz= section.sectionProperties.EIz()
    EIy= section.sectionProperties.EIy()
    Leffi= list()
    for Ncr in Ncri:
        Leffz= math.sqrt((EIz*math.pi**2)/abs(Ncr))
        Leffy= math.sqrt((EIy*math.pi**2)/abs(Ncr))
        if(Ncr>0):
            Leffz= -Leffz
            Leffy= -Leffy
        Leffi.append((Leffz, Leffy))
    # Buckling reduction factor and buckling resistance.
    Xi= list()
    NbRdi= list()
    for (Leffz, Leffy) in Leffi:
        Xz= steelShape.getBucklingReductionFactorZ(Leffz)
        Xy= steelShape.getBucklingReductionFactorY(Leffy)
        Xi.append((Xz,Xy))
        NbRdi.append(steelShape.getBucklingResistance(Leffy,Leffz))
    avgLeff+= Leffi[0][0]
    avgNbRd+= NbRdi[0]

sz= len(xcTotalSet.elements)
avgLeff/=sz
ratio1= abs(avgLeff-3.9407414627361965)/3.9407414627361965
avgNbRd/=sz
ratio2= abs(avgNbRd-2306.398883222727e3)/2306.398883222727e3

'''
print('first euler buckling load factor: ', eulerBucklingLoadFactor1)
print('second euler buckling load factor: ', eulerBucklingLoadFactor2)
print('third euler buckling load factor: ', eulerBucklingLoadFactor3)
print('effective length: ', avgLeff, 'm', ratio1)
print('buckling resistance: ', avgNbRd/1e3, 'kN', ratio2)
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
