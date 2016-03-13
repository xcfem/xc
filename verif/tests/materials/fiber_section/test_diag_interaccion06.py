# -*- coding: utf-8 -*-
''' Test de funcionamiento del cálculo del diagrama de interacción.
   Elaboración propia. '''
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Macros
import xc_base
import geom
import xc

from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel
import math
from materials.sia262 import steelSIA262
from materials.fiber_section import defSeccionHASimple

areaFi6= steelSIA262.section_barres_courantes[6e-3]
areaFi8= steelSIA262.section_barres_courantes[8e-3]
areaFi10= steelSIA262.section_barres_courantes[10e-3]
areaFi12= steelSIA262.section_barres_courantes[12e-3]
areaFi14= steelSIA262.section_barres_courantes[14e-3]
areaFi16= steelSIA262.section_barres_courantes[16e-3]
areaFi18= steelSIA262.section_barres_courantes[18e-3]
areaFi20= steelSIA262.section_barres_courantes[20e-3]
areaFi22= steelSIA262.section_barres_courantes[22e-3]
areaFi26= steelSIA262.section_barres_courantes[26e-3]
areaFi30= steelSIA262.section_barres_courantes[30e-3]
areaFi34= steelSIA262.section_barres_courantes[34e-3]
areaFi40= steelSIA262.section_barres_courantes[40e-3]

concrete= EHE_concrete.HA30
concrete.alfacc=0.85    #coeficiente de fatiga del hormigón (generalmente alfacc=1)

reinfSteel= EHE_reinforcing_steel.B500S

sccData= defSeccionHASimple.RecordRCSimpleSection()
sccData.sectionName= "sccData"
sccData.sectionDescr= "Prueba."
sccData.concrType= concrete
sccData.depth= 0.5
sccData.width= 1.0
sccData.reinfSteelType= reinfSteel
sccData.negatvRebarRows=[defSeccionHASimple.MainReinfLayer(rebarsDiam=40e-3,areaRebar= areaFi40,rebarsSpacing=0.15,width=1.0,nominalCover=0.25-0.19)]
sccData.positvRebarRows=[defSeccionHASimple.MainReinfLayer(rebarsDiam=6e-3,areaRebar= areaFi6,rebarsSpacing=0.15,width=1.0,nominalCover=0.25-0.19)]
#sccData.setMainReinfNeg(40e-3,areaFi40,0.15,0.25-0.19)
#sccData.setMainReinfPos(6e-3,areaFi6,0.15,0.25-0.19)


prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Don't print warnings.
prueba.errFileName= "/tmp/borrar.err" # Don't print errors.

preprocessor=  prueba.getPreprocessor
sccData.defSeccionHASimple(preprocessor, 'd')
param= xc.InteractionDiagramParameters()
diag= sccData.defInteractionDiagramNMy(preprocessor)
#from materials.fiber_section import plotGeomSeccion as pg
#pg.plotInteractionDiagram2D(diag)



fcELU13= diag.getCapacityFactor(geom.Pos2d(136.78e3,24.71e3))
fcELU14= diag.getCapacityFactor(geom.Pos2d(1197.13e3,65.98e3))

''' Test to be replaced LCPT 02/10/2015'''
ratio1= abs(fcELU13-0.777283365776)
ratio2= abs(fcELU14-4.4411488676)

'''
print "fcELU13= ", fcELU13
print "fcELU14= ", fcELU14
'''


import os
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-2) & (abs(ratio2)<1e-2)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
