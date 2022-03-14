# -*- coding: utf-8 -*-
''' Verification of the getEquivalentWallThickness
    method of the TM 5-809-3 manual of US Army.
'''
    
from __future__ import division
from __future__ import print_function

import math
from materials.tm5_809_3 import tm5_809_3_materials
from misc_utils import units_utils

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPLq"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


ratios=[]
cmuWall6= tm5_809_3_materials.CMUWallFabric(thickness= 6*units_utils.inchToMeter,spacing= 1e-7)
eqThickness6= cmuWall6.getEquivalentWallThickness()
effArea6= cmuWall6.getEffectiveAreaPerUnitLength()
cmuWall12= tm5_809_3_materials.CMUWallFabric(thickness= 12*units_utils.inchToMeter,spacing= 72.0*units_utils.inchToMeter)
eqThickness12= cmuWall12.getEquivalentWallThickness()
effArea12= cmuWall12.getEffectiveAreaPerUnitLength()
cmuWall8= tm5_809_3_materials.CMUWallFabric(thickness= 8*units_utils.inchToMeter,spacing= 40*units_utils.inchToMeter)
eqThickness8= cmuWall8.getEquivalentWallThickness()
effArea8= cmuWall8.getEffectiveAreaPerUnitLength()
b12= cmuWall12.getEffectiveWidth()
Ig12= cmuWall12.getGrossMomentOfInertia()
fr= cmuWall12.mortar.fr()*tm5_809_3_materials.fromPascal
fr_ref= 2.5*math.sqrt(1350)
Mcr12= cmuWall12.getCrackingMomentStrength()
Mcr12_ref= 5424
b8= cmuWall8.getEffectiveWidth()
Ig8= cmuWall8.getGrossMomentOfInertia()
Mcr8= cmuWall8.getCrackingMomentStrength()
Mcr8_ref= 2235


ratios.append((eqThickness6/units_utils.inchToMeter-5.62)/5.62) #0
ratios.append((effArea6/units_utils.inchToMeter**2*units_utils.footToMeter-68)/68) #1
ratios.append((eqThickness12/units_utils.inchToMeter-3.9)/3.9) #2
ratios.append((effArea12/units_utils.inchToMeter**2*units_utils.footToMeter-47)/47) #3
ratios.append((eqThickness8/units_utils.inchToMeter-3.46)/3.46) #4
ratios.append((effArea8/units_utils.inchToMeter**2*units_utils.footToMeter-42)/42) #5
ratios.append((Ig12/units_utils.inchToMeter**4-4119)/4119)
ratios.append(abs(fr-fr_ref)/fr_ref)
ratios.append(abs(Mcr8*8.85-Mcr8_ref)/Mcr8_ref)
ratios.append(abs(Mcr12*8.85-Mcr12_ref)/Mcr12_ref)

'''
print('eqThickness6= ', eqThickness6/units_utils.inchToMeter, ' in')
print('ratio= ', ratios[0])
print('effArea6= ', effArea6/units_utils.inchToMeter**2*units_utils.footToMeter, ' in2/ft')
print('ratio= ', ratios[1])
print('eqThickness12= ', eqThickness12/units_utils.inchToMeter, ' in')
print('ratio= ', ratios[2])
print('effArea12= ', effArea12/units_utils.inchToMeter**2*units_utils.footToMeter, ' in2/ft')
print('ratio= ', ratios[3])
print('eqThickness8= ', eqThickness8/units_utils.inchToMeter, ' in')
print('ratio= ', ratios[4])
print('effArea12= ', effArea8/units_utils.inchToMeter**2*units_utils.footToMeter, ' in2/ft')
print('ratio= ', ratios[5])
print('b12= ',b12/units_utils.inchToMeter,' in')
print('Ig12= ',Ig12/(units_utils.inchToMeter**4),' in4')
print('fr= ',fr,' psi')
print('fr_ref= ',fr_ref,' psi')

print('b8= ',b8/units_utils.inchToMeter,' in')
print('Ig8= ',Ig8/(units_utils.inchToMeter**4),' in4')
print('Mcr8= ',Mcr8*8.85,' lb-in')
print('Mcr8_ref= ',Mcr12_ref,' lb-in')

print('Mcr12= ',Mcr12*8.85,' lb-in')
print('Mcr12_ref= ',Mcr12_ref,' lb-in')
print('ratios= ', ratios)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(max(ratios))<1e-5 and abs(min(ratios))<1e-5):
    print('test '+fname+': ok.')
else:
    print(ratios)
    lmsg.error(fname+' ERROR.')
