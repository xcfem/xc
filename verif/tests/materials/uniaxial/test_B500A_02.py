# -*- coding: utf-8 -*-
# Home made test
# Diagrama tensión deformación de cálculo del acero B500A (SIA)
import xc_base
import geom
import xc

from materials.sia262 import SIA_reinforcing_steel
from materials import reinforcingSteelTest

# Model definition
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
preprocessor=  prueba.getPreprocessor
# Definimos materiales
errMax= reinforcingSteelTest.testDiagDAceroArmar(preprocessor, SIA_reinforcing_steel.B500A)


# print "errMax= ",(errMax)
import os
fname= os.path.basename(__file__)
if errMax<1e-10:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
