# -*- coding: utf-8 -*-
''' Test de funcionamiento del cálculo del diagrama de interacción.
   Elaboración propia. '''
from __future__ import division
# Macros
import xc_base
import geom
import xc

from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel

# Coeficientes de seguridad.
gammac= 1.5 # Coeficiente de minoración de la resistencia del hormigón.
gammas= 1.15 # Coeficiente de minoración de la resistencia del acero.

width= 0.2 # Ancho de la sección expresado en metros.
depth= 0.4 # Ancho de la sección expresado en metros.
cover= 0.05 # Recubrimiento de la sección expresado en metros.
diam= 16e-3 # Diámetro de las barras expresado en metros.
areaFi16= 2.01e-4 # Área de las barras expresado en metros cuadrados.


prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Definimos materiales
concr=EHE_concrete.HA25
concr.alfacc=0.85    #coeficiente de fatiga del hormigón (generalmente alfacc=1)
concrMatTag25= concr.defDiagD(preprocessor)
Ec= concr.getDiagD(preprocessor).getTangent
tagB500S= EHE_reinforcing_steel.B500S.defDiagD(preprocessor)
Es= EHE_reinforcing_steel.B500S.getDiagD(preprocessor).getTangent

geomSecHA= preprocessor.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
hormigon= regiones.newQuadRegion(EHE_concrete.HA25.nmbDiagD)
hormigon.nDivIJ= 10
hormigon.nDivJK= 10
hormigon.pMin= geom.Pos2d(-depth/2.0,-width/2.0)
hormigon.pMax= geom.Pos2d(depth/2.0,width/2.0)
reinforcement= geomSecHA.getReinfLayers
reinforcementInf= reinforcement.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
reinforcementInf.numReinfBars= 2
reinforcementInf.barArea= areaFi16
reinforcementInf.p1= geom.Pos2d(cover-depth/2.0,width/2.0-cover) # Armadura inferior.
reinforcementInf.p2= geom.Pos2d(cover-depth/2.0,cover-width/2.0)
reinforcementSup= reinforcement.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
reinforcementSup.numReinfBars= 2
reinforcementSup.barArea= areaFi16
reinforcementSup.p1= geom.Pos2d(depth/2.0-cover,width/2.0-cover) # Armadura superior.
reinforcementSup.p2= geom.Pos2d(depth/2.0-cover,cover-width/2.0)

materiales= preprocessor.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()
fibras= secHA.getFibers()

param= xc.InteractionDiagramParameters()
param.tagHormigon= EHE_concrete.HA25.matTagD
param.tagArmadura= EHE_reinforcing_steel.B500S.matTagD
diagIntsecHA= materiales.calcInteractionDiagram("secHA",param)

diagIntsecHA.writeTo("/tmp/prueba_diag_interaccion01.dat") # Se usa en test test_diag_interaccion03
ratio1= diagIntsecHA.getCapacityFactor(geom.Pos3d(352877,0,0))-1
ratio2= diagIntsecHA.getCapacityFactor(geom.Pos3d(352877/2.0,0,0))-0.5
ratio3= diagIntsecHA.getCapacityFactor(geom.Pos3d(-574457,41505.4,2.00089e-11))-1.0
ratio4= diagIntsecHA.getCapacityFactor(geom.Pos3d(-978599,-10679.4,62804.3))-1.0

''' 
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
 '''

import os
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
