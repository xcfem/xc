# -*- coding: utf-8 -*-
''' home made test
 Horizontal cantilever under tension load at its end.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
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
Iz= depth*width**3/12 # Cross section moment of inertia (m4)
E= 210e9 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
J= .2e-1 # Cross section torsion constant (m4)

# Load
f= 1.5 # Load magnitude en N/m.

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Create nodes.
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)

# Materials definition
fy= 275e6 # Yield stress of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

respT= typical_materials.defElasticMaterial(preprocessor, "respT",G*J) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Shear response in z direction.
# Sections
import os
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
  pth= "."
exec(open(pth+"/../aux/testQuadRegion.py").read())

materialHandler= preprocessor.getMaterialHandler
quadFibers= materialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(testQuadRegion.name)
quadFibers.setupFibers()
A= quadFibers.getFibers().getArea(1.0)

agg= materialHandler.newMaterial("section_aggregator","agg")
agg.setSection("quadFibers")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])
 # Torsion and shear responses.



# Elements definition
## Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= agg.name
elements.numSections= 2 # Number of sections along the element.
el= elements.newElement("ForceBeamColumn3d",xc.ID([n1.tag, n2.tag]))

# Constraints
modelSpace.fixNode000_000(n1.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
mesh= feProblem.getDomain.getMesh
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
    elem.vector3dUniformLoadLocal(xc.Vector([f,0,0]))
    elem= eIter.next()

loadHandler= preprocessor.getLoadHandler

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)
# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(10)

# Get displacements.
delta= n2.getDisp[0]  # Node 2 xAxis displacement

# Get reactions.
nodes.calculateNodalReactions(True,1e-7) 
RN= n1.getReaction[0] 
RN2= n2.getReaction[0] 

# Get internal forces.
el.getResistingForce()
scc0= el.getSections()[0]
N0= scc0.getStressResultantComponent("N")

F= (f*L)
deltateor= (f*L**2/(2*E*A))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs((N0-F)/F))
ratio3= (abs((RN+F)/F))

''' 
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print("N0= ",N0)
print("ratio2= ",ratio2)
print("RN= ",RN)
print("ratio3= ",ratio3)
print("RN2= ",RN2)
   '''
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(RN2)<1e-14):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayBlocks()
# # oh.displayFEMesh()
# #oh.displayLocalAxes()
# # oh.displayStrongWeakAxis()
# oh.displayLoads()
# oh.displayReactions(reactionCheckTolerance= 1e-4)
# #oh.displayDispRot(itemToDisp='uZ', defFScale= 10.0)
# #oh.displayIntForcDiag(itemToDisp= 'Mz')
# #oh.displayIntForcDiag(itemToDisp= 'Qy')
# #oh.displayIntForcDiag(itemToDisp= 'Mz')
# #oh.displayIntForcDiag(itemToDisp= 'T')
