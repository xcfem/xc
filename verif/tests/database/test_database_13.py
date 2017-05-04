# -*- coding: utf-8 -*-
# Test from Ansys manual
# Reference:  Strength of Material, Part I, Elementary Theory and Problems, pg. 26, problem 10
import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
from model import fix_node_6dof

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
l= 10 # Cable length in inches
sigmaPret= 1000.0 # Prestressing force (pounds)
area= 2.0
fPret= sigmaPret*area # Prestressing force (pounds)

# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(l,0.0)

# Materials definition
mat= typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPret,0.0)
    
''' Se definen nodos en los puntos de aplicación de
la carga. Puesto que no se van a determinar tensiones
se emplea una sección arbitraria de área unidad '''

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "cable"
elementos.dimElem= 2
#  sintaxis: truss[<tag>] 
elementos.defaultTag= 1 #Tag for the next element.
truss= elementos.newElement("truss",xc.ID([1,2]));
truss.area= area
    
# Constraints
coacciones= preprocessor.getConstraintLoader
#
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(2,0,0.0) # Nodo 2
spc= coacciones.newSPConstraint(2,1,0.0)


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

import os
os.system("rm -r -f /tmp/test13.db")
db= prueba.newDatabase("BerkeleyDB","/tmp/test13.db")
db.save(100)
prueba.clearAll()
db.restore(100)


nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
R1= nodes.getNode(2).getReaction[0] 
R2= nodes.getNode(1).getReaction[0] 


ratio1= ((R1-fPret)/fPret)
ratio2= ((R2+fPret)/fPret)
    
''' 
print "R1= ",R1
print "R2= ",R2
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
   '''

fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

os.system("rm -rf /tmp/test13.db") # Your garbage you clean it
