# -*- coding: utf-8 -*-
''' Example E2.1a Withdrawal design value - Plain Shank Nail
    from "STRUCTURAL WOOD DESIGN SOLVED EXAMPLE PROBLEMS"
    of the American Wood Council.'''

from __future__ import division
from __future__ import print_function
from materials.awc_nds import specific_gravity as sg
from materials.awc_nds import dowel_type_fasteners as fasteners
from materials.awc_nds import AWCNDS_materials as mat

inch2meter= 0.0254
pound2N= 4.44822

fastener= fasteners.Nail(diameter= 0.131*inch2meter, length= 2.5*inch2meter)

G= sg.specificGravity['Spruce-Pine-Fir'] # Specific gravity
ratioG= (G-0.42)/0.42
sideMemberThickness= 0.105*inch2meter # Side member thickness
screwPenetration= fastener.L-sideMemberThickness # Screw penetration into main member.
ptRef= 2.395*inch2meter
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
print('screw penetration pt= ', screwPenetration*1e3, 'mm (', screwPenetration/inch2meter, ' in)')
print(ratio1)
print('W= ', W/1e3, ' kN/m (', W/mat.pound2N*mat.in2meter,'lbs/in)')
print(ratio2)
print('Wd= ', Wd/1e3, ' kN (', Wd/mat.pound2N,'lbs)')
print(ratio3)
'''


import os
fname= os.path.basename(__file__)
if(abs(ratioG)<1e-10 and abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
