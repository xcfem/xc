# -*- coding: utf-8 -*-


import xc_base
import geom
import xc
from materials.ehe import EHE_materials
from materials.sections.fiber_section import defSimpleRCSection
from postprocess import RC_material_distribution
from materials.sections import RCsectionsContainer as sc
from solution import predefined_solutions
from materials.sia262 import SIA262_limit_state_checking #Change SIA262->EHE
from postprocess import limit_state_data as lsd
from postprocess.config import output_config as oc

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Don't pring warnings
feProblem.errFileName= "/tmp/erase.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.


elementTags= [2524,2527]
#Reinforced concrete sections on each element.
reinfConcreteSections= RC_material_distribution.RCMaterialDistribution()

for eTag in elementTags:
  reinfConcreteSections.sectionDistribution[eTag]= ["deck2","deck1"]

# deck.
concrete= EHE_materials.HA30
concrete.alfacc= 0.85  #f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
reinfSteel= EHE_materials.B500S
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

deckSections= defSimpleRCSection.RecordRCSlabBeamSection("deck","RC deck.",concrete, reinfSteel,0.3)
deckSections.dir2PositvRebarRows= [defSimpleRCSection.MainReinfLayer(rebarsDiam=12e-3,areaRebar=areaFi12,rebarsSpacing=sepT,nominalCover=basicCover)]
deckSections.dir2NegatvRebarRows= [defSimpleRCSection.MainReinfLayer(rebarsDiam=12e-3,areaRebar=areaFi12,rebarsSpacing=sepT,nominalCover=basicCover)]
deckSections.dir1PositvRebarRows= [defSimpleRCSection.MainReinfLayer(rebarsDiam=20e-3,areaRebar=areaFi20,rebarsSpacing=sepL,nominalCover=basicCover+12e-3)]
deckSections.dir1NegatvRebarRows= [defSimpleRCSection.MainReinfLayer(rebarsDiam=20e-3,areaRebar=areaFi20,rebarsSpacing=sepL,nominalCover=basicCover+12e-3)]
deckSections.creaTwoSections()
sections.append(deckSections)


import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."

#Checking normal stresses.
lsd.normalStressesResistance.controller= SIA262_limit_state_checking.BiaxialBendingNormalStressController('ULS_normalStress')
lsd.LimitStateData.internal_forces_results_directory= pth+'/'
lsd.LimitStateData.check_results_directory= '/tmp/'
lsd.normalStressesResistance.outputDataBaseFileName= 'ppTN'
#intForceFileName= lsd.normalStressesResistance.getInternalForcesFileName()

outCfg=oc.verifOutVars(listFile='N',calcMeanCF='Y')

meanFCs= reinfConcreteSections.internalForcesVerification3D(lsd.normalStressesResistance,"d",outCfg)


#print "mean FCs: ", meanFCs

meanFC0Teor= 0.64702580108264973
ratio1= abs(meanFCs[0]-meanFC0Teor)/meanFC0Teor
meanFC1Teor= 0.84660274501497856
ratio2= abs(meanFCs[1]-meanFC1Teor)/meanFC1Teor

'''
print "meanFCs[0]= ", meanFCs[0]
print "ratio1= ",ratio1
print "meanFCs[1]= ", meanFCs[1]
print "ratio2= ",ratio2
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
