# -*- coding: utf-8 -*-
''' Test active static and seismic earth pressure for cohesive soils 
    according to the article:

   Active static and seismic earth pressure for c–φ soils Magued Iskandern, 
   Zhibo (Chris) Chen , Mehdi Omidvar, Ivan Guzman, Omar Elsheri. Polytechnic 
   Institute of New York University, USA.

   https://www.researchgate.net/publication/259094234_Active_static_and_seismic_earth_pressure_for_c-PH_soils
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import math
from geotechnics import iskander_method

# Figure 5 of the article.
# kh: horizontal seismic coefficient.
# kv: vertical seismic coefficient.
# omega: back face inclination of the structure with respect to the vertical (rad).
# beta: slope inclination of terrain (rad).
# phi: angle of internal friction of soil (rad).
# gamma: specific weight of the soil (N/m3).
# c: cohesion (Pa)
# H: wall height (m).
figure5= iskander_method.Iskander(kh= 0.2, kv= -0.1, omega= math.radians(20), beta= math.radians(15), phi= math.radians(30), gamma= 23e3, c= 20e3, H= 15)

Hp2= figure5.Hp2() # Point of application of the the horizontal lateral
                   # earth thrust with respect to the wall heel.
                   
P_AEH2= figure5.P_AEH2() # Pragmatic value of the horizontal lateral
                         # earth thrust.

earthPressure= figure5.getStressDistribution2()

# Compute pressure diagram and store it in a polygon.
zi= [1e-4, 3, 6, 9, 12, 15, figure5.Hl()]
points= list()
points.append(geom.Pos2d(figure5.Hl(),0.0)) # Diagram origin.
points.append(geom.Pos2d(0.0,0.0)) # Top vertex.
for z in zi:
    points.append(geom.Pos2d(z,float(earthPressure(z))))

# Store the diagram in a polygon.
plg= geom.Polygon2d(points)
plgArea= plg.getArea() # Compute diagram area.
plgCentroid= plg.getCenterOfMass() # Compute diagram center of mass.
hCentroid= (figure5.Hl()-plgCentroid.x)

ratio1= abs(Hp2-5.320888862379561)/5.320888862379561
ratio2= abs(P_AEH2-1726.786768897252e3)/1726.786768897252e3
ratio3= abs(P_AEH2-plgArea)/P_AEH2
ratio4= abs(hCentroid-Hp2)/Hp2

'''
print('Hp2= ', Hp2, 'm')
print(ratio1)
print('P_AEH2= ', P_AEH2/1e3, 'kN')
print(ratio2)
print('plgArea= ', plgArea)
print(ratio3)
print('plgCentroid= ', plgCentroid)
print(ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) & (ratio2<1e-6) & (ratio3<1e-6) & (ratio4<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")

# Graphic stuff
# zi= list()
# sigma_AEHi= list()
# for zw in [1e-4, 3, 6, 9, 12, 15]:
#     zi.append(-zw)
#     sigma_AEHi.append(figure5.sigma_AEH(zw)/1e3)

# import matplotlib.pyplot as plt
# plt.title("Pseudo-statik lateral pressure")
# plt.xlabel("Horizontal pressure (kPa)")
# plt.ylabel("Depth (m)")
# plt.plot(sigma_AEHi, zi)
# plt.grid()
# plt.show()

