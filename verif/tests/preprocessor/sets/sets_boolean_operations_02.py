# -*- coding: utf-8 -*-
CooMax= 10
import xc_base
import geom
import xc
import math
from model.sets import sets_mng as sUtils

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor

s1= prep.getSets.defSet("S1")

s2= prep.getSets.defSet("S2")

s3= s1+s2
s4= s1*s2
s5= s1-s2

name3= s3.name
name4= s4.name
name5= s5.name
'''
print name3, name4, name5
'''

s= prep.getSets.getSet('S1')
gs3= prep.getSets.getSet(name3)
gs4= prep.getSets.getSet(name4)
gs5= prep.getSets.getSet(name5)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (gs3.name==name3) and (gs4.name==name4) and (gs5.name==name5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
