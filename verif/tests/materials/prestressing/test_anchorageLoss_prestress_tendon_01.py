# -*- coding: utf-8 -*-
from __future__ import division
'''Test for checking the calculation of loss of prestress due to 
the anchorage slip.
Data for the problem and approximate calculation are taken from 
Example 4.3 of the topic 4 of course "Prestressed Concrete Design 
(SAB 4323) by Baderul Hisham Ahmad 
ocw.utm.my
'''
__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@xcengineering.xyz"


import numpy as np
from scipy import interpolate
from scipy.spatial import distance
import math
from materials.prestressing import prestressed_concrete as presconc

#Geometry
lBeam=20e3    #beam span [mm]
#Parabola
eEnds=0         #eccentricity of cables at both ends of the beam
eMidspan=-558   #eccentricity of cables at midspan
angl_Parab_XZ=math.pi/4 #angle between the vertical plane that contains the
                        #parabola and the plane XZ
#Material
Ep=195e3       #elastic modulus of prestressing steel [MPa]
#Prestressing process
mu=0.25        #coefficient of friction between the cables and their sheating
k=0.0017*1e-3  #wobble coefficient per millimeter length of cable
sigmap0max=1239 #Initial stress of cable [MPa]

# Interpolation
n_points_rough=5    #number of points provided to the interpolation algorithm
n_points_fine=101   #number of points interpolated

#Anchorage slip
deltaL=5            #anchorage draw-in (provided by manufacturer) [mm]

#Rough calculation
Aps=2850          #area of tendon cross-section [mm2]
P_i=Aps*sigmap0max*1e-3 #kN
rps=(lBeam*1e-3)**2/(8*abs(eMidspan)*1e-3)   # parabolic curve [m]
p=P_i*(1-math.exp(-(mu/rps+k*1e3)))       # friction loss [kN/m]
xA_rough=(deltaL*Ep*Aps/p)**0.5*1e-3         # effective length of tendon
                             #affected by the anchorage slip [m]
DeltaPa_rough=2*p*xA_rough   #loss of prestress force at the left-hand end [kN]
P_left_end=P_i-DeltaPa_rough
P_mid_span=P_i-DeltaPa_rough+p*lBeam*1e-3/2.0
P_right_end=P_i-p*lBeam*1e-3
                              
sigma_left_end=P_left_end*1e3/Aps #stress at left-end [MPa]
sigma_mid_span=P_mid_span*1e3/Aps #stress at mid-span [MPa]
sigma_right_end=P_right_end*1e3/Aps #stress at right-end [MPa]

#XC calculation
#Exact parabola
from model.geometry import geom_utils
a,b,c=geom_utils.fit_parabola(x=np.array([0,lBeam/2.0,lBeam]), y=np.array([eEnds,eMidspan,eEnds]))
x_parab_rough,y_parab_rough,z_parab_rough=geom_utils.eq_points_parabola(0,lBeam,n_points_rough,a,b,c,angl_Parab_XZ)

#Tendon definition, layout and friction losses
tendon=presconc.PrestressTendon([])
tendon.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline 
tendon.pntsInterpTendon(n_points_fine,smoothness=1,kgrade=3)
# Losses of prestressing due to friction
lssFrict=tendon.getLossFriction(coefFric=mu,k=k,sigmaP0_extr1=sigmap0max,sigmaP0_extr2=0.0)
# Losses of prestressing due to anchorage slip (loss due to friction must be
# previously calculated
LssAnch=tendon.getLossAnchor(Ep=Ep,anc_slip_extr1=deltaL,anc_slip_extr2=0.0)

Laffected=tendon.projXYcoordZeroAnchLoss[0] # effective length of tendon affected by
                           #the anchorage slip in extremity 1 [mm]

ratio=(xA_rough-Laffected*1e-3)/xA_rough

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1.95e-2) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
