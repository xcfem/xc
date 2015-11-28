# -*- coding: utf-8 -*-
# Home made test

import math
import xc_base
import geom
import xc
from materials import typical_materials
from materials import concreteBase
from materials.ehe import EHE_concrete


# Definición del modelo
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
preprocessor=  prueba.getPreprocessor
errMax= concreteBase.testDiagDHormigon(preprocessor, EHE_concrete.HA25)


# print "errMax= ",(errMax)

import os
fname= os.path.basename(__file__)
if errMax<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
