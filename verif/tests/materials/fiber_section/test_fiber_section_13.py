# -*- coding: utf-8 -*-
# Prestressed concrete section verification test.
from __future__ import division

import xc_base
import geom
import xc
from misc import scc3d_testing_bench


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


from materials.ehe import EHE_materials
from materials.sections.fiber_section import fiber_sets
from model import predefined_spaces
from solution import predefined_solutions


MzDato= 1e6
NDato= 0.0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
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

materialHandler= preprocessor.getMaterialHandler
secHP= materialHandler.newMaterial("fiber_section_3d","secHP")
fiberSectionRepr= secHP.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("prestressedConcretSectionGeom02")
secHP.setupFibers()

elem= scc3d_testing_bench.sectionModel(preprocessor, "secHP")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_00F(2)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,0,-MzDato]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)


# Solution procedure
solution= predefined_solutions.SolutionProcedure()
analysis= solution.simpleNewtonRaphson(feProblem)
solution.ctest.tol= 1e-8
solution.ctest.printFlag= 0 #flag used to print information on convergence (optional)
analOk= analysis.analyze(10)

if(analOk!=0): 
  print "ERROR: Thist test fails when running in 32 bits machines. Solution pending (2013/03/11). It seems to be a problem with the tolerance of the convergence test. In my new machine (Dell precision M4800 with Ubuntu 64bits it fails with tol= 1e-9 and works if tol= 1e-5) (2016/01/06) LP.\n"
  exit()


nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)

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
concrFibers= fiber_sets.FiberSet(scc,'concrete',EHE_materials.HP45.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Minumum concrete strain.
yEpsCMin= fibraCEpsMin.getPos().x
fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Maximum concrete strain.
reinfFibers= fiber_sets.FiberSet(scc,"reinforcement",EHE_materials.B500S.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Maximum steel strain
yEpsCMax= fibraCEpsMax.getPos().x

from materials.sections import section_properties
from materials.ehe import EHE_limit_state_checking
solicitationType= section_properties.solicitationType(epsCMin,epsSMax)
solicitationTypeString= section_properties.solicitationTypeString(solicitationType)
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
print "Solicitation type: ",solicitationTypeString," (",(solicitationType),") \n"
print "Cumple a ",solicitationTypeString,": ",(cumpleFT)
print "Aprovechamiento a ",solicitationTypeString,": ",(aprovSecc)
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
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-6) & (abs(ratio3)<1e-6) & (abs(ratio5)<1e-6) & (abs(RN2)<1e-6) & (abs(esfMy)<1e-6) & (solicitationType == 3) & (abs(ratio4)<1e-6) & (analOk == 0.0) & (yEpsCMax>0.0) & (yEpsCMin<0.0) : #(yEpsCMax<0.0) & (yEpsCMin>0.0) 2014.11.21
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
