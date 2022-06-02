# -*- coding: utf-8 -*-
''' Home made test.'''

from materials.ehe import EHE_materials
from rough_calculations import ng_pile_cap

pileCap= ng_pile_cap.TwoPileCap(pileDiameter=150e-3, mainReinfDiameter=16e-3, reinfSteel= EHE_materials.B400S, h= 0.6, b= 0.6, l= 1.6, d= 1.0, corbelLength= 0.3-150e-3/2.0)

# Loads.
Nd= 1.5*183e3

# Main reinforcement.
Td= pileCap.getTensionOnBottomReinforcement(Nd= Nd)
ratio1= abs(Td-219.6e3)/219.6e3
As1= pileCap.getBottomReinforcementReqArea(Nd= Nd)
ratio2= abs(As1-6.3135e-4)/6.3135e-4

# Secondary reinforcement.
As2= pileCap.getTopReinforcementReqArea(Nd= Nd)
ratio3= abs(As2-6.3135e-5)/6.3135e-5
As3v= pileCap.getShearReinforcementReqArea()
ratio4= abs(As3v-19.2e-4)/19.2e-4
As3h= pileCap.getHorizontalStirrupsReqArea()
ratio5= abs(As3h-7.2e-4)/7.2e-4

'''
print('Td= ', Td/1e3,'kN')
print('ratio1= ',ratio1)
print('As1= ', As1*1e4,'cm2')
print('ratio2= ', ratio2)
print('As2= ', As2*1e4,'cm2')
print('ratio3= ', ratio3)
print('As3v= ', As3v*1e4,'cm2')
print('ratio4= ', ratio4)
print('As3h= ', As3h*1e4,'cm2')
print('ratio5= ', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12 and abs(ratio4)<1e-12 and abs(ratio5)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')





