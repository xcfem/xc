# -*- coding: utf-8 -*-
# Home made test

import math
import xc_base
import geom
import xc
from materials import typical_materials
from materials.ehe import auxEHE
from materials.ehe import hormigonesEHE



# Definición del modelo
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
mdlr= prueba.getModelador
HP45= hormigonesEHE.HA45
errMax= hormigonesEHE.testTangDHormigon(mdlr,HP45)

#print "errMax= ",errMax
import os
fname= os.path.basename(__file__)
if(errMax<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

