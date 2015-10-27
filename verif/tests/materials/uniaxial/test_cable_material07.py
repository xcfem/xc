# -*- coding: utf-8 -*-
# Test tomado del manual de Ansys
# Referencia:  Strength of Material, Part I, Elementary Theory and Problems, pg. 26, problem 10

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
from model import fix_node_6dof

E= 30e6 # Young modulus (psi)
l= 10 # Longitud del cable en pulgadas
sigmaPret= 1000 # Magnitud del pretensado final en libras
sigmaPretA= sigmaPret/2 # Magnitud del pretensado parcial en libras
area= 2
fPret= sigmaPret*area # Magnitud de la fuerza de tesado final
fPretA= fPret/2 # Magnitud de la fuerza de tesado parcial

# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
# Problem type
predefined_spaces.gdls_elasticidad2D(nodos)


nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(0,0)
nod= nodos.newNodeXY(l,0.0)

# Materials definition
mat= typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPretA,0.0)

    
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



nodos.calculateNodalReactions(True)
nodos= preprocessor.getNodeLoader
R1A= nodos.getNode(2).getReaction[0] 
R2A= nodos.getNode(1).getReaction[0] 




ratio1A= ((R1A-fPretA)/fPretA)
ratio2A= ((R2A+fPretA)/fPretA)

elementos= preprocessor.getElementLoader
elem1= elementos.getElement(1)
elem1.getMaterial().prestress= sigmaPret
#            \material{\prestress{sigmaPret}}




# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodos.calculateNodalReactions(True)
R1B= nodos.getNode(2).getReaction[0] 
R2B= nodos.getNode(1).getReaction[0]




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
fname= os.path.basename(__file__)
if (abs(ratio1A)<1e-5) & (abs(ratio2A)<1e-5) & (abs(ratio1B)<1e-5) & (abs(ratio2B)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
