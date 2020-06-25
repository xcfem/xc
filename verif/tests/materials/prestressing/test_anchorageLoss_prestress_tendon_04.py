# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division
'''Test for checking the calculation of loss of prestress due to 
the anchorage slip when this loss affects the whole length of tendon.
It compares the results obtained from symetrical 
problems when prestressing from extremity 1 (left-end) of the cable
and from extremity 2 (right-end)
'''
__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@xcengineering.xyz"


import numpy as np
import matplotlib.pyplot as plt
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
mu=0.10        #coefficient of friction between the cables and their sheating
k=0.00017*1e-3     #wobble coefficient per millimeter length of cable
sigmap0max=1239 #Initial stress of cable [MPa]

# Interpolation
n_points_rough=5    #number of points provided to the interpolation algorithm
n_points_fine=101   #number of points interpolated

#Anchorage slip
deltaL=5            #anchorage draw-in (provided by manufacturer) [mm]


#XC calculation
#Exact parabola
from model.geometry import geom_utils
a,b,c=geom_utils.fit_parabola(x=np.array([0,lBeam/2.0,lBeam]), y=np.array([eEnds,eMidspan,eEnds]))
x_parab_rough,y_parab_rough,z_parab_rough=geom_utils.eq_points_parabola(0,lBeam,n_points_rough,a,b,c,angl_Parab_XZ)

# Tendon prestressed from extremity 1 (left-end)
#Tendon definition, layout and friction losses
tendon1=presconc.PrestressTendon([])
tendon1.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline 
tendon1.pntsInterpTendon(n_points_fine,smoothness=1,kgrade=3)
# Losses of prestressing due to friction
lssFrict1=tendon1.getLossFriction(coefFric=mu,k=k,sigmaP0_extr1=sigmap0max,sigmaP0_extr2=0.0)
# Losses of prestressing due to anchorage slip (loss due to friction must be
# previously calculated
lssAnch1=tendon1.getLossAnchor(Ep=Ep,anc_slip_extr1=deltaL,anc_slip_extr2=0.0)
stressAfterLossAnch1=tendon1.stressAfterLossFriction-lssAnch1

# Tendon prestressed from extremity 2 (right-end)
#Tendon definition, layout and friction losses
tendon2=presconc.PrestressTendon([])
tendon2.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline 
tendon2.pntsInterpTendon(n_points_fine,smoothness=1,kgrade=3)
# Losses of prestressing due to friction
lssFrict2=tendon2.getLossFriction(coefFric=mu,k=k,sigmaP0_extr1=0.0,sigmaP0_extr2=sigmap0max)
# Losses of prestressing due to anchorage slip (loss due to friction must be
# previously calculated
lssAnch2=tendon2.getLossAnchor(Ep=Ep,anc_slip_extr1=0.0,anc_slip_extr2=deltaL)
stressAfterLossAnch2=tendon2.stressAfterLossFriction-lssAnch2

#Plot
# fig1,ax2d=tendon1.plot2D(XaxisValues='S',resultsToPlot=[[stressAfterLossAnch2,'g-','Stress after loss due to anchorage slip']])
#fig1.show()

from sklearn.metrics import mean_squared_error
ratio=mean_squared_error(stressAfterLossAnch1,np.flipud(stressAfterLossAnch2))

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-20) :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
