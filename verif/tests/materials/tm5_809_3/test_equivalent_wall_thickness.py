# -*- coding: utf-8 -*-
''' Verification of the getEquivalentWallThickness
    method of the TM 5-809-3 manual of US Army.
'''
    
from __future__ import division
from __future__ import print_function

import math
from materials.tm5_809_3 import tm5_809_3_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

ratios=[]
cmuWall6= tm5_809_3_materials.CMUWallFabric(thickness= 6*0.0254,spacing= 0.0)
eqThickness6= cmuWall6.getEquivalentWallThickness()
effArea6= cmuWall6.getEffectiveArea()
cmuWall12= tm5_809_3_materials.CMUWallFabric(thickness= 12*0.0254,spacing= 72.0*0.0254)
eqThickness12= cmuWall12.getEquivalentWallThickness()
effArea12= cmuWall12.getEffectiveArea()
cmuWall8= tm5_809_3_materials.CMUWallFabric(thickness= 8*0.0254,spacing= 40*0.0254)
eqThickness8= cmuWall8.getEquivalentWallThickness()
effArea8= cmuWall8.getEffectiveArea()
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


ratios.append((eqThickness6/0.0254-5.62)/5.62)
ratios.append((effArea6/0.0254**2*0.3048-68)/68)
ratios.append((eqThickness12/0.0254-3.9)/3.9)
ratios.append((effArea12/0.0254**2*0.3048-47)/47)
ratios.append((eqThickness8/0.0254-3.46)/3.46)
ratios.append((effArea8/0.0254**2*0.3048-42)/42)
ratios.append((Ig12/0.0254**4-4119)/4119)
ratios.append(abs(fr-fr_ref)/fr_ref)
ratios.append(abs(Mcr8*8.85-Mcr8_ref)/Mcr8_ref)
ratios.append(abs(Mcr12*8.85-Mcr12_ref)/Mcr12_ref)

'''
print('eqThickness6= ', eqThickness6/0.0254, ' in')
print('ratio= ', ratios[0])
print('eqThickness12= ', eqThickness12/0.0254, ' in')
print('ratio= ', ratios[2])
print('eqThickness8= ', eqThickness8/0.0254, ' in')
print('ratio= ', ratios[3])
print(effArea12/0.0254**2*0.3048)
print(effArea8/0.0254**2*0.3048)
print('b12= ',b12/0.0254,' in')
print('Ig12= ',Ig12/(0.0254**4),' in4')
print('fr= ',fr,' psi')
print('fr_ref= ',fr_ref,' psi')

print('b8= ',b8/0.0254,' in')
print('Ig8= ',Ig8/(0.0254**4),' in4')
print('Mcr8= ',Mcr8*8.85,' lb-in')
print('Mcr8_ref= ',Mcr12_ref,' lb-in')

print('Mcr12= ',Mcr12*8.85,' lb-in')
print('Mcr12_ref= ',Mcr12_ref,' lb-in')
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(max(ratios))<1e-15 and abs(min(ratios))<1e-15):
  print("test ",fname,": ok.")
else:
  print(ratios)
  lmsg.error(fname+' ERROR.')
