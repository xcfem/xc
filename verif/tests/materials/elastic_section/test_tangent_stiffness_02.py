# -*- coding: utf-8 -*-
''' Verifica la formación de la matriz de rigidez tangente de la sección.
   Elaboración propia. '''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials import section_properties
from materials import defSeccAggregation


h= 0.30 # Beam cross-section depth.
b= 0.2 # Beam cross-section width.
E= 2e6 # Elastic modulus

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader

seccPrueba= section_properties.RectangularSection("prueba",b,h)
matseccPrueba=typical_materials.MaterialData(name='mt',E=E,nu=0.3,rho=2500)

# Define materials
defSeccAggregation.defSeccAggregation2d(preprocessor, seccPrueba,matseccPrueba)
matPrueba= preprocessor.getMaterialLoader.getMaterial("prueba")
tang= matPrueba.getTangentStiffness()
EI= tang.at(1,1)
EA= tang.at(0,0)




EITeor= (1/12.0*b*h**3*E)
EITeor2= matseccPrueba.E*seccPrueba.Iz()
ratio1= ((EI-EITeor)/EITeor)
EATeor= (b*h*E)
ratio2= ((EA-EATeor)/EATeor)

''' 
print "EI= ",EI
print "E= ", matseccPrueba.E
print "I= ", seccPrueba.Iz()
print "EITeor= ",EITeor
print "EITeor2= ",EITeor2
print "ratio1= ",ratio1
print "EA= ",EA
print "EATeor= ",EATeor
print "ratio2= ",ratio2
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-2) & (abs(ratio2)<1e-2):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
