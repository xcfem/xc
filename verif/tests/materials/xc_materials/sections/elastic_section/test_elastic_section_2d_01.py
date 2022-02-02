# -*- coding: utf-8 -*-
from __future__ import print_function
'''Elastic 2D section verification test.'''

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from misc import scc2d_testing_bench

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor


# Rectangular cross-section definition
scc10x20= section_properties.RectangularSection(name="rectang",b=.10,h=.20) # Section geometry.
matscc10x20=typical_materials.MaterialData(name='mtrectang',E=2.1e6,nu=0.3,rho=2500) # Section material.

# Materials definition
matPoteau= scc10x20.defElasticSection2d(preprocessor,matscc10x20) 
elemZLS= scc2d_testing_bench.sectionModel(preprocessor, scc10x20.name)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics2DSpace(preprocessor)
modelSpace.fixNode000(1)
spc= modelSpace.constraints.newSPConstraint(2,1,0.0)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

loadMz= 1e3
lp0.newNodalLoad(2,xc.Vector([0,0,loadMz]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

nodes= preprocessor.getNodeHandler
RM= nodes.getNode(1).getReaction[2] 

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
ele1.getResistingForce()
scc0= ele1.getSection()

esfMz= scc0.getStressResultantComponent("Mz")

ratio1= (RM+loadMz)/loadMz
ratio2= (esfMz-loadMz)/loadMz

''' 
print(" ratio1= ",(ratio1))
print(" ratio2= ",(ratio2))
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
