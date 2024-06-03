# -*- coding: utf-8 -*-
''' Home made test. Horizontal cantilever under uniform vertical load along its length.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Geometry
width= .05
depth= .1
A= width*depth
E= 210e9
I= width*depth**3/12
nu= 0.3
G= E/(2*(1+nu))
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude en N

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(L,0.0)


# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

# Materials definition
section= typical_materials.defElasticShearSection2d(preprocessor, "section",A,E,G,I,1.0)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name # Coordinate transformation for the new elements
elements.defaultMaterial= section.name
beam2d= elements.newElement("ForceBeamColumn2d",xc.ID([n1.tag,n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(n2.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
crdTransf= beam2d.getCoordTransf
vIElem= crdTransf.getIVector
vJElem= crdTransf.getJVector
loadVector= -(F/L)*vJElem
beam2d.vector2dUniformLoadGlobal(loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(10)


nodes.calculateNodalReactions(True,1e-7) 
delta= n1.getDisp[1]  # Node 2 xAxis displacement
Ry= n2.getReaction[1] 
RMz= n2.getReaction[2] 

elements= preprocessor.getElementHandler

beam2d.getResistingForce()
scc= beam2d.getSections()[-1]

N0= scc.getStressResultantComponent("N")
M= scc.getStressResultantComponent("Mz")

deltateor= (-F*L**3/(8*E*I))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs(N0))
MTeor= -F*L/2.0
ratio3= (abs((M-MTeor)/MTeor))
Vscc= scc.getStressResultantComponent("Vy")
ratio4a= (abs((Vscc+F)/F))
V= beam2d.getV()
ratio4b= (abs((V-(F/2))/(F/2)))
ratio5= (abs((Ry-F)/F))
ratio6= (abs((RMz-MTeor)/MTeor))

# Check getM1 and getM2 (LP 28/04/2024).
M1= beam2d.getM1
M2Ref= MTeor
M2= beam2d.getM2
ratio7= math.sqrt((M1)**2+(M2-M2Ref)**2)

# Check getV1 and getV2 (LP 28/04/2024).
V1= beam2d.getV1
VRef= F
V2= beam2d.getV2
ratio8= math.sqrt((V1)**2+(V2-VRef)**2)

'''
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print("N0= ",N0)
print("ratio2= ",ratio2)
print("M= ",M)
print("MTeor= ",MTeor)
print("ratio3= ",ratio3)
print("Vscc= ",Vscc)
print("ratio4a= ",ratio4a)
print("V= ",V)
print("ratio4b= ",ratio4b)
print("Ry= ",Ry)
print("ratio5= ",ratio5)
print("RMz= ",RMz)
print("ratio6= ",ratio6)
print('M2Ref= ', M2Ref/1e3, 'M1= ', M1/1e3, ' M2= ', M2/1e3, ' ratio7= ', ratio7)
print('V1Ref= ', VRef/1e3, 'V1= ', V1/1e3, ' V2= ', V2/1e3, ' ratio8= ', ratio8)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.005) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4a)<1e-10) & (abs(ratio4b)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10)& (abs(ratio7)<1e-10) & (abs(ratio8)<1e-10):
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
# oh.displayIntForcDiag('M')
# oh.displayIntForcDiag('V')
# #oh.displayLocalAxes()

