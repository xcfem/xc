# -*- coding: utf-8 -*-
''' Testing the Drucker-Prager model --> conventional triaxial compression test.

Based on the example in: https://opensees.github.io/OpenSeesDocumentation/user/manual/material/ndMaterials/DruckerPrager.html
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2023, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc
from model import predefined_spaces
from materials import typical_materials
from model.boundary_cond import opensees_style
from solution import predefined_solutions

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

## Problem type
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)


## Define material.

#---bulk modulus
k= 27777.78 # soil bulk modulus
#---shear modulus
G= 9259.26 # soil shear modulus
#---yield stress
sigY= 5.0 # Drucker-Prager yield stress.
#---failure surface and associativity
rho= 0.398 # Drucker-Prager failure surface and associativity volumetric term.
rhoBar= 0.398 # related to dilation? controls evolution of plastic volume change, 0 ≤ rhoBar ≤ rho
#---isotropic hardening
Kinf= 0.0 # nonlinear isotropic strain hardening parameter, $Kinf ≥ 0
Ko= 0.0 # nonlinear isotropic strain hardening parameter, $Ko ≥ 0
delta1= 0.0 # nonlinear isotropic strain hardening parameter, delta1 ≥ 0.
#---kinematic hardening
H= 0.0 # linear strain hardening parameter, $H ≥ 0
theta= 1.0 # controls relative proportions of isotropic and kinematic hardening, 0 ≤ $theta ≤ 1
#---tension softening
delta2= 0.0 # nonlinear isotropic strain hardening parameter, $delta1 ≥ 0
#---mass density
mDen= 1.7

druckerPragerMaterial= typical_materials.defDruckerPrager3d(preprocessor, "druckerPrager3d",k= k, G= G, sigY= sigY, mRho= rho, mRhoBar= rhoBar, Kinf= Kinf, Ko= Ko, delta1= delta1, H= H, theta= theta, delta2= delta2, mDen= mDen, pAtm= 101)

## Define mesh.
### Define nodes.
n1= nodes.newNodeXYZ(1.0, 0.0, 0.0)
n2= nodes.newNodeXYZ(1.0, 1.0, 0.0)
n3= nodes.newNodeXYZ(0.0, 1.0, 0.0)
n4= nodes.newNodeXYZ(0.0, 0.0, 0.0)
n5= nodes.newNodeXYZ(1.0, 0.0, 1.0)
n6= nodes.newNodeXYZ(1.0, 1.0, 1.0)
n7= nodes.newNodeXYZ(0.0, 1.0, 1.0)
n8= nodes.newNodeXYZ(0.0, 0.0, 1.0)

### Define elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= druckerPragerMaterial.name
brick= elements.newElement("Brick",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag,n5.tag,n6.tag,n7.tag,n8.tag]))

### Define boundary conditions.
constraintHandler= modelSpace.constraints
opensees_style.fix(constraintHandler= constraintHandler, idNode= n1.tag, dofs=[0,1,1])
opensees_style.fix(constraintHandler= constraintHandler, idNode= n2.tag, dofs=[0,0,1])
opensees_style.fix(constraintHandler= constraintHandler, idNode= n3.tag, dofs=[1,0,1])
opensees_style.fix(constraintHandler= constraintHandler, idNode= n4.tag, dofs=[1,1,1])
opensees_style.fix(constraintHandler= constraintHandler, idNode= n5.tag, dofs=[0,1,0])
opensees_style.fix(constraintHandler= constraintHandler, idNode= n6.tag, dofs=[0,0,0])
opensees_style.fix(constraintHandler= constraintHandler, idNode= n7.tag, dofs=[1,0,0])
opensees_style.fix(constraintHandler= constraintHandler, idNode= n8.tag, dofs=[1,1,0])


### Create the recorders.
step= 0.1
ti= list()

#### Node recorders.
displacements= list() # Node displacements.
recDFree= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
#recDFree.setNodes(xc.ID([n1.tag, n2.tag, n3.tag, n4.tag, n5.tag, n6.tag, n7.tag, n8.tag]))
recDFree.setNodes(xc.ID([n6.tag]))
recDFree.callbackRecord= "ti.append(self.getDomain.getTimeTracker.getCurrentTime); displacements.append(self.getDisp)"
recDFree.deltaT= step

reactions= list() # list of reactions.
recReactions= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recReactions.setNodes(xc.ID([n4.tag]))
recReactions.callbackRecord= "reactions.append([self.getDomain.getTimeTracker.getCurrentTime,self.getReaction])"
recReactions.callbackSetup= "self.getDomain.calculateNodalReactions(True,1e-4)"
recReactions.deltaT= step

#### Element recorders.
stress= list()
strain= list()
state= list()
stage= list()
elemRecorder= feProblem.getDomain.newRecorder("element_prop_recorder",None)
elemRecorder.setElements(xc.ID([brick.tag]))
elemRecorder.callbackRecord= "stress.append(self.physicalProperties.getVectorMaterials.getValues('stress', False)); strain.append(self.physicalProperties.getVectorMaterials.getValues('strain', False)); state.append(self.physicalProperties.getVectorMaterials.getValues('state', False)); stage.append(self.physicalProperties.getVectorMaterials.getValues('stage', False));"
elemRecorder.callbackRestart= 'print("Restart method called.")'
elemRecorder.deltaT= step

### Create loads.
p= 10.0
pNode= -p/4.0

loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#### Create path time series TS1.
timeValues= [0, 10, 100]
ts1= lPatterns.newTimeSeries("path_time_ts","ts1")
ts1.path= xc.Vector([0, 1, 1])
ts1.time= xc.Vector(timeValues)
lPatterns.currentTimeSeries= ts1.name
#### Create load pattern LP1.
lp1= lPatterns.newLoadPattern('default', '1')
lp1.newNodalLoad(n1.tag, xc.Vector([pNode, 0.0, 0.0]))
lp1.newNodalLoad(n2.tag, xc.Vector([pNode, pNode, 0.0]))
lp1.newNodalLoad(n3.tag, xc.Vector([0.0, pNode, 0.0]))
lp1.newNodalLoad(n5.tag, xc.Vector([pNode,0.0, 0.0]))
lp1.newNodalLoad(n6.tag, xc.Vector([pNode, pNode, 0.0]))
lp1.newNodalLoad(n7.tag, xc.Vector([0.0, pNode, 0.0]))
lPatterns.addToDomain(lp1.name)

#### Create path time series TS2.
ts2= lPatterns.newTimeSeries("path_time_ts","ts2")
ts2.path= xc.Vector([0, 1, 5])
ts2.time= xc.Vector(timeValues)
lPatterns.currentTimeSeries= ts2.name
#### Create load pattern LP2.
lp2= lPatterns.newLoadPattern('default', '2')
lp2.newNodalLoad(n5.tag, xc.Vector([0.0, 0.0, pNode]))
lp2.newNodalLoad(n6.tag, xc.Vector([0.0, 0.0, pNode]))
lp2.newNodalLoad(n7.tag, xc.Vector([0.0, 0.0, pNode]))
lp2.newNodalLoad(n8.tag, xc.Vector([0.0, 0.0, pNode]))
lPatterns.addToDomain(lp2.name)

# Static analysis.
solProc= predefined_solutions.TransformationNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-5, convTestType= 'norm_disp_incr_conv_test', maxNumIter= 1000, printFlag= 0)
solProc.setup()
solProc.integrator.dLambda1= 0.1

#solProc.analysis.analyze(1000)
solProc.analysis.analyze(950)

# Check last reaction
R= reactions[-1][1][2]
ratio1= abs(R-11.944444444444322)/11.944444444444322
uz= displacements[-1][2]
ratio2= abs(uz+0.0016311109806222128)/0.0016311109806222128

'''
print(R)
print(ratio1)
print(uz)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Display results
# ## Displacements.
# n6uz= list()
# for disp in displacements:
#     n6uz.append(disp[2])
# import matplotlib.pyplot as plt
# plt.xlabel('t')
# plt.ylabel('$U_z$')
# plt.plot(ti, n6uz)
# plt.title("Vertical displacement.")
# plt.show()
# ## Reactions.
# n4R= list()
# for r in reactions:
#     n4R.append(r[1][2])
# plt.xlabel('t')
# plt.ylabel('$R_z$')
# plt.plot(ti, n4R)
# plt.title("Vertical reaction.")
# plt.show()


# ## Stresses.
# sgz= list()
# for sgGaussPoints in stress:
#     avgSgZ= (sgGaussPoints(0,2)+sgGaussPoints(1,2)+sgGaussPoints(2,2)+sgGaussPoints(3,2)+sgGaussPoints(4,2)+sgGaussPoints(5,2)+sgGaussPoints(6,2)+sgGaussPoints(7,2))/8
#     sgz.append(avgSgZ)
# plt.xlabel('t')
# plt.ylabel('$\sigma_z$')
# plt.plot(ti, sgz)
# plt.title("Average vertical stress.")
# plt.show()


## Uncomment to display the mesh
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLoads()
