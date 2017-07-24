# -*- coding: utf-8 -*-
''' Reinforced concrete section. Crack control verification test.
   results are compared with those of the «Prontuario informático del hormigón armado». '''
import xc_base
import geom
import xc

from misc import banco_pruebas_scc3d
from solution import predefined_solutions # Solution procedure
from materials.fiber_section import defSeccionHASimple

from materials.ehe import EHE_materials
from materials.ehe import fisuracionEHE
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

datosScc1LosC= defSeccionHASimple.RecordRCSimpleSection()
datosScc1LosC.sectionName= "secHA1LosC"
datosScc1LosC.sectionDescr= "Losa. Tramo Central. Sección normal al eje X."
datosScc1LosC.concrType= EHE_materials.HA25
datosScc1LosC.depth= 0.25
datosScc1LosC.width= 1.0
datosScc1LosC.reinfSteelType= EHE_materials.B500S
rebNeg=defSeccionHASimple.MainReinfLayer()
rebNeg.setUp(nRebars= 5, areaRebar= EHE_materials.Fi10,cover=0.025+0.010+0.010/2.0)
datosScc1LosC.negatvRebarRows=[rebNeg]
rebPos=defSeccionHASimple.MainReinfLayer()
rebNeg.setUp(nRebars= 5, areaRebar= EHE_materials.Fi10,cover=0.025+0.010/2.0)
datosScc1LosC.positvRebarRows=[rebPos]

# datosScc1LosC.nRebarsNeg= 5
# datosScc1LosC.areaRebarNeg= EHE_materials.Fi10
# datosScc1LosC.coverNeg= 0.025+0.010+0.010/2.0
# datosScc1LosC.nRebarsPos= 5
# datosScc1LosC.areaRebarPos= EHE_materials.Fi10
# datosScc1LosC.coverPos= 0.025+0.010/2.0



NDato= 0 # Axial force for crack control checking.
MyDato= 1000 # Bending moment force for crack control checking.

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
concreteMatTag= EHE_materials.HA25.defDiagK(preprocessor)
matTagAceroArmar= EHE_materials.B500S.defDiagK(preprocessor)


datosScc1LosC.defRCSimpleSection(preprocessor, "k")

banco_pruebas_scc3d.sectionModel(preprocessor, datosScc1LosC.sectionName)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_0F0(2)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,MyDato,3.55271e-15]))

#We add the load case to domain.
casos.addToDomain("0")



# Solution procedure
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(10)


secHAParamsFis= fisuracionEHE.CrackControl('SLS_crack')


elements= preprocessor.getElementLoader
ele1= elements.getElement(1)
scc= ele1.getSection()
secHAParamsFis.calcApertCaracFis(scc,EHE_materials.HA25.matTagK,EHE_materials.B500S.matTagK,EHE_materials.HA25.fctm())


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
