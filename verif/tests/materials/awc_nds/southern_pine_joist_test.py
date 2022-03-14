# -*- coding: utf-8 -*-
''' Home made test.'''

 
from __future__ import division
from __future__ import print_function
from materials.awc_nds import AWCNDS_materials as mat
from materials.awc_nds import dimensional_lumber
from materials.sections import section_properties as sp
from rough_calculations import ng_simple_beam as sb
from misc_utils import units_utils

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2015, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"




span= 16*units_utils.footToMeter
designLoad= 77.3*units_utils.poundToN/units_utils.footToMeter # Design load
designMoment= designLoad*span**2/8.0

wood= dimensional_lumber.SouthernPineWood(name='SouthernPine', grade= 'no_2', sub_grade= '')
joist= mat.DimensionLumberSection(name= '2x10', woodMaterial= wood)

S= joist.Wzel() # Section modulus
I= joist.Iz() # Moment of inertia
fb= designMoment/S # Bendign stress
Fb= joist.getFb()
E= joist.wood.E
Fv= joist.wood.Fv

beam= sb.SimpleBeam(E,I,span)
delta= beam.getDeflectionUnderUniformLoad(designLoad,beam.l/2.0)


ratio1= abs(Fb-5.500882352941176e6)/5.500882352941176e6
ratio2= abs(Fv-1.2e6)/1.2e6
ratio3= abs(delta-20.801166867982307e-3)/20.801166867982307e-3

'''
print('b= ', joist.b*1000, 'mm (',joist.b/units_utils.inchToMeter,'in)')
print('h= ', joist.h*1000, 'mm (',joist.h/units_utils.inchToMeter,'in)')
print('S= ', S, 'm3 (',S/units_utils.inchToMeter**3,'in3)')
print('Bending stress fb= ', fb/1e6, 'MPa (',fb/units_utils.poundToN*units_utils.inchToMeter**2,'psi)')
print('Fb= ', Fb/1e6, 'MPa (',Fb/units_utils.poundToN*units_utils.inchToMeter**2,'psi)')
print('ratio1= ',ratio1)
print('E= ', E/1e6, 'MPa (',E/units_utils.poundToN*units_utils.inchToMeter**2,'psi)')
print('Fv= ', Fv/1e6, 'MPa (',Fv/units_utils.poundToN*units_utils.inchToMeter**2,'psi)')
print('ratio2= ',ratio2)
print('delta= ', delta*1000, 'mm (',delta/units_utils.inchToMeter,'in)')
print('ratio3= ',ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
