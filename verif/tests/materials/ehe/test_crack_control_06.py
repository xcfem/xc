# -*- coding: utf-8 -*-
''' Reinforced concrete section. Crack control verification test.
   results are compared with those of the «Prontuario informático del hormigón armado». '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure
from materials.sections.fiber_section import def_simple_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from model import predefined_spaces

datosScc1LosC= def_simple_RC_section.RCRectangularSection(name= "secHA1LosC", sectionDescr= "Losa. Central portion. Section normal to X axis.", concrType= EHE_materials.HA25, depth= 0.25, width= 1.0, reinfSteelType= EHE_materials.B500S)
rebNeg=def_simple_RC_section.ReinfRow(nRebars= 5, areaRebar= EHE_materials.Fi10,nominalCover=0.025+0.010,width=0.999)
datosScc1LosC.negatvRebarRows= def_simple_RC_section.LongReinfLayers([rebNeg])
rebPos=def_simple_RC_section.ReinfRow(nRebars= 5, areaRebar= EHE_materials.Fi10,nominalCover=0.025,width=0.999)
datosScc1LosC.positvRebarRows= def_simple_RC_section.LongReinfLayers([rebPos])

# datosScc1LosC.nRebarsNeg= 5
# datosScc1LosC.areaRebarNeg= EHE_materials.Fi10
# datosScc1LosC.coverNeg= 0.025+0.010+0.010/2.0
# datosScc1LosC.nRebarsPos= 5
# datosScc1LosC.areaRebarPos= EHE_materials.Fi10
# datosScc1LosC.coverPos= 0.025+0.010/2.0



NDato= 0 # Axial force for crack control checking.
MyDato= 1000 # Bending moment force for crack control checking.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concreteMatTag= EHE_materials.HA25.defDiagK(preprocessor)
reinfSteelMaterialTag= EHE_materials.B500S.defDiagK(preprocessor)


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


secHAParamsFis= EHE_limit_state_checking.CrackControl('SLS_crack')


scc= zlElement.getSection()
secHAParamsFis.computeWk(scc,EHE_materials.HA25.matTagK,EHE_materials.B500S.matTagK,EHE_materials.HA25.fctm())


ratio1= secHAParamsFis.Wk


''' 
datosScc1LosC.pdfReport(showPDF= True, keepPDF= False)
datosScc1LosC.plot(preprocessor) # Display section geometry
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
