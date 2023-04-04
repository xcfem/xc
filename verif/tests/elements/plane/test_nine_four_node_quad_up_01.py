# -*- coding: utf-8 -*-
''' Trivial test of PressureIndependentMultiYieldMaterial material class. Home made test.'''

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
for node in [n1, n2, n3]: # base nodes.
    constraintHandler.newSPConstraint(node.tag,0,0.0) # node 1: Ux fixed.
    constraintHandler.newSPConstraint(node.tag,1,0.0) # node 1: Uy fixed.

## fix horizontal displacement and pore pressure DOF for corner nodes at the top
for node in [n7, n9]: # top corner nodes.
    constraintHandler.newSPConstraint(node.tag, 0, 0.0) # node Ux fixed.
    constraintHandler.newSPConstraint(node.tag, 2, 0.0) # node pore pressure fixed.

# fix horizontal displacement for all remaining contour nodes
for node in [n4, n6]: # lateral nodes.
    constraintHandler.newSPConstraint(node.tag,0,0.0) # node 1: Ux fixed.

# define equalDOF for surface nodes
eqDof= constraintHandler.newEqualDOF(n8.tag,n9.tag,xc.ID([0,1]))
eqDof= constraintHandler.newEqualDOF(n8.tag,n7.tag,xc.ID([0,1]))

# update material stage to zero for elastic behavior
soil.updateMaterialStage(0)

# Define load.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
lts= lPatterns.newTimeSeries("linear_ts","lts") # Load increases linearly with time.
lPatterns.currentTimeSeries= lts.name
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
nl= lp0.newNodalLoad(n8.tag, xc.Vector([0,-1e3]))
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
## Compute solution.
duration= 10
dT= 0.1
numberOfSteps= int(duration/dT)
solProc= predefined_solutions.TransformationNewmarkNewtonRaphson(feProblem, numSteps= numberOfSteps, timeStep= dT, gamma= 0.5, beta= 0.25, printFlag= 0)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

# Check results.
refDy= [-4.956648551494465e-05, -0.00010583656582297943, -0.00016991441363171854, -0.00023903759282672808, -0.00031083261801528807, -0.00038476001560127195, -0.0004593736800487632, -0.0005351220067935865, -0.0006107398922776015, -0.0006872027735304609, -0.0007631535084395648, -0.00083991988987224, -0.000915955858955854, -0.0009928721591903021, -0.0010689029572805181, -0.001145910824798892, -0.0012219044788279933, -0.0012989806778567607, -0.001374927036396747, -0.0014520611785903345, -0.0015279583374452246, -0.0016051446385111684, -0.0016809939018126905, -0.0017582281359635218, -0.001834032139476745, -0.001911310520683364, -0.0019870725262350754, -0.0020643913041218922, -0.002140114927380553, -0.0022174702485344656, -0.002293159346803238, -0.002370547214978324, -0.002446205833765803, -0.0025236221072242257, -0.0025992544481346106, -0.002676694851181414, -0.0027523052473012884, -0.0028297653874528525, -0.0029053582811719553, -0.002982833668725593, -0.0030584135901767875, -0.00313589965892176, -0.003211471204430202, -0.0032889633329703035, -0.0033645311433492748, -0.003442024676776721, -0.0035175934154741593, -0.0035950836872338625, -0.0036706580183957062, -0.0037481403722155666, -0.0038237249387552048, -0.0039011947505315924, -0.003976794152303074, -0.004054246851836105, -0.004129865624011387, -0.004207296716487158, -0.004282939308237856, -0.004360344395356776, -0.004436015148939856, -0.00451338994959211, -0.004589093079937206, -0.004666433450328655, -0.004742173025222397, -0.0048194749783568224, -0.004895254899316748, -0.004972514623743409, -0.005048338607670823, -0.005125552485409758, -0.005201424047107203, -0.005278588670668561, -0.005354511106303549, -0.005431623294721503, -0.00550759966631366, -0.005584656480120092, -0.005660689601124118, -0.005737688356192217, -0.00581378077824388, -0.0058907190584370946, -0.0059668730593240415, -0.006043748727891489, -0.006119966300804923, -0.006196777510470128, -0.006273060354587372, -0.006349805556283424, -0.006426155068725199, -0.006502833018935716, -0.006579250288135474, -0.006655860054807277, -0.006732345855323342, -0.00680888682232348, -0.00688544161111803, -0.0069619134812145345, -0.007038537395416513, -0.007114940191769243, -0.007191633047931433, -0.007267967114086294, -0.007344728408939749, -0.007420994407326574, -0.0074978233200285885, -0.007574022228969976]

err= 0.0
for dxi, dyi, rdyi in zip(dx,dy,refDy):
    err+= dxi**2
    err+= (dyi-rdyi)**2

err= math.sqrt(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(err)<1e-11):
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
