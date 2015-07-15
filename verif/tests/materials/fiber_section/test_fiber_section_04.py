# -*- coding: utf-8 -*-
''' Test de funcionamiento de una sección de hormigón armado.
   Elaboración propia. '''

# Macros
import math
import xc_base
import geom
import xc
from materials.ehe import auxEHE

nmbHorm= "HA25"
from materials.ehe import hormigonesEHE
from materials.ehe import EHE_reinforcing_steel
from materials import reinforcingSteelTest

# Coeficientes de seguridad.
gammac= 1.5 # Coeficiente de minoración de la resistencia del hormigón.
gammas= 1.15 # Coeficiente de minoración de la resistencia del acero.

ancho= 0.2 # Ancho de la sección expresado en metros.
canto= 0.4 # Ancho de la sección expresado en metros.
recub= 0.05 # Recubrimiento de la sección expresado en metros.
diam= 16e-3 # Diámetro de las barras expresado en metros.
areaFi16= 2.01e-4 # Área de las barras expresado en metros cuadrados.

e1= -10e-3 # Deformación axil de prueba 1.
sgc1= hormigonesEHE.sigmac(e1,hormigonesEHE.HA25.fcd())
sgs1= reinforcingSteelTest.sigmaDAceroArmar(e1,EHE_reinforcing_steel.B500S)

e2= -3e-3 # Deformación axil de prueba 1.
sgc2= hormigonesEHE.sigmac(e2,hormigonesEHE.HA25.fcd())
sgs2= reinforcingSteelTest.sigmaDAceroArmar(e2,EHE_reinforcing_steel.B500S)

e3= -1e-3 # Deformación axil de prueba 1.
sgc3= hormigonesEHE.sigmac(e3,hormigonesEHE.HA25.fcd())
sgs3= reinforcingSteelTest.sigmaDAceroArmar(e3,EHE_reinforcing_steel.B500S)

e4= 1e-3 # Deformación axil de prueba 1.
sgc4= hormigonesEHE.sigmac(e4,hormigonesEHE.HA25.fcd())
sgs4= reinforcingSteelTest.sigmaDAceroArmar(e4,EHE_reinforcing_steel.B500S)

e5= 3e-3 # Deformación axil de prueba 1.
sgc5= hormigonesEHE.sigmac(e5,hormigonesEHE.HA25.fcd())
sgs5= reinforcingSteelTest.sigmaDAceroArmar(e5,EHE_reinforcing_steel.B500S)

e6= 10e-3 # Deformación axil de prueba 1.
sgc6= hormigonesEHE.sigmac(e6,hormigonesEHE.HA25.fcd())
sgs6= reinforcingSteelTest.sigmaDAceroArmar(e6,EHE_reinforcing_steel.B500S)

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Don't print warnings.

mdlr= prueba.getModelador
# Definimos materiales
tag= hormigonesEHE.HA25.defDiagD(mdlr)
dgDHA25= hormigonesEHE.HA25.getDiagD(mdlr)
Ec= dgDHA25.getTangent

tag= EHE_reinforcing_steel.B500S.defDiagD(mdlr)
dgDB500S= EHE_reinforcing_steel.B500S.getDiagD(mdlr)
#Es= dgDB500S.getTangent

geomSecHA= mdlr.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
rg= regiones.newQuadRegion(hormigonesEHE.HA25.nmbDiagD)
rg.nDivIJ= 10
rg.nDivJK= 10
rg.pMin= geom.Pos2d(-canto/2,-ancho/2)
rg.pMax= geom.Pos2d(canto/2,ancho/2)
armaduras= geomSecHA.getReinfLayers
armaduraInf= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraInf.numReinfBars= 2
armaduraInf.barDiam= 16e-3
armaduraInf.barArea= areaFi16
armaduraInf.p1= geom.Pos2d(recub-canto/2,ancho/2-recub) # Armadura inferior.
armaduraInf.p2= geom.Pos2d(recub-canto/2,recub-ancho/2)
armaduraSup= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraSup.numReinfBars= 2
armaduraSup.barDiam= 16e-3
armaduraSup.barArea= areaFi16
armaduraSup.p1= geom.Pos2d(canto/2-recub,ancho/2-recub) # Armadura superior.
armaduraSup.p2= geom.Pos2d(canto/2-recub,recub-ancho/2)

