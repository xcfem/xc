# -*- coding: utf-8 -*-

# home made test
K= 1000 # Spring constant
l= 1e-7 # Distancia entre nodos
F= 1 # Force magnitude

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials

# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
nodos.dimEspace= 1
nodos.numGdls= 1

nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(1,0)
nod= nodos.newNodeXY(1.0+l,0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",K)

''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "elast"
elementos.dimElem= 1
elementos.defaultTag= 1
zl= elementos.newElement("zero_length",xc.ID([1,2]))

''' 
print "dimensión: ",getDim
print "filas trf: ",getTrf.nrows
print "cols. trf: ",getTrf.ncols
print "trf[0]: ","getTrf[0,0]"," ","getTrf[0,1]"," ","getTrf[0,2]"
print "trf[1]: ","getTrf[1,0]"," ","getTrf[1,1]"," ","getTrf[1,2]"
print "trf[2]: ","getTrf[2,0]"," ","getTrf[2,1]"," ","getTrf[2,2]"
   '''

    
# Constraints
coacciones= preprocessor.getConstraintLoader
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
nl= lp0.newNodalLoad(2,xc.Vector([F]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodos.calculateNodalReactions(True)
nodos= preprocessor.getNodeLoader
nod2= nodos.getNode(2)
deltax= nod2.getDisp[0] 
nod1= nodos.getNode(1)
R= nod1.getReaction[0] 

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()



ratio1= -R/F
ratio2= abs(((K*deltax)-F)/F)

''' 
print "R= ",R
print "dx= ",deltax
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
   '''
  
import os
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (ratio2<1e-11) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
