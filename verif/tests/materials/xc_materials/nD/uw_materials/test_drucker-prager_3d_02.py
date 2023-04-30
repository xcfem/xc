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
sigY= 5.0
#---failure surface and associativity
rho= 0.398
rhoBar= 0.398
#---isotropic hardening
Kinf= 0.0
Ko= 0.0
delta1= 0.0
#---kinematic hardening
H= 0.0
theta= 1.0
#---tension softening
delta2= 0.0
#---mass density
mDen= 1.7

druckerPragerMaterial= typical_materials.defDruckerPrager3d(preprocessor, "druckerPrager3d",k= k, G= G, sigY= sigY, mRho= rho, mRhoBar= rhoBar, Kinf= Kinf, Ko= Ko, delta1= delta1, H= H, theta= theta, delta2= delta2, mDen= mDen, pAtm= 101)

## Define mesh.
### Define nodes.
n1= modelSpace.newNode(1.0, 0.0, 0.0)
n2= modelSpace.newNode(1.0, 1.0, 0.0)
n3= modelSpace.newNode(0.0, 1.0, 0.0)
n4= modelSpace.newNode(0.0, 0.0, 0.0)
n5= modelSpace.newNode(1.0, 0.0, 1.0)
n6= modelSpace.newNode(1.0, 1.0, 1.0)
n7= modelSpace.newNode(0.0, 1.0, 1.0)
n8= modelSpace.newNode(0.0, 0.0, 1.0)

meshNodes= [n1, n2, n3, n4, n5, n6, n7, n8]

### Define elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= druckerPragerMaterial.name
brick= elements.newElement("Brick",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag,n5.tag,n6.tag,n7.tag,n8.tag]))


### Define boundary conditions.
constraintHandler= modelSpace.constraints
for n in meshNodes[:-1]:
    modelSpace.fixNode('000',n.tag)

### Create loads.
lp1= modelSpace.newLoadPattern('1')
lp1.newNodalLoad(n8.tag, xc.Vector([1e3, 1e3, -1e3]))
modelSpace.addLoadCaseToDomain(lp1.name)


solProc= predefined_solutions.PlainNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-3, convTestType= 'norm_disp_incr_conv_test')
solProc.setup()
solProc.analysis.analyze(1)

disp= n8.getDisp
ratio1= abs(disp.Norm()-0.1485488162005314)/0.1485488162005314

materialTangent= druckerPragerMaterial.getTangent()
elemTangent= brick.getTangentStiff()

'''
print('material tangent: ', materialTangent)
print('element tangent: ', elemTangent)
print('node displacement: ', disp, ' norm: ', disp.Norm())
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# ## Uncomment to display the mesh
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayDispRot('uX')
