# -*- coding: utf-8 -*-
''' Test of J2PlateFibre material.Home made test'''
from __future__ import print_function
from __future__ import division

E= 29e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
fy= 36e3 # yield stress
alpha= .01 # strain hardening ratio
rho= 7850

import xc_base
import geom
import xc
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

j2plate= typical_materials.defJ2PlateFibre(preprocessor, 'j2plate',E,nu,fy,alpha, rho)
EE= j2plate.E
V= j2plate.nu
Fy= j2plate.fy
h= alpha*E/(1-alpha)
Hiso= j2plate.Hiso
Hkin= j2plate.Hkin

'''
print('h= ', h)
print('Hiso= ', Hiso)
print('Hkin= ', Hkin)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(E-EE)<1e-15) and (abs(V-nu)<1e-15) and (abs(fy-Fy)<1e-15) and (abs(h-Hiso)<1e-15) and (abs(Hkin)<1e-15) and (abs(j2plate.rho-rho)<1e-15):
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')
