# -*- coding: utf-8 -*-
'''Elastic 3D section verification.'''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from misc import scc3d_testing_bench

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/borrar.log" # Don't pring warnings
preprocessor=  feProblem.getPreprocessor

# Rectangular cross-section definition
scc10x20=  section_properties.RectangularSection(name="rectang",b=.10,h=.20) # Section geometry.
matscc10x20=typical_materials.MaterialData(name='mtrectang',E=2.1e6,nu=0.3,rho=2500) # Section material.


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
matPoteau= scc10x20.defElasticShearSection3d(preprocessor,matscc10x20) 
elemZLS= scc3d_testing_bench.sectionModel(preprocessor, scc10x20.sectionName)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)

# Loads definition
cargas= preprocessor.getLoadHandler
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
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)
RM= nodes.getNode(1).getReaction[5] 

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
scc0= ele1.getSection()
esfMz= scc0.getStressResultantComponent("Mz")

ratio1= (RM+loadMz)/loadMz
ratio2= (esfMz-loadMz)/loadMz
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
