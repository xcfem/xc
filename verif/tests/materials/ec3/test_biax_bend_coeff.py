# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

#Coefficients alpha, beta for biaxial bending  (clause 6.2.9 of EC3.1.1)
__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2018, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

import xc_base
import geom
import xc
from materials.ec3 import EC3_materials


S235JR= EC3_materials.S235JR
S235JR.gammaM= 1.00

sctClass=1    #section class
Nd=7e5        #axial force [N]

IPE300= EC3_materials.IPEShape(steel=S235JR,name='IPE_A_300')
alpha_IPE300,beta_IPE300=IPE300.getBiaxBendCoeffs(Nd,IPE300.getNcRd(sctClass))
alpha_IPE300_comp=2
beta_IPE300_comp=5*Nd/1093455.0

IPN300= EC3_materials.IPNShape(steel=S235JR,name='IPN_300')
alpha_IPN300,beta_IPN300=IPN300.getBiaxBendCoeffs(Nd,IPN300.getNcRd(sctClass))
alpha_IPN300_comp=2
beta_IPN300_comp=5*Nd/1621500.0

HE300= EC3_materials.HEShape(steel=S235JR,name='HE_300_A')
alpha_HE300,beta_HE300=HE300.getBiaxBendCoeffs(Nd,HE300.getNcRd(sctClass))
alpha_HE300_comp=2
beta_HE300_comp=5*Nd/2643750.0

SHS175= EC3_materials.SHSShape(steel=S235JR,name='SHS175x175x8')
alpha_SHS175,beta_SHS175=SHS175.getBiaxBendCoeffs(Nd,SHS175.getNcRd(sctClass))
alpha_SHS175_comp=1.66/(1-1.13*(Nd/1246205.0)**2)
beta_SHS175_comp=1.66/(1-1.13*(Nd/1246205.0)**2)

RHS250= EC3_materials.RHSShape(steel=S235JR,name='RHS250x150x16')
alpha_RHS250,beta_RHS250=RHS250.getBiaxBendCoeffs(Nd/2.,RHS250.getNcRd(sctClass))
alpha_RHS250_comp=1.66/(1-1.13*(0.5*Nd/2702500.0)**2)
beta_RHS250_comp=1.66/(1-1.13*(0.5*Nd/2702500.0)**2)

CHS193= EC3_materials.CHSShape(steel=S235JR,name='CHS_193.7_11.0')
alpha_CHS193,beta_CHS193=CHS193.getBiaxBendCoeffs(Nd,CHS193.getNcRd(sctClass))
alpha_CHS193_comp=2
beta_CHS193_comp=2


ratioLst=[alpha_IPE300-alpha_IPE300_comp,beta_IPE300-beta_IPE300_comp,
          alpha_IPN300-alpha_IPN300_comp,beta_IPN300-beta_IPN300_comp,
          alpha_HE300-alpha_HE300_comp,beta_HE300-beta_HE300_comp,
          alpha_SHS175-alpha_SHS175_comp,beta_SHS175-beta_SHS175_comp,
          alpha_RHS250-alpha_RHS250_comp,beta_RHS250-beta_RHS250_comp,
          alpha_CHS193-alpha_CHS193_comp,beta_CHS193-beta_CHS193_comp]

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(sum(ratioLst)/len(ratioLst))<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

