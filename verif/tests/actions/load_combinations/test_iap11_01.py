# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import loadCombinations
from actions.load_combination_utils import iap11 # Spanish IAP-11
from misc_utils import log_messages as lmsg

lcg= iap11.combGenerator
#print('*******', pond.permanentActions.gammaF.getGammaFELU.desfavorable)

G1= lcg.newPermanentAction(actionName=  "G1", actionDescription= "Peso propio")
G2= lcg.newPermanentAction(actionName=  "G2", actionDescription= "Carga muerta")
G3= lcg.newPermanentAction(actionName=  "G3", actionDescription=  "Retracc.")
Q1a_1= lcg.newHeavyVehicleAction(actionName=  "Q1a_1", actionDescription= "Tren cargas pos. 1a_1")
Q1a_2= lcg.newHeavyVehicleAction(actionName=  "Q1a_2", actionDescription= "Tren cargas pos. 1a_2")

      
#lcg.setVerbosityLevel(3)
lcg.computeCombinations()
coeficientesRef_elu_persistentes= [[1.0, 1.0, 1.0, 0.0, 0.0], [1.0, 1.0, 1.35, 0.0, 0.0], [1.0, 1.35, 1.0, 0.0, 0.0], [1.0, 1.35, 1.35, 0.0, 0.0], [1.35, 1.0, 1.0, 0.0, 0.0], [1.35, 1.0, 1.35, 0.0, 0.0], [1.35, 1.35, 1.0, 0.0, 0.0], [1.35, 1.35, 1.35, 0.0, 0.0], [1.0, 1.0, 1.0, 1.35, 0.0], [1.0, 1.0, 1.0, 1.35, 1.01], [1.0, 1.0, 1.0, 0.0, 1.35], [1.0, 1.0, 1.0, 1.01, 1.35], [1.0, 1.0, 1.35, 1.35, 0.0], [1.0, 1.0, 1.35, 1.35, 1.01], [1.0, 1.0, 1.35, 0.0, 1.35], [1.0, 1.0, 1.35, 1.01, 1.35], [1.0, 1.35, 1.0, 1.35, 0.0], [1.0, 1.35, 1.0, 1.35, 1.01], [1.0, 1.35, 1.0, 0.0, 1.35], [1.0, 1.35, 1.0, 1.01, 1.35], [1.0, 1.35, 1.35, 1.35, 0.0], [1.0, 1.35, 1.35, 1.35, 1.01], [1.0, 1.35, 1.35, 0.0, 1.35], [1.0, 1.35, 1.35, 1.01, 1.35], [1.35, 1.0, 1.0, 1.35, 0.0], [1.35, 1.0, 1.0, 1.35, 1.01], [1.35, 1.0, 1.0, 0.0, 1.35], [1.35, 1.0, 1.0, 1.01, 1.35], [1.35, 1.0, 1.35, 1.35, 0.0], [1.35, 1.0, 1.35, 1.35, 1.01], [1.35, 1.0, 1.35, 0.0, 1.35], [1.35, 1.0, 1.35, 1.01, 1.35], [1.35, 1.35, 1.0, 1.35, 0.0], [1.35, 1.35, 1.0, 1.35, 1.01], [1.35, 1.35, 1.0, 0.0, 1.35], [1.35, 1.35, 1.0, 1.01, 1.35], [1.35, 1.35, 1.35, 1.35, 0.0], [1.35, 1.35, 1.35, 1.35, 1.01], [1.35, 1.35, 1.35, 0.0, 1.35], [1.35, 1.35, 1.35, 1.01, 1.35]]

coeficientes_elu_persistentes= []
combAcc= lcg.getULSTransientCombinations()
base= xc_base.vector_string_from_py_list(["G1","G2","G3","Q1a_1","Q1a_2"])
for comb in combAcc:
  coeficientes_elu_persistentes.append(xc_base.vector_double_to_py_list(comb.getCoeficientes(base)))

error= 0
import numpy
szRef= len(coeficientesRef_elu_persistentes)
sz= len(coeficientes_elu_persistentes)
import os
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
fname= os.path.basename(__file__)
if (abs(error)<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

