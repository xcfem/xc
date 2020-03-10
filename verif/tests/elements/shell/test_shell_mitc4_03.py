# -*- coding: utf-8 -*-
''' Taken from -Development of Membrane, Plate and Flat Shell Elements in Java- article '''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 6.0 # Beam length expressed in inches.
h= 0.8 # Beam cross-section depth expressed in inches.
t= 1 # Beam cross-section width expressed in inches.
E= 30000 # Young modulus of the material expressed in ksi.
nu= 0.3 # Poisson's ratio.
# Load
F= 10 # Load magnitude en kips

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(0,0,0)
nod2= nodes.newNodeXYZ(L/3,0,0)
nod3= nodes.newNodeXYZ(2*L/3,0,0)
nod4= nodes.newNodeXYZ(L,0,0)
nod5= nodes.newNodeXYZ(0,h,0)
nod6= nodes.newNodeXYZ(L/3,h,0)
nod7= nodes.newNodeXYZ(2*L/3,h,0)
nod8= nodes.newNodeXYZ(L,h,0)


# Materials definition
nmb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,h)

elements= preprocessor.getElementHandler
elements.defaultMaterial= "memb1"
elem= elements.newElement("ShellMITC4",xc.ID([nod1.tag,nod2.tag,nod6.tag,nod5.tag]))

elem= elements.newElement("ShellMITC4",xc.ID([nod2.tag,nod3.tag,nod7.tag,nod6.tag]))
elem= elements.newElement("ShellMITC4",xc.ID([nod3.tag,nod4.tag,nod8.tag,nod7.tag]))

# Constraints
modelSpace.fixNode000_FFF(nod1.tag)
spc= modelSpace.constraints.newSPConstraint(nod2.tag,2,0.0)
spc= modelSpace.constraints.newSPConstraint(nod3.tag,2,0.0)
spc= modelSpace.constraints.newSPConstraint(nod4.tag,2,0.0)
modelSpace.fixNode000_FFF(nod5.tag)
spc= modelSpace.constraints.newSPConstraint(nod6.tag,2,0.0)
spc= modelSpace.constraints.newSPConstraint(nod7.tag,2,0.0)
spc= modelSpace.constraints.newSPConstraint(nod8.tag,2,0.0)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(nod8.tag,xc.Vector([0,-F,0,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)



# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)



# Retrieving node displacements.
UX8= nod8.getDisp[0] # Node 8 xAxis displacement
UY8= nod8.getDisp[1] # Node 8 yAxis displacement
UX3= nod3.getDisp[0] # Node 3 xAxis displacement                 
UY3= nod3.getDisp[1] # Node 3 yAxis displacement

UX8SP2K= 0.016110
UY8SP2K= -0.162735
UX3SP2K= -0.014285
UY3SP2K= -0.084652


# Differences from the results attributed to SAP-2000 in that article.
ratio1= abs(UX8-UX8SP2K)/UX8SP2K
ratio2= abs(UY8-UY8SP2K)/UY8SP2K
ratio3= abs(UX3-UX3SP2K)/UX3SP2K
ratio4= abs(UY3-UY3SP2K)/UY3SP2K

''' 
print "UX8= ",UX8
print "UX8SP2K= ",UX8SP2K
print "UY8= ",UY8
print "UY8SP2K= ",UY8SP2K
print "UX3= ",UX3
print "UY3= ",UY3

print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4

'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.12) & (abs(ratio2)<0.15) & (abs(ratio3)<0.12) & (abs(ratio4)<0.15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
