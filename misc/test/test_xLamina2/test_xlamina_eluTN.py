# -*- coding: utf-8 -*-


from materials.xLamina import modelo
#from materials.xLamina import postproceso_xLamina
from materials.xLamina import calculo_tn
from solution import resuelve_combinacion
from postprocess.reports import listados_factor_capacidad



import xc_base
import geom
import xc
from materials.ehe import hormigonesEHE
from materials.ehe import acerosEHE
import defSectionsXLamina
from solution import predefined_solutions

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
prueba.errFileName= "/tmp/borrar.err" # Para no imprimir mensajes de error.
mdlr= prueba.getModelador
# Definimos el procedimiento de solución.
analysis= predefined_solutions.simple_static_linear(prueba)
# Definimos materiales
tagHA30= hormigonesEHE.HA30.defDiagD(mdlr)
Ec= hormigonesEHE.HA30.getDiagD(mdlr).getTangent
tagB500S= acerosEHE.B500S.defDiagD(mdlr)
Es= acerosEHE.B500S.getDiagD(mdlr).getTangent
scc1= defSections.getDiagIntSection1(mdlr)
scc2= defSections.getDiagIntSection2(mdlr)

meanFCs= calculo_tn.lanzaCalculoTNFromXCData(mdlr,analysis,"esf_test_xLamina.csv","/tmp/ppTN",scc1,scc2)

#print "mean FCs: ", meanFCs

ratio1= abs(meanFCs[0]-0.84660274501497856)/0.84660274501497856
ratio2= abs(meanFCs[1]-0.64702580108264973)/0.64702580108264973

import os
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
