# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import xc_base
from actions.load_combination_utils import iap11
from misc_utils import log_messages as lmsg
import re
import math
import json


def computeError(coefs, coefsRef):
    retval= 0.0
    for l,lRef in zip(coefs, coefsRef):
        for c, cRef in zip(l,lRef):
            retval+= (c-cRef)**2
    retval= math.sqrt(retval)
    return retval
    
lcg= iap11.combGenerator
#print('*******', pond.permanentActions.gammaF.getGammaFELU.desfavorable)
actions= list()
G1= lcg.newPermanentAction(actionName= "G1", actionDescription= "Peso propio")
actions.append(G1)
G2= lcg.newPermanentAction(actionName= "G2", actionDescription= "Carga muerta")
actions.append(G2)
Q1= lcg.newFootbridgeAction(actionName= "Q1", actionDescription= "Carga uniforme.")
actions.append(Q1)
Q2= lcg.newFootbridgeAction(actionName= "Q2", actionDescription= "Carga horizontal en barandilla.")
Q2.relationships.appendMain("Q1")
actions.append(Q2)
#Qviento= lcg.insert(weighting= "IAP11", family="variables", action= loadCombinations.Action("Qviento", "Viento transversal."), combination_factors_name= "viento_pasarelas", partial_safety_factors_name= "variables_SCuso")
Qviento= lcg.newFootbridgeWindAction(actionName= "Qviento", actionDescription= "Viento transversal.")
actions.append(Qviento)
Qtemp1= lcg.newThermalAction(actionName= "Qtemp1", actionDescription= "Incremento temperatura.", incompatibleActions= ['Qviento'])
actions.append(Qtemp1)
Qtemp2= lcg.newThermalAction(actionName= "Qtemp2", actionDescription= "Incremento temperatura.", incompatibleActions= ['Qviento', 'Qtemp1'])
actions.append(Qtemp2)
Qnieve= lcg.newSnowAction(actionName= "Qnieve", actionDescription= "Nieve.")
actions.append(Qnieve)
Asismo= lcg.newSeismicAction(actionName= "Asismo", actionDescription= "Sismo.")
actions.append(Asismo)

actionNames= list()
for a in actions:
    actionNames.append(a.name)
    
#lcg.setVerbosityLevel(3)
lcg.computeCombinations()
eluPersist= lcg.getULSTransientCombinations()

gSmallRegex= re.compile(r'.*1.00\*G.*') # permanent load favourable.
gBigRegex= re.compile(r'.*1.35\*G.*') # permanent load unfavourable.
liveLoadRegex= re.compile(r'.*Q[12].*')
windRegex= re.compile('.*Qviento.*') # wind load present.
for comb in eluPersist:
    expr= comb.name
    gSmall= gSmallRegex.match(expr) # true if small permanent load.
    gBig= gBigRegex.match(expr) # true if big permanent load.
    liveLoad= liveLoadRegex.match(expr) # true if live load present.
    wind= windRegex.match(expr) # true if wind present.
    drop= False
    if(wind):
        drop= gBig # drop if big permanent load.
        drop= drop or liveLoad # drop if live load present.
    else:
        drop= gSmall # drop if small permanent load.
        
# Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'
# Read reference data.
refFile= pth+'/../../../aux/reference_files/ref_test_iap11_02.json'
with open(refFile) as f:
   refValues= json.load(f)
eluPersistCoefsRef= refValues['eluPersistCoefsRef']
eluSeismicCoefsRef= refValues['eluSeismicCoefsRef']
elsCharacteristicCoefsRef= refValues['elsCharacteristicCoefsRef']
elsFrequentCoefsRef= refValues['elsFrequentCoefsRef']
elsQuasiPermanentCoefsRef= refValues['elsQuasiPermanentCoefsRef']

eluPersistCoefs= eluPersist.getCoefficients(actionNames)


err= computeError(eluPersistCoefs, eluPersistCoefsRef)

eluSeismic= lcg.getULSSeismicCombinations()
eluSeismicCoefs= eluSeismic.getCoefficients(actionNames)

err+= computeError(eluSeismicCoefs, eluSeismicCoefsRef)

elsCharacteristic= lcg.getSLSCharacteristicCombinations()
elsCharacteristicCoefs= elsCharacteristic.getCoefficients(actionNames)

err+= computeError(elsCharacteristicCoefs, elsCharacteristicCoefsRef)

elsFrequent= lcg.getSLSFrequentCombinations()
elsFrequentCoefs= elsFrequent.getCoefficients(actionNames)

err+= computeError(elsFrequentCoefs, elsFrequentCoefsRef)

elsQuasiPermanent= lcg.getSLSQuasiPermanentCombinations()
elsQuasiPermanentCoefs= elsQuasiPermanent.getCoefficients(actionNames)

err+= computeError(elsQuasiPermanentCoefs, elsQuasiPermanentCoefsRef)

# values= dict()
# values['eluPersistCoefsRef']= eluPersistCoefs
# values['eluSeismicCoefsRef']= eluSeismicCoefs
# values['elsCharacteristicCoefsRef']= elsCharacteristicCoefs
# values['elsFrequentCoefsRef']= elsFrequentCoefs
# values['elsQuasiPermanentCoefsRef']= elsQuasiPermanentCoefs
# outputFile= pth+'/ref_test_iap11_02.json'
# with open(outputFile,'w') as f:
#    json.dump(values, f)


fname= os.path.basename(__file__)
if (abs(err)<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
