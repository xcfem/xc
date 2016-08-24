# -*- coding: utf-8 -*-
# Tomado del Ejemplo A47 del SOLVIA Verification Manual
import xc_base
import geom
import xc

from model import fix_node_6dof
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math


L= 1 # Longitud de la ménsula en metros
b= 0.05 # Cross section width en metros
h= 0.10 # Cross section heighten metros
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Momento de inercia en m4
J= 10*I # Cross section torsion constant
theta= math.radians(30)
E=2.0E11 # Elastic modulus en N/m2
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
dens= 7800 # Densidad del acero en kg/m3
m= A*dens

NumDiv= 10
# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

predefined_spaces.gdls_resist_materiales3D(nodes)
# Definimos materiales
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,I,I,J)


nodes.newSeedNode()

# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,0,1])

# Definimos elemento semilla
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "scc"
seedElemLoader.defaultTransformation= "lin"
seedElemLoader.defaultTag= 1 #Tag for the next element.
beam3d= seedElemLoader.newElement("elastic_beam_3d",xc.ID([0,0]))
beam3d.rho= m


puntos= preprocessor.getCad.getPoints
pt1= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt2= puntos.newPntIDPos3d(2,geom.Pos3d(L*math.cos(theta),L*math.sin(theta),0.0))
lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l= lines.newLine(1,2)
l.nDiv= NumDiv


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

tagN1= pt1.getNode().tag
tagN2= pt2.getNode().tag

# Constraints
coacciones= preprocessor.getConstraintLoader

spc= coacciones.newSPConstraint(tagN1,0,0.0) # gdl 0
spc= coacciones.newSPConstraint(tagN1,1,0.0) # gdl 1
spc= coacciones.newSPConstraint(tagN1,2,0.0) # gdl 2
spc= coacciones.newSPConstraint(tagN1,3,0.0) # gdl 0
spc= coacciones.newSPConstraint(tagN1,4,0.0) # gdl 1
spc= coacciones.newSPConstraint(tagN1,5,0.0) # gdl 2


# Procedimiento de solución
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/sol_modos_propios.py")

omega= eig1**0.5
T= 2*math.pi/omega
fcalc= 1/T



lambdA= 1.87510407
fteor= lambdA**2/(2*math.pi*L**2)*math.sqrt(E*I/m)
ratio1= abs(fcalc-fteor)/fteor

''' 
print "omega= ",(omega)
print "T= ",(T)
print "fcalc= ",(fcalc)
print "fteor= ",(fteor)
print "ratio1= ",(ratio1)
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<5e-3):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
