# -*- coding: utf-8 -*-
# Prestressed concrete section verification test.
from __future__ import division

import xc_base
import geom
import xc
from misc import banco_pruebas_scc3d


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


from materials.ehe import EHE_materials
from materials.sections.fiber_section import createFiberSets
from model import predefined_spaces
from solution import predefined_solutions


MzDato= 1e6
NDato= 0.0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
tag= EHE_materials.Y1860S7.defDiagD(preprocessor, EHE_materials.Y1860S7.tInic())
tag= EHE_materials.HP45.defDiagD(preprocessor)
tag= EHE_materials.B500S.defDiagD(preprocessor)
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/prestressed_concrete_section_02.py")

materiales= preprocessor.getMaterialLoader
secHP= materiales.newMaterial("fiber_section_3d","secHP")
fiberSectionRepr= secHP.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("prestressedConcretSectionGeom02")
secHP.setupFibers()

elem= banco_pruebas_scc3d.sectionModel(preprocessor, "secHP")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_00F(2)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,0,-MzDato]))

#We add the load case to domain.
casos.addToDomain("0")


# Solution procedure
solution= predefined_solutions.SolutionProcedure()
analysis= solution.simpleNewtonRaphson(prueba)
solution.ctest.tol= 1e-8
solution.ctest.printFlag= 0 #flag used to print information on convergence (optional)
analOk= analysis.analyze(10)

if(analOk!=0): 
  print "ERROR: Thist test fails when running in 32 bits machines. Solution pending (2013/03/11). It seems to be a problem with the tolerance of the convergence test. In my new machine (Dell precision M4800 with Ubuntu 64bits it fails with tol= 1e-9 and works if tol= 1e-5) (2016/01/06) LP.\n"
  exit()


nodes= preprocessor.getNodeLoader
nodes.calculateNodalReactions(True)

RN= nodes.getNode(1).getReaction[0] 
RM= nodes.getNode(1).getReaction[5] 
RN2= nodes.getNode(2).getReaction[0] 

elements= preprocessor.getElementLoader
ele1= elements.getElement(1)
scc= ele1.getSection()
esfN= scc.getStressResultantComponent("N")
esfMy= scc.getStressResultantComponent("My")
esfMz= scc.getStressResultantComponent("Mz")
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")
concrFibers= createFiberSets.FiberSet(scc,'concrete',EHE_materials.HP45.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Minumum concrete strain.
yEpsCMin= fibraCEpsMin.getPos().x
fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Maximum concrete strain.
reinfFibers= createFiberSets.FiberSet(scc,"reinforcement",EHE_materials.B500S.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Maximum steel strain
yEpsCMax= fibraCEpsMax.getPos().x

from materials.sections import section_properties
from materials.ehe import EHE_limit_state_checking
tipoSolic= section_properties.solicitationType(epsCMin,epsSMax)
strTipoSolic= section_properties.solicitationTypeString(tipoSolic)
cumpleFT= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.bendingOK(epsCMin,epsSMax)
aprovSecc= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.getBendingEfficiency(epsCMin,epsSMax)

ratio1= (RM-MzDato)/MzDato
ratio2= (esfMz+MzDato)/MzDato
ratio3= (esfN-NDato)
ratio4= (cumpleFT-1)
ratio5= (RN+NDato)

''' 
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)

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
print "analOk= ",(analOk)
print "epsCMin= ",epsCMin
print "yEpsCMin= ",(yEpsCMin)
print "epsCMax= ",epsCMax
print "yEpsCMax= ",(yEpsCMax)
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-6) & (abs(ratio3)<1e-6) & (abs(ratio5)<1e-6) & (abs(RN2)<1e-6) & (abs(esfMy)<1e-6) & (tipoSolic == 3) & (abs(ratio4)<1e-6) & (analOk == 0.0) & (yEpsCMax>0.0) & (yEpsCMin<0.0) : #(yEpsCMax<0.0) & (yEpsCMin>0.0) 2014.11.21
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
