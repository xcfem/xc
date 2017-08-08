# -*- coding: utf-8 -*-
'''Obtaining «previous» combination and combination difference. Home made test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor  

#Load modulation.
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lpA= casos.newLoadPattern("default","A")
lpB= casos.newLoadPattern("default","B")
lpC= casos.newLoadPattern("default","C")

combs= cargas.getLoadCombinations
comb1= combs.newLoadCombination("COMB1","1.33*A+1.5*B+0.5*B")
comb2= combs.newLoadCombination("COMB2","1.33*A+1.5*B+1.0*C")
comb4= combs.newLoadCombination("COMB4","1.33*A")
comb3= combs.newLoadCombination("COMB3","1.33*A+0.5*B")
resta= combs.newLoadCombination("RESTA","")
nombrePrevia= combs.getCombPrevia(combs.getComb("COMB1")).getName
resta.asigna("COMB1")
resta.resta(nombrePrevia)
descompResta= resta.getDescomp("%3.1f")

ratio1= (nombrePrevia=="COMB3")
ratio2= (descompResta=="1.5*B")


# print "ratio1= ",ratio1
# print "ratio2= ",ratio2


import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if ratio1 & ratio2 :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
