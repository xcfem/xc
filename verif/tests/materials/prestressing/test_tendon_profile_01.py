# -*- coding: utf-8 -*-
''' Define tendon profile according to document: section
        3.8 of FHWA Post-Tensioned Box Girder Design Manual. June 2016.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from materials.prestressing import tendon_profile
import numpy as np

ft= .3048

vertex= geom.Pos2d(0, 0.0)
e_max= 2.5*ft
e_min= -2.0*ft # There is an error in the document.

h1= 2.5*ft
b1= 48*ft

b2= 60*ft
b3= 12*ft

# Equal tangent in x= b2 (equation 3.35 of the reference document) and
# h1+h2== e_max - e_min implies:
h2= (e_max-e_min)/(1+b3/b2)
h3= h2*b3/b2

control_points= [geom.Pos2d(-b1, h1), geom.Pos2d(0, 0), geom.Pos2d(b2, h2), geom.Pos2d(b2+b3, h2+h3)]
curve_sequence= [{'point_indexes':[0,1], 'type':'parabolic', 'vertex_index':1}, {'point_indexes':[1,2], 'type':'parabolic', 'vertex_index':0}, {'point_indexes':[2,3], 'type':'parabolic', 'vertex_index':1}]

tendonProfile= tendon_profile.TendonProfile(control_points, curve_sequence)
pointsAlongTendon= tendonProfile.getPoints(spacing= 1*ft)
refPoints= [geom.Pos2d(-14.6304, 0.762), geom.Pos2d(-14.323, 0.73032), geom.Pos2d(-14.0157, 0.699312), geom.Pos2d(-13.7083, 0.668976), geom.Pos2d(-13.401, 0.639314), geom.Pos2d(-13.0936, 0.610324), geom.Pos2d(-12.7862, 0.582006), geom.Pos2d(-12.4789, 0.554362), geom.Pos2d(-12.1715, 0.527389), geom.Pos2d(-11.8641, 0.50109), geom.Pos2d(-11.5568, 0.475463), geom.Pos2d(-11.2494, 0.450509), geom.Pos2d(-10.9421, 0.426227), geom.Pos2d(-10.6347, 0.402618), geom.Pos2d(-10.3273, 0.379682), geom.Pos2d(-10.02, 0.357418), geom.Pos2d(-9.71262, 0.335827), geom.Pos2d(-9.40526, 0.314908), geom.Pos2d(-9.0979, 0.294662), geom.Pos2d(-8.79053, 0.275089), geom.Pos2d(-8.48317, 0.256188), geom.Pos2d(-8.17581, 0.23796), geom.Pos2d(-7.86845, 0.220405), geom.Pos2d(-7.56109, 0.203522), geom.Pos2d(-7.25373, 0.187312), geom.Pos2d(-6.94637, 0.171774), geom.Pos2d(-6.63901, 0.156909), geom.Pos2d(-6.33164, 0.142717), geom.Pos2d(-6.02428, 0.129197), geom.Pos2d(-5.71692, 0.11635), geom.Pos2d(-5.40956, 0.104176), geom.Pos2d(-5.1022, 0.0926739), geom.Pos2d(-4.79484, 0.0818446), geom.Pos2d(-4.48748, 0.0716881), geom.Pos2d(-4.18011, 0.0622041), geom.Pos2d(-3.87275, 0.0533927), geom.Pos2d(-3.56539, 0.045254), geom.Pos2d(-3.25803, 0.0377879), geom.Pos2d(-2.95067, 0.0309944), geom.Pos2d(-2.64331, 0.0248736), geom.Pos2d(-2.33595, 0.0194253), geom.Pos2d(-2.02858, 0.0146497), geom.Pos2d(-1.72122, 0.0105467), geom.Pos2d(-1.41386, 0.00711634), geom.Pos2d(-1.1065, 0.00435859), geom.Pos2d(-0.799139, 0.00227346), geom.Pos2d(-0.491778, 0.000860956), geom.Pos2d(-0.184417, 0.000121072), geom.Pos2d(0.122945, 5.16574e-05), geom.Pos2d(0.430306, 0.000632803), geom.Pos2d(0.737667, 0.00185967), geom.Pos2d(1.04503, 0.00373224), geom.Pos2d(1.35239, 0.00625054), geom.Pos2d(1.65975, 0.00941456), geom.Pos2d(1.96711, 0.0132243), geom.Pos2d(2.27447, 0.0176797), geom.Pos2d(2.58184, 0.0227809), geom.Pos2d(2.8892, 0.0285278), geom.Pos2d(3.19656, 0.0349204), geom.Pos2d(3.50392, 0.0419587), geom.Pos2d(3.81128, 0.0496427), geom.Pos2d(4.11864, 0.0579725), geom.Pos2d(4.426, 0.066948), geom.Pos2d(4.73336, 0.0765691), geom.Pos2d(5.04073, 0.086836), geom.Pos2d(5.34809, 0.0977487), geom.Pos2d(5.65545, 0.109307), geom.Pos2d(5.96281, 0.121511), geom.Pos2d(6.27017, 0.134361), geom.Pos2d(6.57753, 0.147856), geom.Pos2d(6.88489, 0.161998), geom.Pos2d(7.19226, 0.176784), geom.Pos2d(7.49962, 0.192217), geom.Pos2d(7.80698, 0.208295), geom.Pos2d(8.11434, 0.225019), geom.Pos2d(8.4217, 0.242389), geom.Pos2d(8.72906, 0.260405), geom.Pos2d(9.03642, 0.279066), geom.Pos2d(9.34378, 0.298373), geom.Pos2d(9.65115, 0.318326), geom.Pos2d(9.95851, 0.338924), geom.Pos2d(10.2659, 0.360168), geom.Pos2d(10.5732, 0.382058), geom.Pos2d(10.8806, 0.404593), geom.Pos2d(11.188, 0.427775), geom.Pos2d(11.4953, 0.451602), geom.Pos2d(11.8027, 0.476074), geom.Pos2d(12.11, 0.501193), geom.Pos2d(12.4174, 0.526957), geom.Pos2d(12.7248, 0.553367), geom.Pos2d(13.0321, 0.580422), geom.Pos2d(13.3395, 0.608123), geom.Pos2d(13.6468, 0.63647), geom.Pos2d(13.9542, 0.665463), geom.Pos2d(14.2616, 0.695102), geom.Pos2d(14.5689, 0.725386), geom.Pos2d(14.8763, 0.756316), geom.Pos2d(15.1837, 0.787891), geom.Pos2d(15.491, 0.820112), geom.Pos2d(15.7984, 0.852979), geom.Pos2d(16.1057, 0.886492), geom.Pos2d(16.4131, 0.920651), geom.Pos2d(16.7205, 0.955455), geom.Pos2d(17.0278, 0.990905), geom.Pos2d(17.3352, 1.027), geom.Pos2d(17.6425, 1.06374), geom.Pos2d(17.9499, 1.10113), geom.Pos2d(18.2573, 1.13916), geom.Pos2d(18.5646, 1.17627), geom.Pos2d(18.872, 1.21017), geom.Pos2d(19.1793, 1.24084), geom.Pos2d(19.4867, 1.26829), geom.Pos2d(19.7941, 1.2925), geom.Pos2d(20.1014, 1.31349), geom.Pos2d(20.4088, 1.33124), geom.Pos2d(20.7162, 1.34577), geom.Pos2d(21.0235, 1.35707), geom.Pos2d(21.3309, 1.36514), geom.Pos2d(21.6382, 1.36999), geom.Pos2d(21.9456, 1.3716)]

sz1= len(pointsAlongTendon)
sz2= len(refPoints)
testOK= (sz1==sz2)

err= 0.0
for p1, p2 in zip(pointsAlongTendon, refPoints):
    err+= p2.dist2(p1)
err= math.sqrt(err)
testOK= testOK and (abs(err)<1e-3)

# print('h2= ', h2/ft, 'ft')
# print('h3= ', h3/ft, 'ft')
# print('sz1= ', len(pointsAlongTendon))
# print('err= ', err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Display tendon profile.
# import matplotlib.pyplot as plt
# xi= list()
# yi= list()
# for p in pointsAlongTendon:
#     xi.append(p.x)
#     yi.append(p.y)
# # plt.axis('equal')
# plt.plot(xi, yi, '-')
# plt.grid()
# plt.show()
