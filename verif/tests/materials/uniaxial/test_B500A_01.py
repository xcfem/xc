# -*- coding: utf-8 -*-
# Home made test
import xc_base
import geom
import xc

from materials.sia262 import SIA_reinforcing_steel
from materials import reinforcingSteelTest


# Definición del modelo
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
mdlr= prueba.getModelador
# Definimos materiales
errMax= reinforcingSteelTest.testDiagKAceroArmar(mdlr,SIA_reinforcing_steel.B500A)


# print "errMax= ",(errMax)
import os
fname= os.path.basename(__file__)
if errMax<1e-10:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
