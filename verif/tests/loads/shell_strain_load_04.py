# -*- coding: utf-8 -*-
''' home made test based on symmetry and equilibrium considerations.
'''

from __future__ import print_function
from __future__ import division

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
#from postprocess import output_handler

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1.0 # Size of plate edge (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 400.0 # Temperature increment (Celsius degrees)
h= 0.2
A= L*h

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
steel= typical_materials.defJ2PlateFibre(preprocessor=preprocessor, name='steel', E= E, nu= nu, fy=250e6,alpha=1e-6,rho= 0.0)
memb1= typical_materials.defMembranePlateFiberSection(preprocessor,name='ring_mat',h= h ,nDMaterial= steel)

# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= memb1.name
elem= seedElemHandler.newElement("ShellNLDKGQ",xc.ID([0,0,0,0]))

# Block topology (ne-quarter of the plate)
L_2= L/2.0
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntFromPos3d(geom.Pos3d(L_2,0.0,0.0))
pt3= points.newPntFromPos3d(geom.Pos3d(L_2,L_2,0.0))
pt4= points.newPntFromPos3d(geom.Pos3d(0.0,L_2,0.0))
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(pt1.tag, pt2.tag, pt3.tag, pt4.tag)

# Mesh generation
nDiv= 4
s1.nDivI= nDiv
s1.nDivJ= nDiv
s1.genMesh(xc.meshDir.I)

# Constraints
for n in s1.nodes:
    p= n.getInitialPos3d
    x= p.x; y= p.y
    if(x==0.0):
        modelSpace.fixNode('0FF_F00',n.tag)
    if(y==0.0):
        modelSpace.fixNode('F0F_0F0',n.tag)
    if(x==L_2):
        modelSpace.fixNode('FF0_0FF',n.tag)
    if(y==L_2):
        modelSpace.fixNode('FF0_F0F',n.tag)
        
# Load case definition.
#factors= [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("shell_strain_load")
eleLoad.setElementTags(s1)
for gp in [0,1,2,3]: # Gauss points
    for compo in [0,1]: # Components
        eleLoad.setStrainComp(gp,compo,alpha*AT) #(id of Gauss point, id of component, value)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

factors= [0.1, 0.25, 0.4, 0.55, 0.7, 0.85, 1.0]
modelSpace.analysis= predefined_solutions.penalty_newton_raphson(feProblem)
for f in factors:
    lp0.gammaF= f
    result= modelSpace.analyze(calculateNodalReactions= True)
    if(result!=0):
        break

error= 0.0
for e in s1.elements:
    e.getResistingForce()
    avgN1= e.getMeanInternalForce("n1")
    avgN2= e.getMeanInternalForce("n2")
    error+= avgN1**2+avgN2**2

error= math.sqrt(error)

''' 
print('error: ', error)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-5):
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
