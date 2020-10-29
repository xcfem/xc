# -*- coding: utf-8 -*-
'''Classification of steel cross-sections (clause 5.5 of EN 1993-1-1:2005).'''

from __future__ import print_function
from __future__ import division


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2018, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

from materials.ec3 import EC3_materials

S235JR= EC3_materials.S235JR
S235JR.gammaM= 1.00

IPE300= EC3_materials.IPEShape(steel=S235JR,name='IPE_A_300')
c1_1=IPE300.getClassInternalPartInCompression(S235JR)
c1_2=IPE300.getClassInternalPartInBending(S235JR)
c1_3=IPE300.getClassOutstandPartInCompression(S235JR)

IPN300= EC3_materials.IPNShape(steel=S235JR,name='IPN_300')
c2_1=IPN300.getClassInternalPartInCompression(S235JR)
c2_2=IPN300.getClassInternalPartInBending(S235JR)
c2_3=IPN300.getClassOutstandPartInCompression(S235JR)


HE300= EC3_materials.HEShape(steel=S235JR,name='HE_300_A')
c3_1=HE300.getClassInternalPartInCompression(S235JR)
c3_2=HE300.getClassInternalPartInBending(S235JR)
c3_3=HE300.getClassOutstandPartInCompression(S235JR)


SHS175= EC3_materials.SHSShape(steel=S235JR,name='SHS175x175x8')
c4_1=SHS175.getClassInternalPartInCompression(S235JR)
c4_2=SHS175.getClassInternalPartInBending(S235JR)


RHS250= EC3_materials.RHSShape(steel=S235JR,name='RHS250x150x16')
c5_1=RHS250.getClassInternalPartInCompression(S235JR)
c5_2=RHS250.getClassInternalPartInBending(S235JR)

UPN400=EC3_materials.UPNShape(steel=S235JR,name='UPN_400')
c6_1=UPN400.getClassInternalPartInCompression(S235JR)
c6_2=UPN400.getClassInternalPartInBending(S235JR)
c6_3=UPN400.getClassOutstandPartInCompression(S235JR)

ratioLst=[c1_1-3,c1_2-1,c1_3-1,
          c2_1-1,c2_2-1,c2_3-1,
          c3_1-1,c3_2-1,c3_3-1,
          c4_1-1,c4_2-1,
          c5_1-1,c5_2-1,
          c6_1-1,c6_2-1,c6_3-1]

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(sum(ratioLst)/len(ratioLst))<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

