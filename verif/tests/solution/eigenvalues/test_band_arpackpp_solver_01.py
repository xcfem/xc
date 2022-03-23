# -*- coding: utf-8 -*-
# Tomado del example A47 del SOLVIA Verification Manual
from __future__ import print_function
from __future__ import division
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1 # Cantilever length in meters
b= 0.05 # Cross section width in meters
h= 0.10 # Cross section depth in meters
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Moment of inertia in m4
theta= math.radians(30)
E=2.0E11 # Elastic modulus en N/m2
dens= 7800 # Steel density kg/m3
m= A*dens

NumDiv= 10

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)




# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")

elements= preprocessor.getElementHandler
# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultTransformation= lin.name
seedElemHandler.defaultMaterial= scc.name
beam2d= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))
beam2d.h= h
beam2d.rho= m


points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(L*math.cos(theta),L*math.sin(theta),0.0))
lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newLine(pt1.tag,pt2.tag)
l.nDiv= NumDiv

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

# Constraints
constraints= preprocessor.getBoundaryCondHandler
nTag= pt1.getNode().tag
spc= constraints.newSPConstraint(nTag,0,0.0) # gdl 0
spc= constraints.newSPConstraint(nTag,1,0.0) # gdl 1
spc= constraints.newSPConstraint(nTag,2,0.0) # gdl 2


# Solution procedure
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
exec(open(pth+"/../../aux/arpackpp_solver.py").read())


omega= eig1**0.5
T= 2*math.pi/omega
fcalc= 1/T


lambdA= 1.87510407
fteor= lambdA**2/(2*math.pi*L**2)*math.sqrt(E*I/m)
ratio1= (fcalc-fteor)/fteor

'''
print("fteor= ",fteor)
print("eig1= ",(omega))
print("omega= ",(omega))
print("T= ",(T))
print("fcalc= ",(fcalc))
''' 

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
