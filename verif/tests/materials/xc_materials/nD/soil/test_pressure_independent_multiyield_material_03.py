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
newElem.physicalProperties.thickness= 1.0
newElem.bulk= bulk
mPermH= 5.0e-5      
newElem.horizontalPermeability= mPermH/9.81/H2ODensity
mPermV= 5.0e-5      
newElem.verticalPermeability= mPermV/9.81/H2ODensity
newElem.fluidRho= H2ODensity
newElem.bodyForces= xc.BodyForces2D(0.0,-9.81)


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
## Run the analysis.
duration= 10
dT= 0.1
numberOfSteps= int(duration/dT)
printFlag= 0
solProc= predefined_solutions.TransformationNewmarkNewtonRaphson(feProblem, numSteps= numberOfSteps, timeStep= dT, printFlag= printFlag)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

# Check results.
refDy= [-5.7016913799406075e-05, -0.00010886820899122995, -0.0001606548465205983, -0.00021253983647112712, -0.00026429283259134755, -0.0003162113923532712, -0.0003679309084692092, -0.00041988284026765776, -0.00047156911041690763, -0.0005235541440785123, -0.0005752074744236672, -0.0006272252679626829, -0.0006788460361271354, -0.0007308961764873579, -0.0007824848307360693, -0.000834566834686933, -0.0008861238929539519, -0.0009382372081388696, -0.0009897632569037065, -0.0010419072630383703, -0.0010934029560536673, -0.0011455769662717164, -0.0011970430231449756, -0.0012492462854881074, -0.0013006834901205437, -0.0013529151891698422, -0.001404324388055764, -0.0014565836467006967, -0.0015079657470910884, -0.001560251628432341, -0.0016116075963666394, -0.0016639191057486616, -0.0017152499639589984, -0.0017675860511278433, -0.0018188928768202886, -0.001871252438202072, -0.0019225363607197087, -0.0019749182418147427, -0.0020261804401876275, -0.002078583438075033, -0.0021298251384623624, -0.00218224800440973, -0.0022334704774397726, -0.0022859119196122065, -0.002337116477625753, -0.002389575163888425, -0.0024407631580917475, -0.0024932377188998786, -0.0025444105364333777, -0.002596899567803376, -0.0026480586287322676, -0.0027005606952875847, -0.0027517074495211563, -0.002804221087606241, -0.002855357011752372, -0.002907880732607979, -0.002959007326769728, -0.0030115396197626875, -0.003062658404283921, -0.003115197740184357, -0.003166310252351455, -0.0032188550866503562, -0.003269962877357171, -0.003322511653617101, -0.003373616284000389, -0.00342616743723208, -0.003477270475284719, -0.0035298224353422134, -0.003580925452511543, -0.0036334766474985267, -0.0036845812152771886, -0.003737130074957119, -0.003788237761473807, -0.003840782720676443, -0.003891895087293949, -0.003944434589310883, -0.003995553187238821, -0.004048085687200661, -0.00409921205413021, -0.004151736022358077, -0.004202871679126071, -0.0042553856044501345, -0.0043065320517396925, -0.004359034444777562, -0.004410193159862456, -0.004462682556250299, -0.004513854989790106, -0.004566329953359489, -0.004617517526252473, -0.004669976652146037, -0.004721180752446605, -0.004773622670165824, -0.004824844650073228, -0.004877268026451602, -0.0049285091993764305, -0.004980912741471719, -0.005032174379186534, -0.0050845568370857025, -0.005135840166966015, -0.005188200336496846]

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

# Display results.
# import matplotlib.pyplot as plt
# plt.plot(ti, dx, '-')
# plt.grid()
# plt.show()
# plt.plot(ti, dy, '-')
# plt.grid()
# plt.show()
