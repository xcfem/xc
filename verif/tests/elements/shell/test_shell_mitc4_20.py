# -*- coding: utf-8 -*-
from __future__ import print_function
# Test taken from section 6.7 (control 1) of the book
# «Los forjados reticulares: diseño, análisis, construcción y patología»
# by Florentino Regalado Tesoro ISBN: 978-8493069650.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from actions.basic_loads import nodal_loads
from materials import typical_materials
import math

CooMaxX= 10
CooMaxY= 10
xMidP= CooMaxX/2
yMidP= CooMaxY/2
xMidL= xMidP
yMidL= CooMaxY
NumDivI= int(math.ceil(CooMaxX/0.25))
NumDivJ= int(math.ceil(CooMaxY/0.25))
E= 2.1e10 # Elastic modulus en N/m2
nu= 0.0 # Poisson's ratio
G= E/2/(1+nu)
thickness= 0.2 # Cross section depth expressed in meters.
unifLoad= 20e3 # Uniform load in N/m2.
nLoad= unifLoad*CooMaxX*CooMaxY/NumDivI/NumDivJ # Tributary load on each node

centerElemTag= 0
sideElemTag= 0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
feProblem.logFileName= "/tmp/erase.log" # Don't pring warnings
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)



# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

# Define materials
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,thickness)



seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= memb1.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))



points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(CooMaxX,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(CooMaxX,CooMaxY,0.0))
pt4= points.newPoint(geom.Pos3d(0.0,CooMaxY,0.0))
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= NumDivI
s.nDivJ= NumDivJ

# Constraints
s.genMesh(xc.meshDir.I)
sides= s.getSides
# Edge iterator
for l in sides:
  for i in l.getEdge.getNodeTags():
    modelSpace.fixNode000_000(i)

# Loads definition
# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0') 


nodal_loads.load_on_nodes_in_face(s,lp0,[0,0,-nLoad,0,0,0])

centerElemTag= s.getNearestElement(geom.Pos3d(xMidP,yMidP,0.0)).tag
sideElemTag= s.getNearestElement(geom.Pos3d(xMidL,yMidL,0.0)).tag
nodes= preprocessor.getNodeHandler

nNodes= s.getNumNodes

nElems= s.getNumElements
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.simple_static_linear(feProblem)
analOk= analysis.analyze(1)

m1Center= 0.0
m2SideCenter= 0.0

node= s.getNodeIJK(1, int(NumDivI/2+1), int(NumDivJ/2+1))

# \print{"Central node: ",tag
# print("Central node coordinates: ",coord)
# print("Central node displacements: ", node.getDisp)
UZ= node.getDisp[2]


centerElem= preprocessor.getElementHandler.getElement(centerElemTag)
centerElem.getResistingForce()
m1Center= centerElem.getMeanInternalForce("m1")
sideElem= preprocessor.getElementHandler.getElement(sideElemTag)
sideElem.getResistingForce()
m2SideCenter= sideElem.getMeanInternalForce("m2")

UZTeor= -1.821e-2
ratio1= (abs((UZ-UZTeor)/UZTeor))
ratio2= (abs((nElems-1600)/1600))
ratio3= (abs((m1Center+35.20e3)/35.20e3))
ratio4= (abs((m2SideCenter-103.09e3)/103.09e3))

''' 
print("centerElemTag= ",centerElemTag)
print("sideElemTag= ",sideElemTag)
print("UZ= ",UZ)
print("m1Center= ",m1Center/1e3," kN \n")
print("m2SideCenter= ",m2SideCenter/1e3," kN \n")
print("Number of nodes: ",nNodes)
print("Number of elements: ",nElems)
print("ratio1: ",ratio1)
print("ratio2: ",ratio2)
print("ratio3: ",ratio3)
print("ratio4: ",ratio4)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.5e-2) & (abs(ratio2)<1e-9) & (abs(ratio3)<2e-4) & (abs(ratio4)<0.11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
