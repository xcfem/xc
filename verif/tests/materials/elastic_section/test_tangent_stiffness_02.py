# -*- coding: utf-8 -*-
''' Verifies the assembly of cross-section tangent stiffness matrix.
   Home made test.'''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from materials.sections import def_secc_aggregation


h= 0.30 # Beam cross-section depth.
b= 0.2 # Beam cross-section width.
E= 2e6 # Elastic modulus

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler

sectionTest= section_properties.RectangularSection("sectionTest",b,h) # Section geometry.
sectionTestMaterial=typical_materials.MaterialData(name='sectionTestMaterial',E=E,nu=0.3,rho=2500) # Section material.

# Define materials
def_secc_aggregation.def_secc_aggregation2d(preprocessor, sectionTest,sectionTestMaterial)
tang= preprocessor.getMaterialHandler.getMaterial("sectionTest").getTangentStiffness()
EI= tang.at(1,1)
EA= tang.at(0,0)




EITeor= (1/12.0*b*h**3*E)
EITeor2= sectionTestMaterial.E*sectionTest.Iz()
ratio1= ((EI-EITeor)/EITeor)
EATeor= (b*h*E)
ratio2= ((EA-EATeor)/EATeor)

''' 
print "EI= ",EI
print "E= ", sectionTestMaterial.E
print "I= ", sectionTest.Iz()
print "EITeor= ",EITeor
print "EITeor2= ",EITeor2
print "ratio1= ",ratio1
print "EA= ",EA
print "EATeor= ",EATeor
print "ratio2= ",ratio2
   '''

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-2) & (abs(ratio2)<1e-2):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
