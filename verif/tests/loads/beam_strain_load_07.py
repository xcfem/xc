# -*- coding: utf-8 -*-
# home made test
# Reference:  Cálculo de estructuras por el método de los elementos finitos. E. Oñate, pg. 165, ejemplo 5.3

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
alpha= 1.2e-5 # Coeficiente de dilatación of the steel
A= 4e-4 # bar area expressed in square meters
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
AT= 10 # Incremento de temperatura expressed in grados centígrados

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0.0,0.0,0.0)
nod= nodes.newNodeXYZ(L/2,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])
# Materials definition
seccion= typical_materials.defElasticShearSection3d(preprocessor, "seccion",A,E,G,Iz,Iy,J,1.0)

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1
beam1= elementos.newElement("force_beam_column_3d",xc.ID([1,2]));
beam2= elementos.newElement("force_beam_column_3d",xc.ID([2,3]));

    
# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.fixNode6DOF(coacciones,3)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("linear_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([1,2])
defTermica= xc.DeformationPlane(alpha*AT)
eleLoad.backEndDeformationPlane= defTermica
eleLoad.frontEndDeformationPlane= defTermica



cargas= preprocessor.getLoadLoader

#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_static_modified_newton(prueba)
result= analisis.analyze(1)


nod2= nodes.getNode(2)
dX= nod2.getDisp[0] 
dY= nod2.getDisp[1] 
dZ= nod2.getDisp[2] 

elem1= elementos.getElement(1)
elem1.getResistingForce()
scc0= elem1.getSections()[0]


axil= scc0.getStressResultantComponent("N")
momentoY= scc0.getStressResultantComponent("My")
momentoZ= scc0.getStressResultantComponent("Mz")
cortanteY= scc0.getStressResultantComponent("Vy")
cortanteZ= scc0.getStressResultantComponent("Vz")



N= (-E*A*alpha*AT)
ratio= ((axil-N)/N)

''' 
print "dX= ",dX
print "dY= ",dY
print "dZ= ",dZ
print "N= ",N
print "axil= ",axil
print "ratio= ",ratio
print "momentoY= ",momentoY
print "momentoZ= ",momentoZ
print "cortanteY= ",cortanteY
print "cortanteZ= ",cortanteZ
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio)<1e-10) & (abs(momentoY)<1e-10) & (abs(momentoZ)<1e-10) & (abs(cortanteY)<1e-10) & (abs(cortanteZ)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
