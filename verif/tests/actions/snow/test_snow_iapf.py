# -*- coding: utf-8 -*-
''' Wind load calculation.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
from actions.snow import iapf_snow

# Site data
climaticZone= 'III' # climatic zone.
z= 560 # height above sea level.

s_k= iapf_snow.get_snow_load_horizontal_terrain(z= z, climaticZone= climaticZone)

gammaSnow= iapf_snow.get_snow_specific_weight(z= z)

error= math.sqrt((s_k-400)**2+(gammaSnow-1.5e3)**2)

# print('Snow.' ) 
# print('  Climatic zone: ', climaticZone)
# print('  Height above sea level z= ', z, 'm')
# print('  Characteristic value of the snow load s_k= '+str(s_k/1e3)+' kN/m2') 
# print('  Snow specific weight gamma= '+str(gammaSnow/1e3)+' kN/m3') 

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
