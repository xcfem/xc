# -*- coding: utf-8 -*-
''' Check the structural integrity of the top plates of a bearing wall.'''

from __future__ import division
from __future__ import print_function

import math
import yaml
import geom
import xc
from model import predefined_spaces
from materials.awc_nds import AWCNDS_materials
from materials.awc_nds import AWCNDS_limit_state_checking
from materials.awc_nds import dimensional_lumber
from actions import combinations as combs
from misc_utils import units_utils

def getLoadCombDurationFactor(loadComb):
    ll= ('liveLoad' in loadComb)
    dl= ('deadLoad' in loadComb)
    sl= ('snowLoad' in loadComb)
    wl= ('windLoad' in loadComb)
    return AWCNDS_materials.getLoadCombinationDurationFactor(deadLoad= dl, liveLoad= ll, snowLoad= sl, windLoad= wl)

title= 'wall top plates test.'
studSpacing= 16.0*units_utils.inchToMeter
trussSpacing= 12.0*units_utils.inchToMeter
# Materials
# Spruce-pine-fir No. 2 
wood= dimensional_lumber.SprucePineFirWood(grade= 'no_2')
#wood= dimensional_lumber.SprucePineFirWood(grade= 'stud')
xc_material= wood.defXCMaterial()
plateSection= AWCNDS_materials.CustomLumberSection(name= '2x6',b= 5.5*units_utils.inchToMeter, h= 1.5*units_utils.inchToMeter, woodMaterial= wood)


# Reduction in uniform live loads.
liveLoadReductionFactor= 1.0 # No reduction factor.

# Actions
## Load definition (values from truss_AB_reactions.ods)
deadLoad= 4.65e3 # kN/truss
liveLoad= liveLoadReductionFactor*7.98e3 # kN/truss
snowLoad= 3.44e3 # kN/truss
windLoad= -2.17e3 # kN/truss
loadDict= {'deadLoad':deadLoad, 'liveLoad':liveLoad, 'snowLoad':snowLoad, 'windLoad':windLoad}

# Load combination definition
combContainer= combs.CombContainer()

## Read load combinations.
import os
pth= os.path.dirname(__file__)
#print("pth= ", pth)
if(not pth):
    pth= "."
fName= pth+'/../../aux/awc_nds_load_combinations.yaml'
with open(fName) as file:
    try:
        combData= yaml.safe_load(file)
    except yaml.YAMLError as exception:
        print(exception)
        
for combName in combData:
    combExpr= combData[combName]
    combContainer.SLS.qp.add(combName,combExpr)


doublePlate= AWCNDS_limit_state_checking.WallTopPlates(title= title, plateSection= plateSection, studSpacing= studSpacing, trussSpacing= trussSpacing, pointLoadFactor= 1.0/3.0, loadCombDurationFactorFunction= getLoadCombDurationFactor);

results, worstResults= doublePlate.check(loadDict, combContainer)

# worst cases.
worstDeflectionCase= worstResults['worstDeflectionCase']
worstDeflectionValue= worstResults['worstDeflectionValue']
worstBendingCase= worstResults['worstBendingCase']
worstBendingCF= worstResults['worstBendingCF']
worstShearCase= worstResults['worstShearCase']
worstShearCF= worstResults['worstShearCF']
worstPerpComprCase= worstResults['worstPerpComprCase']
worstPerpComprCF= worstResults['worstPerpComprCF']

worstCasesOk= (worstDeflectionCase=='EQ1611')
worstCasesOk= worstCasesOk and (worstBendingCase=='EQ1611')
worstCasesOk= worstCasesOk and (worstShearCase=='EQ1611')
worstCasesOk= worstCasesOk and (worstPerpComprCase=='EQ1611')

ratio1= abs(worstDeflectionValue-0.0006571529990458183)/0.0006571529990458183
ratio2= abs(worstBendingCF-0.8454475777931175)/0.8454475777931175
ratio3= abs(worstShearCF-0.6582678785975511)/0.6582678785975511
ratio4= abs(worstPerpComprCF-1.0306325564675434)/1.0306325564675434

'''
doublePlate.printResults(combContainer, results)
print('worstDeflectionCase=', worstDeflectionCase)
print('worstDeflectionValue=', worstDeflectionValue)
print('worstBendingCase=', worstBendingCase)
print('worstBendingCF=', worstBendingCF)
print('worstShearCase=', worstShearCase)
print('worstShearCF=', worstShearCF)
print('worstPerpComprCase=', worstPerpComprCase)
print('worstPerpComprCF=', worstPerpComprCF)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(worstCasesOk and (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6) and (ratio4<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
