# -*- coding: utf-8 -*-
''' Verification of the getEquivalentWallThickness
    method of the TM 5-809-3 manual of US Army.
'''
    
from __future__ import division
from __future__ import print_function

from materials.tm5_809_3 import tm5_809_3_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

cmuWall6= tm5_809_3_materials.CMUWall(thickness= 6*0.0254,spacing= 0.0)
eqThickness6= cmuWall6.getEquivalentWallThickness()
cmuWall12= tm5_809_3_materials.CMUWall(thickness= 12*0.0254,spacing= 64.0*0.0254)
eqThickness12= cmuWall12.getEquivalentWallThickness()
cmuWall8= tm5_809_3_materials.CMUWall(thickness= 8*0.0254,spacing= 40*0.0254)
eqThickness8= cmuWall8.getEquivalentWallThickness()

ratio1= (eqThickness6/0.0254-5.62)/5.62
ratio2= (eqThickness12/0.0254-4.01)/4.01
ratio3= (eqThickness8/0.0254-3.46)/3.46

'''
print('eqThickness6= ', eqThickness6/0.0254, ' in')
print('ratio1= ', ratio1)
print('eqThickness12= ', eqThickness12/0.0254, ' in')
print('ratio2= ', ratio2)
print('eqThickness8= ', eqThickness8/0.0254, ' in')
print('ratio3= ', ratio3)
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15 and abs(ratio1)<1e-15 and abs(ratio1)<1e-15):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
