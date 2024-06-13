# -*- coding: utf-8 -*-
''' Check linear buckling analysis results for many load cases.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import xc
import math
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
from misc_utils import log_messages as lmsg
# from postprocess import output_handler

L= 4.0 # Column length in meters
b= 0.2 # Cross section width in meters
h= 0.2 # Cross section depth in meters
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Moment of inertia in m4
E=30E9 # Elastic modulus en N/m2
P= -10 # Vertical load over the column.

numDiv= 6

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

# Geometric transformation(s)
lin= modelSpace.newPDeltaCrdTransf("lin")


# Seed element definition
modelSpace.setDefaultMaterial(scc)
modelSpace.setDefaultCoordTransf(lin)
beam2d= modelSpace.newSeedElement("ElasticBeam2d")
beam2d.h= h
beam2d.rho= 0.0

pt1= modelSpace.newKPoint(0,0)
pt2= modelSpace.newKPoint(0,L)

l= modelSpace.newLine(pt1,pt2)
l.nDiv= numDiv


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)
n1= pt1.getNode() # Back end node.
n2= pt2.getNode() # Front end node.
# Constraints
modelSpace.fixNode('00', n1.tag) # pinned node.
modelSpace.fixNode('0F', n2.tag) # horizontal displacement restrained.

# Test element.
testElement= l.elements[int(numDiv/2)]

# Loads definition
loadPatterns= list()
loads= list()
for i in range(0, 10):
    loadPatternName= str(i)
    newLP= modelSpace.newLoadPattern(name= loadPatternName)
    load= (i+1)*P
    newLP.newNodalLoad(n2.tag,xc.Vector([0,load,0]))
    loadPatterns.append(newLP)
    loads.append(load)
    
# Solution procedure
linearBucklingAnalysis= predefined_solutions.LinearBucklingAnalysis(prb= feProblem, numModes= 2, constraintHandlerType= 'transformation', numberingMethod= 'rcm', convTestType= "norm_disp_incr_conv_test", convergenceTestTol= 1e-8, maxNumIter= 1000, soeType= "band_gen_lin_soe", solverType= "band_gen_lin_lapack_solver", solutionAlgorithmType= 'krylov_newton_soln_algo', eigenSOEType= "spectra_soe", eigenSolverType= "spectra_solver")
linearBucklingAnalysis.setup()

# Perform linear buckling analysis for each load case.
eigenvalues= list()
axialForces= list()
for lp in loadPatterns:
    preprocessor.resetLoadCase()
    preprocessor.getDomain.revertToStart()
    modelSpace.addLoadCaseToDomain(lp.name)
    analOk= linearBucklingAnalysis.solve()
    if(analOk!=0):
        lmsg.error("can't solve.")
        exit(1)
    eig1= linearBucklingAnalysis.analysis.getEigenvalue(1)
    eigenvalues.append(eig1)
    N= testElement.getN()
    axialForces.append(N)
    modelSpace.removeLoadCaseFromDomain(lp.name)

load0= loads[0]
eigenvalue0= eigenvalues[0]
N0= axialForces[0]
loadRatios= list()
eigenvalueRatios= list()
axialForcesRatios= list()
for load, eigenvalue, N in zip(loads, eigenvalues, axialForces):
    eigenvalueRatios.append(eigenvalue0/eigenvalue)
    loadRatios.append(load/load0)
    axialForcesRatios.append(load/load0)
    

err= 0.0
for ev, lr in zip(eigenvalueRatios, loadRatios):
    err+= (ev-lr)**2
for ev, afr in zip(eigenvalueRatios, axialForcesRatios):
    err+= (ev-afr)**2

err= math.sqrt(err)

'''
print(eigenvalueRatios)
print(loadRatios)
print(axialForcesRatios)
print(err)
'''

import os
fname= os.path.basename(__file__)
if (err<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
