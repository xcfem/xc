# -*- coding: utf-8 -*-
''' Reinforced concrete section verification test.
   results are compared with those of the prontuario.
   informático del hormigón estructural (Cátedra de hormigón de la ETSICCP-IECA
   UPM). '''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# feProblem.logFileName= "/tmp/erase.log"  # Ignore warning messages

import os
import sys
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions


from materials.ehe import EHE_materials
from materials.sections.fiber_section import fiber_sets
from model import predefined_spaces

MzDato= 8e3
NDato= 299.54e3 # The "prontuario informático" reaches only ~285 kN because of the steel simplified diagram.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
tag= EHE_materials.B500S.defDiagD(preprocessor)
tag= EHE_materials.HA25.defDiagD(preprocessor)

# Create RC section model.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
sys.path.append(pth+"/../../../../../../aux/")
import concrete_section_01
concreteSectionGeom01= concrete_section_01.gmSecHA01(preprocessor, nmbGeomSecc="concreteSectionGeom01",defSec= concrete_section_01.defSec,concrDiagName= EHE_materials.HA25.nmbDiagD,reinfSteelDiagramName= EHE_materials.B500S.nmbDiagD)
materialHandler= preprocessor.getMaterialHandler
secHA= materialHandler.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(concreteSectionGeom01.name)
secHA.setupFibers()

zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, "secHA")

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

scc= zlElement.getSection()
esfN= scc.getStressResultantComponent("N")
esfMy= scc.getStressResultantComponent("My")
esfMz= scc.getStressResultantComponent("Mz")
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")
concrFibers= fiber_sets.FiberSet(scc,'concrete',EHE_materials.HA25.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Minimum concrete strain.
fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Maximum concrete strain.
reinfFibers= fiber_sets.FiberSet(scc,"reinforcement",EHE_materials.B500S.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Maximum steel strain

from materials.sections import section_properties
from materials.ehe import EHE_limit_state_checking
solicitationType= section_properties.solicitationType(epsCMin,epsSMax)
solicitationTypeString= section_properties.solicitationTypeString(solicitationType)
cumpleFT= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.tensileBendingOK(epsSMax)
aprovSecc= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.getTensileBendingEfficiency(epsSMax)

ratio1= (RM+MzDato)/MzDato
ratio2= (esfMz-MzDato)/MzDato
ratio3= (esfN-NDato)
ratio4= cumpleFT
ratio5= (RN+NDato)/NDato
ratio6= (epsSMax-10e-3)/10e-3

''' 
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
print("ratio4= ",(ratio4))
print("ratio5= ",(ratio5))
print("ratio6= ",(ratio6))

print("Minimum concrete strain: ",(epsCMin))
print("Maximum concrete strain: ",(epsCMax))
print("Maximum rebar strain: ",(epsSMax))
print("Solicitation type: ",solicitationTypeString," (",(solicitationType),") \n")
print("Cumple a ",solicitationTypeString,": ",(cumpleFT))
print("Aprovechamiento a ",solicitationTypeString,": ",(aprovSecc))
print("RN= ",(RN/1e3))
print("RN2= ",(RN2/1e3))
print("N= ",(esfN/1e3))
print("My= ",(esfMy/1e3))
print("Mz= ",(esfMz/1e3))
print("defMz= ",(defMz))
print("defN= ",(defN))
 '''

ratiosOk= (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-9) & (abs(ratio5)<1e-10) & (abs(ratio6)<0.01) & ratio4
miscOk= (abs(RN2)<1e-9) & (abs(esfMy)<1e-10) & (solicitationType == 1) & (analOk == 0.0)

# print("ratiosOk= ", ratiosOk)
# print("miscOk= ", miscOk)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratiosOk & miscOk :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Section output
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section
# fig = plt.figure()
# ax= fig.add_subplot(111)
# plot_fiber_section.mplot_section_geometry(ax, sectionGeometry= concreteSectionGeom01)
# plt.show()
