# -*- coding: utf-8 -*-
''' Taken from example 1.2.4 of "Abacus Benchmarks Manual".
'''

from __future__ import print_function
from __future__ import division

import xc_base
import geom
import xc

import math
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
#from postprocess import output_handler

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

b= 2 # Length of plate edge.
t= .01 # Plate thickness.
E= 1e8 # Young's modulus
nu= 0.3

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Material definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,t)
# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= memb1.name
elem= seedElemHandler.newElement("ShellNLDKGQ",xc.ID([0,0,0,0]))

# Block topology
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntFromPos3d(geom.Pos3d(b/2.0,0.0,0.0))
pt3= points.newPntFromPos3d(geom.Pos3d(b/2.0,b/2.0,0.0))
pt4= points.newPntFromPos3d(geom.Pos3d(0.0,b/2.0,0.0))
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(pt1.tag, pt2.tag, pt3.tag, pt4.tag)
nDiv= 4
s1.nDivI= nDiv
s1.nDivJ= nDiv

s1.genMesh(xc.meshDir.I)

b_2= b/2.0

# Constraints.
for n in s1.nodes:
    p= n.getInitialPos3d
    x= p.x; y= p.y
    if(x==0.0):
        modelSpace.fixNode('0FF_F00',n.tag)
    if(y==0.0):
        modelSpace.fixNode('F0F_0F0',n.tag)
    if(x==b_2):
        modelSpace.fixNode('FF0_0FF',n.tag)
    if(y==b_2):
        modelSpace.fixNode('FF0_F0F',n.tag)

# Load.
lp0= modelSpace.newLoadPattern(name= '0')
P= -100.0
p= P/nDiv
for n in s1.nodes:
    pt= n.getInitialPos3d
    x= pt.x; y= pt.y
    if(x==b_2):
        if((y==0.0) or (y==b_2)):
            lp0.newNodalLoad(n.tag,xc.Vector([p/2.0,0,0,0,0,0]))
        else:
            lp0.newNodalLoad(n.tag,xc.Vector([p,0,0,0,0,0]))
modelSpace.addLoadCaseToDomain(lp0.name) # add the load case to domain.
            
# Solution procedure
import os
pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
# print("pth= ", pth)
exec(open(pth+"/../../aux/solu_linear_buckling.py").read())

eig1= analysis.getEigenvalue(1)
D= E*t**3/12/(1-nu**2) # elastic bending stiffness of the plate
N_crit= -4.0*math.pi**2*D/b**2
N_calc= eig1*P
ratio1= abs(N_crit-N_calc)/-N_crit

# Shape
shape= list()
wMax= 0.0
for n in s1.nodes:
    pt= n.getInitialPos3d
    x= pt.x; y= pt.y
    wRef= math.cos(math.pi*x/b)*math.cos(math.pi*y/b)
    ev= n.getEigenvector(1)
    wCalc= abs(ev[2])
    wMax= max(wMax, wCalc)
    if(abs(wCalc)>0.0 and abs(wRef)>0.0):
        shape.append([wRef, wCalc])

shapeErr= 0.0
for s in shape:
    s[1]/= wMax
    shapeErr+= (s[0]-s[1])**2
shapeErr= math.sqrt(shapeErr)

'''
print("eig1= ",(eig1))
print("N_crit= ",(N_crit))
print("N_calc= ",(N_calc))
print("ratio1= ",(ratio1))
print("shapeErr= ", shapeErr)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<.01) & (abs(shapeErr)<1e-2):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)
#oh.displayFEMesh()
#oh.displayLoads()
#oh.displayEigenvectors(1)
#oh.displayEigenResult(1)
#oh.displayLocalAxes()
#oh.displayReactions()
