# -*- coding: utf-8 -*-
''' Home made test. Horizontal cantilever under uniform horizontal load along
    its length.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import sys
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Auxiliary modules.
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= "."
auxModulePath= pth+"/../../../aux"
sys.path.append(auxModulePath)
import test_quad_region as tqr # quad region definition convenience functions.

# Cross-section geometry
width= .05 # section width
depth= .1 # section depth
nDivIJ= 5 # number of divisions along IJ direction.
nDivJK= 10 # number of divisions along JK direction.
y0= 0 # lower left corner y coordinate. 
z0= 0 # lowr left corner z coordinate.
Iy= width*depth**3/12 # Cross section moment of inertia (m4)

L= 1.5 # Bar length (m)

# Load
f= 1.5e3 # uniform load magnitude (N)

# Define problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define mesh.
## Define nodes.
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)

## Define coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,-1,0]))

## Define materials
fy= 275e6 # Yield stress of the steel.
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

### Define torsion and shear responses. 
respT= typical_materials.defElasticMaterial(preprocessor, "respT",1e10) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Shear response in z direction.
### Define fiber section.
materialHandler= preprocessor.getMaterialHandler
quadFibers= materialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
testQuadRegion= tqr.get_test_quad_region(preprocessor, y0, z0, width, depth, nDivIJ, nDivJK)
fiberSectionRepr.setGeomNamed(testQuadRegion.name)
quadFibers.setupFibers()
# A= quadFibers.getFibers().getArea
### Define section aggregation. 
agg= materialHandler.newMaterial("section_aggregator","agg") # create section aggregator.
agg.setSection("quadFibers") # assign section.
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"]) # assign responses.

## Define element.
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name # Coordinate transformation for the new element.
elements.defaultMaterial= agg.name # Material for the new element.
elements.numSections= 3 # Number of sections along the element.
el= elements.newElement("ForceBeamColumn3d",xc.ID([n1.tag, n2.tag]))

## Define constraints
modelSpace.fixNode000_000(n1.tag)

## Define loads.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([el.tag]) 
eleLoad.transZComponent= f
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(10)


nodes.calculateNodalReactions(True,1e-7) 

delta= n2.getDisp[1]  # Node 2 xAxis displacement
deltateor= (-f*L**4/(8*E*Iy))
ratio1= (abs((delta-deltateor)/deltateor))


el.getResistingForce()
scc= el.getSections()[0]
N0= scc.getStressResultantComponent("N")
ratio2= (abs(N0))

M= scc.getStressResultantComponent("My")
MTeor= -f*L*L/2
ratio3= abs(M-MTeor)/MTeor
V= scc.getStressResultantComponent("Vz")
ratio4= abs(V-f*L)/(f*L)

Ry= n1.getReaction[1] 
ratio5= (abs((Ry-f*L)/(f*L)))
RMz= n1.getReaction[5] 
ratio6= (abs((RMz+MTeor)/MTeor))

# Check getMy1 and getMy2 (LP 28/04/2024).
My1= el.getMy1
My1Ref= MTeor
My2= el.getMy2
ratio7= math.sqrt((My1-My1Ref)**2+(My2)**2)

# Check getVz1 and getVz2 (LP 28/04/2024).
Vz1= el.getVz1
VzRef= f*L
Vz2= el.getVz2
ratio8= math.sqrt((Vz1-VzRef)**2+(Vz2)**2)

''' 
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print("N0= ",N0)
print("ratio2= ",ratio2)
print("M= ",M)
print("MTeor= ",MTeor)
print("ratio3= ",ratio3)
print("V= ",V)
print("ratio4= ",ratio4)
print("Ry= ",Ry)
print("ratio5= ",ratio5)
print("RMz= ",RMz)
print("ratio6= ",ratio6)
print('My1Ref= ', My1Ref/1e3, 'My1= ', My1/1e3, ' My2= ', My2/1e3, ' ratio7= ', ratio7)
print('Vz1Ref= ', VzRef/1e3, 'Vz1= ', Vz1/1e3, ' Vz2= ', Vz2/1e3, ' ratio8= ', ratio8)
   '''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.02) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10) & (abs(ratio7)<1e-10) & (abs(ratio8)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# # oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
# oh.displayLocalAxes()
# oh.displayLoads()
# # oh.displayIntForcDiag('N')
# # oh.displayIntForcDiag('Mz')
# oh.displayIntForcDiag('My')
# oh.displayIntForcDiag('Vz')
# # oh.displayIntForcDiag('Vy')
# #oh.displayLocalAxes()
