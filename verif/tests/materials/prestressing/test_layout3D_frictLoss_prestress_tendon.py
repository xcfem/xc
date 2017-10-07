import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate
from scipy.spatial import distance
import math
from materials.prestressing import prestressed_concrete as presconc

#Geometry
lBeam=20    #beam span [m]
hBeam=0.8  #beam height [m]
wBeam=1.0  #beam width [m]
nCables=5 #number of prestressing cables
nWires=7   #number or wires each cable
aWire=35.5e-6   #area of the wire cross-section
#Parabola
eEnds=0         #eccentricity of cables at both ends of the beam
eMidspan=-0.3   #eccentricity of cables at midspan
angl_Parab_XZ=math.pi/4 #angle between the vertical plane that contains the
                        #parabola and the plane XZ

# Material properties
#Concrete
Eci=40e9   #modulus of elasticity instantaneous [Pa]
Ec=Eci/3.  #modulus of elasticity secant [Pa]

#Prestressing steel
sigmap=1600e6  #breaking strength [Pa]
sigmapk=1400e6 #characteristic strength [Pa]
relax1000h=5   #relaxation of steel after 1000 hours
Ep=200e9       #modulus of elasticity [Pa]

#Prestressing process
mu=0.18        #coefficient of friction between the cables and their sheating
k=0.0015/mu       #unintentional angular deviation [rad/m]
sigmap0max=1400e6 #Initial stress of cable [Pa]

n_points_rough=5
n_points_fine=101

#Approximation of the losses of prestressing due to friction
# Assimilating the parabolic profile of the cable to a circular profile
# the angular deviation is constant in the beam length and can be
# expressed as:

alphaUnit=8*abs(eMidspan)/lBeam**2     #alpha/x [rad/m]

def fricLosses(x,sigmap0max,alphaUnit,mu,unintDev):
    sigmaLoss=sigmap0max*(1-math.exp(-mu*(alphaUnit+unintDev)*x))
    return sigmaLoss

#Exact parabola
def fit_parabola(x, y):
    '''Fits the equation "y = ax^2 + bx + c" given exactly 3 points as two
    lists or arrays of x & y coordinates
    '''
    A = np.zeros((3,3), dtype=np.float)
    A[:,0] = x**2
    A[:,1] = x
    A[:,2] = 1
    a, b, c = np.linalg.solve(A, y)
    return a, b, c

a,b,c=fit_parabola(x=np.array([0,lBeam/2.0,lBeam]), y=np.array([eEnds,eMidspan,eEnds]))

def eq_points_parabola(startS,stopS,numPts,a,b,c,angSX):
    '''Returns equispaced nPts points of the parabola "y=ax^2 + bx + c" 
    in the X range [startX,stopX]
    '''
    s_parab=np.linspace(startS,stopS,numPts)
    x_parab=np.linspace(startS*math.cos(angSX),stopS*math.cos(angSX),numPts)
    y_parab=np.linspace(startS*math.sin(angSX),stopS*math.sin(angSX),numPts)
    z_parab=a*s_parab**2+b*s_parab+c
    return x_parab,y_parab,z_parab

x_parab_rough,y_parab_rough,z_parab_rough=eq_points_parabola(0,lBeam,n_points_rough,a,b,c,angl_Parab_XZ)
x_parab_fine,y_parab_fine,z_parab_fine=eq_points_parabola(0,lBeam,n_points_fine,a,b,c,angl_Parab_XZ)
aprox_cum_angle=alphaUnit*(x_parab_fine**2+y_parab_fine**2)**(1/2.0)
aprox_length_sequence=[0]+[distance.euclidean((x_parab_fine[i],y_parab_fine[i],z_parab_fine[i]),(x_parab_fine[i+1],y_parab_fine[i+1],z_parab_fine[i+1])) for i in range(len(x_parab_fine)-1)]
aprox_cumulative_length=np.cumsum(aprox_length_sequence)

#aprox_cum_loss=np.array([fricLosses(x,sigmap0max,alphaUnit,mu,k) for x in x_parab_fine])
aprox_cum_loss=np.array([sigmap0max*(1-math.exp(-mu*(aprox_cum_angle[i]+k*aprox_cumulative_length[i]))) for i in range(len(aprox_cum_angle))])

tendon=presconc.PrestressTendon([])
tendon.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline from the points of parabola
tendon.pntsInterpTendon(n_points_fine,1)
#lengths of the sequence of segments
cumulative_length=tendon.getCumLength()


ratio1= np.mean((cumulative_length-aprox_cumulative_length)**2)/np.mean(cumulative_length)

# Cumulative deviation

cumulative_angl=tendon.getCumAngle()


ratio2= np.mean((cumulative_angl-aprox_cum_angle)**2)/np.mean(cumulative_angl)

# Losses of prestressing due to friction
tendon.getLossFriction(coefFric=mu,uninDev=k,sigmaP0_extr1=sigmap0max,sigmaP0_extr2=0.0)

ratio3= np.mean((tendon.lossFriction-aprox_cum_loss)**2)/np.mean(tendon.lossFriction)
#Plot
#tendon.plot2D(XaxisValues='S',fileName='parab.png',symbolRougPoints='b*',symbolFinePoints='r*',symbolTendon='g-',symbolLossFriction='m-')

#tendon.plot3D(fileName='parab.png',symbolRougPoints='b*',symbolFinePoints='r*',symbolTendon='g-',symbolLossFriction=None)

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-8) & (abs(ratio3)<0.113):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
