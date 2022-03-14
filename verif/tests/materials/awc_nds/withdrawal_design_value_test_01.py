# -*- coding: utf-8 -*-
''' Example E2.1a Withdrawal design value - Plain Shank Nail
    from "STRUCTURAL WOOD DESIGN SOLVED EXAMPLE PROBLEMS"
    of the American Wood Council.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from materials.awc_nds import specific_gravity as sg
from materials.awc_nds import dowel_type_fasteners as fasteners
from materials.awc_nds import AWCNDS_materials as mat
from misc_utils import units_utils

fastener= fasteners.Nail(diameter= 0.131*units_utils.inchToMeter, length= 2.5*units_utils.inchToMeter)

G= sg.specificGravity['Spruce-Pine-Fir'] # Specific gravity
ratioG= (G-0.42)/0.42
sideMemberThickness= 0.105*units_utils.inchToMeter # Side member thickness
screwPenetration= fastener.L-sideMemberThickness # Screw penetration into main member.
ptRef= 2.395*units_utils.inchToMeter
ratio1= (screwPenetration-ptRef)/ptRef
W= fastener.getReferenceWithdrawal(G)
WRef= 3.61931305943e3
ratio2= (W-WRef)/WRef
Wd= fastener.getDesignWithdrawal(G,sideMemberThickness)
WdRef= WRef*ptRef
ratio3= abs(Wd-WdRef)/WdRef

'''
print('G= ', G)
print('ratio G= ', ratioG)
print('screw penetration pt= ', screwPenetration*1e3, 'mm (', screwPenetration/units_utils.inchToMeter, ' in)')
print(ratio1)
print('W= ', W/1e3, ' kN/m (', W/mat.units_utils.poundToN*units_utils.inchToMeter,'lbs/in)')
print(ratio2)
print('Wd= ', Wd/1e3, ' kN (', Wd/mat.units_utils.poundToN,'lbs)')
print(ratio3)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratioG)<1e-10 and abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
