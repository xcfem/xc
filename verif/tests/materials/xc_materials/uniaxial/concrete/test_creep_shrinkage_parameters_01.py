# -*- coding: utf-8 -*-
''' Trivial test of CreeShrinkageParameters class.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials import typical_materials

Tcr = 28 # creep model age (in days)
epsshu = -600e-6 # Ultimate shrinkage strain
psish = 75.4218 # Based on section dimensions
phiu = 3.0 # ultimate creep coefficient as per ACI 209R-92
psicr1 = 1.0 # Recommended value
psicr2 = 75.4218 # fitting parameter of the creep time evolution function as per ACI 209R-92. Based on section dimensions
csParameters= typical_materials.def_creep_and_shrinkage_parameters(tcr= Tcr, epsshu= epsshu, epssha= psish, epscru= phiu, epscra= psicr1, epscrd= psicr2)

error= (csParameters.tcr-Tcr)**2 # 1
error+= (csParameters.epsshu-epsshu)**2 # 2
error+= (csParameters.epssha-psish)**2 # 3
error+= (csParameters.epscru-phiu)**2 # 4
error+= (csParameters.epscra-psicr1)**2 # 5
error+= (csParameters.epscrd-psicr2)**2 # 6
error= math.sqrt(error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
