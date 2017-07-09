# -*- coding: utf-8 -*-
''' Test made from the example 7.2 of the book: 
    Introducción Al Estudio Del Elemento Finito en Ingeniería. 
    Segunda Edición. Tirupathi R. Chandrupatla, Ashok D. Belegundu. 
    Pearson Educación, 1999
    isbn={9789701702604},
    url={https://books.google.ch/books?id=auQwAAAACAAJ},
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

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
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


n1= nodes.newNodeIDXY(1,0,0)
n2= nodes.newNodeIDXY(2,1,0)
n3= nodes.newNodeIDXY(3,1,1)
n4= nodes.newNodeIDXY(4,0,1)


elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "elast2d"
tri31= elements.newElement("tri31",xc.ID([1,2,3]))
tri31.thickness= t
tri31= elements.newElement("tri31",xc.ID([3,4,1]))
tri31.thickness= t

# Constraints
constraints= preprocessor.getConstraintLoader

spc= constraints.newSPConstraint(1,0,0.0)
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(4,0,0.0)
spc= constraints.newSPConstraint(4,1,0.0)

# Loads definition
cargas= preprocessor.getLoadLoader

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


nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
 
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (analOk == 0.0):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
