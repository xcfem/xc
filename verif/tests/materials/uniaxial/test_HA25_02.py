# -*- coding: utf-8 -*-
# Home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import math
import xc_base
import geom
import xc
from materials import concreteBase
from materials.ehe import EHE_concrete


# Model definition
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Ignore warning messages
mdlr= prueba.getPreprocessor
concr=EHE_concrete.HA25
concr.alfacc=0.85 #f_maxd= 0.85*fcd
errMax= concreteBase.testTangDHormigon(mdlr,concr)

#print "errMax= ",errMax
import os
fname= os.path.basename(__file__)
if(errMax<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
