# -*- coding: utf-8 -*-
''' Example E2.3 Withdrawal design value - Lag Screw
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



fastener= fasteners.LagScrew(diameter= 0.5*units_utils.inchToMeter, length= 4.0*units_utils.inchToMeter, tip= .3125*units_utils.inchToMeter, rootDiameter= 0.265*units_utils.inchToMeter) 
G= sg.specificGravity['Southern Pine'] # Specific gravity
sideMemberThickness= 1.5*units_utils.inchToMeter # Side member thickness
screwPenetration= fastener.getPenetration(sideMemberThickness) # Screw penetration into main member.
W= fastener.getReferenceWithdrawal(G)
Wd= fastener.getDesignWithdrawal(G,sideMemberThickness,endGrainFactor= 1.0)
ratio1= abs(Wd-4.2479398904e3)/4.2479398904e3

'''
print('G= ', G)
print('W= ', W/1e3, ' kN/m (', W/mat.pound2N*units_utils.inchToMeter,'lbs/in)')
print('screw penetration pt= ', screwPenetration*1e3, 'mm (', screwPenetration/units_utils.inchToMeter, ' in)')
print('Wd= ', Wd/1e3, ' kN (', Wd/mat.pound2N,'lbs)')
print(ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
