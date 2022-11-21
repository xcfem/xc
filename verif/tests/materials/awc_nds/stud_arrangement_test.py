# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2022, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es"

import yaml
import xc
from materials.sections import section_properties
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

# Geometry
wallHeight= 11*units_utils.footToMeter-22*units_utils.inchToMeter
studSpacing= 8.0*units_utils.inchToMeter
# Materials
# Spruce-pine-fir No. 2 
wood= dimensional_lumber.SprucePineFirWood(grade= 'stud')
studSection= AWCNDS_materials.DimensionLumberSection(name= '2x6', woodMaterial= wood)

#Loads
## Wind loads
windWallPressure= 852.0 # Pa
windStudPressure= windWallPressure*studSpacing # N/m

title= '1st floor facade stud.'
# Actions
## Reduction in uniform live loads.
liveLoadReductionFactor= 1.0 # No reduction.

## Load definition (values from truss_AB_reactions.ods)
loadDict= {'deadLoad': xc.Vector([0.0,15.25e3]), 'liveLoad': liveLoadReductionFactor*xc.Vector([0.0,26.17e3]), 'snowLoad': xc.Vector([0.0,11.28e3]), 'windLoad': xc.Vector([windStudPressure,-7.13e3])} # kN/m

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

studObj= AWCNDS_limit_state_checking.StudArrangement(name= title, studSection= studSection, studSpacing= studSpacing, wallHeight= wallHeight, loadCombDurationFactorFunction= getLoadCombDurationFactor);

results, worstCase= studObj.check(loadDict= loadDict, loadCombinations= combContainer.SLS.qp)

worstCF= results[worstCase]['CF'][0]
worstCFRef= 0.8273532593387054
ratio= abs(worstCF-worstCFRef)/worstCFRef

'''
print('worst case: ', worstCase)
print('worst efficiency: ', worstCF)
print('ratio: ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((worstCase=='EQ1609') and (ratio<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
