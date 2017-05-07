# -*- coding: utf-8 -*-
''' Home-made Verification test of la comprobación a fisuración de una sección de hormigón armado.
'''

import xc_base
import geom
import xc

from misc import banco_pruebas_scc3d
from solution import predefined_solutions # Procedimiento de solución
from materials.fiber_section import defSeccionHASimple
from materials.ehe import areaBarrasEHE
# from materials import paramRectangularSection


from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel
from materials.sia262 import steelSIA262
from materials.sia262 import crackControlSIA262 as cc
from model import fix_node_6dof

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

areaFi22= steelSIA262.section_barres_courantes[22e-3]
areaFi26= steelSIA262.section_barres_courantes[26e-3]

datosScc1LosC= defSeccionHASimple.RecordRCSimpleSection()
datosScc1LosC.sectionName= "secHA1LosC"
datosScc1LosC.sectionDescr= "Losa. Tramo Central. Sección normal al eje X."
concr=EHE_concrete.HA30
concr.alfacc=0.85 #f_maxd= 0.85*fcd
datosScc1LosC.concrType= concr
datosScc1LosC.depth= 0.35
datosScc1LosC.width= 1.0
datosScc1LosC.reinfSteelType= EHE_reinforcing_steel.B500S
negRebRow=defSeccionHASimple.MainReinfLayer()
negRebRow.setUp(nRebars=0,rebarsDiam=0.0,areaRebar=0.0,width=datosScc1LosC.width,cover=0.1)
datosScc1LosC.negatvRebarRows=[negRebRow]
posRebRow=defSeccionHASimple.MainReinfLayer()
posRebRow.setUp(nRebars=6,rebarsDiam=26e-3,areaRebar=(areaFi22+areaFi26)/2.0,width=datosScc1LosC.width,cover=0.05+0.026/2.0)
datosScc1LosC.positvRebarRows=[posRebRow]

#datosScc1LosC.negatvRebars.setUp(0,0.0,0.0,datosScc1LosC.width,0.1)
#datosScc1LosC.positvRebars.setUp(6,26e-3,(areaFi22+areaFi26)/2.0,datosScc1LosC.width,0.05+0.026/2.0)



NDato= 0 # Axil para comprobar fisuración.
MyDato= 117e3 # Momento para comprobar fisuración.

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
# matTagHormigon= EHE_concrete.HA30.defDiagK(preprocessor)
# matTagAceroArmar= EHE_reinforcing_steel.B500S.defDiagK(preprocessor)


datosScc1LosC.defRCSimpleSection(preprocessor, "k")

banco_pruebas_scc3d.modeloSecc3d(preprocessor, datosScc1LosC.sectionName)

# Constraints
constraints= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(constraints,1)
fix_node_6dof.Nodo6DOFMovXGiroYLibres(constraints,2)

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



# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(10)


secHAParamsFis= cc.CrackControlSIA262('SLS_crack',400e6)


elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
sigma_s= secHAParamsFis.calcRebarStress(scc)


ratio1= (sigma_s-166.925177095e6)/166.925177095


''' 
print "sigma_s= ",sigma_s/1e6, " MPa"
print "ratio1= ",ratio1
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
