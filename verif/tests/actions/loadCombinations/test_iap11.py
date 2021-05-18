# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import loadCombinations
from actions.load_combination_utils import iap11
from actions.load_combination_utils import utils
from misc_utils import log_messages as lmsg

lcg= iap11.controlCombGenerator
#print('*******', pond.permanentActions.gammaF.getGammaFELU.desfavorable)

G1= lcg.insert("IAP11","permanentes",loadCombinations.Action("G1", "Peso propio"),"permanentes","permanentes")
G2= lcg.insert("IAP11","permanentes",loadCombinations.Action("G2", "Carga muerta"),"permanentes","permanentes")
G3= lcg.insert("IAP11","permanentes",loadCombinations.Action("G3", "Retracc."),"permanentes","permanentes")
Q1a_1= lcg.insert("IAP11","variables",loadCombinations.Action("Q1a_1", "Tren cargas pos. 1a_1"),"vehículos_pesados","variables_SCuso")
Q1a_2= lcg.insert("IAP11","variables",loadCombinations.Action("Q1a_2", "Tren cargas pos. 1a_2"),"vehículos_pesados","variables_SCuso")

      
#lcg.setVerbosityLevel(3)
lcg.genera()
coeficientesRef_elu_persistentes= [[1.0, 1.0, 1.0, 0.0, 0.0], [1.0, 1.0, 1.35, 0.0, 0.0], [1.0, 1.35, 1.0, 0.0, 0.0], [1.0, 1.35, 1.35, 0.0, 0.0], [1.35, 1.0, 1.0, 0.0, 0.0], [1.35, 1.0, 1.35, 0.0, 0.0], [1.35, 1.35, 1.0, 0.0, 0.0], [1.35, 1.35, 1.35, 0.0, 0.0], [1.0, 1.0, 1.0, 1.35, 0.0], [1.0, 1.0, 1.0, 1.35, 1.01], [1.0, 1.0, 1.0, 0.0, 1.35], [1.0, 1.0, 1.0, 1.01, 1.35], [1.0, 1.0, 1.35, 1.35, 0.0], [1.0, 1.0, 1.35, 1.35, 1.01], [1.0, 1.0, 1.35, 0.0, 1.35], [1.0, 1.0, 1.35, 1.01, 1.35], [1.0, 1.35, 1.0, 1.35, 0.0], [1.0, 1.35, 1.0, 1.35, 1.01], [1.0, 1.35, 1.0, 0.0, 1.35], [1.0, 1.35, 1.0, 1.01, 1.35], [1.0, 1.35, 1.35, 1.35, 0.0], [1.0, 1.35, 1.35, 1.35, 1.01], [1.0, 1.35, 1.35, 0.0, 1.35], [1.0, 1.35, 1.35, 1.01, 1.35], [1.35, 1.0, 1.0, 1.35, 0.0], [1.35, 1.0, 1.0, 1.35, 1.01], [1.35, 1.0, 1.0, 0.0, 1.35], [1.35, 1.0, 1.0, 1.01, 1.35], [1.35, 1.0, 1.35, 1.35, 0.0], [1.35, 1.0, 1.35, 1.35, 1.01], [1.35, 1.0, 1.35, 0.0, 1.35], [1.35, 1.0, 1.35, 1.01, 1.35], [1.35, 1.35, 1.0, 1.35, 0.0], [1.35, 1.35, 1.0, 1.35, 1.01], [1.35, 1.35, 1.0, 0.0, 1.35], [1.35, 1.35, 1.0, 1.01, 1.35], [1.35, 1.35, 1.35, 1.35, 0.0], [1.35, 1.35, 1.35, 1.35, 1.01], [1.35, 1.35, 1.35, 0.0, 1.35], [1.35, 1.35, 1.35, 1.01, 1.35]]

coeficientes_elu_persistentes= []
combAcc= lcg.getLoadCombinations.getULSTransientCombinations
base= xc_base.vector_string_from_py_list(["G1","G2","G3","Q1a_1","Q1a_2"])
for comb in combAcc:
  coeficientes_elu_persistentes.append(xc_base.vector_double_to_py_list(comb.getCoeficientes(base)))

error= 0
import numpy
szRef= len(coeficientesRef_elu_persistentes)
sz= len(coeficientes_elu_persistentes)
import os
fname= os.path.basename(__file__)
if(szRef!=sz):
  lmsg.error('different sizes: '+str(szRef)+', '+str(sz))
  lmsg.error(fname+' ERROR.')
  error= 100
else:
  for i in range(0,szRef):
    vR= coeficientesRef_elu_persistentes[i]
    v= coeficientes_elu_persistentes[i]
    #print("vR", vR, " v= ", v)
    resta= numpy.subtract(vR,v)
    norm= numpy.linalg.norm(resta)**2
    # if(norm>0.1):
    #    print('i= ', i)
    #    print('vR= ', vR)
    #    print('v= ', v)
    #    print('diff: ', resta)
    #    print('error= ', norm)
    error+= norm
    #print(resta)
    #print("error= ", error)

import math
error= math.sqrt(error)


'''
print(coeficientes_elu_persistentes)
print("error= ", error)
utils.listActionWeighting(lcg.actionWeighting)
lc=lcg.getLoadCombinations
ULS=lc.getULSTransientCombinations
comb=[c for c in ULS]
for c in comb:
    print(c.descripcion)
'''
if (abs(error)<1e-6):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")

