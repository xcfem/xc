# -*- coding: utf-8 -*-
''' Example E2.3 Withdrawal design value - Lag Screw
    from "STRUCTURAL WOOD DESIGN SOLVED EXAMPLE PROBLEMS"
    of the American Wood Council.'''

from __future__ import division
from __future__ import print_function
from materials.awc_nds import specific_gravity as sg
from materials.awc_nds import dowel_type_fasteners as fasteners
from materials.awc_nds import AWCNDS_materials as mat

inch2meter= 0.0254

fastener= fasteners.LagScrew(diameter= 0.5*inch2meter, length= 4.0*0.0254, tip= .3125*inch2meter, rootDiameter= 0.265*mat.in2meter) 
G= sg.specificGravity['Southern Pine'] # Specific gravity
sideMemberThickness= 1.5*inch2meter # Side member thickness
screwPenetration= fastener.getScrewPenetration(sideMemberThickness) # Screw penetration into main member.
W= fastener.getReferenceWithdrawal(G)
Wd= fastener.getDesignWithdrawal(G,sideMemberThickness,endGrainFactor= 1.0)
ratio1= abs(Wd-4.2479398904e3)/4.2479398904e3

'''
print('G= ', G)
print('W= ', W/1e3, ' kN/m (', W/mat.pound2N*mat.in2meter,'lbs/in)')
print('screw penetration pt= ', screwPenetration*1e3, 'mm (', screwPenetration/inch2meter, ' in)')
print('Wd= ', Wd/1e3, ' kN (', Wd/mat.pound2N,'lbs)')
print(ratio1)
'''

import os
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-10):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
