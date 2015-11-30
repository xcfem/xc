# -*- coding: utf-8 -*-
''' Test de funcionamiento de la comprobación a fisuración de una sección de hormigón armado.
   Los resultados se comparan con los obtenidos del «Prontuario informático del hormigón armado». '''

import xc_base
import geom
import xc
# Macros
from materials.ehe import auxEHE
from misc import banco_pruebas_scc3d
from solution import predefined_solutions # Procedimiento de solución
from materials.fiber_section import defSeccionHASimple
from materials.ehe import areaBarrasEHE
from materials import parametrosSeccionRectangular


from materials.ehe import hormigonesEHE
from materials.ehe import EHE_reinforcing_steel
from materials.sia262 import steelSIA262
from materials.sia262 import crackControlSIA262 as cc
from model import fix_node_6dof

areaFi22= steelSIA262.section_barres_courantes[22e-3]
areaFi26= steelSIA262.section_barres_courantes[26e-3]

datosScc1LosC= defSeccionHASimple.RecordSeccionHASimple()
datosScc1LosC.nmbSeccion= "secHA1LosC"
datosScc1LosC.descSeccion= "Losa. Tramo Central. Sección normal al eje X."
datosScc1LosC.tipoHormigon= hormigonesEHE.HA30
datosScc1LosC.canto= 0.35
datosScc1LosC.ancho= 1.0
datosScc1LosC.tipoArmadura= EHE_reinforcing_steel.B500S
datosScc1LosC.barrasNeg.setUp(0,0.0,0.0,datosScc1LosC.ancho,0.1)
datosScc1LosC.barrasPos.setUp(6,26e-3,(areaFi22+areaFi26)/2.0,datosScc1LosC.ancho,0.05+0.026/2.0)



NDato= 0 # Axil para comprobar fisuración.
MyDato= 117e3 # Momento para comprobar fisuración.

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
# tagDiagHormigon= hormigonesEHE.HA30.defDiagK(preprocessor)
# tagDiagAceroArmar= EHE_reinforcing_steel.B500S.defDiagK(preprocessor)


datosScc1LosC.defSeccionHASimple(preprocessor, "k")

banco_pruebas_scc3d.modeloSecc3d(preprocessor, datosScc1LosC.nmbSeccion)

# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.Nodo6DOFMovXGiroYLibres(coacciones,2)

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


secHAParamsFis= cc.CrackControlSIA262()


elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
sigma_s= secHAParamsFis.calcRebarStress(scc)


ratio1= (sigma_s-168.951255239e6)/168.951255239e6


''' 
print "sigma_s= ",sigma_s/1e6, " MPa"
print "ratio1= ",ratio1
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
