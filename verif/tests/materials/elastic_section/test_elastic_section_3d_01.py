# -*- coding: utf-8 -*-
# Verification test of una sección elástica en 3d

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from materials import paramRectangularSection
from misc import banco_pruebas_scc3d

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Ignore warning messages
preprocessor=  prueba.getPreprocessor

# Rectangular cross-section definition
scc10x20=  paramRectangularSection.RectangularSection(name="rectang",b=.10,h=.20)
matscc10x20=typical_materials.MaterialData(name='mtrectang',E=2.1e6,nu=0.3,rho=2500)


# Materials definition
matPoteau= scc10x20.defSeccElastica3d(preprocessor,matscc10x20)
elemZLS= banco_pruebas_scc3d.modeloSecc3d(preprocessor, scc10x20.nmb)

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

nodes= preprocessor.getNodeLoader
nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader

RM= nodes.getNode(1).getReaction[5] 

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
