# -*- coding: utf-8 -*-
''' Test calculation of anchorage lengths according to EHE.b Art. 69.5.2
    Home made test, comparison values from https://calculocivil.com/es/ehe08/solape/
'''
from __future__ import division
from __future__ import print_function

import math
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
import os
from misc_utils import log_messages as lmsg
from collections import OrderedDict

__author__= "Ana Ortega (AO_O) Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2023, AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort@gmail.com l.pereztato@gmail.com"

# Materials
ha25= EHE_materials.HA25
ha30= EHE_materials.HA30
b500s= EHE_materials.B500S
cCover= .035 
# Comparative values case 1
cvCase1=[1.63,2.28,0.81,1.14] # tension posI, tension posII, compression posI,compression posII
# Comparative values case 2
cvCase2=[1.14,1.59,0.81,1.14] # tension posI, tension posII, compression posI,compression posII
# Comparative values case 3
cvCase3=[1.43,1.88,1.19,1.56] # tension posI, tension posII, compression posI,compression posII
#
rebarControllerPosIcompr= EHE_limit_state_checking.RebarController(concreteCover= cCover, pos= 'I', compression= True)
rebarControllerPosIIcompr= EHE_limit_state_checking.RebarController(concreteCover= cCover, pos= 'II', compression= True)
rebarControllerPosItens= EHE_limit_state_checking.RebarController(concreteCover= cCover, pos= 'I', compression= False)
rebarControllerPosIItens= EHE_limit_state_checking.RebarController(concreteCover= cCover, pos= 'II', compression= False)
# Case 1
resCase1=list()
resCase1.append(rebarControllerPosItens.getLapLength(concrete=ha30,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.15,steelEfficiency=1,ratioOfOverlapedTensionBars=1,lateralConcreteCover=0,dynamicEffects=False))
resCase1.append(rebarControllerPosIItens.getLapLength(concrete=ha30,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.15,steelEfficiency=1,ratioOfOverlapedTensionBars=1,lateralConcreteCover=0,dynamicEffects=False))
resCase1.append(rebarControllerPosIcompr.getLapLength(concrete=ha30,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.15,steelEfficiency=1,ratioOfOverlapedTensionBars=1,lateralConcreteCover=0,dynamicEffects=False))
resCase1.append(rebarControllerPosIIcompr.getLapLength(concrete=ha30,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.15,steelEfficiency=1,ratioOfOverlapedTensionBars=1,lateralConcreteCover=None,dynamicEffects=False))
                
# Case 2
resCase2=list()
resCase2.append(rebarControllerPosItens.getLapLength(concrete=ha30,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.30,steelEfficiency=1,ratioOfOverlapedTensionBars=1,lateralConcreteCover=0,dynamicEffects=False))
resCase2.append(rebarControllerPosIItens.getLapLength(concrete=ha30,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.30,steelEfficiency=1,ratioOfOverlapedTensionBars=1,lateralConcreteCover=0,dynamicEffects=False))
resCase2.append(rebarControllerPosIcompr.getLapLength(concrete=ha30,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.30,steelEfficiency=1,ratioOfOverlapedTensionBars=1,lateralConcreteCover=0,dynamicEffects=False))
resCase2.append(rebarControllerPosIIcompr.getLapLength(concrete=ha30,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.30,steelEfficiency=1,ratioOfOverlapedTensionBars=1,lateralConcreteCover=None,dynamicEffects=False))

# Case 3
resCase3=list()
resCase3.append(rebarControllerPosItens.getLapLength(concrete=ha25,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.30,steelEfficiency=1,ratioOfOverlapedTensionBars=0.33,lateralConcreteCover=0,dynamicEffects=True))
resCase3.append(rebarControllerPosIItens.getLapLength(concrete=ha25,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.30,steelEfficiency=1,ratioOfOverlapedTensionBars=0.33,lateralConcreteCover=0,dynamicEffects=True))
resCase3.append(rebarControllerPosIcompr.getLapLength(concrete=ha25,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.30,steelEfficiency=1,ratioOfOverlapedTensionBars=0.33,lateralConcreteCover=0,dynamicEffects=True))
resCase3.append(rebarControllerPosIIcompr.getLapLength(concrete=ha25,rebarDiameter=25e-3, steel=b500s,distBetweenNearestSplices=0.30,steelEfficiency=1,ratioOfOverlapedTensionBars=0.33,lateralConcreteCover=None,dynamicEffects=True))

ratio1=sum([abs(resCase1[i]-cvCase1[i])/cvCase1[i] for i in range(4)])
ratio2=sum([abs(resCase2[i]-cvCase2[i])/cvCase2[i] for i in range(4)])
ratio3=sum([abs(resCase3[i]-cvCase3[i])/cvCase3[i] for i in range(4)])

fname= os.path.basename(__file__)
if ((abs(ratio1)<1.5e-2) and (abs(ratio2)<1e-2) and abs(ratio3<1e-2)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
