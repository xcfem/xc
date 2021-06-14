# -*- coding: utf-8 -*-

''' Verification test based on the EXAMPLE 1 of
the technical report no. 10 (Calculating the Fire Resistance of
Wood Members and Assemblies) of the American Wood Council
. July 2018.'''

 
from __future__ import division
from __future__ import print_function
from materials.awc_nds import AWCNDS_materials as mat
from materials.sections import section_properties as sp

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2015, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

CD= mat.getLoadDurationFactor(10)
CM= mat.getWetServiceFactor('Fb',5)
Ct= mat.getTemperatureFactor('Fb','dry',mat.convertToFahrenheit(25))
KF= mat.getFormatConversionFactor('Fb')
phi= mat.getResistanceFactor('Fb')
L= 18*0.3048
b= 6.75*0.0254
h= 13.5*0.0254
CV= mat.getVolumeFactor(L,b,h)
ratio= (CD-1.0)**2
ratio+= (Ct-1.0)**2
ratio+= (CM-1.0)**2
ratio+= (KF-2.54)**2
ratio+= (phi-0.85)**2
ratio+= (CV-0.98)**2

# Bending
Fb= 2400.0*6.89476e3 # Tabulated bending stress N/m2
Fv= 265*6.89476e3 # Tabulated shear stress
section= sp.RectangularSection('s',b,h)
Ss= section.getElasticSectionModulusZ()
SRef= 205*(0.0254)**3
ratio+= abs(Ss-SRef)/SRef

Fb_adj= Fb*CD*CM*Ct*CV
Fb_adjRef= 2343*6.89476e3
ratio+= (Fb_adj-Fb_adjRef)/Fb_adjRef
Mu= Fb_adj*Ss
MuRef= 40032*1.355818 # N.m
ratio+= abs(Mu-MuRef)/MuRef


# Shear
As= section.A()
Fv_adjRef= 265*6.89476e3
Fv_adj= Fv*CD*CM*Ct
ratio+= (Fv_adj-Fv_adjRef)/Fv_adjRef
Vu= Fv_adj*As
VuRef= 24140*4.45 # N
ratio+= abs(Vu-VuRef)/VuRef


# Fire design
# Bending
a_eff= 1.8*0.0254
burnedSection= sp.RectangularSection('s',b-2*a_eff,h-a_eff)
Ss= burnedSection.getElasticSectionModulusZ()
SRef= 71.9*(0.0254)**3
ratio+= abs(Ss-SRef)/SRef

Cfire= mat.getFireDesignAdjustementFactor('Fb')
Fb_adj= Cfire*Fb*CD*CM*Ct*CV
Fb_adjRef= 6703*6.89476e3
ratio+= abs(Fb_adj-Fb_adjRef)/Fb_adjRef
Mu= Fb_adj*Ss
MuRef= 40145*1.355818 # N.m
ratio+= abs(Mu-MuRef)/MuRef

# Shear
As= burnedSection.A()
Cfire= mat.getFireDesignAdjustementFactor('Fv')
Fv_adjRef= 729.0*6.89476e3
Fv_adj= Cfire*Fv*CD*CM*Ct
ratio+= abs(Fv_adj-Fv_adjRef)/Fv_adjRef
Vu= Fv_adj*As
VuRef= 26890*4.45 # N
ratio+= abs(Vu-VuRef)/VuRef

'''
print(KF)
print(Ss,SRef)
print(CV)
print(Fb, Fb_adj)
print(Mu, MuRef)
print(Vu, VuRef)
print(ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<0.015:
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
