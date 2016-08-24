# -*- coding: utf-8 -*-
# Test de funcionamiento de la sección de fibras con rigideces
#    a cortante y torsion. Elaboración propia.


# Rectangular cross-section definition
nmbSecc= "scc1x1"
from materials import sccRectg
from misc import banco_pruebas_scc3d
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

scc1x1= sccRectg.sccRectang()
scc1x1.b= 1 # Cross section width expresado en cm.
scc1x1.h= 1 # Cross section heightexpresado en cm.
scc1x1.nDivIJ= 32
scc1x1.nDivJK= 32

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/macros_test_fiber_section.py")

fy= 2600 # Tensión de cedencia del material expresada en kp/cm2.
E= 1e6 # Módulo de Young del material en kp/cm2.

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
preprocessor=  prueba.getPreprocessor
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
respT= typical_materials.defElasticMaterial(preprocessor, "respT",1e6) # Respuesta de la sección a torsión.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e6) # Respuesta de la sección a cortante según y.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e6) # Respuesta de la sección a cortante según y.
# Secciones
geomRectang= preprocessor.getMaterialLoader.newSectionGeometry("geomRectang")
reg= scc1x1.discretization(geomRectang,"elast")
sa= preprocessor.getMaterialLoader.newMaterial("fiberSectionShear3d","sa")
fiberSectionRepr= sa.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomRectang")
sa.setupFibers()
extraeParamSccFibras(sa,scc1x1)
sa.setRespVyByName("respVy")
sa.setRespVzByName("respVz")
sa.setRespTByName("respT")

banco_pruebas_scc3d.modeloSecc3d(preprocessor, "sa")
# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
loadN= -1
loadVy= -2
loadVz= -3
loadMx= -4
loadMy= -5
loadMz= -6
lp0.newNodalLoad(2,xc.Vector([loadN,loadVy,loadVz,loadMx,loadMy,loadMz]))

#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(1)
if(analOk!=0):
  print "Error!; failed to converge."
  exit()

nodes= preprocessor.getNodeLoader
nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
n1= nodes.getNode(1)
reacN1= n1.getReaction

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
N= scc.getStressResultantComponent("N")
Vy= scc.getStressResultantComponent("Vy")
Vz= scc.getStressResultantComponent("Vz")
Mx= scc.getStressResultantComponent("T")
My= scc.getStressResultantComponent("My")
Mz= scc.getStressResultantComponent("Mz")
esfElem= xc.Vector([N,Vy,Vz,Mx,My,Mz])
defN= scc.getSectionDeformationByName("defN")
defVy= scc.getSectionDeformationByName("defVy")
defVz= scc.getSectionDeformationByName("defVz")
defT= scc.getSectionDeformationByName("defT")
defMy= scc.getSectionDeformationByName("defMy")
defMz= scc.getSectionDeformationByName("defMz")
defElem= xc.Vector([defN,defVy,defVz,defT,defMy,defMz])

respVy= scc.getRespVy()
respVz= scc.getRespVz()
respT= scc.getRespT()
esfRespVy= respVy.getStress()
esfRespVz= respVz.getStress()
esfRespT= respT.getStress()

defRespVy= respVy.getStrain()
defRespVz= respVz.getStrain()
defRespT= respT.getStrain()

fibras= scc.getFibers()

defFibras= fibras.getDeformation()

esfSeccion= xc.Vector([fibras.getResultant(),esfRespVy,esfRespVz,esfRespT,fibras.getMy(0.0),fibras.getMz(0.0)])
defSeccion= xc.Vector([defFibras[0],defRespVy,defRespVz,defRespT,defFibras[2],defFibras[1]])



ratio1= (reacN1+esfElem).Norm()
ratio2= (reacN1+esfSeccion).Norm()
ratio3= (defElem-defSeccion).Norm()

'''
print "reacN1= ",reacN1
print "esfElem= ",esfElem
print "esfSeccion= ",esfSeccion
print "defElem= ",defElem
print "defSeccion= ",defSeccion
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
''' 


import os
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-12)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
