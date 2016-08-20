# -*- coding: utf-8 -*-
# Home made test
E= 2.1e6 # Módulo de Young del acero.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Espesor.

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Definimos materiales
prueba= typical_materials.defElasticPlateSection(preprocessor, "prueba",E,nu,0.0,h)

ratio1= (prueba.E-E)/E
ratio2= (prueba.nu-nu)/nu
ratio3= (prueba.h-h)/h

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
