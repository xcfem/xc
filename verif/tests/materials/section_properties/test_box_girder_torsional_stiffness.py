# -*- coding: utf-8 -*-
''' Home made test from the example at page C-17 of the
"Bridge Strudl Manual" november 1973 '''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

##
##                        bs
##             |----------------------|
##
##                        ts
##    -    ---------------------------------
##    |         \                    /
##    |h         \td                /
##    |           \       ti       /
##    -            ----------------
##
##                        bi
##                 |--------------|
##

bs= 26.51 # bs: Upper deck width (without the overhangs)
bi= 21.05 # bi: Lower deck width.
h= 5.48 # h: Box depth (between mid-planes).
ts= .56 # ts: Upper deck thickness.
ti= .48 # ti: Lower deck thickness.
td= 1 # td: Thickness of the webs.

from materials.sections import section_properties

J= section_properties.getInerciaTorsionCajonMonocelular(bs,bi,h,ts,ti,td)
ratio1= abs(J-656.6)/656.6

''' 
print("J= ",J)
print("ratio1= ",ratio1)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<2e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
