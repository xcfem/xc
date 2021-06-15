# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import xc_base
import geom
import xc
from materials.sia262 import SIA262_materials
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import element_section_map
from postprocess import RC_material_distribution
from materials.sections import RC_sections_container as sc
from materials.sia262 import SIA262_limit_state_checking
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
import shutil

import logging


#Hide INFO messages from modules.
rootLogger = logging.getLogger()
rootLogger.setLevel(logging.ERROR)

feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Don't print warnings
feProblem.errFileName= "/tmp/erase.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.


elementTags= [2524,2527]
#Reinforced concrete sections on each element.
reinfConcreteSections= RC_material_distribution.RCMaterialDistribution()

for eTag in elementTags:
    reinfConcreteSections.sectionDistribution[eTag]= ["deck2","deck1"]

# deck.
concrete= SIA262_materials.c30_37
concrete.alfacc= 0.85  #f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
reinfSteel= SIA262_materials.B500B
areaFi8= 0.50e-4 #XXX Rebar area expressed in square meters.
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

sections= reinfConcreteSections.sectionDefinition

deckSections= element_section_map.RCSlabBeamSection("deck","RC deck.",concrete, reinfSteel,0.3)
deckSections.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam=12e-3,areaRebar=areaFi12,rebarsSpacing=sepT,nominalCover=basicCover)])
deckSections.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam=12e-3,areaRebar=areaFi12,rebarsSpacing=sepT,nominalCover=basicCover)])
deckSections.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam=20e-3,areaRebar=areaFi20,rebarsSpacing=sepL,nominalCover=basicCover+12e-3)])
deckSections.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam=20e-3,areaRebar=areaFi20,rebarsSpacing=sepL,nominalCover=basicCover+12e-3)])
sections.append(deckSections)

import os
pth= os.path.dirname(__file__)
#print("pth= ", pth)
if(not pth):
    pth= "."
fname= os.path.basename(__file__)

#Checking normal stresses.
cfg=default_config.EnvConfig(language='en', resultsPath= 'tmp_results/', intForcPath= 'internalForces/',verifPath= 'verifications/',reportPath='./',reportResultsPath= 'annex/',grWidth='120mm')
cfg.projectDirTree.workingDirectory= '/tmp/'+os.path.splitext(fname)[0]
cfg.projectDirTree.createTree() # To allow copying existing internal force data into.
lsd.LimitStateData.envConfig= cfg
shutil.copy(pth+'/intForce_ULS_normalStressesResistance.csv',lsd.normalStressesResistance.getInternalForcesFileName())
#lsd.LimitStateData.internal_forces_results_directory= pth+'/'
#lsd.LimitStateData.check_results_directory= '/tmp/'
#lsd.normalStressesResistance.outputDataBaseFileName= 'ppTN'
#intForceFileName= lsd.normalStressesResistance.getInternalForcesFileName()

outCfg= lsd.VerifOutVars(listFile='N',calcMeanCF='Y')
outCfg.controller= SIA262_limit_state_checking.BiaxialBendingNormalStressController('ULS_normalStress')

meanFCs= reinfConcreteSections.internalForcesVerification3D(lsd.normalStressesResistance,"d",outCfg)


#print("mean FCs: ", meanFCs)

meanFC0Teor= 0.6467847191964301
ratio1= abs(meanFCs[0]-meanFC0Teor)/meanFC0Teor
meanFC1Teor= 0.8366279701238778
ratio2= abs(meanFCs[1]-meanFC1Teor)/meanFC1Teor

'''
print("meanFCs[0]= ", meanFCs[0])
print("ratio1= ",ratio1)
print("meanFCs[1]= ", meanFCs[1])
print("ratio2= ",ratio2)
'''

feProblem.errFileName= "cerr" # Display errors if any.
import os
from misc_utils import log_messages as lmsg
if (ratio1<0.01) & (ratio2<0.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
