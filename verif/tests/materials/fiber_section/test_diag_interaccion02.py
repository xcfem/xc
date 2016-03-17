# -*- coding: utf-8 -*-
''' Test de funcionamiento del cálculo del diagrama de interacción.
   Elaboración propia. '''

# Macros
import xc_base
import geom
import xc

nmbHorm= "HA25"
from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel

# Coeficientes de seguridad.
gammac= 1.5 # Coeficiente de minoración de la resistencia del hormigón.
gammas= 1.15 # Coeficiente de minoración de la resistencia del acero.

width= 0.5 # Ancho de la sección expresado en metros.
depth= 0.75 # Ancho de la sección expresado en metros.
cover= 0.06 # Recubrimiento de la sección expresado en metros.
diam= 20e-3 # Diámetro de las barras expresado en metros.
areaFi20= 3.14e-4 # Área de las barras expresado en metros cuadrados.


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
reinforcementInf.numReinfBars= 6
reinforcementInf.barArea= areaFi20
reinforcementInf.p1= geom.Pos2d(cover-depth/2.0,width/2.0-cover) # Armadura inferior.
reinforcementInf.p2= geom.Pos2d(cover-depth/2.0,cover-width/2.0)
reinforcementPielInf= reinforcement.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
reinforcementPielInf.numReinfBars= 2
reinforcementPielInf.barArea= areaFi20
y= (depth-2*cover)/3.0/2.0
reinforcementPielInf.p1= geom.Pos2d(-y,width/2-cover) # # Armadura piel inferior.
reinforcementPielInf.p2= geom.Pos2d(-y,cover-width/2)
reinforcementPielSup= reinforcement.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
reinforcementPielSup.numReinfBars= 2
reinforcementPielSup.barArea= areaFi20
y= (depth-2*cover)/3.0/2.0
reinforcementPielSup.p1= geom.Pos2d(y,width/2-cover) # # Armadura piel superior.
reinforcementPielSup.p2= geom.Pos2d(y,cover-width/2)
reinforcementSup= reinforcement.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
reinforcementSup.numReinfBars= 6
reinforcementSup.barArea= areaFi20
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

diagIntsecHA.writeTo("/tmp/prueba_diag_interaccion02.dat")  # Se usa en test test_diag_interaccion04
FCs= []
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(2185.5e3,0,0)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(1595.8e3,0,186.5e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(1006.2e3,0,380.2e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(416.6e3,0,565.9e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-173e3,0,739.3e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-762.6e3,0,870.8e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-1352.2e3,0,962.2e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-1941.9e3,0,1004.1e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-2531.5e3,0,1021.9e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-3121.1e3,0,950.7e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-3710.7e3,0,872.1e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-4300.3e3,0,782.8e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-4889.9e3,0,678.1e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-5479.6e3,0,551.8e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-6069.2e3,0,394.6e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-6658.8e3,0,230.7e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-7248.4e3,0,0.0)))

szFCs= float(len(FCs))
sumQuad= 0
for f in FCs:
  sumQuad+= f**2


ratio1= abs(sumQuad-szFCs)/szFCs

''' 
print "FCs= ",FCs
print "sumQuad= ",sumQuad
print "ratio1= ",ratio1
 '''

import os
fname= os.path.basename(__file__)
if(abs(ratio1)<0.02):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
