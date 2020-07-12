# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division
# Reference:  Deflection formulas from the publication: "Prontuario de
# Estructuras Metálicas del CEDEX". Apartado 3.3 Punctual load on cantilever.
# ISBN: 84-7790-336-0
# url={https://books.google.ch/books?id=j88yAAAACAAJ},

'''vector3d_point_load_local verification test. Home made test.'''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


E= 2e6 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
L= 20 # Bar length.
h= 0.30 # Cross section depth
b= 0.20 # Cross section width.
A= b*h # Cross section area.
Iz= b*h**3/12.0 # Cross section moment of inertia
Iy= h*b**3/12.0 # Cross section moment of inertia
J= 1e-8 # Cross section torsion constant
x= 0.5 # Relative abscissae where the punctual load is applied.
P= 1e3 # punctual load transversal.
n= 1e6 # punctual load axial.

ptoAplic= xc.Vector([1+x*L*math.sqrt(2)/2.0,3+x*L*math.sqrt(2)/2.0,0])

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(1,3,0)
nod= nodes.newNodeXYZ(1+L*math.sqrt(2)/2.0,3+L*math.sqrt(2)/2.0,0)

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))    
# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= "scc"
elements.defaultTag= 1 #Tag for next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]))

    
# Constraints
modelSpace.fixNode000_000(1)


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lPatterns.currentLoadPattern= "0"

vIElem= xc.Vector([1,0,0])
vJElem= xc.Vector([0,1,0])
mesh= feProblem.getDomain.getMesh
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
  crdTransf= elem.getCoordTransf
  vIElem= crdTransf.getIVector
  vJElem= crdTransf.getJVector
  vCarga= n*vIElem-P*vJElem
  elem.vector3dPointLoadGlobal(ptoAplic,vCarga)
  elem= eIter.next()

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)


nodes= preprocessor.getNodeHandler
nod2= nodes.getNode(2)
tmp= nod2.getDisp
vDispUVW= xc.Vector([tmp[0],tmp[1],tmp[2]])
a= x*L
delta0= vDispUVW.dot(vIElem)
delta0Teor= (n*a/E/A)
ratio0= ((delta0-delta0Teor)/delta0Teor)
delta1= vDispUVW.dot(vJElem)
delta1Teor= (-P*a**2*(3*L-a)/6.0/E/Iz)
ratio1= ((delta1-delta1Teor)/delta1Teor)

''' 
print("vDispUVW= ",vDispUVW)
print("vIElem= ",vIElem)
print("vJElem= ",vJElem)
print("delta0= ",delta0)
print("delta0Teor= ",delta0Teor)
print("ratio0= ",ratio0)
print("delta1= ",delta1)
print("delta1Teor= ",delta1Teor)
print("ratio1= ",ratio1)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-10) & (abs(ratio1)<1e-11):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
