# -*- coding: utf-8 -*-
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
import math

b= 0.4
h= 0.8
A= b*h
E= 200000*9.81/1e-4 # Estimated concrete elastic modulus.
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
Iy= (1/12.0*h*b**3) # Cross section moment of inertia (m4)
Iz= (1/12.0*b*h**3) # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
L= 1 # Element length expressed in meters.
F= 1.0e3 # Load magnitude (kN)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 # First node number.
nod= nodes.newNodeXYZ(0.0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0)

# Materials
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
sectionProperties.rotate(math.radians(90))
section= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)

lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
elements.defaultTag= 1 # Tag for the next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]))



sectionAngle= 0
fuerte= beam3d.getVDirStrongAxisGlobalCoord(True) # initialGeometry= True
debil= beam3d.getVDirWeakAxisGlobalCoord(True) # initialGeometry= True
sectionAngle= beam3d.getStrongAxisAngle()
ratio1= ((debil[0])**2+(debil[2])**2)
ratio2= ((fuerte[0])**2+(fuerte[1])**2)


# Constraints

modelSpace.fixNode000_000(1)

# Loads definition
# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0') 
lp0.newNodalLoad(2,xc.Vector([0,-F,F,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
deltaYTeor= (-F*L**3/(3*E*Iz))
deltaZTeor= (F*L**3/(3*E*Iy))

nodes= preprocessor.getNodeHandler

nod2= nodes.getNode(2)
deltaY= nod2.getDisp[1]
deltaZ= nod2.getDisp[2] # Node 2 yAxis displacement



ratio3= (deltaY-deltaYTeor)/deltaYTeor
ratio4= (deltaZ-deltaZTeor)/deltaZTeor
ratio5= (deltaY/deltaZ)+(Iy/Iz)

''' 
print("deltaY/deltaZ= ",deltaY/deltaZ)
print("Iy/Iz= ",(Iy/Iz))
print("fuerte: ",fuerte)
print("ratio1= ",ratio1)
print("debil: ",debil)
print("ratio2= ",ratio2)
print("deltaY= ",deltaY)
print("deltaYTeor= ",deltaYTeor)
print("ratio3= ",ratio3)
print("deltaZ= ",deltaZ)
print("deltaZTeor= ",deltaZTeor)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
  '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-15) & (ratio2 < 1e-15) & (abs(sectionAngle) < 1e-12) & (ratio3 < 1e-5) & (ratio4 < 1e-6) & (ratio5 < 1e-6):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
