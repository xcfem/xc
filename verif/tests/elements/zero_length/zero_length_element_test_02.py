# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

KX= 1000 # Spring constant
KY= 2000 # Spring constant
l= 1e-7 # Distancia entre nodos
FX= 1 # Force magnitude
FY= 2 # Force magnitude

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Model definition

nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(1,1)
nod= nodes.newNodeXY(1,1)

# Materials definition
kx= typical_materials.defElasticMaterial(preprocessor, "kx",KX)
ky= typical_materials.defElasticMaterial(preprocessor, "ky",KY)


''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "kx"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1
elem= elements.newElement("zero_length",xc.ID([1,2]))
elem.clearMaterials()
elem.setMaterial(0,"kx")
elem.setMaterial(1,"ky")

    
# Constraints
constraints= preprocessor.getConstraintLoader
#
spc= constraints.newSPConstraint(1,0,0.0) # Nodo 1
spc= constraints.newSPConstraint(1,1,0.0)


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([FX,FY]))
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
nod1= nodes.getNode(1)
RX= nod1.getReaction[0]
RY= nod1.getReaction[1] 

elements= preprocessor.getElementLoader

elem1= elements.getElement(1)
elem1.getResistingForce()

ratio1= -RX/FX
ratio2= (KX*deltax)/FX
ratio3= -RY/FY
ratio4= (KY*deltay)/FY

''' 
print "RX= ",RX
print "dx= ",deltax
print "RY= ",RY
print "dy= ",deltay
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
   '''
  
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4-1.0)<1e-5) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
