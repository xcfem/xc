# -*- coding: utf-8 -*-
from __future__ import print_function
import xc_base
import geom
import xc
from materials import typical_materials
import math

# Verification test of the «rotate» method of an 3D elastic section.
Iz= 2
Iy= 3

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
# Define materials
scc= typical_materials.defElasticSection3d(preprocessor, "scc",1,1,1,Iz,Iy,4)
scc.sectionProperties.rotate(math.radians(90))
ratio0= abs(Iy-scc.sectionProperties.Iz)/Iy
ratio1= abs(Iz-scc.sectionProperties.Iy)/Iz
ratio2= abs(scc.sectionProperties.Iyz)


''' 
print("ratio0= ",ratio0)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0<1e-15) & (ratio1<1e-15) & (ratio2<1e-15)): 
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
