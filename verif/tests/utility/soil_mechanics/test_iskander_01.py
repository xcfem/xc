# -*- coding: utf-8 -*-
''' Test active static and seismic earth pressure for cohesive soils 
    according to the article:

   Active static and seismic earth pressure for c–φ soils Magued Iskandern, 
   Zhibo (Chris) Chen , Mehdi Omidvar, Ivan Guzman, Omar Elsheri. Polytechnic 
   Institute of New York University, USA.

   https://www.researchgate.net/publication/259094234_Active_static_and_seismic_earth_pressure_for_c-PH_soils
.'''

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

fg5Results= list()
for zw in [1e-4, 3, 6, 9, 12, 15]:
    z= figure5.z(zw)
    Ja= figure5.Ja(zw)
    alpha_a= figure5.alpha_a(zw)
    Ka= figure5.Ka(zw)
    sigma_a= figure5.sigma_a(zw)
    sigma_AEH= figure5.sigma_AEH(zw)
    fg5Results.append([z, Ja, math.degrees(alpha_a), Ka, sigma_a, sigma_AEH])

#                z(m)                     Ja(Pa)             alpha_a(º)            Ka                  sigma_a (Pa)          sigma_AEH(Pa)
fg5RefResults= [[0.00010975255303405955, -11.54567636899507e3, -5.0030105652597605, -9113.536056282801, -23.005378534934817e3, -22.221802100484652e3],
                [3.2925765910217866, 39.624142002169485e3, 53.65638163223621, 0.5606894450006845, 42.46059765317178e3, 11.948298047976374e3],
                [6.585153182043573, 97.58927026199609e3, 36.94162413620927, 0.7164623337152638, 108.51432698362699e3, 59.193831004640025e3],
                [9.877729773065358, 157.4638942613574e3, 32.234474051287705, 0.7914086188453318, 179.79837097166853e3, 110.11418922580414e3],
                [13.170306364087146, 218.2298615967228e3, 29.974390493305236, 0.8354075334373847, 253.05918254744486e3, 162.74993803029048e3],
                [16.46288295510893, 279.49854871962066e3, 28.632822281664453, 0.864580790766208, 327.370324363251e3, 216.35317125379885e3]]

# Figure 13 of the article.
figure13= iskander_method.Iskander(kh= 0.2, kv= 0.1, omega= math.radians(10), beta= math.radians(15), phi= math.radians(0), gamma= 20e3, c= 100e3, H= 10)

#                 z(m)                     Ja(Pa)             alpha_a(º)            Ka                  sigma_a (Pa)          sigma_AEH(Pa)
fg13RefResults= [[0.00010472466720846515, -99.99795511220992e3, 4.9997742755186305, -95124.19188522166, -199.2369867730804e3, -192.4483542271645e3],
                 [2.0944933441693028, -57.215032915844944e3, -0.7309840142959365, -3.752881644591212, -157.2077125210288e3, -155.15504561773542e3],
                 [4.1889866883386055, -10.424000171180566e3, -11.554739032100432, -1.3152458207106852, -110.19094469700089e3, -110.15037899454919e3],
                 [6.283480032507909, 41.23803802032039e3, -41.227270335072475, -0.5189863501581828, -65.220807367262e3, -55.77146056815329e3],
                 [8.377973376677211, 100.20598645700841e3, 69.5081639641272, 0.41517314366171965, 69.56619088618541e3, 12.667684856040626e3],
                 [10.472466720846514, 178.94375850066143e3, 33.52918273840195, 0.7846517637709427, 164.34478967089436e3, 119.15386320176144e3]]

fg13Results= list()
for zw in [1e-4, 2, 4, 6, 8, 10]:
    z= figure13.z(zw)
    Ja= figure13.Ja(zw)
    alpha_a= figure13.alpha_a(zw)
    Ka= figure13.Ka(zw)
    sigma_a= figure13.sigma_a(zw)
    sigma_AEH= figure13.sigma_AEH(zw)
    fg13Results.append([z, Ja, math.degrees(alpha_a), Ka, sigma_a, sigma_AEH])


err= 0.0

for rowA, rowB in zip(fg5Results, fg5RefResults):
    for x, y in zip(rowA, rowB):
        err+= (x-y)**2
for rowA, rowB in zip(fg13Results, fg13RefResults):
    for x, y in zip(rowA, rowB):
        err+= (x-y)**2
        
err= math.sqrt(err)

#print(fg5Results)
#print(fg5RefResults)
#print(fg13Results)
#print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
