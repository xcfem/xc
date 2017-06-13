# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

FX= 1.0 # Force magnitude
FY= 2.0 
FZ= 3.0
MX= 4.0
MY= 5.0 
MZ= 6.0

import xc_base
import geom
import xc

from model import predefined_spaces
from materials import bridge_bearings
from solution import predefined_solutions



# Materials definition
G= 900e3 # Módulo de cortante del neopreno
a= 0.3 # Dimension parallel to deck axis.
b= 0.3 # Dimension normal to deck axis.
e= 0.002 # Espesor neto del neopreno (sin chapas).


# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

neop= bridge_bearings.ElastomericBearing(G,a,b,e)
neop.defineMaterials(preprocessor)

materiales= preprocessor.getMaterialLoader
KX= neop.getMaterial(0).E
KY= neop.getMaterial(1).E
KZ= neop.getMaterial(2).E
KTHX= neop.getMaterial(3).E
KTHY= neop.getMaterial(4).E
KTHZ= neop.getMaterial(5).E

nodes= preprocessor.getNodeLoader
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod1= nodes.newNodeXYZ(1,1,1)
nod2= nodes.newNodeXYZ(1,1,1)



zl= neop.putBetweenNodes(modelSpace, nod1.tag,nod2.tag)

''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Constraints
constraints= preprocessor.getConstraintLoader
#
spc= constraints.newSPConstraint(nod1.tag,0,0.0) # Nodo 1
spc= constraints.newSPConstraint(nod1.tag,1,0.0)
spc= constraints.newSPConstraint(nod1.tag,2,0.0)
spc= constraints.newSPConstraint(nod1.tag,3,0.0)
spc= constraints.newSPConstraint(nod1.tag,4,0.0)
spc= constraints.newSPConstraint(nod1.tag,5,0.0)


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(nod2.tag,xc.Vector([FX,FY,FZ,MX,MY,MZ]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
deltax= nod2.getDisp[0]
deltay= nod2.getDisp[1]
deltaz= nod2.getDisp[2] 
RX= nod1.getReaction[0]
RY= nod1.getReaction[1]
RZ= nod1.getReaction[2] 


elementos= preprocessor.getElementLoader


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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4-1.0)<1e-5)  & (abs(ratio5)<1e-5) & (abs(ratio6-1.0)<1e-3) & (abs(ratio7)<1e-15) & (abs(ratio8)<1e-15) & (abs(ratio9)<1e-15) & (abs(ratio10)<1e-15) & (abs(ratio11)<1e-15) & (abs(ratio12)<1e-15) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
