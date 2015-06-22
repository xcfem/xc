# Home made test
# -*- coding: utf-8 -*-
# Ménsula sometida a carga vertical en su extremo.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

# Propiedades del material
E= 2.1e6*9.81/1e-4 # Módulo elástico en Pa
nu= 0.3 # Coeficiente de Poisson
G= E/(2*(1+nu)) # Módulo de elasticidad a cortante

# Propiedades de la sección (IPE-80)
A= 7.64e-4 # Área de la sección expresada en m2
Iy= 80.1e-8 # Momento de inercia de la sección expresada en m4
Iz= 8.49e-8 # Momento de inercia de la sección expresada en m4
J= 0.721e-8 # Momento de inercia a torsión expresado en m4

# Geometry
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Magnitud de la carga en kN

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
# Problem type
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nodos.newNodeXYZ(0,0.0,0.0)
nodos.newNodeXYZ(L,0.0,0.0)

trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials
caracMecSeccion= xc.ConstantesSecc3d()
caracMecSeccion.A= A; caracMecSeccion.E= E; caracMecSeccion.G= G;
caracMecSeccion.Iz= Iz; caracMecSeccion.Iy= Iy; caracMecSeccion.J= J
seccion= typical_materials.defElasticSectionFromMechProp3d(mdlr,"seccion",caracMecSeccion)

# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1 #Tag for the next element.
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));



coacciones= mdlr.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)

cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F,0,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

delta= nodos.getNode(2).getDisp[1] #ydisplacement of node 2.
elementos.getElement(1).getResistingForce()
M= elementos.getElement(1).getMy1
V= elementos.getElement(1).getVz

deltateor= (-F*L**3/(3*E*Iy))
ratio1= (delta/deltateor)
ratio2= (-M/(F*L))
ratio3= (V/F)

# print delta
# print deltateor
# print ratio1
# print "M= ",M
# print ratio2
# print "V1= ",V
# print ratio3

import os
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
