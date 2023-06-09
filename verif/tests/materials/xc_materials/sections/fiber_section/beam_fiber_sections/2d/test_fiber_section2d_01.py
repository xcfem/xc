# -*- coding: utf-8 -*-
''' Verification test of a rectangular 2D fiber section with an
    elasto plastic material model.
    written from "Nociones de cálculo plástico." Carlos Benito Hernández.
    page 26 and following.
    url={https://books.google.es/books?id=v7bbMwEACAAJ},
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import xc
from materials.sections import section_properties
from misc import scc2d_testing_bench
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from model import predefined_spaces
from materials import typical_materials

# Rectangular cross-section definition
b= 10 # Cross section width  [cm]
h= 20 # Cross section depth [cm]
scc10x20= section_properties.RectangularSection('scc10x20',b,h)
scc10x20.nDivIJ= 1 # 2D discretization
scc10x20.nDivJK= 32 # number of cells in JK direction

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
sys.path.append(pth+"/../../../../../../aux/")
import fiber_section_test_macros

fy= 2600 # Yield stress of the material expressed in kp/cm2.
E= 2.1e6 # Young’s modulus of the material (kp/cm2).

feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor
# Materials definition
epp= typical_materials.defElasticPPMaterial(preprocessor, "epp",E,fy,-fy) # elastic perfectly-plastic uniaxial material
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e6) # Shear response in y direction.
# Section geometry
# creation
geomRectang= preprocessor.getMaterialHandler.newSectionGeometry("geomRectang")
# generation of a quadrilateral region of the scc10x20 sizes and number of
# divisions made of material nmbMat
reg= scc10x20.getRegion(gm=geomRectang, nmbMat= epp.name, twoDimensionalMember= True)
rectang= preprocessor.getMaterialHandler.newMaterial("fiber_section_2d","rectang")

fiberSectionRepr= rectang.getFiberSectionRepr() # Create fiber representation of the section.
fiberSectionRepr.setGeomNamed(geomRectang.name) # Assign the geometry.
rectang.setupFibers()
fibers= rectang.getFibers()

fiber_section_test_macros.extractFiberSectionProperties(rectang,scc10x20, fy)
curvM= 0.005
rectang.setTrialSectionDeformation(xc.Vector([0.0,curvM,0.0]))
rectang.commitState()
Mp1= rectang.getStressResultantComponent("Mz")
rectang.revertToStart()

matStiffnessMatrix= rectang.getTangentStiffness()
EA= matStiffnessMatrix(0,0)
EARef= fiber_section_test_macros.sumAreas*E
EI= matStiffnessMatrix(1,1)
EIRef= scc10x20.Iz()*E

# Check quantities:
referenceCenterOfMassY= 0.0
referenceCenterOfMassZ= 0.0
ratio1= ((fiber_section_test_macros.sumAreas-scc10x20.A())/scc10x20.A())
ratio2= (fiber_section_test_macros.centerOfMassY-referenceCenterOfMassY)
ratio3= (fiber_section_test_macros.centerOfMassZ-referenceCenterOfMassZ)
ratio4= ((fiber_section_test_macros.I1-scc10x20.Iz())/scc10x20.Iz())
ratio5= (fiber_section_test_macros.i1-scc10x20.iz())/scc10x20.iz()
ratio6= (fiber_section_test_macros.Me1-scc10x20.getYieldMomentZ(fy))/scc10x20.getYieldMomentZ(fy)
ratio7= (fiber_section_test_macros.SzPosG-scc10x20.getPlasticSectionModulusZ())/scc10x20.getPlasticSectionModulusZ()
ratio8= ((scc10x20.getPlasticMomentZ(fy)-Mp1)/scc10x20.getPlasticMomentZ(fy))
ratio9= abs(EA-EARef)/EARef
ratio10= abs(EI-EIRef)/EIRef

'''
print('EA= ', EA)
print('EARef= ', EARef)
print('EI= ', EI)
print('EIRef= ', EIRef)
print('I1= ', I1)
print('Iz= ', scc10x20.Iz())
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
print('ratio4= ', ratio4)
print('ratio5= ', ratio5)
print('ratio6= ', ratio6)
print('ratio7= ', ratio7)
print('ratio8= ', ratio8)
print('ratio9= ', ratio9)
print('ratio10= ', ratio10)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-2) & (abs(ratio5)<1e-2) & (abs(ratio6)<1e-2) & (abs(ratio7)<1e-12) & (abs(ratio8)<1e-12) & (abs(ratio9)<1e-12) & (abs(ratio10)<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
