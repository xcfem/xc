# -*- coding: utf-8 -*-
''' Trivial test of the behaviour of PressureIndependentMultiYieldMaterial material class. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials import typical_materials
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Define finite element problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define material.
satDensity= 2.3 # saturated mass density (Mg/m3)
H2ODensity= 1.0 # fluid mass density (Mg/m3)

shear= 2.5e4 # shear modulus (kPa)
bulk= 6.2e5 # bulk modulus (kPa)
uBulk= 2.2e5 # undrained bulk modulus (kPa)


cohesion = 45.0 # cohesion (kPa)
phi= 0.0 # friction angle (degrees)

gammaPeak= 0.1 # peak shear strain
refPress= -80.0 # reference pressure (compression, kPa)

pressCoeff = 0.0 # pressure dependency coefficient

numSurf= 22 # number of yield surfaces
soil= typical_materials.defPressureIndependentMultiYieldMaterial(preprocessor, name= '1', nd= 2, rho= satDensity, refShearModul= shear, refBulkModul= bulk, cohesi= cohesion, peakShearStra= gammaPeak, frictionAng= phi, refPress= refPress, pressDependCoe= pressCoeff, numberOfYieldSurf= numSurf, gredu= [])


# Define mesh
## Define nodes.

#         8
# 7 +-----+-----+ 9
#   |           |
#   |     5     |
# 4 +     +     +6
#   |           |
#   |           |
#   +-----+-----+
#   1     2     3
nodeHandler= preprocessor.getNodeHandler
nodeHandler.dimEspace= 2 #two coord. for each node (x,y).

### Define corner nodes.
nodeHandler.numDOFs= 3 #three DOF for each node (Ux,Uy, pore pressure).

n1= nodeHandler.newNodeXY(0.0,0.0)
n3= nodeHandler.newNodeXY(1.0,0.0)
n7= nodeHandler.newNodeXY(0.0,1.0)
n9= nodeHandler.newNodeXY(1.0,1.0)

### Define interior nodes
nodeHandler.numDOFs= 2 #two DOF for each node (Ux,Uy).
n2= nodeHandler.newNodeXY(0.5,0.0)
n4= nodeHandler.newNodeXY(0.0,0.5)
n5= nodeHandler.newNodeXY(0.5,0.5)
n6= nodeHandler.newNodeXY(1.0,0.5)
n8= nodeHandler.newNodeXY(0.5,1.0)

## Define element.
# 
#         p
# l +-----+-----+ k
#   |           |
#   |     r     |
# q +     +     + n   Order of the nodes in the element.
#   |           |
#   |           |
#   +-----+-----+
#   i     m     j
#
elementHandler= preprocessor.getElementHandler
elementHandler.defaultMaterial= soil.name
newElem= elementHandler.newElement("NineFourNodeQuadUP",xc.ID([n1.tag, n3.tag, n9.tag, n7.tag, n2.tag, n6.tag, n8.tag, n4.tag, n5.tag]))


# Define constraints.
constraintHandler= preprocessor.getBoundaryCondHandler
## boundary conditions for base nodes.
for node in [n1, n2, n3, n4, n6, n7, n8, n9]: # base nodes.
    constraintHandler.newSPConstraint(node.tag,0,0.0) # node 1: Ux fixed.
    constraintHandler.newSPConstraint(node.tag,1,0.0) # node 1: Uy fixed.
for node in [n1, n3, n7, n9]: # corner nodes.
    constraintHandler.newSPConstraint(node.tag, 2, 0.0) #  # node pore pressure fixed.

# update material stage to zero for elastic behavior
soil.updateMaterialStage(0)

# Define load.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
cts= lPatterns.newTimeSeries("linear_ts","cts") # Load increases linearly with time.
lPatterns.currentTimeSeries= cts.name
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
nl= lp0.newNodalLoad(n5.tag, xc.Vector([0,-1e3]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Dynamic analysis.
## Define recorders
ti= list()
dx= list()
dy= list()
recDFree= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recDFree.setNodes(xc.ID([n5.tag]))
recDFree.callbackRecord= "ti.append(self.getDomain.getTimeTracker.getCurrentTime); dx.append(self.getDisp[0]); dy.append(self.getDisp[1])"
## 
duration= 10
dT= 0.1
numberOfSteps= int(duration/dT)
printFlag= 0
solProc= predefined_solutions.TransformationNewmarkNewtonRaphson(feProblem, numSteps= numberOfSteps, timeStep= dT, printFlag= printFlag)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

# Check results.
refDy=  [-5.182036736049558e-05, -0.00010366884377625737, -0.000155461117328754, -0.00020733765706625095, -0.000259101913005736, -0.00031100640944985484, -0.00036274278475373583, -0.00041467507063907584, -0.0004663837627700169, -0.0005183436105437612, -0.0005700248770212996, -0.000622011999336877, -0.0006736661571787522, -0.000725680207519216, -0.0007773076325536193, -0.0008293482059833951, -0.0008809493320336338, -0.000933015966077001, -0.0009845912840203475, -0.0010366834596647497, -0.0010882335163675142, -0.0011403506591895253, -0.0011918760563206648, -0.001244017537732163, -0.0012955189304579978, -0.0013476840690698467, -0.0013991621646327196, -0.001451350227733001, -0.001502805783916961, -0.001555015989060543, -0.0016064498125473799, -0.001658681329253384, -0.0017100942738725855, -0.0017623462254260612, -0.001813739190302482, -0.0018660106556563833, -0.001917384583259654, -0.0019696745990329936, -0.002021030473132891, -0.0020733380357007307, -0.0021246768792329523, -0.002177000946903707, -0.0022283238197506787, -0.002280663315025998, -0.0023319713117175267, -0.0023843251236298564, -0.0024356193709686165, -0.002487986357491376, -0.0025392680121083836, -0.002591647002633516, -0.0026429172484788927, -0.0026953070463564257, -0.002746567092130893, -0.0027989664772649927, -0.0028502175537976743, -0.0029026252852935703, -0.0029538686428717854, -0.003006283461727806, -0.003057520367384653, -0.0031099409992235527, -0.0031611727339891605, -0.0032135978918227987, -0.003264825747945215, -0.003317254134966597, -0.003368479413108335, -0.0034209097255049467, -0.003472133731921288, -0.003524564661703639, -0.0035757887054087903, -0.003628218943248009, -0.0036794443331752812, -0.0037318725712436255, -0.0037831006134057842, -0.003835525548213887, -0.0038867575428698487, -0.00393917787809455, -0.0039904151169285465, -0.004042829566225193, -0.004094073329544545, -0.0041464806193376265, -0.0041977321732952055, -0.004250131045541298, -0.004301391639388678, -0.004353780854305685, -0.004405051717682982, -0.004457430056439769, -0.004508712396708008, -0.004561078664068571, -0.0046123736636903955, -0.004664726690606855, -0.004716035504581253, -0.004768374150730025, -0.004819697904086627, -0.004872021060342285, -0.004923360845700689, -0.004975667436542138, -0.0050270243117415386, -0.00507931329758529, -0.005130688283389562, -0.0051829586628450485]

err= 0.0
for dxi, dyi, rdyi in zip(dx,dy,refDy):
    err+= dxi**2
    err+= (dyi-rdyi)**2

err= math.sqrt(err)

'''
print('dx= ', dx)
print('dy= ', dy)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(err)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Display results.
# import matplotlib.pyplot as plt
# plt.plot(ti, dx, '-')
# plt.grid()
# plt.show()
# plt.plot(ti, dy, '-')
# plt.grid()
# plt.show()
