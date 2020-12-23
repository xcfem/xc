# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega.Ort@gmail.com"

import math
from rough_calculations import vaults_rough_calc as vault

# Pallazzetto Dello Sport in Rome, by Pier Luigi Nervi
# Example taken from 'The art of structural engineering : Vaults' course,
# by María Garlock

#Data. Units: ft,psf
radius=159 # spherical radius [ft]
angle=38 #angle from the top of the dome to the bottom of the dome
#          measured along the meridian [degrees]
thikness=5  # shell thickness [in]
thikness_ft=5*0.083333333  # shell thickness [ft]
q=120   # uniform vertical load (self weight+insulation+roofing+cupola) [psf]

#


pallazeto=vault.SphericalDome(sphRadius=radius,alpha=math.radians(angle))
#Meridonial forces per unit length
Nphi16=pallazeto.getMeridonialLinearForce(qVsurf=q,Phi=math.radians(16))
Nphi27=pallazeto.getMeridonialLinearForce(qVsurf=q,Phi=math.radians(27))
#Hoop forces per unit length
Nhoop16=pallazeto.getHoopLinearForce(qVsurf=q,Phi=math.radians(16))
Nhoop27=pallazeto.getHoopLinearForce(qVsurf=q,Phi=math.radians(27))
#Meridonial stresses
sgphi16=Nphi16/thikness_ft
sgphi27=Nphi27/thikness_ft
#Hoop stresses
sghoop16=Nhoop16/thikness_ft
sghoop27=Nhoop27/thikness_ft


Nphi16_comp=-9728.43
Nphi27_comp=-10089.86
Nhoop16_comp=-8612.44
Nhoop27_comp=-6910.54
sgphi16_comp=-23348.24
sgphi27_comp=-24215.68
sghoop16_comp=-20669.86
sghoop27_comp=-16585.30

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if max(abs(Nphi16-Nphi16_comp),abs(Nphi27-Nphi27_comp),abs(Nhoop16-Nhoop16_comp),abs(Nhoop27-Nhoop27_comp),abs(sgphi16-sgphi16_comp),abs(sgphi27-sgphi27_comp),abs(sghoop16-sghoop16_comp),abs(sghoop27-sghoop27_comp))<0.01:
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
