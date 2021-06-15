# -*- coding: utf-8 -*-
'''Verification test based on the paper: "Finite Element Modeling of Gusset Plate Failure 
   Using Opensees" by Andrew J. Walker
   section 3.7 "Simple Tension Verification", page 54 and following.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2020, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Data
# Units
in2m=0.0254       # inches to m
ksi2Pa=6.89476e6  # ksi to Pa
kip2N=4448.2216   # kips to N

width=1*in2m   # Rod width (m)
thickness=1*in2m   # Rod thickness (m)
L= 50*in2m # Length of the rod (m)

E= 29000*ksi2Pa # Young’s modulus (Pa)
nu= 0.3 # Poisson’s ratio
sg_yield= 60*ksi2Pa # Allowable stress: yield stress of steel (Pa)
alpha=0.05  # strain hardening ratio. In the paper, the results obtained match
            # a value of the hardening radio alpha=0.01 (there must be an error in
            # page 55 where it's written "five percent strain hardening parameter 
            # is taken", the stress-strain slope in figure 3.12 corresponds
            # approximately to 0.01E

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)


# Material definition
j2plate= typical_materials.defJ2PlateFibre(preprocessor, "j2plate",E,nu,fy= sg_yield, alpha=alpha,rho= 7850.0)

plateFiber= typical_materials.defMembranePlateFiberSection(preprocessor, name= "plateFiber", h= thickness, nDMaterial= j2plate);

# Problem geometry
# The “bottom-up method” generates the geometry of the system
# going  from  key  points (0D) to lines (1D) and areas (2D)
# up to volumes (3D).

p1= modelSpace.newKPoint(0.0,width/2)
p2= modelSpace.newKPoint(L,width/2)
p3= modelSpace.newKPoint(L,-width/2)
p4= modelSpace.newKPoint(0.0,-width/2.0)

s= modelSpace.newSurface([p1,p2,p3,p4])
s.setElemSizeIJ(L/70,width/10)
#s.nDivI= 50 # Number of divisions on p1->p2 and p3->p4
#s.nDivJ= 10 # Number of divisions on p2->p3 and p4->p1

# Meshing

## Define template element
## Before we can create a mesh for our model, we need to choose which
## type of finite element we will want to use for our mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= plateFiber.name
quad= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

## Generate mesh
s.genMesh(xc.meshDir.I)

xcTotalSet= modelSpace.getTotalSet()
numElementsTeor= s.nDivI*s.nDivJ
numElements= xcTotalSet.getNumElements
numNodesTeor= (s.nDivI+1)*(s.nDivJ+1)
numNodes= xcTotalSet.getNumNodes

## Boundary conditions

### Get the line between p4 and p1
l14= modelSpace.getLineWithEndPoints(p1,p4)
### Fix nodes in 1->4 line.
for n in l14.nodes:
    modelSpace.fixNode000_FFF(n.tag)

## Loads
l23= modelSpace.getLineWithEndPoints(p2,p3)
nnod=len(l23.nodes)
maxLoad=100*kip2N
### Create load pattern.
lp0= modelSpace.newLoadPattern('0')
### Distribute the load over nodes in line 2-3
for n in l23.nodes:
    lp0.newNodalLoad(n.tag,xc.Vector([maxLoad/nnod,0,0,0,0,0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution

modelSpace.analysis= predefined_solutions.plain_krylov_newton(modelSpace.preprocessor.getProblem,mxNumIter=300, convergenceTestTol=1e-12,printFlag=0)

areaSect=width**2
Fdata=list() ; sigma_Pa=list() ; eps=list() ; xDisp_m=list()
#lstGamma=[0.1*(i+1) for i in range(5)]+[0.6+(i*0.025) for i in range(6)]
lstGamma=[0.1,0.6,0.7]
for gamma in lstGamma:
    lp0.gammaF= gamma
    analOK= modelSpace.analyze(calculateNodalReactions= True, reactionCheckTolerance= 1e-6)
    dispX=0
    for n in l23.nodes:
        dispX+=n.getDisp[0]
    dispX=dispX/nnod
    sigma_Pa.append(maxLoad*lp0.gammaF/areaSect)
    eps.append(dispX/L)
    xDisp_m.append(dispX)
    F= round(maxLoad*lp0.gammaF,1)
    Fdata.append(round(maxLoad*lp0.gammaF,1))
                 
# Slope in the plastic branch
target_slp=alpha*E
calc_slp= (sigma_Pa[-1]-sigma_Pa[-2])/(eps[-1]-eps[-2])
ratio1=(target_slp-calc_slp)/target_slp
                 
target_sg=[68947572.6951454, 413685436.17087233, 482633008.8660177]
ratio2=sum(target_sg)-sum(sigma_Pa)
target_eps=[0.00034471272829441393, 0.0020776358773107843, 0.008936473450664928]
ratio3=sum(target_eps)-sum(eps)
                 
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<6e-3 and ratio2<1e-6 and ratio3<1e-6): # Good results :)
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

'''
# Graphic stuff
import matplotlib.pyplot as plt
plt.plot(eps,sigma_Pa)
plt.xlabel('Strain')
plt.ylabel('Stress (Pa)')
plt.show()

plt.plot(xDisp_m,Fdata)
plt.xlabel('Displacement (m)')
plt.ylabel('Load (N)')
plt.show()
'''
