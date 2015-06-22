# -*- coding: utf-8 -*-
# Home made test
E= 30e6 # Young modulus (psi)
nu= 0.33 # Coeficiente de Poison
rho= 1.5 # Densidad

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

# Definimos materiales
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
mat= typical_materials.defElasticIsotropic3d(mdlr,"elastIso3d",E,nu,rho)

import os
fname= os.path.basename(__file__)
if (abs(mat.E-E)<1e-15) & (abs(mat.nu-nu)<1e-15) & (abs(mat.rho-rho)<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
