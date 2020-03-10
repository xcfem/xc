# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function
from materials.aci import ACI_materials
from materials.aci import ACI_limit_state_checking
import os
from misc_utils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#Example in R26.4.2.3 of ACI 318-14
rebar= '#7'
rebarDiam= ACI_materials.standard_bars_diameters[rebar]
clearCover= 2.0*rebarDiam
cCover= clearCover+rebarDiam/2.0

rebarController= ACI_limit_state_checking.RebarController(psi_t= 1.0, concreteCover= cCover)

confinementTerm= rebarController.getConfinementTerm(rebarDiam)

ratio1= (confinementTerm-2.5)/2.5

# Example in R26.4.2.2 of ACI 318-14 but using
# expression (25.4.2.3a).
concrete= ACI_materials.c4000
reinfSteel= ACI_materials.A615G60
ld= rebarController.getBasicAnchorageLength(concrete,rebarDiam,reinfSteel)
ldRef= 28.5509262607*rebarDiam
ratio2= (ld-ldRef)/ldRef

'''
print('confinementTerm= ', confinementTerm)
print('ld= ', ld/rebarDiam,'d_b')
print('ratio2= ', ratio2)
'''

fname= os.path.basename(__file__)
if((ratio1<1e-15) and (ratio2<1e-3)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
