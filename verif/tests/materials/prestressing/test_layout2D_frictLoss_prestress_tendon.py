# -*- coding: utf-8 -*-

'''Home made test to check the accuracy of the 2D spline interpolation to 
be used in the layout of prestressing tendons and to check the calculation
of prestress losss due to firction.
'''
__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
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
lBeam=20    #beam span [m]
#Parabola
eEnds=0         #eccentricity of cables at both ends of the beam
eMidspan=-0.3   #eccentricity of cables at midspan

# Material properties

#Prestressing steel
sigmap=1600e6  #breaking strength [Pa]
sigmapk=1400e6 #characteristic strength [Pa]

#Prestressing process
mu=0.18        #coefficient of friction between the cables and their sheating
k=0.0015       #wobble coefficient per meter length of cable
sigmap0max=1400e6 #Initial stress of cable [Pa]

# Interpolation
n_points_rough=5    #number of points provided to the interpolation algorithm
n_points_fine=101   #number of points interpolated

# Approximation of the loss of prestressing due to friction
# Assimilating the parabolic profile of the cable to a circular profile
# the angular deviation is constant in the beam length and can be
# expressed as:

alphaUnit=8*abs(eMidspan)/lBeam**2     #alpha/x [rad/m]

#Exact parabola
from model.geometry import geom_utils

a,b,c=geom_utils.fit_parabola(x=np.array([0,lBeam/2.0,lBeam]), y=np.array([eEnds,eMidspan,eEnds]))

x_parab_rough,y_parab_rough,z_parab_rough=geom_utils.eq_points_parabola(0,lBeam,n_points_rough,a,b,c,0)
x_parab_fine,y_parab_fine,z_parab_fine=geom_utils.eq_points_parabola(0,lBeam,n_points_fine,a,b,c,0)
aprox_cum_angle=alphaUnit*x_parab_fine
aprox_length_sequence=[0]+[distance.euclidean((x_parab_fine[i],y_parab_fine[i],z_parab_fine[i]),(x_parab_fine[i+1],y_parab_fine[i+1],z_parab_fine[i+1])) for i in range(len(x_parab_fine)-1)]
aprox_cumulative_length=np.cumsum(aprox_length_sequence)

aprox_cum_loss=np.array([sigmap0max*(1-math.exp(-mu*aprox_cum_angle[i]-k*aprox_cumulative_length[i])) for i in range(len(aprox_cum_angle))])

#Tendon definition, layout and friction losses
tendon=presconc.PrestressTendon([])
tendon.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline 
tendon.pntsInterpTendon(n_points_fine,1)
#Cumulative lengths of the sequence of segments
cumulative_length=tendon.getCumLength()
   
ratio1= np.mean((cumulative_length-aprox_cumulative_length)**2)/np.mean(cumulative_length)

# Cumulative deviation
cumulative_angl=tendon.getCumAngle()

ratio2= np.mean((cumulative_angl-aprox_cum_angle)**2)/np.mean(cumulative_angl)

# Losses of prestressing due to friction
lssFrict=tendon.getLossFriction(coefFric=mu,k=k,sigmaP0_extr1=sigmap0max,sigmaP0_extr2=0.0)
ratio3= np.mean((lssFrict-aprox_cum_loss)**2)/np.mean(lssFrict)

'''
#Plot
fig1,ax2d=tendon.plot2D(XaxisValues='X',symbolRougPoints='b*',symbolFinePoints='r*',symbolTendon='g-')
fig1.show()
fig2,ax2d=tendon.plot2D(XaxisValues='X',symbolRougPoints=None,symbolFinePoints=None,symbolTendon=None,resultsToPlot=[[lssFrict,'m-','Immediate loss due to friction']])
fig2.savefig('fig2.png')
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-8) & (abs(ratio3)<0.113):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
