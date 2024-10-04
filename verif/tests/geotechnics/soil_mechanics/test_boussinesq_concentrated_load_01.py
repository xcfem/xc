# -*- coding: utf-8 -*-
''' Test based on the example from the table 11.7 of the text

  "Fondation Analysis and Design". Fifth Edition. Joseph E. Bowles. 
   McGraw-Hill 1982
   ISBN= 9780070067707
   LCCN= lc81013649
   url= https://books.google.es/books?id=eNtRAAAAMAAJ
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import math
from geotechnics import boussinesq
from scipy import integrate
import numpy as np

# Wall height
H= 4.0

# Define load.
P= -50e3 # 50 kN concentrated load.
x= 1.3 # load position, x coordinate distance from the wall.
y= 0.0 # load position, y coordinate.
z= -2.0 # load position, z coordinate.
eta= 1.0 # Poisson's ratio.

testValue= boussinesq.radial_stress_increment_under_concentrated_load(P= P, x= x, y= y, z= z, eta= eta)

# Equation 11.20a:
r2= x**2+y**2
R= math.sqrt(r2+z**2)
refValue= P/math.pi/2.0*(3*r2*(-z)/R**5-(1-2*eta)/R/(R-z))
err= abs(testValue-refValue)/refValue

# Integrate along the wall.
## Function to integrate.
def pressure(z):
    return boussinesq.radial_stress_increment_under_concentrated_load(P= P, x= x, y= y, z= z, eta= eta)   

## Sample points
n= 11
deltaZ= H/n
nDiv= int(math.ceil(H/deltaZ))
zi= np.linspace(start= -deltaZ/2.0, stop= -(H-deltaZ/2.0), num= n, endpoint= True)
pi= [ pressure(z) for z in zi]

##  call quad to integrate f from -4 to 0
#force, int_err = integrate.quad(pressure, -4, 0)
force= integrate.simpson(pi, zi)
err= abs(force-10e3)/10e3

'''
print(testValue, refValue)
print(zi)
print(pi)
print(force)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-2):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
