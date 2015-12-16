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

# Coeficientes de seguridad.
gammac= 1.5 # Coeficiente de minoración de la resistencia del hormigón.
gammas= 1.15 # Coeficiente de minoración de la resistencia del acero.

radio= 0.75/2.0 # Radio de la sección expresado en metros.
diam= 20e-3 # Diámetro de las barras expresado en metros.
cover= 0.06+12e-3+diam/2.0 # Recubrimiento de la sección expresado en metros.
areaFi20= math.pi*(diam/2.0)**2 # Área de las barras expresado en metros cuadrados.


prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Definimos materiales
concr=EHE_concrete.HA30
concr.alfacc=0.85    #coeficiente de fatiga del hormigón (generalmente alfacc=1)
concrMatTag30= concr.defDiagD(preprocessor)
Ec= concr.getDiagD(preprocessor).getTangent
tagB500S= EHE_reinforcing_steel.B500S.defDiagD(preprocessor)
Es= EHE_reinforcing_steel.B500S.getDiagD(preprocessor).getTangent

pileGeometry= preprocessor.getMaterialLoader.newSectionGeometry("pileGeometry")
regiones= pileGeometry.getRegions
hormigon= regiones.newCircularRegion(EHE_concrete.HA30.nmbDiagD)
hormigon.nDivCirc= 20
hormigon.nDivRad= 5
hormigon.extRad= radio
hormigon.intRad= 0.0
hormigon.initAngle= 0.0
hormigon.finalAngle= 2*math.pi

armaduras= pileGeometry.getReinfLayers
armadura= armaduras.newCircReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armadura.numReinfBars= 14
armadura.barArea= areaFi20
armadura.initAngle= 0.0
armadura.finalAngle= 2*math.pi
armadura.radius= hormigon.extRad-cover

materiales= preprocessor.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("pileGeometry")
secHA.setupFibers()
fibras= secHA.getFibers()

param= xc.InteractionDiagramParameters()
param.tagHormigon= EHE_concrete.HA30.matTagD
param.tagArmadura= EHE_reinforcing_steel.B500S.matTagD
diagIntsecHA= materiales.calcInteractionDiagram("secHA",param)

fc1= diagIntsecHA.getCapacityFactor(geom.Pos3d(1850e3,0,0))
fc2= diagIntsecHA.getCapacityFactor(geom.Pos3d(-152e3,530e3,0))
fc3= diagIntsecHA.getCapacityFactor(geom.Pos3d(-152e3,0,590e3))
fc4= diagIntsecHA.getCapacityFactor(geom.Pos3d(-9097e3,0.0,0.0))

ratio1= abs(fc1-1)
ratio2= abs(fc2-1)
ratio3= abs(fc3-1.0)
ratio4= abs(fc4-1.0)

''' 
print "fc1= ",fc1
print "ratio1= ",ratio1
print "fc2= ",fc2
print "ratio2= ",ratio2
print "fc3= ",fc3
print "ratio3= ",ratio3
print "fc4= ",fc4
print "ratio4= ",ratio4
 '''

import os
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-2) & (abs(ratio2)<1e-2) & (abs(ratio3)<1e-2) & (abs(ratio4)<1e-2)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
