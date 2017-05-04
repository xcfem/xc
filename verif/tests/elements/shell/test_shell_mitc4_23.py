# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 2.0 # Lado del elemento.
t= 0.2 # Canto del elemento.
E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
# Load
F= 10 # Load magnitude

import xc_base
import geom
import xc
from model import fix_node_6dof
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.newNodeIDXYZ(1,0,0,0)
nodes.newNodeIDXYZ(2,L,0,0)
nodes.newNodeIDXYZ(3,L,L,0)
nodes.newNodeIDXYZ(4,0,L,0)


# Materials definition
nmb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,t)

elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "memb1"
elementos.defaultTag= 1
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,4]))


# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 1)
spc= coacciones.newSPConstraint(2,2,0.0)
spc= coacciones.newSPConstraint(3,2,0.0)
spc= coacciones.newSPConstraint(4,0,0.0)
spc= coacciones.newSPConstraint(4,2,0.0)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,0,0]))
lp0.newNodalLoad(3,xc.Vector([F,0,0,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")



# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


n1MedioElem= 0.0
n2MedioElem= 0.0
n12MedioElem= 0.0

elem.getResistingForce()
mats= elem.getPhysicalProperties.getVectorMaterials #Materials at gauss points.
#Edge iterator
for m in mats:
  n1MedioElem= n1MedioElem+m.getStressResultantComponent("n1")
  n2MedioElem= n2MedioElem+m.getStressResultantComponent("n2")
  n12MedioElem= n12MedioElem+m.getStressResultantComponent("n12")

n1MedioElem= n1MedioElem/4
n2MedioElem= n2MedioElem/4

n1MedioElemTeor= 2*F/L
n2MedioElemTeor= 0.0
n12MedioElemTeor= 0.0
ratio1= abs((n1MedioElem-n1MedioElemTeor)/n1MedioElemTeor)
ratio2= abs((n2MedioElem-n2MedioElemTeor))
ratio3= abs((n12MedioElem-n12MedioElemTeor))

''' 
print "n1MedioElem= ",n1MedioElem
print "n1MedioElemTeor= ",n1MedioElemTeor
print "ratio1= ",ratio1
print "n2MedioElem= ",n2MedioElem
print "n2MedioElemTeor= ",n2MedioElemTeor
print "ratio2= ",ratio2
print "n12MedioElem= ",n12MedioElem
print "n12MedioElemTeor= ",n12MedioElemTeor
print "ratio3= ",ratio3
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
