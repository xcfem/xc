# -*- coding: utf-8 -*-
''' Home-made Reinforced concrete section. Crack control verification test.
'''

import xc_base
import geom
import xc

from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure
from materials.sections.fiber_section import def_simple_RC_section
# from materials.sections import section_properties


from materials.ehe import EHE_materials
from materials.sia262 import SIA262_materials
from materials.sia262 import SIA262_limit_state_checking
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

areaFi22= SIA262_materials.section_barres_courantes[22e-3]
areaFi26= SIA262_materials.section_barres_courantes[26e-3]

datosScc1LosC= def_simple_RC_section.RCSimpleSection()
datosScc1LosC.sectionName= "secHA1LosC"
datosScc1LosC.sectionDescr= "Deck. Central portion. Section normal to X axis."
concr= EHE_materials.HA30
concr.alfacc=0.85 #f_maxd= 0.85*fcd
datosScc1LosC.fiberSectionParameters.concrType= concr
datosScc1LosC.h= 0.35
datosScc1LosC.b= 1.0001
datosScc1LosC.fiberSectionParameters.reinfSteelType= EHE_materials.B500S
negRebRow=def_simple_RC_section.MainReinfLayer(nRebars=2,rebarsDiam=10e-10,width=datosScc1LosC.b)
#negRebRow=def_simple_RC_section.MainReinfLayer()
#negRebRow=def_simple_RC_section.MainReinfLayer(rebarsDiam=10e-3,areaRebar= EHE_materials.Fi10,rebarsSpacing=0.2,width=1.0,nominalCover=0.03)
#negRebRow.setUp(nRebars=0,rebarsDiam=0.0,areaRebar=0.0,width=datosScc1LosC.b,cover=0.1)
datosScc1LosC.negatvRebarRows=[negRebRow]
posRebRow=def_simple_RC_section.MainReinfLayer(nRebars=6,rebarsDiam=26e-3,areaRebar=(areaFi22+areaFi26)/2.0,width=datosScc1LosC.b,nominalCover=0.05,nominalLatCover=0.08333-26e-3/2.)
#posRebRow=def_simple_RC_section.MainReinfLayer()
#posRebRow=def_simple_RC_section.MainReinfLayer(rebarsDiam=10e-3,areaRebar= EHE_materials.Fi10,rebarsSpacing=0.2,width=1.0,nominalCover=0.03)
#posRebRow.setUp(nRebars=6,rebarsDiam=26e-3,areaRebar=(areaFi22+areaFi26)/2.0,width=datosScc1LosC.b,cover=0.05+0.026/2.0)
datosScc1LosC.positvRebarRows=[posRebRow]

#datosScc1LosC.negatvRebars.setUp(0,0.0,0.0,datosScc1LosC.b,0.1)
#datosScc1LosC.positvRebars.setUp(6,26e-3,(areaFi22+areaFi26)/2.0,datosScc1LosC.b,0.05+0.026/2.0)



NDato= 0 # Axial force for crack control checking.
MyDato= 117e3 # Bending moment force for crack control checking.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
# concreteMatTag= EHE_materials.HA30.defDiagK(preprocessor)
# reinfSteelMaterialTag= EHE_materials.B500S.defDiagK(preprocessor)


datosScc1LosC.defRCSimpleSection(preprocessor, "k")

scc3d_testing_bench.sectionModel(preprocessor, datosScc1LosC.sectionName)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_0F0(2)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,MyDato,3.55271e-15]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)



# Solution procedure
analysis= predefined_solutions.simple_newton_raphson(feProblem)
analOk= analysis.analyze(10)


secHAParamsFis= SIA262_limit_state_checking.CrackControlSIA262('SLS_crack',400e6)


elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
scc= ele1.getSection()
sigma_s= secHAParamsFis.calcRebarStress(scc)


ratio1= (sigma_s-166.925177095e6)/166.925177095e6


''' 
print "sigma_s= ",sigma_s/1e6, " MPa"
print "ratio1= ",ratio1
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
