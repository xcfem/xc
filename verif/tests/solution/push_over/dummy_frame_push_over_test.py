# -*- coding: utf-8 -*-
''' Home made test.  Horizontal cantilever under tension load at its end..'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg

# Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties (IPE-80)
A= 7.64e-4 # Cross section area (m2)
Iz= 8.49e-8 # Cross section moment of inertia (m4)

# Geometry
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

# Define FE Problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0,L)

## Coordinate transformation.
pDelta= modelSpace.newPDeltaCrdTransf("pDelta")

## Define material.
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)

# Define element.
elements= preprocessor.getElementHandler
elements.defaultTransformation= pDelta.name
elements.defaultMaterial= section.name
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))

# Define constraints.
modelSpace.fixNode000(n1.tag)

# Define gravity loads
P= 10.0e3  # 10% of axial capacity of columns
# Create a Plain load pattern with a Linear TimeSeries
glp= modelSpace.newLoadPattern(name= 'glp', setCurrent= True)
# Create nodal loads at nodes 3 & 4
glp.newNodalLoad(n1.tag, xc.Vector([0,-P,0]))
modelSpace.addLoadCaseToDomain(glp.name)

# Solve
solProc= predefined_solutions.TransformationNewtonRaphsonBandGen(feProblem, maxNumIter= 10, convergenceTestTol= 1e-12, printFlag= 0, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test')
solProc.setup()
analysis= solProc.getAnalysis()
# perform the gravity load analysis.
analysis.analyze(1)

# Compute reference values.
totalStiffness= E*A
epsilon= P/totalStiffness
vertDisplacement= epsilon*L

# Get results.
u2= n2.getDisp[1]
ratio1= abs(u2 + vertDisplacement)


# Dummy push-over analysis.

# Set the gravity loads to be constant & reset the time in the domain
modelSpace.setLoadConstant(t= 0.0)

# Set some parameters
H = 10.0e3  # Reference lateral load

# Create a Plain load pattern with a Linear TimeSeries
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
# Set lateral load pattern with a Linear TimeSeries
hLoad= modelSpace.newLoadPattern(name= 'hLoad', setCurrent= True)

# Create nodal load at node 2
hLoad.newNodalLoad(n2.tag, xc.Vector([H, 0, 0]))

modelSpace.addLoadCaseToDomain(hLoad.name)

# Set some parameters
dU = 0.1  # Displacement increment

solProc.displacementControlIntegratorSetup(node= n2, dof= 0, increment= dU, numIter= 1, dUmin= dU, dUmax= dU)

# Finally perform the analysis

# Set some parameters
maxU = 15.0  # Max displacement
# Set convergence test.
solProc.convTestType= 'norm_disp_incr_conv_test'
solProc.convergenceTestTol= 1e-12
solProc.maxNumIter= 1000
solProc.printFlag= 0
solProc.convergenceTestSetup()
# Set solution algorithm.
solProc.solutionAlgorithmType= 'modified_newton_soln_algo'
solProc.solutionAlgorithmSetup()

ok= 0
displacements= list()
baseShear= list()
currentDisp= 0.0
analysis= solProc.getAnalysis()
while ok == 0 and currentDisp < maxU:

    ok= analysis.analyze(1)

    # if the analysis fails try initial tangent iteration
    if ok != 0:
        print("modified newton failed at disp: ", currentDisp)
        break
    else:
        displacements.append(currentDisp)
        # Compute reactions
        modelSpace.calculateNodalReactions()
        # # Get base shear
        # Rx= n1.getReaction[0]+n2.getReaction[0]
        # baseShear.append(-Rx)

    currentDisp= n2.getDisp[0]
    
testOK= (ratio1<5e-3)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
