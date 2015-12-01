# -*- coding: utf-8 -*-

#*    Nov. 2015   Ana Ortega    *

# Home made test

import math
import xc_base
import geom
import xc
from materials import concreteBase
from materials.ehe import EHE_concrete

# Definición del modelo
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
mdlr= prueba.getPreprocessor
concr=EHE_concrete.HA25
concr.alfacc=0.85
errMax= concreteBase.testDiagDHormigon(mdlr,concr)


#print "errMax= ",(errMax)

import os
fname= os.path.basename(__file__)
if errMax<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
