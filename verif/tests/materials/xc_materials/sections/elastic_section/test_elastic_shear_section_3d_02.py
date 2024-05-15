# -*- coding: utf-8 -*-
from __future__ import print_function
'''Elastic 3D section verification.'''

import xc
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
feProblem.logFileName= "/tmp/erase.log" # Don't print warnings
preprocessor=  feProblem.getPreprocessor

# Rectangular cross-section definition
testSection= section_properties.RectangularSection(name="testSection", b= 1.4, h=0.45) # Section geometry.

mat= typical_materials.MaterialData(name='mtrectang',E=2.1e6,nu=0.3,rho=2500) # Section material.

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
xcSection= testSection.defElasticShearSection3d(preprocessor,mat)

# Check warping constant values.
iwRef= 0.0011404747886800959
## On the Python rectangular section object
Iw= testSection.getWarpingConstant()
ratio1= (Iw-iwRef)/iwRef
## On the XC cross section properties.
xcIw= xcSection.sectionProperties.Iw
ratio2= (xcIw-iwRef)/iwRef

'''
print(Iw, ratio1)
print(xcIw, ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
