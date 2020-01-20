# -*- coding: utf-8 -*-

''' Home made test.'''

 
from __future__ import division
from __future__ import print_function
from materials.awc_nds import AWCNDS_materials as mat
from materials.sections import section_properties as sp
from rough_calculations import ng_simple_beam as sb

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2015, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

inch2meter= 0.0254
pound2Newton= 4.4482216282509
foot2meter= 0.3048

span= 16*foot2meter
designLoad= 77.3*pound2Newton/foot2meter # Design load
designMoment= designLoad*span**2/8.0

joist= mat.SouthernPineDimensionLumber(name= '2x6',b= 1.5*inch2meter, h= 9.25*inch2meter, grade= 'no_2', sub_grade= '')

S= joist.Wzel() # Section modulus
I= joist.Iz() # Moment of inertia
fb= designMoment/S # Bendign stress
Fb= joist.getFb()
E= joist.E
Fv= joist.Fv

beam= sb.SimpleBeam(E,I,span)
delta= beam.getDeflectionUnderUniformLoad(designLoad,beam.l/2.0)


ratio1= (Fb-5.50088235294e6)/5.50088235294e6
ratio2= (Fv-1.2e6)/1.2e6
ratio3= (delta-20.801166868e-3)/20.801166868e-3

'''
print('b= ', joist.b*1000, 'mm (',joist.b/inch2meter,'in)')
print('h= ', joist.h*1000, 'mm (',joist.h/inch2meter,'in)')
print('S= ', S, 'm3 (',S/inch2meter**3,'in3)')
print('Bending stress fb= ', fb/1e6, 'MPa (',fb/pound2Newton*inch2meter**2,'psi)')
print('Fb= ', Fb/1e6, 'MPa (',Fb/pound2Newton*inch2meter**2,'psi)')
print('E= ', E/1e6, 'MPa (',E/pound2Newton*inch2meter**2,'psi)')
print('delta= ', delta*1000, 'mm (',delta/inch2meter,'in)')
print('Fv= ', Fv/1e6, 'MPa (',Fv/pound2Newton*inch2meter**2,'psi)')
print('ratio1= ',ratio1)
print('ratio2= ',ratio2)
print('ratio3= ',ratio3)
'''

import os
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
