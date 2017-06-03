# -*- coding: utf-8 -*-

# ************** DEPRECATED 11/12/2016 ******************

from materials.xLamina import modelo
#from materials.xLamina import postproceso_xLamina
from solution import resuelve_combinacion
from postprocess.reports import listados_factor_capacidad



import xc_base
import geom
import xc
from materials.ehe import hormigonesEHE
from materials.ehe import EHE_reinforcing_steel
from materials.fiber_section import defSeccionHASimple
from materials.xLamina import membranePlateRCSectionContainer as sc
from solution import predefined_solutions
from materials.sia262 import SIA262_reinforcing_steel

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
#prueba.errFileName= "/tmp/borrar.err" # Para no imprimir mensajes de advertencia sobre error máximo en cálculo del diagrama de interacción.
mdlr= prueba.getModelador
# Definimos el procedimiento de solución.
analysis= predefined_solutions.simple_static_linear(prueba)

import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
csvFile= pth+"/esf_test_xLamina02.csv"

areaFi6= SIA262_reinforcing_steel.section_barres_courantes[6e-3]
areaFi8= SIA262_reinforcing_steel.section_barres_courantes[8e-3]
areaFi10= SIA262_reinforcing_steel.section_barres_courantes[10e-3]
areaFi12= SIA262_reinforcing_steel.section_barres_courantes[12e-3]
areaFi14= SIA262_reinforcing_steel.section_barres_courantes[14e-3]
areaFi16= SIA262_reinforcing_steel.section_barres_courantes[16e-3]
areaFi18= SIA262_reinforcing_steel.section_barres_courantes[18e-3]
areaFi20= SIA262_reinforcing_steel.section_barres_courantes[20e-3]
areaFi22= SIA262_reinforcing_steel.section_barres_courantes[22e-3]
areaFi26= SIA262_reinforcing_steel.section_barres_courantes[26e-3]
areaFi30= SIA262_reinforcing_steel.section_barres_courantes[30e-3]
areaFi34= SIA262_reinforcing_steel.section_barres_courantes[34e-3]
areaFi40= SIA262_reinforcing_steel.section_barres_courantes[40e-3]

concrete= hormigonesEHE.HA30
reinfSteel= EHE_reinforcing_steel.B500S

basicCover= 40e-3

elementTags= [3770,3796]
mapSectionsForEveryElement= {}
for eTag in elementTags:
  mapSectionsForEveryElement[eTag]= ["genericPiles1","genericPiles2"]

concrete= hormigonesEHE.HA30
reinfSteel= EHE_reinforcing_steel.B500S

datosGenericPiles= defSeccionHASimple.RecordSeccionHALosa('genericPiles',"pont Grande Eau.",0.5,concrete, reinfSteel,basicCover)
datosGenericPiles.setMainReinf2neg(20e-3,areaFi20,0.15)
datosGenericPiles.setMainReinf2pos(20e-3,areaFi20,0.15)
datosGenericPiles.setMainReinf1neg(20e-3,areaFi30/2.0,0.15)

print "areaFi6= ", areaFi6*1e6, " mm2"
datosGenericPiles.setMainReinf1pos(20e-3,areaFi30/2.0,0.15)
outputFileName= "/tmp/ppTN06" 

# print "areaFi8= ", areaFi8*1e6, " mm2"
# datosGenericPiles.setMainReinf1pos(8e-3,areaFi8,0.15)
# outputFileName= "/tmp/ppTN08" 

# print "areaFi10= ", areaFi10*1e6, " mm2"
# datosGenericPiles.setMainReinf1pos(10e-3,areaFi10,0.15) #XXX Ver qué pasa
# outputFileName= "/tmp/ppTN10" 

# print "areaFi12= ", areaFi12*1e6, " mm2"
# datosGenericPiles.setMainReinf1pos(12e-3,areaFi12,0.15) #XXX Ver qué pasa
# outputFileName= "/tmp/ppTN12" 

sections= sc.SectionContainer()
sections.append(datosGenericPiles)


meanFCs= sections.internalForcesVerification3D(mdlr,analysis,csvFile,outputFileName,mapSectionsForEveryElement, "d")


print "mean FCs: ", meanFCs


# import os
# fname= os.path.basename(__file__)
# if (ratio1<0.01) & (ratio2<0.01):
#   print "test ",fname,": ok."
# else:
#   print "test ",fname,": ERROR."
