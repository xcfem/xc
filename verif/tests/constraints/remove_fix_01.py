# -*- coding: utf-8 -*-
# home made test
# Se comprueba el funcionamiento de remove_fix.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model import fix_node_6dof

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

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0,0.0,0.0)
nod= nodos.newNodeXYZ(L,0.0,0.0)


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

# Constraints
coacciones= mdlr.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)
spc= coacciones.newSPConstraint(2,1,0.0)
spcTag= spc.tag
# \fix[2,1,20]{ \valor{0.0} }

# Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F,0,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos= mdlr.getNodeLoader 
nod2= nodos.getNode(2)
delta0= nod2.getDisp[1]  # Desplazamiento del nodo 2 según y




coacciones= mdlr.getConstraintLoader
coacciones.removeSPConstraint(spcTag)
#remove_fix{20}



# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodos= mdlr.getNodeLoader 
nod2= nodos.getNode(2)
delta= nod2.getDisp[1]  # Desplazamiento del nodo 2 según y

elem1= elementos.getElement(1)
elem1.getResistingForce()
M= elem1.getMy1
V= elem1.getVz

deltateor= (-F*L**3/(3*E*Iy))
ratio1= ((delta-deltateor)/deltateor)
ratio2= (-M/(F*L))
ratio3= (V/F)

''' 
print "delta0= ",delta0
print "delta= ",delta
print "deltaTeor= ",deltateor
print "ratio1= ",ratio1
print "M= ",M
print "ratio2= ",ratio2
print "V1= ",V
print "ratio3= ",ratio3
   '''

import os
fname= os.path.basename(__file__)
if (abs(delta0)<1e-11) & (abs(ratio1)<1e-9) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
