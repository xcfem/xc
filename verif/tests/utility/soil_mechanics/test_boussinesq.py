# -*- coding: utf-8 -*-
'''
   Example 5.1 from the book "Principles of Foundation Engineering, SI Edition.
   Seventh edition. Braja M. Das page 231
   ISBN-13: 978-0-495-66812-1
'''
from __future__ import print_function
from geotechnics import settlement

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2017, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

incSigma= settlement.stress_increment_under_rectangular_loaded_area(q=150e3, B= 2.5, L=5.0, x=0.0, y=0.0, z=6.25, eSize= 0.1)
averageIncSigma= settlement.average_stress_increment_under_rectangular_loaded_area(q=100e3, B= 3.0, L=3.0, x=0.0, y=0.0, z1=3.0, z2= 5.0, eSize= 0.2, zInc= 0.5)

ratio1= abs(incSigma-19.65e3)/19.65e3
ratio2= abs(averageIncSigma-21e3)/21e3

'''
print("incSigma= ",incSigma/1e3, 'kN/m2')
print("ratio1= ",ratio1)
print("averageIncSigma= ",averageIncSigma/1e3, 'kN/m2')
print("ratio2= ",ratio2)
'''

import os
fname= os.path.basename(__file__)
if (ratio1<5e-3) and (ratio2<0.1):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
