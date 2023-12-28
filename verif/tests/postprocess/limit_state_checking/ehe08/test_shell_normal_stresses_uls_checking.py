# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import xc
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import element_section_map
from postprocess import RC_material_distribution
from materials.ehe import EHE_limit_state_checking
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
import shutil

import logging


#Hide INFO messages from modules.
rootLogger = logging.getLogger()
rootLogger.setLevel(logging.ERROR)

feProblem= xc.FEProblem()

elementTags= [2524,2527]
#Reinforced concrete sections on each element.
reinfConcreteSections= RC_material_distribution.RCMaterialDistribution()

# Set-up material distribution and element dimensions.
for eTag in elementTags:
    reinfConcreteSections.sectionDistribution[eTag]= ["deck2","deck1"]
    reinfConcreteSections.sectionDistribution.elementDimension[eTag]= 2

# deck.
concrete= EHE_materials.HA30
concrete.alfacc= 0.85  #f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
reinfSteel= EHE_materials.B500S
areaFi8= 0.50e-4 # Rebar area expressed in square meters.
areaFi10= 0.785e-4
areaFi12= 1.13e-4 
areaFi16= 2.01e-4
areaFi20= 3.14e-4
areaFi25= 4.608e-4
basicCover= 0.06
numReinfBarsT= 5
sepT= 1.0/numReinfBarsT
numReinfBarsL= 7
sepL= 1.0/numReinfBarsL

sections= reinfConcreteSections.sectionDefinition # Get the sections container.

deckSections= element_section_map.RCSlabBeamSection("deck","RC deck.",concrete, reinfSteel,0.3)
deckSections.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam=12e-3,rebarsSpacing=sepT,nominalCover=basicCover)])
deckSections.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam=12e-3,rebarsSpacing=sepT,nominalCover=basicCover)])
deckSections.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam=20e-3,rebarsSpacing=sepL,nominalCover=basicCover+12e-3)])
deckSections.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam=20e-3,rebarsSpacing=sepL,nominalCover=basicCover+12e-3)])
sections.append(deckSections)

# # Display sections geometry
# feProblem= xc.FEProblem()
# preprocessor=  feProblem.getPreprocessor
# deckSections.plot(preprocessor, matDiagType= 'k')

import os
pth= os.path.dirname(__file__)
#print("pth= ", pth)
if(not pth):
    pth= "."

#Checking normal stresses.
cfg= default_config.get_temporary_env_config()
cfg.projectDirTree.createTree() # To allow copying existing internal force data into.
lsd.LimitStateData.envConfig= cfg
internalForcesFName= pth+"/../../../aux/internal_forces/intForce_ULS_normalStressesResistance.csv"
shutil.copy(internalForcesFName, lsd.normalStressesResistance.getInternalForcesFileName())

# Check limit state.
## Limit state to check.
limitState= lsd.normalStressesResistance
## Build controller.
controller= EHE_limit_state_checking.BiaxialBendingNormalStressController(limitState.label)
## Perform checking.
feProblem.errFileName= "/tmp/erase.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSections,listFile='N', calcMeanCF='Y', threeDim= True, controller= controller)
feProblem.errFileName= "cerr" # From now on display errors if any.

# Check results.
meanCF0Teor= 0.64702580108264973
ratio1= abs(meanCFs[0]-meanCF0Teor)/meanCF0Teor
meanCF1Teor= 0.84660274501497856
ratio2= abs(meanCFs[1]-meanCF1Teor)/meanCF1Teor

'''
print("meanCFs[0]= ", meanCFs[0])
print("ratio1= ",ratio1)
print("meanCFs[1]= ", meanCFs[1])
print("ratio2= ",ratio2)
'''

cfg.cleandirs() # Clean after yourself.
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
