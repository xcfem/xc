# -*- coding: utf-8 -*-
# Test de funcionamiento de una sección elástica en 2d

import xc_base
import geom
import xc
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials
from materials import parametrosSeccionRectangular
from solution import predefined_solutions
from misc import banco_pruebas_scc2d

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
mdlr= prueba.getModelador


# Definición de la sección rectangular
scc10x20=  parametrosSeccionRectangular.RectangularSection("rectang",.10,.20,2.1e6, 0.3)

# Materials definition
matPoteau= scc10x20.defSeccShElastica2d(mdlr)
elemZLS= banco_pruebas_scc2d.modeloSecc2d(mdlr,scc10x20.nmb)

# Constraints
coacciones= mdlr.getConstraintLoader
fix_node_3dof.fixNode000(coacciones,1)

# Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
loadMz= 1e3
lp0.newNodalLoad(2,xc.Vector([0,0,loadMz]))

#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos= mdlr.getNodeLoader
nodos.calculateNodalReactions(True)
RM= nodos.getNode(1).getReaction[2] 

elementos= mdlr.getElementLoader
ele1= elementos.getElement(1)
ele1.getResistingForce()
scc0= ele1.getSection()

esfMz= scc0.getStressResultantComponent("Mz")

ratio1= (RM+loadMz)/loadMz
ratio2= (esfMz-loadMz)/loadMz

''' 
print " ratio1= ",(ratio1)
print " ratio2= ",(ratio2)
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
