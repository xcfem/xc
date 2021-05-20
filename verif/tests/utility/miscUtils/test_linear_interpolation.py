# -*- coding: utf-8 -*-
''' Test linear interpolation class.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import math_utils

li= math_utils.LinearInterpolation(0.0,0.0,1.0,1.0)
y= li.getY(0.5)
error= (y-0.5)/0.5

import os
fname= os.path.basename(__file__)
if (error<1e-15):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
