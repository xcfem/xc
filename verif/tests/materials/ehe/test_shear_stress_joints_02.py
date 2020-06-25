# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division
''' Verification test for the concrete shrinkage computation
   according to EHE-08. '''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort@gmail.com"

import math
import xc_base
import geom
import xc
from materials.ehe import EHE_materials as EHEmat
from materials.ehe import EHE_limit_state_checking as EHEverif

'''
Verification of ultimate limit state due to longitudinal shear stress 
at joints between concretes according to art. 47 EHE.

Results are compared with those obtained in "Cálculo civil" app  
'''

#Data
concrtype=EHEmat.HA25  #weakest EHE concrete type at joint
steelType=EHEmat.B500S  #reinforcing steel type
widthContact=0.30      #width in contact (parallel to Vd) [m]
widthBeam=0.3          #width beam (perpendicular to Vd)
areaContact=widthContact*widthBeam
unitAreaContact=areaContact/widthBeam #contact area per unit length
Vd=15e3                #Shear force [N]
Nd=-30e3               #Axial force  (perpendicular to the 
                       #plane of joint) [N/m]. Compression: Nd<0
fiRebar=12             #diameter rebar [mm]
roughness='L'          #roughness of surface ('L'=low, 'H'=high)
dynamic='N'            #no low fatigue or dynamic stresses consideration
Ast=math.pi*(fiRebar*1e-3)**2/4
spacement=0.30/4.       #rebars spacement [m]
angRebars=90           #Angle formed by the joining bars with the plane of the
                       #joint (degrees)

#End data

tao_rd=Vd/areaContact   #design longitudinal shear stress at joint [N/m2]
tao_rd_comp=0.17       #comparison longitudinal shear stress at joint [MPa]
ratio1=round(tao_rd*1e-6,2)-tao_rd_comp

sigma_cd=Nd/areaContact #External design tensile stress perpendicular to the
                        #plane of the joint [N/m]

shJoint=EHEverif.LongShearJoints(concrtype,steelType,unitAreaContact,roughness,dynamic,sigma_cd,Ast,spacement,angRebars)

beta=shJoint.getBetaCoef()
beta_comp=0.2
ratio2=beta-beta_comp

f_ctd=concrtype.fctd()   #[N/m2]
f_ctd_comp=1.20          #MPa
ratio3=round(f_ctd*1e-6,2)-f_ctd_comp


tao_u=shJoint.getUltShearStressWithReinf(tao_rd)
tao_u_comp=0.94
ratio4=round(tao_u*1e-6,2)-tao_u_comp

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-12) and (ratio3<1e-12) and (ratio4<1e-12):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

