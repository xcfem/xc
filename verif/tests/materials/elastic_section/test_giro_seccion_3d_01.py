# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
from materials import typical_materials
import math

# Test de funcionamiento del comando «gira» para una sección elástica en 3d.
Iz= 2
Iy= 3

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
# Definimos materiales
scc= typical_materials.defElasticSection3d(preprocessor, "scc",1,1,1,Iz,Iy,4)
scc.sectionProperties.gira(math.radians(90))
ratio0= abs(Iy-scc.sectionProperties.Iz)/Iy
ratio1= abs(Iz-scc.sectionProperties.Iy)/Iz
ratio2= abs(scc.sectionProperties.Iyz)


''' 
print "ratio0= ",ratio0
print "ratio1= ",ratio1
print "ratio2= ",ratio2
   '''

import os
fname= os.path.basename(__file__)
if((ratio0<1e-15) & (ratio1<1e-15) & (ratio2<1e-15)): 
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
