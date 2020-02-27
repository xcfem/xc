# -*- coding: utf-8 -*-
''' Reinforced concrete section. Crack control verification test.
   results are compared with those of the «Prontuario informático del hormigón armado». '''
import xc_base
import geom
import xc

from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure
from materials.sections.fiber_section import def_simple_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

datosScc1LosC= def_simple_RC_section.RecordRCSimpleSection()
datosScc1LosC.sectionName= "secHA1LosC"
datosScc1LosC.sectionDescr= "Losa. Central portion. Section normal to X axis."
datosScc1LosC.concrType= EHE_materials.HA25
datosScc1LosC.depth= 0.25
datosScc1LosC.width= 1.0
datosScc1LosC.reinfSteelType= EHE_materials.B500S
rebNeg=def_simple_RC_section.MainReinfLayer(nRebars= 5, areaRebar= EHE_materials.Fi10,nominalCover=0.025+0.010,width=0.999)
datosScc1LosC.negatvRebarRows=[rebNeg]
rebPos=def_simple_RC_section.MainReinfLayer(nRebars= 5, areaRebar= EHE_materials.Fi10,nominalCover=0.025,width=0.999)
datosScc1LosC.positvRebarRows=[rebPos]

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
analisis= predefined_solutions.simple_newton_raphson(feProblem)
analOk= analisis.analyze(10)


secHAParamsFis= EHE_limit_state_checking.CrackControl('SLS_crack')


elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
scc= ele1.getSection()
secHAParamsFis.computeWk(scc,EHE_materials.HA25.matTagK,EHE_materials.B500S.matTagK,EHE_materials.HA25.fctm())


ratio1= secHAParamsFis.Wk


''' 
    print "ratio1= ",ratio1
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
