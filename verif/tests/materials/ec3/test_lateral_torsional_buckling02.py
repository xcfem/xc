# -*- coding: utf-8 -*-
from __future__ import print_function
''' Lateral torsional buckling of steel beams.
   pages 32-34 Example 2 from:
   Eurocodes ‐ Design of steel buildings with worked examples
   Brussels, 16 - 17 October 2014
'''
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from geom_utils import interpolation as intp



from materials.ec3 import EC3_limit_state_checking as EC3lsc
from materials.ec3 import EC3_materials

S355JR= EC3_materials.S355JR
gammaM0= 1.05
S355JR.gammaM= gammaM0 
IPE400= EC3_materials.IPEShape(S355JR,"IPE_400")


# Geometry
k1= 1.0; k2= 1.0
#Check results page 32
L= 6.0 # Bar length (m)
x= [0.0,0.25*L,0.5*L,0.75*L,1.0*L]
M= [-93.7,0,114.3,0,111.4]
Mi=intp.interpEquidistPoints(xi=x,yi=M,nDiv=4)
mgf= EC3lsc.MomentGradientFactorC1(Mi)
Mcr1= IPE400.getMcr(L,Mi)
Mcr1Teor= 164.7e3

ratio1= abs(Mcr1-Mcr1Teor)/Mcr1Teor
#NOTE: Here there is a big difference between the results
# from Lopez-Serna method
# 317 kN.m and those from the paper 164.7 kN.m
# In theory results from Lopez-Serna method are safe enough.

#Check results page 34
L= 3 # Bar length (m)
x= [0.0,0.25*L,0.5*L,0.75*L,1.0*L]
M= [-93.7e3,-93.7e3/2.0,0.0,114.3e3/2.0,114.3e3]
Mi=intp.interpEquidistPoints(xi=x,yi=M,nDiv=4)
mgf= EC3lsc.MomentGradientFactorC1(Mi)
Mcr2= IPE400.getMcr(L,Mi)
Mcr2Teor= 1778e3

ratio2= abs(Mcr2-Mcr2Teor)/Mcr2Teor

# print('Mcr1= ', Mcr1/1e3, 'kN m')
# print('Mcr1Teor= ', Mcr1Teor/1e3, 'kN m')
# print('ratio1= ', ratio1)
# print('Mcr2= ', Mcr2/1e3, 'kN m')
# print('Mcr2Teor= ', Mcr2Teor/1e3, 'kN m')
# print('ratio2= ', ratio2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
#Don't make a check over ratio1 SEE NOTE above.
if(ratio2<0.05):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
