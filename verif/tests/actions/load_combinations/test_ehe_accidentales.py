# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import loadCombinations
from actions.load_combination_utils import ehe
from misc_utils import log_messages as lmsg

dbName= "/tmp/test_accidentales.db"
tbName= "combinations"

lcg= ehe.combGenerator
# G= lcg.newPermanentAction(actionName= "G", actionDescription= "Self weight")
a1= lcg.newAccidentalAction(actionName= "A1", actionDescription= "Hipot. 1")
lcg.newAccidentalAction(actionName= "A2", actionDescription= "Hipot. 2")
lcg.newAccidentalAction(actionName= "A3", actionDescription= "Hipot. 3")
lcg.newAccidentalAction(actionName= "A4", actionDescription= "Hipot. 4")
lcg.newAccidentalAction(actionName= "A5", actionDescription= "Hipot. 5")
lcg.newAccidentalAction(actionName= "A6", actionDescription= "Hipot. 6")

import os
os.system("rm -f "+dbName)

from misc.sqlite_utils import sqlite_macros


sqlite_macros.SQLTcreaDBase(dbName)
nmbTablaComb= "comb_elu_accidentales"

lcg.computeCombinations()
combAcc= lcg.getULSAccidentalCombinations()
cont= len(combAcc)
    
    
ratio1= abs(cont-6.0)/6.0

'''    
for comb in combAcc:
    print(comb.name)
print("cont= ", cont)
print("ratio1= ", ratio1)
'''   

fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
