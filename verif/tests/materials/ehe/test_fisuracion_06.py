# -*- coding: utf-8 -*-
''' Test de funcionamiento de la comprobación a fisuración de una sección de hormigón armado.
   Los resultados se comparan con los obtenidos del «Prontuario informático del hormigón armado». '''

import xc_base
import geom
import xc
# Macros
from misc import banco_pruebas_scc3d
from solution import predefined_solutions # Procedimiento de solución
from materials.fiber_section import defSeccionHASimple
from materials.ehe import areaBarrasEHE
from materials import parametrosSeccionRectangular


from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel
from materials.ehe import fisuracionEHE
from model import fix_node_6dof

datosScc1LosC= defSeccionHASimple.RecordSeccionHASimple()
datosScc1LosC.nmbSeccion= "secHA1LosC"
datosScc1LosC.descSeccion= "Losa. Tramo Central. Sección normal al eje X."
datosScc1LosC.tipoHormigon= EHE_concrete.HA25
datosScc1LosC.canto= 0.25
datosScc1LosC.ancho= 1.0
datosScc1LosC.tipoArmadura= EHE_reinforcing_steel.B500S
datosScc1LosC.nBarrasNeg= 5
datosScc1LosC.areaBarrasNeg= areaBarrasEHE.Fi10
datosScc1LosC.recubNeg= 0.025+0.010+0.010/2.0
datosScc1LosC.nBarrasPos= 5
datosScc1LosC.areaBarrasPos= areaBarrasEHE.Fi10
datosScc1LosC.recubPos= 0.025+0.010/2.0



NDato= 0 # Axil para comprobar fisuración.
MyDato= 1000 # Momento para comprobar fisuración.

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
tagDiagHormigon= EHE_concrete.HA25.defDiagK(preprocessor)
tagDiagAceroArmar= EHE_reinforcing_steel.B500S.defDiagK(preprocessor)


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


secHAParamsFis= fisuracionEHE.ParamsFisuracionEHE()


elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
secHAParamsFis.calcApertCaracFis(scc,EHE_concrete.HA25.tagDiagK,EHE_reinforcing_steel.B500S.tagDiagK,EHE_concrete.HA25.fctm())


ratio1= secHAParamsFis.Wk


''' 
    print "ratio1= ",ratio1
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
