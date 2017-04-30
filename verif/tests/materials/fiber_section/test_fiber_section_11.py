# -*- coding: utf-8 -*-
''' Verification test of una sección de hormigón armado.
   los resultados se comparan con los obtenidos del prontuario.
   informático del hormigón estructural (Cátedra de hormigón de la ETSICCP-IECA
   UPM). '''

# prueba.logFileName= "/tmp/borrar.log"  #Ignore warning messages

import xc_base
import geom
import xc
from misc import banco_pruebas_scc3d
from solution import predefined_solutions


from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel
from materials.fiber_section import createFiberSets
from model import fix_node_6dof

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

MzDato= 55.949e3 #El prontuario informático sólo llega hasta ~54.4 kN m al considerar el diagrama simplificado of the steel.
NDato= 0.0 
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
tag= EHE_reinforcing_steel.B500S.defDiagD(preprocessor)
concr=EHE_concrete.HA25
concr.alfacc=0.85    #f_maxd= 0.85*fcd coeficiente de fatiga del hormigón (generalmente alfacc=1)
tag= concr.defDiagD(preprocessor)
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/secc_hormigon_01.py")
materiales= preprocessor.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHormigon01")
secHA.setupFibers()

banco_pruebas_scc3d.modeloSecc3d(preprocessor, "secHA")

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
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,0,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(10)

nodes= preprocessor.getNodeLoader
nodes.calculateNodalReactions(True)

RN= nodes.getNode(1).getReaction[0] 
RM= nodes.getNode(1).getReaction[5] 
RN2= nodes.getNode(2).getReaction[0] 

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
esfN= scc.getStressResultantComponent("N")
esfMy= scc.getStressResultantComponent("My")
esfMz= scc.getStressResultantComponent("Mz")
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")
concrFibers= createFiberSets.FiberSet(scc,"hormigon",EHE_concrete.HA25.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Deformación mínima en el hormigón.
fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Deformación máxima en el hormigón.
reinfFibers= createFiberSets.FiberSet(scc,"reinforcement",EHE_reinforcing_steel.B500S.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Deformación máxima en el acero

from materials import regimenSeccion
from materials.ehe import comprobTnEHE
tipoSolic= regimenSeccion.tipoSolicitacion(epsCMin,epsSMax)
strTipoSolic= regimenSeccion.strTipoSolicitacion(tipoSolic)
cumpleFT= comprobTnEHE.cumpleFlexotraccion(epsCMin,epsSMax)
aprovSecc= comprobTnEHE.aprovFlexotraccion(epsCMin,epsSMax)

ratio1= (RM+MzDato)/MzDato
ratio2= (esfMz-MzDato)/MzDato
ratio3= (esfN-NDato)
ratio4= cumpleFT
ratio5= (RN+NDato)
ratio6= (epsSMax-10e-3)/10e-3

''' 
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)
print "ratio6= ",(ratio6)

print "Deformación mínima en el hormigón: ",(epsCMin)
print "Deformación máxima en el hormigón: ",(epsCMax)
print "Deformación máxima en la reinforcement: ",(epsSMax)
print "Tipo solicitación: ",strTipoSolic," (",(tipoSolic),") \n"
print "Cumple a ",strTipoSolic,": ",(cumpleFT)
print "Aprovechamiento a ",strTipoSolic,": ",(aprovSecc)
print "RN= ",(RN/1e3)," kN\n"
print "RN2= ",(RN2/1e3)," kN\n"
print "N= ",(esfN/1e3)," kN\n"
print "esfMy= ",(esfMy/1e3),"kN m\n"
print "esfMz= ",(esfMz/1e3),"kN m\n"
print "defMz= ",(defMz)
print "defN= ",(defN)
print "analOk= ",(analOk)
 '''


import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<5e-10) & ratio4 & (abs(ratio5)<5e-10) & (abs(ratio6)<0.001) & (abs(RN2)<5e-10) & (abs(esfMy)<1e-10) & (tipoSolic == 2)  & (analOk == 0.0) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
