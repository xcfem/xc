# -*- coding: utf-8 -*-


import xc_base
import geom
import xc
from materials.ehe import EHE_materials
from materials.fiber_section import defSeccionHASimple
from postprocess import RC_material_distribution
from materials import RCsectionsContainer as sc
from solution import predefined_solutions
from materials.sia262 import SIA262_limit_state_checking #Change SIA262->EHE

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Don't pring warnings
prueba.errFileName= "/tmp/borrar.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.

import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
intForcCombFileName= pth+"/esf_test_xLamina.csv"


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

deckSections= defSeccionHASimple.RecordRCSlabBeamSection("deck","RC deck.",concrete, reinfSteel,0.3)
deckSections.lstRCSects[1].positvRebarRows= [defSeccionHASimple.MainReinfLayer(rebarsDiam=12e-3,areaRebar=areaFi12,rebarsSpacing=sepT,nominalCover=basicCover)]
deckSections.lstRCSects[1].negatvRebarRows= [defSeccionHASimple.MainReinfLayer(rebarsDiam=12e-3,areaRebar=areaFi12,rebarsSpacing=sepT,nominalCover=basicCover)]
deckSections.lstRCSects[0].positvRebarRows= [defSeccionHASimple.MainReinfLayer(rebarsDiam=20e-3,areaRebar=areaFi20,rebarsSpacing=sepL,nominalCover=basicCover+12e-3)]
deckSections.lstRCSects[0].negatvRebarRows= [defSeccionHASimple.MainReinfLayer(rebarsDiam=20e-3,areaRebar=areaFi20,rebarsSpacing=sepL,nominalCover=basicCover+12e-3)]
sections.append(deckSections)



controller= SIA262_limit_state_checking.BiaxialBendingNormalStressController('ULS_normalStress')
meanFCs= reinfConcreteSections.internalForcesVerification3D(intForcCombFileName,"/tmp/ppTN", "d",controller)


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
