# -*- coding: utf-8 -*-
'''Prestressed concrete section verification test.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import xc
from misc import scc3d_testing_bench
from materials.ehe import EHE_materials
from materials.sections.fiber_section import fiber_sets
from model import predefined_spaces
from solution import predefined_solutions

MzDato= 0.0
NDato= 0.0 # Presstressing is the only load

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
tag= EHE_materials.Y1860S7.defDiagD(preprocessor, EHE_materials.Y1860S7.tInic())
tag= EHE_materials.HP45.defDiagD(preprocessor)

# Create RC section model.
pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
sys.path.append(pth+"/../../../../../../aux/")
import prestressed_concrete_section_01
geomSecPret01= prestressed_concrete_section_01.gmSecHP01(preprocessor, "prestressedConcretSectionGeom01",EHE_materials.HP45.nmbDiagD,EHE_materials.Y1860S7.nmbDiagD)
materialHandler= preprocessor.getMaterialHandler
secHP= materialHandler.newMaterial("fiber_section_3d","secHP")
fiberSectionRepr= secHP.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(geomSecPret01.name)
secHP.setupFibers()

zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, "secHP")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_00F(nodB.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag,xc.Vector([NDato,0,0,0,0,MzDato]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(10)


nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)

RN= nodA.getReaction[0] 
RM= nodA.getReaction[5] 
RN2= nodB.getReaction[0] 

elements= preprocessor.getElementHandler
scc= zlElement.getSection()
esfN= scc.getStressResultantComponent("N")
esfMy= scc.getStressResultantComponent("My")
esfMz= scc.getStressResultantComponent("Mz")
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")
concrFibers= fiber_sets.FiberSet(scc,'concrete',EHE_materials.HP45.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Minimum concrete strain.
fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Maximum concrete strain.
reinfFibers= fiber_sets.FiberSet(scc,"reinforcement",EHE_materials.Y1860S7.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Maximum steel strain.

from materials.sections import section_properties
from materials.ehe import EHE_limit_state_checking
solicitationType= section_properties.solicitationType(epsCMin,epsSMax)
solicitationTypeString= section_properties.solicitationTypeString(solicitationType)
cumpleFT= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.bendingOK(epsCMin,epsSMax)
aprovSecc= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.getBendingEfficiency(epsCMin,epsSMax)

ratio1= RM
ratio2= esfMz
ratio3= (esfN-NDato)
ratio4= (cumpleFT-1)
ratio5= (RN+NDato)

''' 
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
print("ratio4= ",(ratio4))
print("ratio5= ",(ratio5))

print("Minimum strain in concrete: ",(epsCMin))
print("Maximum strain in concrete: ",(epsCMax))
print("Maximum strain in rebars: ",(epsSMax))
print("Solicitation type: ",solicitationTypeString," (",(solicitationType),") \n")
print("Cumple a ",solicitationTypeString,": ",(cumpleFT))
print("Aprovechamiento a ",solicitationTypeString,": ",(aprovSecc))
print("RN= ",(RN/1e3),"kN \n")
print("RN2= ",(RN2/1e3))
print("N= ",(esfN/1e3))
print("My= ",(esfMy/1e3))
print("Mz= ",(esfMz/1e3))
print("defMz= ",(defMz))
print("defN= ",(defN))
 '''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-9) & (abs(ratio5)<1e-9) & (abs(RN2)<1e-9) & (abs(esfMy)<1e-10) & (solicitationType == 2) & (abs(ratio4)<1e-6) & (analOk == 0.0) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Section output
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section
# fig = plt.figure()
# ax= fig.add_subplot(111)
# plot_fiber_section.mplot_section_geometry(ax, sectionGeometry= geomSecPret01)
# plt.show()
