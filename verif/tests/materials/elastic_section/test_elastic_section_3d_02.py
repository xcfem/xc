# -*- coding: utf-8 -*-
'''Elastic 3D section verification test.'''

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
feProblem.logFileName= "/tmp/borrar.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor

# Rectangular cross-section definition
#XXX Is not a rectangular sections so THE TEST SEEMS NOT HAVE MUCH SENSE
# At last we must create a class for this kind to be consistent with
# the classes already defined to deal with steel profiles.
SHS50x50x2_5=  section_properties.RectangularSection("SHS50x50x2_5",b=0.05,h=0.05) # Section geometry.
matSHS50x50x2_5=  typical_materials.MaterialData(name='matSec',E=210000e6,nu=0.3e6,rho=2500) #Section material.

SHS50x50x2_5.sectionName= "SHS50x50x2_5"
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
mat= SHS50x50x2_5.defElasticSection3d(preprocessor,matSHS50x50x2_5)
elemZLS= scc3d_testing_bench.sectionModel(preprocessor, SHS50x50x2_5.sectionName)

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
#casos.currentLoadPattern= "0"
loadMz= 1e3
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,0,loadMz]))

#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

nodes= preprocessor.getNodeLoader
nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader

RM= nodes.getNode(1).getReaction[5] 

elements= preprocessor.getElementLoader
ele1= elements.getElement(1)
ele1.getResistingForce()
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
