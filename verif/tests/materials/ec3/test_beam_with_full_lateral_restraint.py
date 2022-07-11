# -*- coding: utf-8 -*-
''' Beam with full lateral restraint.
   Example 2.2 from:

   Handbook of Structural Steelwork (Eurocode Edition)

   The British Constructional Steelwork Association Ltd 
   4 Whitehall Court London SW1A 2ES

   The Steel Construction Institute
   Silwood Park
   Ascot
   SL5 7QN
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
from materials.ec3 import EC3_materials
from rough_calculations import ng_simple_beam as sb
from materials.sections.structural_shapes import arcelor_metric_shapes

# Material
steel= EC3_materials.S275JR
steel.setGammaM0(1.0)
shape= EC3_materials.UBShape(steel= steel, name= 'UB356x127x33')

# Loads
deadLoad= 14e3 # N/m
liveLoad= 19e3 # N/m

# Ultimate limit state load.
q= -(0.925*1.35*deadLoad+1.5*liveLoad) # N/m

# Beam.
beam= sb.SimpleBeam()
beam.l= 5 # m

# Internal forces.
Md= beam.getBendingMomentUnderUniformLoad(q= q, x= beam.l/2.0)
MdTeor= -q*beam.l**2/8.0
ratio1= abs(Md-MdTeor)/MdTeor

Vd= beam.getShearUnderUniformLoad(q= q, x= beam.l)
VdTeor= -q*beam.l/2.0
ratio2= abs(Vd-VdTeor)/VdTeor

# Section classification.
c1_3= shape.getClassOutstandPartInCompression() # Flange in compression.
ratio3= abs(c1_3-1)
c1_2= shape.getClassInternalPartInBending() # Web in bending
ratio4= abs(c1_2-1)

# Shear resistance.
Av= shape.getAvy() # Shear area
A= shape.A()
b= shape.b()
tf= shape.tf()
tw= shape.tw()
r= float(shape.get('r'))
AvRef= A-2*b*tf+(tw+2*r)*tf
ratio5= abs(Av-AvRef)/AvRef
Vpl_Rd= shape.getVplRdy()
Vpl_RdRef= AvRef*275e6/math.sqrt(3)
ratio6= abs(Vpl_Rd-Vpl_RdRef)/Vpl_RdRef
# Bending resistance
Mc_Rd= shape.getMvRdz(sectionClass= max(c1_2, c1_3), Vd= Vd)
Wpl= shape.get('Wzpl')
Mc_RdRef= Wpl*steel.fy/1.0
ratio7= abs(Mc_Rd-Mc_RdRef)/Mc_RdRef

'''
print('Md= ', Md/1e3, ' kN.m')
print('ratio1= ', ratio1)
print('Vd= ', Vd/1e3, ' kN/m')
print('ratio2= ', ratio2)
print('compression class: ', c1_3)
print('ratio3= ', ratio3)
print('bending class: ', c1_2)
print('ratio4= ', ratio4)
print('shear area: ', Av*1e6, 'mm2')
print('reference shear area: ', AvRef*1e6, 'mm2')
print('ratio5= ', ratio5)
print('gammaM0= ', shape.steelType.gammaM0())
print('shear resistance: Vpl_Rd=', Vpl_Rd/1e3, 'kN')
print('reference shear resistance: Vpl_RdRef=', Vpl_RdRef/1e3, 'kN')
print('ratio6= ', ratio6)
print('bending resistance: Mc_Rd=', Mc_Rd/1e3, 'kN.m')
print('reference bending resistance: Mc_RdRef=', Mc_RdRef/1e3, 'kN.m')
print('ratio7= ', ratio7)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-11 and abs(ratio2)<1e-11 and abs(ratio3)<1e-11 and (abs(ratio4)<1e-11) and (abs(ratio5)<1e-11) and (abs(ratio6)<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
