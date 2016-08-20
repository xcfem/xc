# -*- coding: utf-8 -*-
# Test from Ansys manual
# Reference:  Strength of Material, Part I, Elementary Theory and Problems, pg. 26, problem 10

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math


E= 30e6 # Young modulus (psi)
l= 10 # Cable length in inches
sigmaPret= 1500 # Prestressing force (pounds)
area= 2
fPret= sigmaPret*area # Prestressing force (pounds)
F= 100 # Prestressing force (pounds)

# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader

# Problem type
predefined_spaces.gdls_elasticidad2D(nodos)


nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(0,0)
nod= nodos.newNodeXY(l/2,0.0)
nod= nodos.newNodeXY(l,0.0)

# Materials definition
mat= typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPret,0.0)
    
''' Se definen nodos en los puntos de aplicación de
la carga. Puesto que no se van a determinar tensiones
se emplea una sección arbitraria de área unidad '''
    
# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "cable"
elementos.dimElem= 2
elementos.defaultTag= 1 #First node number.
truss1= elementos.newElement("corot_truss",xc.ID([1,2]));
truss1.area= area
truss2= elementos.newElement("corot_truss",xc.ID([2,3]));
truss2.area= area
    
# Constraints
coacciones= preprocessor.getConstraintLoader
#
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(3,0,0.0) # Nodo 3
spc= coacciones.newSPConstraint(3,1,0.0)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lPattern= "0"
lp0= casos.newLoadPattern("default",lPattern)
#casos.currentLoadPattern= lPattern
lp0.newNodalLoad(2,xc.Vector([0,-F]))
casos.addToDomain(lPattern)

# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
result= analisis.analyze(10)


nodos.calculateNodalReactions(True)
nodos= preprocessor.getNodeLoader
nod3= nodos.getNode(3)
R1X= nod3.getReaction[0]
R1Y= nod3.getReaction[1] 
nod1= nodos.getNode(1)
R2X= nod1.getReaction[0]
R2Y= nod1.getReaction[1] 
nod2= nodos.getNode(2)
deltaX= nod2.getDisp[0]
deltaY= nod2.getDisp[1]  

ele1= elementos.getElement(1)
tension= ele1.getN()


alpha= -math.atan2(deltaY,l/2)
tensTeor= F/(2*math.sin(alpha))
ratio1= ((R1X+R2X)/fPret)
ratio2= ((tension-tensTeor)/tensTeor)
ratio3= ((R1Y-F/2.0)/(F/2))
ratio4= ((R2Y-F/2.0)/(F/2))
    
''' 
print "alpha= ",rad2deg((alpha))
print "R1X= ",R1X
print "R1Y= ",R1Y
print "R2X= ",R2X
print "R2Y= ",R2Y
print "deltaX= ",deltaX
print "deltaY= ",deltaY
print "tensTeor= ",(tensTeor)
print "tension= ",(tension)
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
   '''
    
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-11) & (abs(ratio2)<1e-11) & (abs(ratio3)<1e-11) & (abs(ratio4)<1e-11):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
