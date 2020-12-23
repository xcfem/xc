# -*- coding: utf-8 -*-
''' Test set renaming.'''
from __future__ import print_function

CooMax= 10
import xc_base
import geom
import xc
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor

s1= prep.getSets.defSet("S1")
s2= prep.getSets.defSet("S2")
s= prep.getSets.defSet("S")

s+= s1
s+= s2
s.name= 'S'
keysA= prep.getSets.getKeys()
namesA= prep.getSets.getSetsNames()

s2.name= 'SII'

keysB= prep.getSets.getKeys()
namesB= prep.getSets.getSetsNames()

'''
print('keysA= ', keysA)
print('namesA= ', namesA)
print('keysB= ', keysB)
print('namesB= ', namesB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)

if (keysA==namesA) and (keysB==namesB):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
