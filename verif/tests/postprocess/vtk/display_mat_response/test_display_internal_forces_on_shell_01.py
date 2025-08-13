# -*- coding: utf-8 -*-
''' Check the graphical output of the strains on shell elements.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from actions import loads

L= 1.0 # Plate length (m)
B= 0.2 # Plate depth (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 400.0 # Temperature increment (Celsius degrees)
h= 0.2
A= L*h

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat",E,nu,0.0,h)

# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= shellMat.name
elem= seedElemHandler.newElement("ShellMITC4")

# Block topology
#
#  p4 +------------------------------+ p3
#     |                              |
#     |                              |
#  p1 +------------------------------+ p2
#     |                              |
#     |                              |
#     +------------------------------+
#    p6                              p5
#
B_2= B/2.0
p1= modelSpace.newKPoint(0,0,0)
p2= modelSpace.newKPoint(L,0,0)
p3= modelSpace.newKPoint(L,0,B_2)
p4= modelSpace.newKPoint(0,0,B_2)
p5= modelSpace.newKPoint(L,0,-B_2)
p6= modelSpace.newKPoint(0,0,-B_2)
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(p1.tag, p2.tag, p3.tag, p4.tag)
s2= surfaces.newQuadSurfacePts(p1.tag, p2.tag, p5.tag, p6.tag)

# Mesh generation
nDiv= 4
for s in [s1, s2]:
    s.nDivI= 8*nDiv
    s.nDivJ= nDiv
    s.genMesh(xc.meshDir.I)

# Constraints
xcTotalSet= modelSpace.getTotalSet()

for n in xcTotalSet.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.x)<1e-3):
        if(abs(pos.z)<1e-3):
            modelSpace.fixNode('000_FF0',n.tag)
        else:
            modelSpace.fixNode('00F_FF0',n.tag)
        
# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
## Compute the bending moment equivalent to the temperature gradient.
I= 1/12.0*h*B**3
M= alpha*E*I*AT/B
#@ Compute the nodes to distribute the bending moment on.
loadedNodes= list()
for n in xcTotalSet.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.x-L)<1e-3):
        loadedNodes.append(n)
        
lVector= xc.Vector([0, 0, 0, 0, M, 0])
slidingVectorLoad= loads.SlidingVectorLoad(name= 'test', nodes= loadedNodes, pntCoord= [L,0.0,0.0], loadVector= lVector)
slidingVectorLoad.appendLoadToCurrentLoadPattern()

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

#modelSpace.analysis= predefined_solutions.penalty_newton_raphson(feProblem)
result= modelSpace.analyze(calculateNodalReactions= True)

# Find node at the top center.
midEndNod= p2.getNode()

deltaZ= midEndNod.getDisp[2]
deltaZRef= -alpha*AT*L**2/B/2.0 # Theoretical displacement.
err= abs(deltaZ-deltaZRef)/-deltaZRef

'''
print('deltaZ= ', deltaZ)
print('deltaZRef= ', deltaZRef)
print('err= ', err)
'''

# # Output stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
internalForcesOnShells= ['N1', 'N2', 'N12', 'M1', 'M2', 'M12', 'Q1', 'Q2']

# for internalForceLabel in internalForcesOnShells:
#     oh.displayIntForc(itemToDisp= internalForceLabel)

fname= os.path.basename(__file__)
outputFileNames= list()
for internalForceLabel in internalForcesOnShells:
    suffix= '_'+internalForceLabel+'.jpeg'
    outputFileName= '/tmp/'+fname.replace('.py', suffix)
    oh.displayIntForc(itemToDisp= internalForceLabel, fileName= outputFileName)
    outputFileNames.append(outputFileName)

# Check that file exists
testOK= (err<.03)
for fName in outputFileNames:
    fileOK= os.path.isfile(outputFileName)
    testOK= testOK and fileOK
    if(not testOK):
        break

from misc_utils import log_messages as lmsg
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
