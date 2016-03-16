# -*- coding: utf-8 -*-
# Test de funcionamiento de una sección de hormigón pretensado.
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Macros
import xc_base
import geom
import xc
from misc import banco_pruebas_scc3d


from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel
from materials.ehe import aceroPretEHE
from materials.fiber_section import createFiberSets
from model import fix_node_6dof
from solution import predefined_solutions


MzDato= 1e6
NDato= 0.0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
tag= aceroPretEHE.Y1860S7.defDiagD(preprocessor, aceroPretEHE.Y1860S7.tInic())
tag= EHE_concrete.HP45.defDiagD(preprocessor)
tag= EHE_reinforcing_steel.B500S.defDiagD(preprocessor)
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/secc_hormigon_pret_02.py")

materiales= preprocessor.getMaterialLoader
secHP= materiales.newMaterial("fiber_section_3d","secHP")
fiberSectionRepr= secHP.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHormigonPret02")
secHP.setupFibers()

elem= banco_pruebas_scc3d.modeloSecc3d(preprocessor, "secHP")

# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.Nodo6DOFMovXGiroZLibres(coacciones,2)

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


# Procedimiento de solución
solution= predefined_solutions.SolutionProcedure()
analysis= solution.simpleNewtonRaphson(prueba)
solution.ctest.tol= 1e-8
solution.ctest.printFlag= 0 #flag used to print information on convergence (optional)
analOk= analysis.analyze(10)

if(analOk!=0): 
  print "ERROR: Este test falla en máquinas de 32 bits. Pendiente de resolver (2013/03/11). It seems to be a problem with the tolerance of the convergence test. In my new machine (Dell precision M4800 with Ubuntu 64bits it fails with tol= 1e-9 and works if tol= 1e-5) (2016/01/06) LP.\n"
  exit()


nodos= preprocessor.getNodeLoader
nodos.calculateNodalReactions(True)

RN= nodos.getNode(1).getReaction[0] 
RM= nodos.getNode(1).getReaction[5] 
RN2= nodos.getNode(2).getReaction[0] 

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
esfN= scc.getStressResultantComponent("N")
esfMy= scc.getStressResultantComponent("My")
esfMz= scc.getStressResultantComponent("Mz")
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")
concrFibers= createFiberSets.FiberSet(scc,"hormigon",EHE_concrete.HP45.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Deformación mínima en el hormigón.
yEpsCMin= fibraCEpsMin.getPos().x
fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Deformación máxima en el hormigón.
reinfFibers= createFiberSets.FiberSet(scc,"reinforcement",EHE_reinforcing_steel.B500S.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Deformación máxima en el acero
yEpsCMax= fibraCEpsMax.getPos().x

from materials import regimenSeccion
from materials.ehe import comprobTnEHE
tipoSolic= regimenSeccion.tipoSolicitacion(epsCMin,epsSMax)
strTipoSolic= regimenSeccion.strTipoSolicitacion(tipoSolic)
cumpleFT= comprobTnEHE.cumpleFlexotraccion(epsCMin,epsSMax)
aprovSecc= comprobTnEHE.aprovFlexotraccion(epsCMin,epsSMax)

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

print "Deformación mínima en el hormigón: ",(epsCMin)
print "Deformación máxima en el hormigón: ",(epsCMax)
print "Deformación máxima en la reinforcement: ",(epsSMax)
print "Tipo solicitación: ",strTipoSolic," (",(tipoSolic),") \n"
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
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-6) & (abs(ratio3)<1e-6) & (abs(ratio5)<1e-6) & (abs(RN2)<1e-6) & (abs(esfMy)<1e-6) & (tipoSolic == 3) & (abs(ratio4)<1e-6) & (analOk == 0.0) & (yEpsCMax>0.0) & (yEpsCMin<0.0) : #(yEpsCMax<0.0) & (yEpsCMin>0.0) 2014.11.21
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
