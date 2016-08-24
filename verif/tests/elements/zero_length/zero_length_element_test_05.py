# -*- coding: utf-8 -*-
''' home made test. Verifica el buen comportamiento del
 elemento «ZeroLength» cuando no está alineado con los ejes globales. '''
K= 1000 # Spring constant
F= 1 # Force magnitude

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodes)

# Model definition
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(1,1)
nod= nodes.newNodeXY(1,1)

# Materials definition
k= typical_materials.defElasticMaterial(preprocessor, "k",K)


''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "k"
elementos.dimElem= 3
zl= elementos.newElement("zero_length",xc.ID([1,2]))
zl.clearMaterials()
zl.setupVectors(xc.Vector([0,1,0]),xc.Vector([-1,0,0]))
zl.setMaterial(0,"k")


''' 
print "Tipo elemento: ",getTipoElemento
print "Núm. materiales: ",getNumMateriales
print "Vector x local: ",getVectorI
print "Vector y local: ",getVectorJ
print "Vector z local: ",getVectorK
print "dimensión: ",getDim
print "filas trf: ",getTrf.nrows
print "cols. trf: ",getTrf.ncols
   '''

# Constraints
coacciones= preprocessor.getConstraintLoader
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(1,2,0.0)
spc= coacciones.newSPConstraint(2,0,0.0) # Nodo 2
spc= coacciones.newSPConstraint(2,2,0.0) # Nodo 2


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,F,0]))

#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
nod2= nodes.getNode(2)
deltax= nod2.getDisp[0]
deltay= nod2.getDisp[1]
deltat= nod2.getDisp[2] 
nod1= nodes.getNode(1)
RX= nod1.getReaction[0]
RY= nod1.getReaction[1]
RT= nod1.getReaction[2] 


ratio1= -RX/F
ratio2= (K*deltax)/F
ratio3= -RY/F
ratio4= (K*deltay)/F
ratio5= -RT/F
ratio6= (K*deltat)/F

''' 
print "RX= ",RX
print "dx= ",deltax
print "RY= ",RY
print "dy= ",deltay
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)
print "ratio6= ",(ratio6)
   '''
  
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3-1.0)<1e-12) & (abs(ratio4-1.0)<1e-12)  & (abs(ratio5)<1e-12) & (abs(ratio6)<1e-12) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
