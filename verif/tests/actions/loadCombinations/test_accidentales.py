# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import loadCombinations
from actions.load_combination_utils import ehe

dbName= "/tmp/test_accidentales.db"
tbName= "combinations"

lcg= loadCombinations.LoadCombGenerator()
actionsAndFactors= lcg.actionWeighting.create("EHEIntenso",ehe.factors)
#G= lcg.insert("EHEIntenso","permanentes",loadCombinations.Action("G","Self weight"),"permanents","permanentes_ctr_intenso")
lcg.insert("EHEIntenso","accidentales",loadCombinations.Action("A1","Hipot. 1"),"accidental","accidentales_ctr_intenso")
lcg.insert("EHEIntenso","accidentales",loadCombinations.Action("A2","Hipot. 2"),"accidental","accidentales_ctr_intenso")
lcg.insert("EHEIntenso","accidentales",loadCombinations.Action("A3","Hipot. 3"),"accidental","accidentales_ctr_intenso")
lcg.insert("EHEIntenso","accidentales",loadCombinations.Action("A4","Hipot. 4"),"accidental","accidentales_ctr_intenso")
lcg.insert("EHEIntenso","accidentales",loadCombinations.Action("A5","Hipot. 5"),"accidental","accidentales_ctr_intenso")
lcg.insert("EHEIntenso","accidentales",loadCombinations.Action("A6","Hipot. 6"),"accidental","accidentales_ctr_intenso")

import os
os.system("rm -f "+dbName)

import sqlite3 as sqlite
from misc.sqlite_utils import sqlite_macros


sqlite_macros.SQLTcreaDBase(dbName)
nmbTablaComb= "comb_elu_accidentales"

lcg.genera()
combAcc= lcg.getLoadCombinations.getULSAccidentalCombinations
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
