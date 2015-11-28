# -*- coding: utf-8 -*-
# Home made test

#*    Nov. 2015   Ana Ortega    *


import math
import xc_base
import geom
import xc
from materials import concreteBase
from materials.ehe import EHE_concrete




# Definición del modelo
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
mdlr= prueba.getPreprocessor
concr=EHE_concrete.HA25
concr.alfacc=0.85
errMax= concreteBase.testTangDHormigon(mdlr,concr)

#print "errMax= ",errMax
import os
fname= os.path.basename(__file__)
if(errMax<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
