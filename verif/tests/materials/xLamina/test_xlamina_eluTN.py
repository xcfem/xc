# -*- coding: utf-8 -*-


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.xLamina import modelo
#from materials.xLamina import postproceso_xLamina
from solution import resuelve_combinacion
from postprocess.reports import listados_factor_capacidad



import xc_base
import geom
import xc
from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel
from materials.fiber_section import defSeccionHASimple
from materials.xLamina import membranePlateRCSectionContainer as sc
from solution import predefined_solutions

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
prueba.errFileName= "/tmp/borrar.err" # Para no imprimir mensajes de advertencia sobre error máximo en cálculo del diagrama de interacción.
preprocessor=  prueba.getPreprocessor
# Definimos el procedimiento de solución.
analysis= predefined_solutions.simple_static_linear(prueba)

import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
csvFile= pth+"/esf_test_xLamina.csv"


elementTags= [2524,2527]
mapSectionsForEveryElement= {}
for eTag in elementTags:
  mapSectionsForEveryElement[eTag]= ["deck2","deck1"]

# deck.
concrete= EHE_concrete.HA30
concrete.alfacc=0.85  #coef. de fatiga del hormigón (en general alfacc=1)
reinfSteel= EHE_reinforcing_steel.B500S
areaFi8= 0.50e-4 #XXX Área de las barras expresado en metros cuadrados.
areaFi10=0.785e-4
areaFi12=1.13e-4 
areaFi16= 2.01e-4
areaFi20= 3.14e-4
areaFi25= 4.608e-4
basicCover= 0.06
numReinfBarsT= 5
sepT= 1.0/numReinfBarsT
numReinfBarsL= 7
sepL= 1.0/numReinfBarsL

deckSections= defSeccionHASimple.RecordRCSlabSection("deck","RC deck.",0.3,concrete, reinfSteel,basicCover)
deckSections.setMainReinf2pos(12e-3,areaFi12,sepT)
deckSections.setMainReinf2neg(12e-3,areaFi12,sepT)
deckSections.setMainReinf1neg(20e-3,areaFi20,sepL)
deckSections.setMainReinf1pos(20e-3,areaFi20,sepL)

sections= sc.SectionContainer()
sections.append(deckSections)



meanFCs= sections.verifyNormalStresses(preprocessor, analysis,csvFile,"/tmp/ppTN",mapSectionsForEveryElement, "d")


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
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
