# -*- coding: utf-8 -*-
''' Home-made Reinforced concrete section. Crack control verification test.
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure
from materials.sections.fiber_section import def_simple_RC_section
# from materials.sections import section_properties


from materials.sia262 import SIA262_materials
from materials.sia262 import SIA262_limit_state_checking
from model import predefined_spaces


areaFi22= SIA262_materials.section_barres_courantes[22e-3]
areaFi26= SIA262_materials.section_barres_courantes[26e-3]

concr= SIA262_materials.c30_37
concr.alfacc=0.85 # f_maxd= 0.85*fcd
datosScc1LosC= def_simple_RC_section.RCRectangularSection(name= "secHA1LosC", sectionDescr= "Deck. Central portion. Section normal to X axis.", concrType= concr, depth= 0.35, width= 1.0001, reinfSteelType= SIA262_materials.B500B)
negRebRow=def_simple_RC_section.ReinfRow(nRebars=2,rebarsDiam=10e-10,width=datosScc1LosC.b)
# negRebRow=def_simple_RC_section.ReinfRow()
# negRebRow=def_simple_RC_section.ReinfRow(rebarsDiam=10e-3,areaRebar= SIA262_materials.Fi10,rebarsSpacing=0.2,width=1.0,nominalCover=0.03)
# negRebRow.setUp(nRebars=0,rebarsDiam=0.0,areaRebar=0.0,width=datosScc1LosC.b,cover=0.1)
datosScc1LosC.negatvRebarRows= def_simple_RC_section.LongReinfLayers([negRebRow])
posRebRow=def_simple_RC_section.ReinfRow(nRebars=6,areaRebar=(areaFi22+areaFi26)/2.0,width=datosScc1LosC.b,nominalCover=0.05,nominalLatCover=0.08333-26e-3/2.)
# posRebRow=def_simple_RC_section.ReinfRow()
# posRebRow=def_simple_RC_section.ReinfRow(rebarsDiam=10e-3,areaRebar= SIA262_materials.Fi10,rebarsSpacing=0.2,width=1.0,nominalCover=0.03)
# posRebRow.setUp(nRebars=6,rebarsDiam=26e-3,areaRebar=(areaFi22+areaFi26)/2.0,width=datosScc1LosC.b,cover=0.05+0.026/2.0)
datosScc1LosC.positvRebarRows= def_simple_RC_section.LongReinfLayers([posRebRow])

# datosScc1LosC.negatvRebars.setUp(0,0.0,0.0,datosScc1LosC.b,0.1)
# datosScc1LosC.positvRebars.setUp(6,26e-3,(areaFi22+areaFi26)/2.0,datosScc1LosC.b,0.05+0.026/2.0)



NDato= 0 # Axial force for crack control checking.
MyDato= 117e3 # Bending moment force for crack control checking.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
# concreteMatTag= SIA262_materials.HA30.defDiagK(preprocessor)
# reinfSteelMaterialTag= SIA262_materials.B500S.defDiagK(preprocessor)


fiberSectionName= datosScc1LosC.defRCSection(preprocessor, "k")

zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, fiberSectionName)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_0F0(nodB.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag,xc.Vector([NDato,0,0,0,MyDato,3.55271e-15]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)



# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(10)


secHAParamsFis= SIA262_limit_state_checking.CrackControlSIA262('SLS_crack',400e6)


scc= zlElement.getSection()
sigma_s= secHAParamsFis.calcRebarStress(scc)


ratio1= (sigma_s-166.35650302345326e6)/166.35650302345326e6

'''
datosScc1LosC.plot(preprocessor) # Display section geometry
print("sigma_s= ",sigma_s/1e6, " MPa")
print("ratio1= ",ratio1) 
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
