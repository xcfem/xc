# -*- coding: utf-8 -*-
# Test de funcionamiento de una sección elástica en 3d

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from materials import paramRectangularSection
from misc import banco_pruebas_scc3d

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
preprocessor=  prueba.getPreprocessor

# Rectangular cross-section definition
#XXX No es una seccion rectangular así que NO PARECE QUE EL
# TEST TENGA DEMASIADO SENTIDO. Cuando menos habría que 
# crear una clase para este tipo de perfiles que sea coherente con
# las ya definidas.
SHS50x50x2_5=  paramRectangularSection.RectangularSection("SHS50x50x2_5",b=0.05,h=0.05)
matSHS50x50x2_5=  typical_materials.MaterialData(name='matSec',E=210000e6,nu=0.3e6,rho=2500)

SHS50x50x2_5.nmb= "SHS50x50x2_5"
SHS50x50x2_5.b= 0.05
SHS50x50x2_5.h= 0.05
SHS50x50x2_5.e= 2.5e-3
SHS50x50x2_5.P= 3.54
#SHS50x50x2_5.A= 4.51e-4
#SHS50x50x2_5.Iz= 17.9e-8
#SHS50x50x2_5.Iy= 17.9e-8
SHS50x50x2_5.Wzel= 7.16e-6
SHS50x50x2_5.Wzpl= 8.47e-6
SHS50x50x2_5.iz= 1.99e-2
SHS50x50x2_5.iy= 1.99e-2
#SHS50x50x2_5.J= 26.8e-8
SHS50x50x2_5.E= 210000e6
SHS50x50x2_5.nu= 0.3e6
#SHS50x50x2_5.G= SHS50x50x2_5.E/(2*(1+SHS50x50x2_5.nu))
SHS50x50x2_5.alpha= 5/6 # XXX Corregir


# Materials definition
mat= SHS50x50x2_5.defSeccElastica3d(preprocessor,matSHS50x50x2_5)
elemZLS= banco_pruebas_scc3d.modeloSecc3d(preprocessor, SHS50x50x2_5.nmb)

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
#casos.currentLoadPattern= "0"
loadMz= 1e3
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,0,loadMz]))

#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

nodos= preprocessor.getNodeLoader
nodos.calculateNodalReactions(True)
nodos= preprocessor.getNodeLoader

RM= nodos.getNode(1).getReaction[5] 

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
ele1.getResistingForce()
scc0= ele1.getSection()

esfMz= scc0.getStressResultantComponent("Mz")

ratio1= (RM+loadMz)/loadMz
ratio2= (esfMz-loadMz)/loadMz
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
