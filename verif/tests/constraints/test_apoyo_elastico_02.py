# -*- coding: utf-8 -*-
# Home made test
KX= 1000 # Spring constant
KY= 2000 # Spring constant
FX= 1 # Magnitude of force
FY= 2 
FZ= 3

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import define_apoyos
from materials import typical_materials

# Problem type
# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(1,1,1)

    
# Define materials
kx= typical_materials.defElasticMaterial(preprocessor, "kx",KX)
ky= typical_materials.defElasticMaterial(preprocessor, "ky",KY)


tagNodoFijo= define_apoyos.defApoyoXYRigZ(preprocessor, nod.tag,7,"kx","ky")
  
# Constraints
coacciones= preprocessor.getConstraintLoader

#
spc= coacciones.newSPConstraint(nod.tag,3,0.0) # nod1 Rx= 0,Ry= 0 and Rz= 0
spc= coacciones.newSPConstraint(nod.tag,4,0.0)
spc= coacciones.newSPConstraint(nod.tag,5,0.0)


# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(nod.tag,xc.Vector([FX,FY,FZ,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodes.calculateNodalReactions(False)
nodes= preprocessor.getNodeLoader

deltax= nod.getDisp[0]
deltay= nod.getDisp[1]
deltaz= nod.getDisp[2] 
RX= nodes.getNode(tagNodoFijo).getReaction[0]
RY= nodes.getNode(tagNodoFijo).getReaction[1] 
RZ= nodes.getNode(tagNodoFijo).getReaction[2] 


ratio1= (FX+RX)/FX
ratio2= (FX-KX*deltax)/FX
ratio3= (FY+RY)/FY
ratio4= (FY-KY*deltay)/FY
ratio5= (FZ+RZ)/FZ
ratio6= deltaz

''' 
print "RX= ",RX
print "dx= ",deltax
print "RY= ",RY
print "dy= ",deltay
print "RZ= ",RZ
print "dz= ",deltaz
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)
print "ratio6= ",(ratio6)
   '''
  
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5)  & (abs(ratio5)<1e-5) & (abs(ratio6)<1e-5) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
