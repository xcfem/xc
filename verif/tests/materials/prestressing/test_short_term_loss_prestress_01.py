# -*- coding: utf-8 -*-
'''Test for checking variation of initial prestress force along a 
post-tensioned member.

Data and rough calculation are taken from 
Example 4.3 of the topic 4 of course "Prestressed Concrete Design 
(SAB 4323) by Baderul Hisham Ahmad 
ocw.utm.my

Problem statement:
Determine the initial prestress force distribution
along the beam if the anchorage draw-in is 5
mm. Given the following:
• Span = 20m, μ= 0.25 & K = 17 x 10-4 per metre
• fpi = 1239 N/ mm2 ; A ps = 2850 mm2
• e at both ends = 0
• e at mid-span = 558 mm
• Es = 195 kN/mm2
'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@xcengineering.xyz"


import numpy as np
import math
from materials.prestressing import prestressed_concrete as presconc
from model.geometry import geom_utils

#Geometry 
lBeam=20   #beam span [m]
#Parabola
eEnds=0         #eccentricity of cables at both ends of the beam
eMidspan=-0.558   #eccentricity of cables at midspan [m]
angl_Parab_XZ=math.pi/4 #angle between the vertical plane that contains the
                        #parabola and the plane XZ
#Material
Ep=195e9       #elastic modulus of prestressing steel [Pa]
#Prestressing process
mu=0.25        #coefficient of friction between the cables and their sheating
k=0.0017  #wobble coefficient per meter length of cable [1/m]
sigmap0max=1239e6 #Initial stress of cable [Pa]
Aps=2850e-6        #Area of cable [m2]

# Interpolation
n_points_rough=5    #number of points provided to the interpolation algorithm
n_points_fine=101   #number of points interpolated

#Anchorage slip
deltaL=5e-3          #anchorage draw-in (provided by manufacturer) [m]

#Rough results from direct calculation (formula):
lp_anch_lhe=419.3    #loss of prestress force at left-hand end anchorage [kN]
fl_frc=15.82         #loss of prestress due to friction [kN/m]
P_le=3111.9        #prestress force at left end [kN]
P_ms=3270.1        #prestress force at midspan  [kN]
P_re=3214.8        #prestress force at right end [kN]

# XC model
#Tendon [m] definition, layout and friction losses
a,b,c=geom_utils.fit_parabola(x=np.array([0,lBeam/2.0,lBeam]), y=np.array([eEnds,eMidspan,eEnds]))
x_parab_rough,y_parab_rough,z_parab_rough=geom_utils.eq_points_parabola(0,lBeam,n_points_rough,a,b,c,angl_Parab_XZ)

tendon=presconc.PrestressTendon([])
tendon.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline 
tendon.pntsInterpTendon(n_points_fine,smoothness=1,kgrade=3)
# Losses of prestressing due to friction
lssFrict=tendon.getLossFriction(coefFric=mu,k=k,sigmaP0_extr1=sigmap0max,sigmaP0_extr2=0.0)
# Losses of prestressing due to anchorage slip (loss due to friction must be
# previously calculated
lssAnch=tendon.getLossAnchor(Ep=Ep,anc_slip_extr1=deltaL,anc_slip_extr2=0.0)

Laffected=tendon.projXYcoordZeroAnchLoss[0] # effective length of tendon
#affected by the anchorage slip in extremity 1 [m]

# Results
lssAnch_e1=lssAnch[0]   #prestress loss due to anchorage draw-in extremity 1
lssAnch_md=lssAnch[int(len(lssAnch)/2)]   #prestress loss due to anchorage draw-in midspan
lssAnch_e2=lssAnch[-1]   #prestress loss due to anchorage draw-in extremity 2

lssFrict_e1=lssFrict[0]   #prestress loss due to friction extremity 1
lssFrict_md=lssFrict[int(len(lssFrict)/2)]   #prestress loss due to friction midspan
lssFrict_e2=lssFrict[-1]   #prestress loss due to friction extremity 2

P_extr1=(sigmap0max-lssAnch_e1-lssFrict_e1)*Aps*1e-3
P_midspan=(sigmap0max-lssAnch_md-lssFrict_md)*Aps*1e-3
P_extr2=(sigmap0max-lssAnch_e2-lssFrict_e2)*Aps*1e-3

ratio1=abs(P_extr1-P_le)/P_le
ratio2=abs(P_midspan-P_ms)/P_ms
ratio3=abs(P_extr2-P_re)/P_re
                           
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<5.e-3 and ratio2<5.e-4  and ratio3<5.e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
