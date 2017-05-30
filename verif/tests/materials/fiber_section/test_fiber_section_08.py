# -*- coding: utf-8 -*-
''' Verification test of una sección de hormigón armado.
   Los resultados se comparan con los obtenidos del prontuario.
   informático del hormigón estructural (Cátedra de hormigón de la ETSICCP-IECA
   UPM).

Los resultados obtenidos son aceptables teniendo en cuenta que:

- El prontuario no tiene en cuenta que el valor máximo de la resistencia de 1.05 veces el de
  la resistencia característica.
- Este programa no descuenta los huecos en el hormigón ocupados por la reinforcement.
- La discretización de la sección empleada en el modelo de fibras introduce un error.
 '''

# prueba.logFileName= "/tmp/borrar.log"  #Ignore warning messages

from __future__ import division
import xc_base
import geom
import xc
from misc import banco_pruebas_scc3d
from solution import predefined_solutions

from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel
from materials.fiber_section import createFiberSets
from materials import section_properties
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Coeficientes de seguridad.
gammac= 1.5 # Coeficiente de minoración de la resistencia del hormigón.
gammas= 1.15 # Coeficiente de minoración de la resistencia of the steel.

CurvZ= 11.474e-3 # Curvatura de prueba 1.
x1= 0.0997 # Profundidad de la fibra neutra 1.
MzDato= 55.949206e3 # Momento que agota la reinforcement a tracción.
NDato= 0.0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
reinfMatTag= EHE_reinforcing_steel.B500S.defDiagD(preprocessor)
dgDB500S= EHE_reinforcing_steel.B500S.getDiagD(preprocessor)
concr=EHE_concrete.HA25
concr.alfacc=0.85    #f_maxd= 0.85*fcd coeficiente de fatiga del hormigón (generalmente se toma alfacc=1)
tagHormigon= concr.defDiagD(preprocessor)

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/secc_hormigon_01.py")
secHA= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHormigon01")
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
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,0,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(1)

IEIz= 0.0
IEIy= 0.0
TEIz= 0.0
TEIy= 0.0
x= 0.0

nodes= preprocessor.getNodeLoader
nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader

RN= nodes.getNode(1).getReaction[0] 
RM= nodes.getNode(1).getReaction[5] 
RN2= nodes.getNode(2).getReaction[0] 

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
fibras= scc.getFibers()
sumAreas= fibras.getSumaAreas(1.0)
Iz= fibras.getIz(1.0,0.0)
IEIy= scc.getInitialTangentStiffness().at(3,3)
IEIz= scc.getInitialTangentStiffness().at(2,2)
TEIy= scc.getTangentStiffness().at(3,3)
TEIz= scc.getTangentStiffness().at(2,2)
Iy= fibras.getIy(1.0,0.0)
esfN= fibras.getResultant()
esfMy= fibras.getMy(0.0)
esfMz= fibras.getMz(0.0)
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")
x= scc.getNeutralAxisDepth()
Resul= scc.getStressResultant()
Deform= scc.getSectionDeformation()
setsRC= createFiberSets.fiberSectionSetupRCSets(scc,EHE_concrete.HA25.matTagD,"hormigon",EHE_reinforcing_steel.B500S.matTagD,"reinforcement")


fibraCEpsMin= -1
fMin= setsRC.concrFibers.getFiberWithMinStrain()

epsCMin= fMin.getMaterial().getStrain() # Deformación mínima en el hormigón.
sgCMin= fMin.getMaterial().getStress() # Tensión mínima en el hormigón.
YepsCMin= fMin.getPos().x # Coordenada y de la deformación mínima en el hormigón.
ZepsCMin= fMin.getPos().y # Coordenada z de la deformación mínima en el hormigón.

fMin= setsRC.reinfFibers.getFiberWithMinStrain()

