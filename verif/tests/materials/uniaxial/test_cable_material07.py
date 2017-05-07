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

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
l= 10 # Cable length in inches
sigmaPret= 1000 # Prestressing force final (pounds)
sigmaPretA= sigmaPret/2 # Prestressing force parcial (pounds)
area= 2
fPret= sigmaPret*area # Force magnitude de tesado final
fPretA= fPret/2 # Force magnitude de tesado parcial

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
mat= typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPretA,0.0)

    
''' Se definen nodos en los puntos de aplicación de
la carga. Puesto que no se van a determinar tensiones
se emplea una sección arbitraria de área unidad '''

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "cable"
elementos.dimElem= 2 # Dimension of element space
#  sintaxis: truss[<tag>] 
elementos.defaultTag= 1 #Tag for the next element.
truss= elementos.newElement("truss",xc.ID([1,2]));
truss.area= area
    
# Constraints
constraints= preprocessor.getConstraintLoader
#
spc= constraints.newSPConstraint(1,0,0.0) # Nodo 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(2,0,0.0) # Nodo 2
spc= constraints.newSPConstraint(2,1,0.0)



# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
R1A= nodes.getNode(2).getReaction[0] 
R2A= nodes.getNode(1).getReaction[0] 




ratio1A= ((R1A-fPretA)/fPretA)
ratio2A= ((R2A+fPretA)/fPretA)

elementos= preprocessor.getElementLoader
elem1= elementos.getElement(1)
elem1.getMaterial().prestress= sigmaPret
#            \material{\prestress{sigmaPret}}




# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True)
R1B= nodes.getNode(2).getReaction[0] 
R2B= nodes.getNode(1).getReaction[0]




ratio1B= ((R1B-fPret)/fPret)
ratio2B= ((R2B+fPret)/fPret)

''' 
print "R1A= ",R1A
print "R2A= ",R2A
print "ratio1A= ",(ratio1A)
print "ratio2A= ",(ratio2A)
print "R1B= ",R1B
print "R2B= ",R2B
print "ratio1B= ",(ratio1B)
print "ratio2B= ",(ratio2B)
   '''
    
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1A)<1e-5) & (abs(ratio2A)<1e-5) & (abs(ratio1B)<1e-5) & (abs(ratio2B)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
