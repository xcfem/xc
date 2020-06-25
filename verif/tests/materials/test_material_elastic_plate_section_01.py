# -*- coding: utf-8 -*-
from __future__ import print_function
# Home made test

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Define materials
elasticPlateSectionTest= typical_materials.defElasticPlateSection(preprocessor, "elasticPlateSectionTest",E,nu,0.0,h)

ratio1= (elasticPlateSectionTest.E-E)/E
ratio2= (elasticPlateSectionTest.nu-nu)/nu
ratio3= (elasticPlateSectionTest.h-h)/h

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
