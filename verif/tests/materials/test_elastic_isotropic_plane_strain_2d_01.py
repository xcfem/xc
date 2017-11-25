# -*- coding: utf-8 -*-
# Home made test

E= 30e6 # Young modulus (psi)
nu= 0.33 # Coeficiente de Poison
rho= 1.5 # Densidad

import xc_base
import geom
import xc
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

prb= xc.ProblemaEF()
preprocessor=  prb.getPreprocessor

elast2d= typical_materials.defElasticIsotropicPlaneStrain(preprocessor, "elast2d",E,nu,rho)
EE= elast2d.E
V= elast2d.nu
dens= elast2d.rho

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(E-EE)<1e-15) & (abs(V-nu)<1e-15) & (abs(dens-rho)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
