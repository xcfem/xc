# -*- coding: utf-8 -*-
# home made test
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE E.10 Rectangular HSS compression member with slender elements

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from materials.astm_aisc import ASTM_materials

in2m= 0.0254
MPa2ksi= 0.145038
kN2kips= 0.2248
m2Toin2= 1.0/in2m**2

steel= ASTM_materials.A500
hss= ASTM_materials.HSSShape(steel,'HSS12X8X3/16')
connection= ASTM_materials.MemberConnection(L= 30*0.3048,rotI='fixed',transI='fixed',rotJ= 'free',transJ= 'fixed')
member= ASTM_materials.MemberWithEndConnections(hss,connection)

Pn= member.getNominalCompressiveStrength()
PnRef= 735.8325968753835e3
ratio1= abs(Pn-PnRef)/PnRef

'''
print('Pn: ', Pn/1e3, ' kN', Pn/1e3*kN2kips, ' kips')
print('ratio1: ', ratio1)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