epsSMin= fMin.getMaterial().getStrain() # Deformación mínima en el acero.
sgSMin= fMin.getMaterial().getStress() # Tensión mínima en el acero.
YepsSMin= fMin.getPos().x # Coordenada y de la deformación mínima en el acero.
ZepsSMin= fMin.getPos().y # Coordenada z de la deformación mínima en el acero.

fMax= setsRC.reinfFibers.getFiberWithMaxStrain()

epsSMax= fMax.getMaterial().getStrain() # Deformación máxima en el acero.
sgSMax= fMax.getMaterial().getStress() # Tensión máxima en el acero.
YepsSMax= fMax.getPos().x # Coordenada y de la deformación mínima en el acero.
ZepsSMax= fMax.getPos().y # Coordenada z de la deformación mínima en el acero.


from materials import section_properties
from materials.ehe import comprobTnEHE
tipoSolic= section_properties.solicitationType(epsCMin,epsSMax)
strTipoSolic= section_properties.solicitationTypeString(tipoSolic)
cumpleFT= comprobTnEHE.cumpleFlexotraccion(epsCMin,epsSMax)
aprovSecc= comprobTnEHE.aprovFlexotraccion(epsCMin,epsSMax)

ratio1= (RM+MzDato)/MzDato
ratio2= (esfMz-MzDato)/MzDato
ratio3= (esfN-NDato)
ratio4= (epsSMin+0.5e-3)/0.5e-3
ratio5= (Resul[1]-MzDato)/MzDato
ratio6= (epsSMax-10e-3)/10e-3

'''
print "sumAreas= ",(sumAreas)
print "Iz= ",(Iz)
print "IEIz= ",IEIz
print "TEIz= ",TEIz
print "E1= ",(IEIz/Iz)
print "Iy= ",(Iy)
print "IEIy= ",IEIy
print "TEIy= ",TEIy
print "E2= ",(IEIy/Iy)
print "Profundidad de la fibra neutra= ",x," m"
print "Resul= ",Resul*1e-3," kN"
print "Deform= ",Deform*1e3,"E-3"

print "\nDeformación mínima en el hormigón: ",(epsCMin*1E3),"E-3"
print "Tensión mínima en el hormigón: ",(sgCMin/1e6),"E6"
print "Coordenada y para deformación mínima en el hormigón: ",(YepsCMin)
print "Coordenada z para deformación mínima en el hormigón: ",(ZepsCMin)

print "\nDeformación mínima en la reinforcement: ",(epsSMin*1E3),"E-3"
print "Tensión mínima en el acero: ",(sgSMin/1e6),"E6"
print "Coordenada y para deformación mínima en la reinforcement: ",(YepsSMin)
print "Coordenada z para deformación mínima en la reinforcement: ",(ZepsSMin)

print "\nDeformación máxima en la reinforcement: ",(epsSMax*1E3),"E-3"
print "Tensión máxima en el acero: ",(sgSMax/1e6),"E6"
print "Coordenada y para deformación máxima en la reinforcement: ",(YepsSMax)
print "Coordenada z para deformación máxima en la reinforcement: ",(ZepsSMax)
print "\nTipo solicitación: ",strTipoSolic," (",(tipoSolic),") "
print "Cumple a ",strTipoSolic,": ",(cumpleFT)
print "Aprovechamiento a ",strTipoSolic,": ",(aprovSecc)
print "RN= ",(RN/1e3)
print "RN2= ",(RN2/1e3)
print "N= ",(esfN/1e3)
print "My= ",(esfMy/1e3)
print "Mz= ",(esfMz/1e3)
print "defMz= ",(defMz)
print "defN= ",(defN)
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)
print "ratio6= ",(ratio6)
 '''


import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-9) & (abs(RN)<1e-9) & (abs(RN2)<1e-9) & (abs(esfMy)<1e-9) & (tipoSolic == 2) & (abs(ratio4)<0.06) & (abs(ratio5)<1e-6) & (abs(ratio6)<1e-6)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
