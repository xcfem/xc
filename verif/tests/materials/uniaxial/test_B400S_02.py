# -*- coding: utf-8 -*-
# Home made test
# Design stress-strain diagram of B400S steel
import xc_base
import geom
import xc
from materials.ehe import EHE_reinforcing_steel
from materials import reinforcingSteelTest

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
preprocessor=  prueba.getPreprocessor
# Define materials
errMax= reinforcingSteelTest.testDiagDAceroArmar(preprocessor, EHE_reinforcing_steel.B400S)


# print "errMax= ",(errMax)
import os
fname= os.path.basename(__file__)
if errMax<1e-10:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
