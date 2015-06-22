# -*- coding: utf-8 -*-
''' Test de funcionamiento del cálculo del diagrama de interacción.
   Elaboración propia. '''

# Macros
import xc_base
import geom
import xc
from materials.ehe import auxEHE

nmbHorm= "HA25"
from materials.ehe import hormigonesEHE
from materials.ehe import EHE_reinforcing_steel

# Coeficientes de seguridad.
gammac= 1.5 # Coeficiente de minoración de la resistencia del hormigón.
gammas= 1.15 # Coeficiente de minoración de la resistencia del acero.

ancho= 0.5 # Ancho de la sección expresado en metros.
canto= 0.75 # Ancho de la sección expresado en metros.
recub= 0.06 # Recubrimiento de la sección expresado en metros.
diam= 20e-3 # Diámetro de las barras expresado en metros.
areaFi20= 3.14e-4 # Área de las barras expresado en metros cuadrados.


prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
# Definimos materiales
tagHA25= hormigonesEHE.HA25.defDiagD(mdlr)
Ec= hormigonesEHE.HA25.getDiagD(mdlr).getTangent
tagB500S= EHE_reinforcing_steel.B500S.defDiagD(mdlr)
Es= EHE_reinforcing_steel.B500S.getDiagD(mdlr).getTangent

geomSecHA= mdlr.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
hormigon= regiones.newQuadRegion(hormigonesEHE.HA25.nmbDiagD)
hormigon.nDivIJ= 10
hormigon.nDivJK= 10
hormigon.pMin= geom.Pos2d(-canto/2.0,-ancho/2.0)
hormigon.pMax= geom.Pos2d(canto/2.0,ancho/2.0)
armaduras= geomSecHA.getReinfLayers
armaduraInf= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraInf.numReinfBars= 6
armaduraInf.barArea= areaFi20
armaduraInf.p1= geom.Pos2d(recub-canto/2.0,ancho/2.0-recub) # Armadura inferior.
armaduraInf.p2= geom.Pos2d(recub-canto/2.0,recub-ancho/2.0)
armaduraPielInf= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraPielInf.numReinfBars= 2
armaduraPielInf.barArea= areaFi20
y= (canto-2*recub)/3.0/2.0
armaduraPielInf.p1= geom.Pos2d(-y,ancho/2-recub) # # Armadura piel inferior.
armaduraPielInf.p2= geom.Pos2d(-y,recub-ancho/2)
armaduraPielSup= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraPielSup.numReinfBars= 2
armaduraPielSup.barArea= areaFi20
y= (canto-2*recub)/3.0/2.0
armaduraPielSup.p1= geom.Pos2d(y,ancho/2-recub) # # Armadura piel superior.
armaduraPielSup.p2= geom.Pos2d(y,recub-ancho/2)
armaduraSup= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraSup.numReinfBars= 6
armaduraSup.barArea= areaFi20
armaduraSup.p1= geom.Pos2d(canto/2.0-recub,ancho/2.0-recub) # Armadura superior.
armaduraSup.p2= geom.Pos2d(canto/2.0-recub,recub-ancho/2.0)

materiales= mdlr.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()
fibras= secHA.getFibers()

param= xc.InteractionDiagramParameters()
param.tagHormigon= hormigonesEHE.HA25.tagDiagD
param.tagArmadura= EHE_reinforcing_steel.B500S.tagDiagD
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
