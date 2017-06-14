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


from materials.ehe import EHE_materials
from materials.fiber_section import createFiberSets
from model import predefined_spaces

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
tag= EHE_materials.B500S.defDiagD(preprocessor)
concr= EHE_materials.HA25
concr.alfacc=0.85    #f_maxd= 0.85*fcd coeficiente de fatiga del hormigón (generalmente alfacc=1)
tag= concr.defDiagD(preprocessor)
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/concrete_section_01.py")
materiales= preprocessor.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("concreteSectionGeom01")
secHA.setupFibers()

banco_pruebas_scc3d.sectionModel(preprocessor, "secHA")

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
concrFibers= createFiberSets.FiberSet(scc,'concrete',EHE_materials.HA25.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Minumum concrete strain.
fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Maximum concrete strain.
reinfFibers= createFiberSets.FiberSet(scc,"reinforcement",EHE_materials.B500S.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Maximum steel strain

from materials import section_properties
from materials.ehe import comprobTnEHE
tipoSolic= section_properties.solicitationType(epsCMin,epsSMax)
strTipoSolic= section_properties.solicitationTypeString(tipoSolic)
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

print "Minumum concrete strain: ",(epsCMin)
print "Maximum concrete strain: ",(epsCMax)
print "Maximum rebar strain: ",(epsSMax)
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<5e-10) & ratio4 & (abs(ratio5)<5e-10) & (abs(ratio6)<0.001) & (abs(RN2)<5e-10) & (abs(esfMy)<1e-10) & (tipoSolic == 2)  & (analOk == 0.0) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
