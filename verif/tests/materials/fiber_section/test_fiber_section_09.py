# -*- coding: utf-8 -*-
''' Reinforced concrete section verification test.
   results are compared with those of the prontuario.
   informático del hormigón estructural (Cátedra de hormigón de la ETSICCP-IECA
   UPM). '''

# feProblem.logFileName= "/tmp/borrar.log"  #Ignore warning messages

import xc_base
import geom
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

# Partial safety factors.
gammac= 1.5 # Partial safety factor for concrete.
gammas= 1.15 # Partial safety factor for steel.

MzDato= 20e3
NDato= -1345e3
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
tag= EHE_materials.B500S.defDiagD(preprocessor)
concr= EHE_materials.HA25
concr.alfacc=0.85    #f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
tag= concr.defDiagD(preprocessor)

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/concrete_section_01.py")
materiales= preprocessor.getMaterialHandler
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("concreteSectionGeom01")
secHA.setupFibers()

scc3d_testing_bench.sectionModel(preprocessor, "secHA")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_00F(2)

# Loads definition
cargas= preprocessor.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,0,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")


# Solution procedure
analisis= predefined_solutions.simple_newton_raphson(feProblem)
analOk= analisis.analyze(10)

nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True)

RN= nodes.getNode(1).getReaction[0] 
RM= nodes.getNode(1).getReaction[5] 
RN2= nodes.getNode(2).getReaction[0] 

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
scc= ele1.getSection()
esfN= scc.getStressResultantComponent("N")
esfMy= scc.getStressResultantComponent("My")
esfMz= scc.getStressResultantComponent("Mz")
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")
concrFibers= fiber_sets.FiberSet(scc,'concrete',EHE_materials.HA25.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Minumum concrete strain.
fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Maximum concrete strain.

reinfFibers= fiber_sets.FiberSet(scc,"reinforcement",EHE_materials.B500S.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Maximum steel strain.



from materials.sections import section_properties
from materials.ehe import EHE_limit_state_checking
tipoSolic= section_properties.solicitationType(epsCMin,epsSMax)
strTipoSolic= section_properties.solicitationTypeString(tipoSolic)
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
print "ratio1= ", ratio1
print "ratio2= ", ratio2
print "ratio3= ", ratio3
print "ratio4= ", ratio4
print "ratio5= ", ratio5

print "Minumum concrete strain: ",(epsCMin)
print "Maximum concrete strain: ",(epsCMax)
print "Maximum rebar strain: ",(epsSMax)
print "Solicitation type: ",strTipoSolic," (",(tipoSolic),") \n"
print "Cumple a ",strTipoSolic,": ",(cumpleFT)
print "Aprovechamiento a ",strTipoSolic,": ",(aprovSecc)
print "RN= ",(RN/1e3)
print "RN2= ",(RN2/1e3)
print "N= ",(esfN/1e3)
print "My= ",(esfMy/1e3)
print "Mz= ",(esfMz/1e3)
print "defMz= ",(defMz)
print "defN= ",(defN)

print "               Cumple ratio1: ",abs(ratio1)<1e-9
print "               Cumple ratio2: ",abs(ratio2)<1e-9
print "               Cumple ratio3: ",abs(ratio3)<1e-9
print "               Cumple ratio4: ",abs(ratio4)<1e-6
print "               Cumple ratio5: ",abs(ratio5)<1e-9
print "               Cumple RN2: ",abs(RN2)<1e-9
print "               Cumple esfMy: ",abs(esfMy)<1e-10
print "               Cumple tipoSolic: ",(tipoSolic == 3)
print "epsSMax= ",epsSMax
print "epsSMaxTeor= ",epsSMaxTeor
print "ratio6= ", ratio6
print "               Cumple ratio6: ",abs(ratio6)<0.01
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9) & (abs(ratio2)<1e-9) & (abs(ratio3)<1e-9) & (abs(ratio5)<1e-9) & (abs(RN2)<1e-9) & (abs(esfMy)<1e-10) & (tipoSolic == 3) & (abs(ratio4)<1e-6) & (abs(ratio6)<0.01) & (analOk == 0.0) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
