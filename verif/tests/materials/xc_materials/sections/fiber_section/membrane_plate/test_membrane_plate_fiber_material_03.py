# -*- coding: utf-8 -*-
'''Verification test based on the paper: "Exercise 13-Point Bending 
Using the GUI and the Bottom-up-Method" Dr.-Ing. Ulrich Simon. Ulm University.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
#from postprocess import output_handler

# Data
F= 5e3 # Applied (full!) force (N)
L= 2 # Length of the(full!)beam (m)
h= 60e-3 # Height of the beam cross section (m)
t= 20e-3 # Thicknessof the beam cross section (m)
E= 210e9 # Young’s modulus (Pa)
nu= 0.3 # Poisson’s ratio
sg_yield= 235e6 # Allowable stress: yield stress of steel (Pa)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)


# Material definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",E,nu,rho= 7850.0)

plateFiber= typical_materials.defMembranePlateFiberSection(preprocessor, name= "plateFiber", h= t, nDMaterial= elast3d);

# Problem geometry
# The “bottom-up method” generates the geometry of the system
# going  from  key  points (0D) to lines (1D) and areas (2D)
# up to volumes (3D).

p1= modelSpace.newKPoint(0.0,h/2.0)
p2= modelSpace.newKPoint(L/2.0,h/2.0)
p3= modelSpace.newKPoint(L/2.0,-h/2.0)
p4= modelSpace.newKPoint(0.0,-h/2.0)

s= modelSpace.newSurface([p1,p2,p3,p4])
s.setElemSizeIJ(1.5*10e-3,10e-3)
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
    modelSpace.fixNode000_F00(n.tag)

## Loads

### Create load pattern.
lp0= modelSpace.newLoadPattern('0')
### Put the load at point 3.
load= F/2.0 # Half! force
n3= p3.getNode()
lp0.newNodalLoad(n3.tag,xc.Vector([0.0,load,0.0,0.0,0.0,0.0]))
modelSpace.addLoadCaseToDomain(lp0.name)

## Solve
analOK= modelSpace.analyze(calculateNodalReactions= True)

## Tests

### Displacement.
n3= p3.getNode()
uYMax= n3.getDisp[1]
I= 1/12.0*t*h*h*h
uYMaxTeor= F*L*L*L/48.0/E/I
ratio1= abs(uYMax-uYMaxTeor)/uYMaxTeor

### Stress.
Mmax= F/2.0*L/2.0
sg_max_theor= -Mmax/I*h/2.0
n1= p1.getNode() # Node at point 1
e= n1.getConnectedElements()[0] # element connected to n1.
stressAtNodes= e.getValuesAtNodes('forces', False)
stressAtNodes[:] = [x*(1.0/t) for x in stressAtNodes]
sg11Min= 1e12
worstCase= None
for s in stressAtNodes:
    if(s[0]<sg11Min): # greater compression
        sg11Min= s[0]
        worstCase= s
#### PlateFiberMaterial stress order= 11, 22, 12, 23, 31, (33)
sg11= worstCase[0]; sg22= worstCase[1]; sg12= worstCase[2];
sg23= worstCase[3]; sg31= worstCase[4];
ratio2= abs(sg_max_theor-sg11)/(-sg_max_theor)

### Von Mises stress.
# vMisesRef: value calculated from stresses at node.
vMisesRef= math.sqrt(0.5*(math.pow(sg11-sg22,2)+(sg22*sg22)+(sg11*sg11)+6*(sg12*sg12+sg23+sg23+sg31+sg31)))
# Theoretical value sg22= 0 (free surface) and sg12= 0
vMisesTheor= abs(sg_max_theor)
# Von mises returned by XC
vMisesAtNodes= e.getValuesAtNodes('max_von_mises_stress', False)
vMisesXC= 0.0
for vm in vMisesAtNodes:
    vMisesXC= max(vMisesXC,vm[0])
ratio3= abs(vMisesXC-vMisesTheor)/vMisesTheor

'''
print('numNodes: ', numNodes)
print('numNodesTeor: ', numNodesTeor)
print('node ratio: ', numNodes-numNodesTeor)
print('element ratio= ', numElements-numElementsTeor)

print('uYMax= ', uYMax*1e3,'mm')
print('uYMaxTeor= ', uYMaxTeor*1e3,'mm')
print('ratio1= ', ratio1)

print('sg_max_theor= ', sg_max_theor/1e6,' MPa')
print('sg11= ', sg11Min/1e6,' MPa')
print('ratio2= ', ratio2)

print('vMisesRef= ', vMisesRef/1e6,' MPa')
print('vMisesTheor= ', vMisesTheor/1e6,' MPa')
print('vMisesXC= ', vMisesXC/1e6,' MPa')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<0.05 and ratio2<0.06 and ratio3<0.05): # Good results :)
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
  
# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)
#oh.displayFEMesh()
#oh.displayLocalAxes()
#oh.displayLoads()
#oh.displayDispRot('uY')
#oh.displayStrains('epsilon_xx')
#oh.displayStresses('sigma_11')
#oh.displayVonMisesStresses(vMisesCode= 'max_von_mises_stress')
