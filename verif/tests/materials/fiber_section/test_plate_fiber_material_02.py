# -*- coding: utf-8 -*-
''' Simple test of shell element with membrane plate fiber material.'''

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

n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(2,0,0)
n3= nodes.newNodeXYZ(2,1,1)
n4= nodes.newNodeXYZ(0,1,1)

# Meshing

## Define element
elements= preprocessor.getElementHandler
elements.defaultMaterial= plateFiber.name
elem= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))

## Boundary conditions

### Fix nodes in 1 and 4.
modelSpace.fixNode000_000(n1.tag)
modelSpace.fixNode000_000(n4.tag)

## Loads

### Create load pattern.
lp0= modelSpace.newLoadPattern('0')
### Put the load at nodes 2 and 3.
load= F/2.0 # Half! force
lp0.newNodalLoad(n2.tag,xc.Vector([load,0.0,0.0,0.0,0.0,0.0]))
lp0.newNodalLoad(n3.tag,xc.Vector([load,0.0,0.0,0.0,0.0,0.0]))
modelSpace.addLoadCaseToDomain(lp0.name)

## Solve
analOK= modelSpace.analyze(calculateNodalReactions= True)

## Tests

### Reactions.
R1= n1.getReaction
R4= n4.getReaction
R= (R1+R4).Norm()
ratio1= abs(R-F)/F

'''
print('R1= ', R1)
print('R4= ', R4)
print('R= ', R)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-12):
  print("test ",fname,": ok.")
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
#oh.displayVonMisesStresses()
