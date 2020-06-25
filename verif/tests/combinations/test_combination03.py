# -*- coding: utf-8 -*-
from __future__ import print_function
'''Obtaining «previous» combination and combination difference. Home made test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor  

#Load modulation.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
lpA= lPatterns.newLoadPattern("default","A")
lpB= lPatterns.newLoadPattern("default","B")
lpC= lPatterns.newLoadPattern("default","C")

combs= loadHandler.getLoadCombinations
comb1= combs.newLoadCombination("COMB1","1.33*A+1.5*B+0.5*B")
comb2= combs.newLoadCombination("COMB2","1.33*A+1.5*B+1.0*C")
comb4= combs.newLoadCombination("COMB4","1.33*A")
comb3= combs.newLoadCombination("COMB3","1.33*A+0.5*B")
difference= combs.newLoadCombination("DIFFERENCE","")
previousName= combs.getCombPrevia(combs.getComb("COMB1")).name
difference.asigna("COMB1")
difference.subtract(previousName)
descompDifference= difference.getDescomp("%3.1f")

ratio1= (previousName=="COMB3")
ratio2= (descompDifference=="1.5*B")


# print("ratio1= ",ratio1)
# print("ratio2= ",ratio2)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1 & ratio2 :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
