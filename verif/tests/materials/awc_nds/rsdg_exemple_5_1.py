# -*- coding: utf-8 -*-

''' Example 5.1 of the residential structural design guide.
    Second Edition. '''

 
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

L= 30*47.88026 # Live load N/m2
D= 10*47.88026 # Dead load N/m2
joistSpacing= 16*0.0254 # Space between joists
joistSection= sp.RectangularSection('joistSection',1.5*0.0254,7.25*0.0254)
Ss= joistSection.getElasticSectionModulusZ()
SsRef= 13.14*0.0254**3
ratio1= (Ss-SsRef)/SsRef
Is= joistSection.Iz()
IsRef= 47.63*0.0254**4
ratio2= (Is-IsRef)/IsRef

# Tabulated design values
Fb= 975*6894.76 # Bending stress (Pa)
E= 1.5e6*6894.76 # Modulus of elasticity (Pa)
Fv= 75*6894.76 # Shear stress (Pa)
FcT= 405*6894.76


CD= mat.getLoadDurationFactor(10)
ratio3= (CD-1.0)
Cr= 1.15 # Repetitive member factor (AWC-NDS2018 supplement table 4F)
CF= 1.2 # Size factor (AWC-NDS2018 supplement table 4F)
CH= 2.0 # Horizontal shear factor
CL= 1.0 # Continuous lateral support. NDS clause 3.3.3
Cb= 1.0 # Bearing area factor. NDS clause 3.10.4

Fb_adj= Fb*Cr*CF*CD*CL
Fb_adjRef= 1345*6.89476e3
ratio4= (Fb_adj-Fb_adjRef)/Fb_adjRef
FcT_adj= FcT*Cb 
FcT_adjRef= 405*6.89476e3
ratio5= (FcT_adj-FcT_adjRef)/FcT_adjRef

As= joistSection.A()
Fv_adj= Fv*CH*CD

# Bending
l= 14.865*0.3048
W= joistSpacing*(D+L)
Mmax= W*l**2/8.0
fb= Mmax/Ss
ratio6= abs(fb-Fb_adj)/Fb_adj

# Shear
l= 40.8*0.3048
Vmax= W*l/2.0
Av= (2/3.0)*As
fv= Vmax/Av
ratio7= abs(fv-Fv_adj)/Fv_adj

# Bearing
l= 45.6*0.3048
bArea= 2*0.0254*joistSection.b
fcT= W*l/2.0/bArea
ratio8= abs(fcT-FcT_adj)/FcT_adj

# Deflection
l= 13.8*0.3048
W= joistSpacing*(L)
beam= sb.SimpleBeam(E,Is,l)
delta= beam.getDeflectionUnderUniformLoad(W,beam.l/2.0)
deltaRef= l/360.0
ratio9= abs(delta-deltaRef)/deltaRef

# Floor vibration
l= 12.5*0.3048
W= joistSpacing*(40*47.88026)
beam= sb.SimpleBeam(E,Is,l)
delta= beam.getDeflectionUnderUniformLoad(W,beam.l/2.0)
deltaRef= l/360.0
ratio10= abs(delta-deltaRef)/deltaRef

'''
print('Fb= ', Fb/1e6, ' MPa')
print('Fb\'= ', Fb_adj/1e6, ' MPa')
print('fb= ', fb/1e6, ' MPa')
print('Fv\'= ', Fv_adj/1e6, ' MPa')
print('fv= ', fv/1e6, ' MPa')
print('FcT\'= ', FcT_adj/1e6, ' MPa')
print('fcT= ', fcT/1e6, ' MPa')
print('Is= ',Is/0.0254**4)
print('delta= ', delta)
print('deltaRef= ', deltaRef)
print('delta2= ',5.0*W/384/E/Is*l**4)
print('ratio1= ',ratio1)
print('ratio2= ',ratio2)
print('ratio3= ',ratio3)
print('ratio4= ',ratio4)
print('ratio5= ',ratio5)
print('ratio6= ',ratio6)
print('ratio7= ',ratio7)
print('ratio8= ',ratio8)
print('ratio9= ',ratio9)
print('ratio10= ',ratio10)
'''

import os
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-4 and abs(ratio2)<1e-3 and abs(ratio3)<1e-15 and abs(ratio4)<1e-3 and abs(ratio5)<1e-15 and abs(ratio6)<1e-3 and abs(ratio7)<1e-3 and abs(ratio8)<1e-3 and abs(ratio9)<0.01 and abs(ratio10)<0.05):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
