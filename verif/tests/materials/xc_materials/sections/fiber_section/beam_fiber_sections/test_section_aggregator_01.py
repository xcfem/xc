# -*- coding: utf-8 -*-
''' Section aggregator class verification test. Checking the response for trivial load cases.'''

from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import xc
from materials.sections import section_properties
from misc import scc3d_testing_bench
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Auxiliary modules.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
sys.path.append(pth+"/../../../../../aux/")
import fiber_section_test_macros # fiber section convenience functions.

# Define section geometry
## Rectangular cross-section definition
b= 10 # Cross section width  [cm]
h= 20 # Cross section depth [cm]
scc10x20= section_properties.RectangularSection('scc10x20',b,h)
scc10x20.nDivIJ= 32 # number of cells in IJ direction  
scc10x20.nDivJK= 32 # number of cells in JK direction

# Define XC problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Materials definition
fy= 2600 # yield stress [kp/cm2]
E= 2.1e6 # initial elastic tangent [kp/cm2.
epp= typical_materials.defElasticPPMaterial(preprocessor, "epp",E,fy,-fy)
respT= typical_materials.defElasticMaterial(preprocessor, "respT",1e10) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e6) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e3) # Shear response in y direction.

# Define section geometry
materialHandler= preprocessor.getMaterialHandler
geomRectang= preprocessor.getMaterialHandler.newSectionGeometry("geomRectang")

reg= scc10x20.getRegion(geomRectang,"epp")
rectang= materialHandler.newMaterial("fiber_section_3d","rectang")
fiberSectionRepr= rectang.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(geomRectang.name)
rectang.setupFibers()
fiber_section_test_macros.extractFiberSectionProperties(rectang,scc10x20, fy)
# Define section aggregator
sa= materialHandler.newMaterial("section_aggregator","sa")
sa.setSection("rectang")
sa.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])

# Define FE mesh.
zlElement, nodA, nodB= scc3d_testing_bench.sectionModel(preprocessor, sa.name)
# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')

loadVy= 2e4
loadVz= 3e4
loadMx= 1e3
loadMz= 0.999*scc10x20.getPlasticMomentZ(fy)
lp0.newNodalLoad(nodB.tag,xc.Vector([0,loadVy,loadVz,loadMx,0,loadMz]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
solProc= predefined_solutions.PlainNewtonRaphson(feProblem, convergenceTestTol= 1e-8)
analOk= solProc.solve(True)
if(analOk!=0):
    lmsg.error('Failed to solve for: '+lp0.name)
    quit()

nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)

RVy= nodA.getReaction[1] 
RVz= nodA.getReaction[2] 
RMx= nodA.getReaction[3] 
RMz= nodA.getReaction[5] 

scc= zlElement.getSection()
esfVy= scc.getStressResultantComponent("Vy")
esfVz= scc.getStressResultantComponent("Vz")
esfMx= scc.getStressResultantComponent("T")
esfMz= scc.getStressResultantComponent("Mz")



referenceCenterOfMassY= 0.0
referenceCenterOfMassZ= 0.0
ratio1= (fiber_section_test_macros.sumAreas-scc10x20.A())/scc10x20.A()
ratio2= rectang.getCenterOfMassY()-referenceCenterOfMassY
ratio3= fiber_section_test_macros.centerOfMassZ-referenceCenterOfMassZ
ratio4= (fiber_section_test_macros.I1-scc10x20.Iz())/scc10x20.Iz()
ratio5= (fiber_section_test_macros.I2-scc10x20.Iy())/scc10x20.Iy()
ratio6= (fiber_section_test_macros.i1-scc10x20.iz())/scc10x20.iz()
ratio7= (fiber_section_test_macros.i2-scc10x20.iy())/scc10x20.iy()
ratio8= (fiber_section_test_macros.Me1-scc10x20.getYieldMomentZ(fy))/scc10x20.getYieldMomentZ(fy)
ratio9= (fiber_section_test_macros.Me2-scc10x20.getYieldMomentY(fy))/scc10x20.getYieldMomentY(fy)
ratio10= (fiber_section_test_macros.SzPosG-scc10x20.getPlasticSectionModulusZ())/scc10x20.getPlasticSectionModulusZ()
ratio11= (fiber_section_test_macros.SyPosG-scc10x20.getPlasticSectionModulusY())/scc10x20.getPlasticSectionModulusY()
ratio12= (RMz+loadMz)/loadMz
ratio13= (esfMz-loadMz)/loadMz

ratio14= (RMx+loadMx)/loadMx
ratio15= (esfMx-loadMx)/loadMx

ratio16= (RVy+loadVy)/loadVy
ratio17= (esfVy-loadVy)/loadVy

ratio18= (RVz+loadVz)/loadVz
ratio19= (esfVz-loadVz)/loadVz

''' 
print("RVy= ",(RVy))
print("esfVy= ",(esfVy))
print("loadVy= ",(loadVy))

print("RVz= ",(RVz))
print("esfVz= ",(esfVz))
print("loadVz= ",(loadVz))

print("RMx= ",(RMx))
print("esfMx= ",(esfMx))
print("loadMx= ",(loadMx))

print("RMz= ",(RMz))
print("esfMz= ",(esfMz))
print("loadMz= ",(loadMz))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-3) & (abs(ratio5)<1e-2) & (abs(ratio6)<1e-3) & (abs(ratio7)<1e-2) & (abs(ratio8)<1e-3) & (abs(ratio9)<1e-3) & (abs(ratio10)<1e-5) & (abs(ratio11)<1e-5) & (abs(ratio12)<1e-5) &  (abs(ratio13)<1e-5) & (abs(ratio14)<1e-5) & (abs(ratio15)<1e-5) & (abs(ratio16)<1e-5) & (abs(ratio17)<1e-5) & (abs(ratio18)<1e-5) & (abs(ratio19)<1e-5) & (analOk==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
