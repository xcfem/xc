# -*- coding: utf-8 -*-
''' Test tomado del libro:
    Introdución al estudio del elemento finito en Ingeniería. Segunda edición.
    Tirupathi R. Chandrupatla
    Ashok D. Belegundu.
    Prentice Hall, México 1999
    Ejemplo 7.2 '''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad
t= 1
F=1000

# Problem type
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_elasticidad2D(nodos)


n1= nodos.newNodeIDXY(1,0,0)
n2= nodos.newNodeIDXY(2,1,0)
n3= nodos.newNodeIDXY(3,1,1)
n4= nodos.newNodeIDXY(4,0,1)


elast2d= typical_materials.defElasticIsotropicPlaneStress(mdlr,"elast2d",E,nu,rho)
# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultMaterial= "elast2d"
tri31= elementos.newElement("tri31",xc.ID([1,2,3]))
tri31.thickness= t
tri31= elementos.newElement("tri31",xc.ID([3,4,1]))
tri31.thickness= t

# Constraints
coacciones= mdlr.getConstraintLoader

spc= coacciones.newSPConstraint(1,0,0.0)
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(4,0,0.0)
spc= coacciones.newSPConstraint(4,1,0.0)

# Loads definition
cargas= mdlr.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0]))
lp0.newNodalLoad(3,xc.Vector([F,0]))

#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
analOk= analisis.analyze(1)


nodos.calculateNodalReactions(True)
nodos= mdlr.getNodeLoader
 
# print "reac nodo 1: ",reac
R1x= n1.getReaction[0]
R1y= n1.getReaction[1]

# print "reac nodo 5: ",reac
R4x= n4.getReaction[0]
R4y= n4.getReaction[1]

ratio1= abs(((R1x+F)/F))
ratio2= abs(((R4x+F)/F))
ratio3= abs(((R1y+R4y)/F))

''' 
print "R1x= ",R1x
print "R1y= ",R1y
print "ratio1= ",ratio1
print "R4x= ",R4x
print "R4y= ",R4y
print "ratio2= ",ratio2
print "ratio3= ",ratio3
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (analOk == 0.0):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
