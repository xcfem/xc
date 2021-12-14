# -*- coding: utf-8 -*-
''' Astrodome Sports Complex, Houston TX
 Example 1 from 'The art of structural engineering : Vaults' course,
 by María Garlock.'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega.Ort@gmail.com"

import math
from rough_calculations import vaults_rough_calc as vault


#Data. Units: ft,psf
radius=600 # spherical radius [ft]
angle=32.3 #angle from the top of the dome to the bottom of the dome
#          measured along the meridian [degrees]
q_dead=30   # uniform vertical dead load [psf]
q_live=15   # uniform vertical live load [psf]
#


astrodome=vault.SphericalDome(sphRadius=radius,alpha=math.radians(angle))
span=astrodome.getSpan()
height=astrodome.getRise()
area=astrodome.getSurfArea()
q_total=q_dead+q_live
Vreaction=astrodome.getVerticalSupportReaction(q_total)
Nreaction=astrodome.getMeridionalSupportReaction(q_total)
Hreaction=astrodome.getHorizSupportReaction(q_total)
Tring=astrodome.getTensionForceRing(q_total)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if max(abs(span-641),abs(height-93),abs(area-350009),abs(Vreaction-7819),abs(Hreaction-12368),abs(Tring-3965300))<0.5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
