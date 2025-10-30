# -*- coding: utf-8 -*-
''' Reference:  Expresiones de la flecha el el Prontuario de
 Estructuras Metálicas del CEDEX. Apartado 3.3 Punctual load on cantilever.
 ISBN: 84-7790-336-0
 url={https://books.google.ch/books?id=j88yAAAACAAJ},

 vector2d_point_load_global verification test. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math

E= 2e6 # Elastic modulus
L= 20 # Bar length.
h= 0.30 # Beam cross-section depth.
b= 0.2 # Beam cross-section width.
A= b*h # Cross section area.
I= b*h**3/12 # Inertia of the beam section in inches to the fourth power.
x= 0.5 # Relative abscissae where the punctual load is applied.
P= 1e3 # Transverse load.
n= 1e6 # Axial load.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

ptoAplic=  geom.Pos2d(1+x*L*math.sqrt(2)/2,2+x*L*math.sqrt(2)/2) # Load application point.

# Define mesh.
## Define nodes.
n1= modelSpace.newNode(1,2)
n2= modelSpace.newNode(1+L*math.sqrt(2)/2,2+L*math.sqrt(2)/2)
## Define elements.    
### Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin") 
### Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)
### Elements definition
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(scc)
beam2d= modelSpace.newElement("ElasticBeam2d", [n1.tag,n2.tag])
beam2d.h= h
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(n1.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")

mesh= feProblem.getDomain.getMesh
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
    crdTransf= elem.getCoordTransf
    vIElem= crdTransf.getIVector
    vJElem= crdTransf.getJVector
    vCarga= n*vIElem-P*vJElem
    elem.vector2dPointLoadGlobal(xc.Vector([ptoAplic.x,ptoAplic.y]),vCarga)
    elem= eIter.next()

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solve.
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

tmp= n2.getDisp
vDisp= xc.Vector([tmp[0],tmp[1]])

a= x*L
delta0= vDisp.dot(vIElem)
delta0Teor= (n*a/E/A)
ratio0= ((delta0-delta0Teor)/delta0Teor)
delta1= vDisp.dot(vJElem)
delta1Teor= (-P*a**2*(3*L-a)/6/E/I)
ratio1= ((delta1-delta1Teor)/delta1Teor)

# print("delta0= ",delta0)
# print("delta0Teor= ",delta0Teor)
# print("ratio0= ",ratio0)
# print("vDisp= ",vDisp)
# print("vJElem= ",vJElem)
# print("delta1= ",delta1)
# print("delta1Teor= ",delta1Teor)
# print("ratio1= ",ratio1)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-10) & (abs(ratio1)<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

