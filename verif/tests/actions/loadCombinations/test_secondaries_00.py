# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import loadCombinations
from actions.load_combination_utils import ehe

lcg= ehe.intenseControlCombGenerator
PP1= lcg.insert("EHEIntenso","permanentes",loadCombinations.Action("PP1","Peso propio"),"permanents","permanentes_ctr_intenso")

SC1= lcg.insert("EHEIntenso","variables",loadCombinations.Action("SC1","Puente grúa"),"azotea_solo_conserv","variables_ctr_intenso")
SC1.getRelaciones.appendIncompatible("SC.*")

SC2= lcg.insert("EHEIntenso","variables",loadCombinations.Action("SC2","Sobrecarga de uso cubierta"),"azotea_solo_conserv","variables_ctr_intenso")
SC2.getRelaciones.appendIncompatible("SC.*")

F1= lcg.insert("EHEIntenso","variables",loadCombinations.Action("F1","Frenado puente grúa"),"azotea_solo_conserv","variables_ctr_intenso")
F1.getRelaciones.appendMain("SC1.*")

#lcg.setVerbosityLevel(3)
lcg.genera()

coeficientesRef_elu_persistentes= [[1,0,0,0],[1.35,0,0,0],[1,1.5,0,0],[1,1.5,0,1.05],[1,0,1.5,0],[1,1.05,0,1.5],[1.35,1.5,0,0],[1.35,1.5,0,1.05],[1.35,0,1.5,0],[1.35,1.05,0,1.5]]

coeficientes_elu_persistentes= []
combAcc= lcg.getLoadCombinations.getULSTransientCombinations
base= xc_base.vector_string_from_py_list(["PP1","SC1","SC2","F1"])
for comb in combAcc:
  coeficientes_elu_persistentes.append(xc_base.vector_double_to_py_list(comb.getCoeficientes(base)))

error= 0
import numpy
sz= len(coeficientesRef_elu_persistentes)
for i in range(0,sz):
  vR= coeficientesRef_elu_persistentes[i]
  v= coeficientes_elu_persistentes[i]
  #print("vR", vR, " v= ", v)
  resta= numpy.subtract(vR,v)
  error+= numpy.linalg.norm(resta)**2
  #print(resta)
  #print("error= ", error)

import math
error= math.sqrt(error)

#print("error= ", error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(error)<1e-5):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
