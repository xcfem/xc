# -*- coding: utf-8 -*-
''' Home made test. Horizontal cantilever under vertical load at his front end.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import sys
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Geometry
width= .05
depth= .1
nDivIJ= 5
nDivJK= 10
y0= 0
z0= 0
L= 1.5 # Bar length (m)
Iy= width*depth**3/12 # Cross section moment of inertia (m4)

# Load
F= 1.5e3 # Load magnitude en N

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)


lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials definition
fy= 275e6 # Yield stress of the steel.
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

respT= typical_materials.defElasticMaterial(preprocessor, "respT",1e10) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Shear response in z direction.
# Sections
import os
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= "."
auxModulePath= pth+"/../../../aux"
sys.path.append(auxModulePath)
import test_quad_region as tqr

materialHandler= preprocessor.getMaterialHandler
quadFibers= materialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
testQuadRegion= tqr.get_test_quad_region(preprocessor, y0, z0, width, depth, nDivIJ, nDivJK)
fiberSectionRepr.setGeomNamed(testQuadRegion.name)
quadFibers.setupFibers()
A= quadFibers.getFibers().getArea

agg= materialHandler.newMaterial("section_aggregator","agg")
agg.setSection("quadFibers")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])
# Torsion and shear responses.


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= agg.name
elements.numSections= 3 # Number of sections along the element.
el= elements.newElement("ForceBeamColumn3d",xc.ID([n1.tag, n2.tag]))

# Constraints
modelSpace.fixNode000_000(n1.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([0,-F,0,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(10)


nodes.calculateNodalReactions(True,1e-7) 

delta= n2.getDisp[1]  # Node 2 xAxis displacement
deltateor= (-F*L**3/(3*E*Iy))
ratio1= (abs((delta-deltateor)/deltateor))


el.getResistingForce()
scc= el.getSections()[0]
N0= scc.getStressResultantComponent("N")
ratio2= (abs(N0))

M= scc.getStressResultantComponent("My")
MTeor= (F*L)
ratio3= (abs((M-MTeor)/MTeor))
V= scc.getStressResultantComponent("Vz")
ratio4= (abs((V+F)/F))

Ry= n1.getReaction[1] 
ratio5= (abs((Ry-F)/F))
RMz= n1.getReaction[5] 
ratio6= (abs((RMz-MTeor)/MTeor))

# Check getMy1 and getMy2 (LP 28/04/2024).
My1= el.getMy1
My1Ref= F*L
My2= el.getMy2
ratio7= math.sqrt((My1-My1Ref)**2+(My2)**2)

# Check getVz1 and getVz2 (LP 28/04/2024).
Vz1= el.getVz1
VzRef= -F
Vz2= el.getVz2
ratio8= math.sqrt((Vz1-VzRef)**2+(Vz2-VzRef)**2)

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
