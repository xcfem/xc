# -*- coding: utf-8 -*-
''' Reinforced concrete section verification test.
   results are compared with those of the prontuario.
   informático del hormigón estructural (Cátedra de hormigón de la ETSICCP-IECA
   UPM). '''

from __future__ import print_function

# feProblem.logFileName= "/tmp/erase.log"  # Ignore warning messages

import os
import sys
import xc
from misc import scc3d_testing_bench
from materials.ehe import EHE_materials
from materials.sections.fiber_section import fiber_sets
from model import predefined_spaces
from solution import predefined_solutions

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

MzDato= 20e3
NDato= -1345e3
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
tag= EHE_materials.B500S.defDiagD(preprocessor)
concr= EHE_materials.HA25
concr.alfacc=0.85    # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
tag= concr.defDiagD(preprocessor)

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
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Maximum steel strain.



from materials.sections import section_properties
from materials.ehe import EHE_limit_state_checking
solicitationType= section_properties.solicitationType(epsCMin,epsSMax)
solicitationTypeString= section_properties.solicitationTypeString(solicitationType)
cumpleFT= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.compressiveBendingOK(epsCMin,epsCMax)
aprovSecc= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.getCompressiveBendingEfficiency(epsCMin,epsSMax)

ratio1= (RM+MzDato)/MzDato
ratio2= (esfMz-MzDato)/MzDato
ratio3= (esfN-NDato)
ratio4= (cumpleFT-1)
ratio5= (RN+NDato)/NDato
epsSMaxTeor= 1.2e-3
ratio6= (epsSMax+epsSMaxTeor)/epsSMaxTeor

'''
print("ratio1= ", ratio1)
print("ratio2= ", ratio2)
print("ratio3= ", ratio3)
print("ratio4= ", ratio4)
print("ratio5= ", ratio5)

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

print("               Cumple ratio1: ",abs(ratio1)<1e-9)
print("               Cumple ratio2: ",abs(ratio2)<1e-9)
print("               Cumple ratio3: ",abs(ratio3)<1e-9)
print("               Cumple ratio4: ",abs(ratio4)<1e-6)
print("               Cumple ratio5: ",abs(ratio5)<1e-9)
print("               Cumple RN2: ",abs(RN2)<1e-9)
print("               Cumple esfMy: ",abs(esfMy)<1e-10)
print("               Cumple solicitationType: ",(solicitationType == 3))
print("epsSMax= ",epsSMax)
print("epsSMaxTeor= ",epsSMaxTeor)
print("ratio6= ", ratio6)
print("               Cumple ratio6: ",abs(ratio6)<0.01)
 '''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9) & (abs(ratio2)<1e-9) & (abs(ratio3)<1e-9) & (abs(ratio5)<1e-9) & (abs(RN2)<1e-9) & (abs(esfMy)<1e-10) & (solicitationType == 3) & (abs(ratio4)<1e-6) & (abs(ratio6)<0.01) & (analOk == 0.0) :
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
