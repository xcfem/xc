# -*- coding: utf-8 -*-
# Test de funcionamiento de una sección metálica rectangular de fibras de material elastoplástico.
#    elaborado a partir de «Nociones de cálculo plástico». C. Benito Hernández.
#    página 26 y siguientes.


# Rectangular cross-section definition
from materials import sccRectg
from misc import banco_pruebas_scc3d
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from solution import predefined_solutions

scc10x20= sccRectg.sccRectang()
scc10x20.b= 10 # Cross section width expresado en cm.
scc10x20.h= 20 # Cross section heightexpresado en cm.
scc10x20.nDivIJ= 32
scc10x20.nDivJK= 32

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/macros_test_fiber_section.py")

fy= 2600 # Tensión de cedencia del material expresada en kp/cm2.
E= 2.1e6 # Módulo de Young del material en kp/cm2.

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
preprocessor=  prueba.getPreprocessor
# Materials definition
epp= typical_materials.defElasticPPMaterial(preprocessor, "epp",E,fy,-fy)
geomRectang= preprocessor.getMaterialLoader.newSectionGeometry("geomRectang")
reg= scc10x20.discretization(geomRectang,"epp")
rectang= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","rectang")
fiberSectionRepr= rectang.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomRectang")
rectang.setupFibers()
extraeParamSccFibras(rectang,scc10x20)

banco_pruebas_scc3d.modeloSecc3d(preprocessor, "rectang")

# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.Nodo6DOFGiroYLibre(coacciones,2)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
loadMy= 0.999*scc10x20.Mp2(fy)
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,loadMy,0]))

#We add the load case to domain.
casos.addToDomain("0")


# Obtención de la solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(1)


nodos= preprocessor.getNodeLoader
nodos.calculateNodalReactions(True)

RM= nodos.getNode(1).getReaction[4] 

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
esfMy= scc.getFibers().getMy(0.0)

yCdgTeor= 0.0
zCdgTeor= 0.0
ratio1= (sumAreas-scc10x20.area())/scc10x20.area()
ratio2= yCdg-yCdgTeor
ratio3= zCdg-zCdgTeor
ratio4= (I1-scc10x20.I1())/scc10x20.I1()
ratio5= (I2-scc10x20.I2())/scc10x20.I2()
ratio6= (i1-scc10x20.i1())/scc10x20.i1()
ratio7= (i2-scc10x20.i2())/scc10x20.i2()
ratio8= (Me1-scc10x20.Me1(fy))/scc10x20.Me1(fy)
ratio9= (Me2-scc10x20.Me2(fy))/scc10x20.Me2(fy)
ratio10= (SzPosG-scc10x20.S1PosG())/scc10x20.S1PosG()
ratio11= (SyPosG-scc10x20.S2PosG())/scc10x20.S2PosG()
ratio12= (RM+loadMy)/loadMy
ratio13= (esfMy-loadMy)/loadMy

'''
printRatios(scc10x20)
    print "RM= ",(RM)
    print "esfMy= ",(esfMy)
    print "loadMy= ",(loadMy)
'''

fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-3) & (abs(ratio5)<1e-2) & (abs(ratio6)<1e-3) & (abs(ratio7)<1e-2) & (abs(ratio8)<1e-3) & (abs(ratio9)<1e-3) & (abs(ratio10)<1e-5) & (abs(ratio11)<1e-5) & (abs(ratio12)<1e-5) & (abs(ratio13)<1e-5) & (analOk == 0.0) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
