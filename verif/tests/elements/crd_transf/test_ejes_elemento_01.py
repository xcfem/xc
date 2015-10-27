# -*- coding: utf-8 -*-
from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model import fix_node_6dof
import math

b= 0.4
h= 0.8
A= b*h
E= 200000*9.81/1e-4 # Módulo elástico aproximado del hormigón.
nu= 0.3 # Coeficiente de Poisson
G= E/(2*(1+nu)) # Módulo de elasticidad a cortante
Iy= (1/12.0*h*b**3) # Momento de inercia de la sección expresado en m4
Iz= (1/12.0*b*h**3) # Momento de inercia de la sección expresado en m4
J= 0.721e-8 # Momento de inercia a torsión expresado en m4
L= 1 # Longitud del elmento expresada en metros.

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0.0,0.0,0.0)
nod= nodos.newNodeXYZ(L,0.0,0)

trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials
caracMecSeccion= xc.ConstantesSecc3d()
caracMecSeccion.A= A; caracMecSeccion.E= E; caracMecSeccion.G= G;
caracMecSeccion.Iz= Iz; caracMecSeccion.Iy= Iy; caracMecSeccion.J= J
seccion= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "seccion",caracMecSeccion)

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultTag= 1 #Tag for the next element.
elementos.defaultMaterial= "seccion"
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));


fuerte= beam3d.getVDirEjeFuerteGlobales()
debil= beam3d.getVDirEjeDebilGlobales()
angSeccion= beam3d.getAnguloEjeFuerte()
ratio1= ((fuerte[0])**2+(fuerte[2])**2)
ratio2= ((debil[0])**2+(debil[1])**2)
ratio3= abs(angSeccion-math.pi/2)/(math.pi/2)



''' 
print "strong axis: ", fuerte
print "ratio1= ",ratio1
print "weak axis: ", debil
print "ratio2= ",ratio2
print "ratio3= ",ratio3
  '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-15) & (ratio2 < 1e-15) & (ratio3 < 1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
