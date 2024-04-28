# -*- coding: utf-8 -*-
''' Home made test. Horizontal cantilever under tension load at its end.'''

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
width= .001
depth= .01
nDivIJ= 2
nDivJK= 2
y0= 0
z0= 0
L= 1.5 # Bar length (m)

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
fy= 275e6 # Yield stress of the steel
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

import os
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
  pth= "."
auxModulePath= pth+"/../../../aux"
sys.path.append(auxModulePath)
import test_quad_region as tqr

# Definition of a new empty fiber section named 'quadFibers' and stored in a
# Python variable of the same name (surprisingly enough).
quadFibers= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr() # Fiber section representation
                                                     # of 'quadFibers'
testQuadRegion= tqr.get_test_quad_region(preprocessor, y0, z0, width, depth, nDivIJ, nDivJK)
fiberSectionRepr.setGeomNamed(testQuadRegion.name) # We assign the geometry (regions and rebars)
                                                  # to the fiber section representation
                                                  # of 'quadFibers'
quadFibers.setupFibers() # Create the fibers from the information contained in th
                           # geometry.
fibras= quadFibers.getFibers() # Get the fiber container from the object.
A= fibras.getArea(1.0) # Get the sum of the fiber areas.



# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= quadFibers.name # Material name for the element (the fiber section).
beam3d= elements.newElement("ForceBeamColumn3d",xc.ID([n1.tag,n2.tag]))

# Constraints
modelSpace.fixNode000_000(n1.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag, xc.Vector([F,0,0,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)
# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(10)

delta= n2.getDisp[0]  # Node 2 xAxis displacement
deltateor= (F*L/(E*A))
ratio1= (abs((delta-deltateor)/deltateor))

beam3d.getResistingForce()
scc= beam3d.getSections()[0]

N0= scc.getStressResultantComponent("N")

ratio2= (abs((N0-F)/F))
# Check getN1 and getN2 (LP 28/04/2024).
N1= beam3d.getN1
N2= beam3d.getN2
ratio3= math.sqrt((F-N1)**2+(F-N2)**2)

''' 
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print(N0)
print("ratio2= ",ratio2)
print('F= ', F/1e3, 'N1= ', N1/1e3, ' N2= ', N2/1e3, ' ratio3= ', ratio3)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10:
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
# oh.displayIntForcDiag('N')
# # oh.displayIntForcDiag('M')
# # oh.displayLocalAxes()
