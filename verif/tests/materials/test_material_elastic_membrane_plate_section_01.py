# -*- coding: utf-8 -*-
# Home made test
E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Define materials
prueba= typical_materials.defElasticMembranePlateSection(preprocessor, "prueba",E,nu,dens,h)


ratio1= (prueba.E-E)/E
ratio2= (prueba.nu-nu)/nu
ratio3= (prueba.h-h)/h
ratio4= (prueba.rho-dens)/dens

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12) & (abs(ratio4)<1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
