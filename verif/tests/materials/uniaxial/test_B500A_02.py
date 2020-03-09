# -*- coding: utf-8 -*-
# Home made test
# Design stress-strain diagram of the steel B500A (SIA)
import xc_base
import geom
import xc

from materials.sia262 import SIA262_materials
from materials import concrete_base

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Para no imprimir mensajes de advertencia
preprocessor=  feProblem.getPreprocessor
# Define materials
errMax= concrete_base.testReinfSteelDesignDiagram(preprocessor, SIA262_materials.B500A)


# print "errMax= ",(errMax)
import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if errMax<1e-10:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
