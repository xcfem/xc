# -*- coding: utf-8 -*-
''' Test for thermal loads according to Spanihsh IAP standard.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.thermal import IAP_thermal

Tmin1= IAP_thermal.getMinAnnualTemp(climateZn= 1, height= 0.0, returnPeriod= 50.0)
ratio1= abs(Tmin1+7)/7
Tmin2= IAP_thermal.getMinAnnualTemp(climateZn= 7, height= 0.0, returnPeriod= 50.0)
ratio2= abs(Tmin2-6)/6
Tmin3= IAP_thermal.getMinAnnualTemp(climateZn= 1, height= 2000.0, returnPeriod= 50.0)
ratio3= abs(Tmin3+33)/33
Tmin4= IAP_thermal.getMinAnnualTemp(climateZn= 7, height= 2000.0, returnPeriod= 50.0)
ratio4= abs(Tmin4+10)/10
Tmin5= IAP_thermal.getMinAnnualTemp(climateZn= 4, height= 1000.0, returnPeriod= 50.0)
ratio5= abs(Tmin5+16)/16

'''
print('Tmin1= ', Tmin1)
print('ratio1= ', ratio1)
print('Tmin2= ', Tmin2)
print('ratio2= ', ratio2)
print('Tmin3= ', Tmin3)
print('ratio3= ', ratio3)
print('Tmin4= ', Tmin4)
print('ratio4= ', ratio4)
print('Tmin5= ', Tmin5)
print('ratio5= ', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((ratio1<1e-15) and (ratio2<1e-15) and (ratio3<1e-15) and (ratio4<1e-15) and (ratio5<1e-15)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
