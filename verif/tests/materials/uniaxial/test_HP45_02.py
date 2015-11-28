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
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
preprocessor=  prueba.getPreprocessor
HP45= EHE_concrete.HA45
errMax= concreteBase.testTangDHormigon(preprocessor, HP45)

#print "errMax= ",errMax
import os
fname= os.path.basename(__file__)
if(errMax<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

