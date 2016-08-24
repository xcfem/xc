# -*- coding: utf-8 -*-
# home made test
FX= 1.0 # Force magnitude
FY= 2.0 
FZ= 3.0
MX= 4.0
MY= 5.0 
MZ= 6.0

import xc_base
import geom
import xc

from model import fix_node_6dof
from materials import apoyosNeopreno
from model import predefined_spaces
from model import define_apoyos
from model import apoyo_pot
from solution import predefined_solutions



# Materials definition
G= 900e3 # Módulo de cortante del neopreno
a= 0.3 # Dimensión paralela al eje del dintel.
b= 0.3 # Dimensión normal al eje del dintel.
e= 0.002 # Espesor neto del neopreno (sin chapas).


# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

neop= apoyosNeopreno.defineMaterialesNeopreno(preprocessor, G,a,b,e,"neopX","neopY","neopZ","neopTHX","neopTHY","neopTHZ")

materiales= preprocessor.getMaterialLoader
KX= materiales.getMaterial("neopX").E
KY= materiales.getMaterial("neopY").E
KZ= materiales.getMaterial("neopZ").E
KTHX= materiales.getMaterial("neopTHX").E
KTHY= materiales.getMaterial("neopTHY").E
KTHZ= materiales.getMaterial("neopTHZ").E

nodes= preprocessor.getNodeLoader
# Problem type
predefined_spaces.gdls_resist_materiales3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(1,1,1)
nod= nodes.newNodeXYZ(1,1,1)



define_apoyos.colocaApoyoEntreNodos(preprocessor, 1,2,1,"neopX","neopY","neopZ","neopTHX","neopTHY","neopTHZ")

''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Constraints
coacciones= preprocessor.getConstraintLoader
#
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(1,2,0.0)
spc= coacciones.newSPConstraint(1,3,0.0) # Nodo 2
spc= coacciones.newSPConstraint(1,4,0.0)
spc= coacciones.newSPConstraint(1,5,0.0)


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([FX,FY,FZ,MX,MY,MZ]))
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
deltaz= nod2.getDisp[2] 
nod1= nodes.getNode(1)
RX= nod1.getReaction[0]
RY= nod1.getReaction[1]
RZ= nod1.getReaction[2] 


elementos= preprocessor.getElementLoader
zl= elementos.getElement(1)

''' 
print "Tipo elemento: ",getTipoElemento
print "Núm. materiales: ",getNumMateriales
print "dimensión: ",getDim
print "Vector x local: ",getIVector
print "Vector y local: ",getJVector
print "Vector z local: ",getKVector
print "filas trf: ",getTrf.nrows
print "cols. trf: ",getTrf.ncols
print "trf[0]: ","getTrf[0,0]"," ","getTrf[0,1]"," ","getTrf[0,2]"
print "trf[1]: ","getTrf[1,0]"," ","getTrf[1,1]"," ","getTrf[1,2]"
print "trf[2]: ","getTrf[2,0]"," ","getTrf[2,1]"," ","getTrf[2,2]"
   '''

VX= zl.getMaterials()[0].getStress()
VY= zl.getMaterials()[1].getStress()
N= zl.getMaterials()[2].getStress()
T= zl.getMaterials()[3].getStress()
momY= zl.getMaterials()[4].getStress()
momZ= zl.getMaterials()[5].getStress()

ratio1= ((FX+RX)/FX)
ratio2= (KX*deltax)/FX
ratio3= ((FY+RY)/FY)
ratio4= (KY*deltay)/FY
ratio5= ((FZ+RZ)/FZ)
ratio6= (KZ*deltaz)/FZ
ratio7= ((FX-VX)/FX)
ratio8= ((FY-VY)/FY)
ratio9= ((FZ-N)/FZ)
ratio10= ((MX-T)/MX)
ratio11= ((MY-momY)/MY)
ratio12= ((MZ-momZ)/MZ)

''' 
print "KX= ",KX
print "RX= ",RX
print "dx= ",deltax,"\n\n"

print "KY= ",KY
print "RY= ",RY
print "dy= ",deltay,"\n\n"

print "KZ= ",KZ
print "RZ= ",RZ
print "dz= ",deltaz,"\n\n"

print "VX= ",VX
print "VY= ",VY
print "N= ",N
print "T= ",T
print "momY= ",momY
print "momZ= ",momZ,"\n\n"

print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)
print "ratio6= ",(ratio6)
print "ratio7= ",(ratio7)
print "ratio8= ",(ratio8)
print "ratio9= ",(ratio9)
print "ratio10= ",(ratio10)
print "ratio11= ",(ratio11)
print "ratio12= ",(ratio12)
   '''
  
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4-1.0)<1e-5)  & (abs(ratio5)<1e-5) & (abs(ratio6-1.0)<1e-3) & (abs(ratio7)<1e-15) & (abs(ratio8)<1e-15) & (abs(ratio9)<1e-15) & (abs(ratio10)<1e-15) & (abs(ratio11)<1e-15) & (abs(ratio12)<1e-15) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
