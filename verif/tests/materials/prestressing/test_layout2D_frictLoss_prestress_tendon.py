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
k=0.0015/mu       #unintentional angular deviation [rad/m]
sigmap0max=1400e6 #Initial stress of cable [Pa]

# Interpolation
n_points_rough=5    #number of points provided to the interpolation algorithm
n_points_fine=101   #number of points interpolated

#Approximation of the losses of prestressing due to friction
# Assimilating the parabolic profile of the cable to a circular profile
# the angular deviation is constant in the beam length and can be
# expressed as:

alphaUnit=8*abs(eMidspan)/lBeam**2     #alpha/x [rad/m]

def fricLosses(x,sigmap0max,alphaUnit,mu,unintDev):
    sigmaLoss=sigmap0max*(1-math.exp(-mu*(alphaUnit+unintDev)*x))
    return sigmaLoss

#Exact parabola
def fit_parabola(x, z):
    '''Fits the equation "z = ax^2 + bx + c" given exactly 3 points as two
    lists or arrays of x & y coordinates
    '''
    A = np.zeros((3,3), dtype=np.float)
    A[:,0] = x**2
    A[:,1] = x
    A[:,2] = 1
    a, b, c = np.linalg.solve(A, z)
    return a, b, c

a,b,c=fit_parabola(x=np.array([0,lBeam/2.0,lBeam]), z=np.array([eEnds,eMidspan,eEnds]))

def eq_points_parabola(startX,stopX,numPts,a,b,c):
    '''Returns equispaced nPts points of the parabola "z=ax^2 + bx + c" 
    in the X range [startX,stopX]
    '''
    x_parab=np.linspace(startX,stopX,numPts)
    z_parab=a*x_parab**2+b*x_parab+c
    return x_parab,z_parab


x_parab_rough,z_parab_rough=eq_points_parabola(0,lBeam,n_points_rough,a,b,c)
y_parab_rough=np.zeros(n_points_rough)
x_parab_fine,z_parab_fine=eq_points_parabola(0,lBeam,n_points_fine,a,b,c)
y_parab_fine=np.zeros(n_points_fine)
aprox_cum_angle=alphaUnit*x_parab_fine
aprox_length_sequence=[0]+[distance.euclidean((x_parab_fine[i],y_parab_fine[i],z_parab_fine[i]),(x_parab_fine[i+1],y_parab_fine[i+1],z_parab_fine[i+1])) for i in range(len(x_parab_fine)-1)]
aprox_cumulative_length=np.cumsum(aprox_length_sequence)

aprox_cum_loss=np.array([sigmap0max*(1-math.exp(-mu*(aprox_cum_angle[i]+k*aprox_cumulative_length[i]))) for i in range(len(aprox_cum_angle))])

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
tendon.getLossFriction(coefFric=mu,uninDev=k,sigmaP0_extr1=sigmap0max,sigmaP0_extr2=0.0)
ratio3= np.mean((tendon.lossFriction-aprox_cum_loss)**2)/np.mean(tendon.lossFriction)

#Plot
# tendon.plot2D(XaxisValues='X',fileName='parab.png',symbolRougPoints='b*',symbolFinePoints='r*',symbolTendon='g-',symbolLossFriction=None)

# tendon.plot2D(XaxisValues='X',fileName='loss.png',symbolRougPoints=None,symbolFinePoints=None,symbolTendon=None,symbolLossFriction='m-')

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-8) & (abs(ratio3)<0.113):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