materiales= mdlr.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()

secHA.setTrialSectionDeformation(xc.Vector([e1,0,0]))
secHA.commitState()
P1= secHA.getStressResultantComponent("N")

secHA.revertToStart()
secHA.setTrialSectionDeformation(xc.Vector([e2,0,0]))
secHA.commitState()
P2= secHA.getStressResultantComponent("N")

secHA.revertToStart()
secHA.setTrialSectionDeformation(xc.Vector([e3,0,0]))
secHA.commitState()
P3= secHA.getStressResultantComponent("N")

secHA.revertToStart()
secHA.setTrialSectionDeformation(xc.Vector([e4,0,0]))
secHA.commitState()
P4= secHA.getStressResultantComponent("N")

secHA.revertToStart()
secHA.setTrialSectionDeformation(xc.Vector([e5,0,0]))
secHA.commitState()
P5= secHA.getStressResultantComponent("N")

secHA.revertToStart()
secHA.setTrialSectionDeformation(xc.Vector([e6,0,0]))
secHA.commitState()
P6= secHA.getStressResultantComponent("N")


P1teor= sgc1*ancho*canto+sgs1*4.0*areaFi16
ratio1= (P1-P1teor)/P1teor
P2teor= sgc2*ancho*canto+sgs2*4.0*areaFi16
ratio2= (P2-P2teor)/P2teor
P3teor= sgc3*ancho*canto+sgs3*4.0*areaFi16
ratio3= (P3-P3teor)/P3teor
P4teor= sgc4*ancho*canto+sgs4*4.0*areaFi16
ratio4= (P4-P4teor)/P4teor
P5teor= sgc5*ancho*canto+sgs5*4.0*areaFi16
ratio5= (P5-P5teor)/P5teor
P6teor= sgc6*ancho*canto+sgs6*4.0*areaFi16
ratio6= (P6-P6teor)/P6teor

''' 
print "sgc1= ",(sgc1/1e6),"MPa \n"
print "sgs1= ",(sgs1/1e6),"MPa \n"
print "Axil teórico; P1teor= ",(P1teor/1e3)," kN \n"
print "Axil calculado; P1= ",(P1/1e3)," kN \n"
print "ratio1= ",(ratio1)

print "sgc2= ",(sgc2/1e6),"MPa \n"
print "sgs2= ",(sgs2/1e6),"MPa \n"
print "Axil teórico; P2teor= ",(P2teor/1e3)," kN \n"
print "Axil calculado; P2= ",(P2/1e3)," kN \n"
print "ratio2= ",(ratio2)

print "sgc3= ",(sgc3/1e6),"MPa \n"
print "sgs3= ",(sgs3/1e6),"MPa \n"
print "Axil teórico; P3teor= ",(P3teor/1e3)," kN \n"
print "Axil calculado; P3= ",(P3/1e3)," kN \n"
print "ratio3= ",(ratio3)

print "sgc4= ",(sgc4/1e6),"MPa \n"
print "sgs4= ",(sgs4/1e6),"MPa \n"
print "Axil teórico; P4teor= ",(P4teor/1e3)," kN \n"
print "Axil calculado; P4= ",(P4/1e3)," kN \n"
print "ratio4= ",(ratio4)

print "sgc5= ",(sgc5/1e6),"MPa \n"
print "sgs5= ",(sgs5/1e6),"MPa \n"
print "Axil teórico; P5teor= ",(P5teor/1e3)," kN \n"
print "Axil calculado; P5= ",(P5/1e3)," kN \n"
print "ratio5= ",(ratio5)

print "sgc6= ",(sgc6/1e6),"MPa \n"
print "sgs6= ",(sgs6/1e6),"MPa \n"
print "Axil teórico; P6teor= ",(P6teor/1e3)," kN \n"
print "Axil calculado; P6= ",(P6/1e3)," kN \n"
print "ratio6= ",(ratio6)
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<5e-3) & (abs(ratio2)<5e-3) & (abs(ratio3)<5e-3) & (abs(ratio4)<5e-3) & (abs(ratio5)<5e-3) & (abs(ratio6)<5e-3) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
