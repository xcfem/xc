# -*- coding: utf-8 -*-
''' Test de funcionamiento de la comprobación a cortante de una sección de hormigón armado.
   Algunos resultados se comparan con los obtenidos del ejercicio ER-CA-02 de
   www.areadecalculo.com. Los cálculos se hacen según la norma EHE-08. '''

# Macros
from __future__ import division
import xc_base
import geom
import xc
from materials.ehe import auxEHE
from materials.ehe import areaBarrasEHE
from misc import banco_pruebas_scc3d
from solution import predefined_solutions


from materials.ehe import hormigonesEHE
from materials.ehe import EHE_reinforcing_steel
from materials.ehe import comprobVEHE08
from materials.ehe import cortanteEHE
from materials.ehe import torsionEHE
from materials.ehe import areaBarrasEHE
from materials import typical_materials

from model import fix_node_6dof
import math

# Coeficientes de seguridad.
gammac= 1.5 # Coeficiente de minoración de la resistencia del hormigón.
gammas= 1.15 # Coeficiente de minoración de la resistencia del acero.

ancho= 0.4 # Ancho de la sección expresado en metros.
canto= 0.6 # Canto de la sección expresado en metros.
recub= 0.05 # Recubrimiento de la sección expresado en metros.

NDato= 0 # Axil para comprobar el cortante.
MyDato= 1.9606e5 # Momento para comprobar el cortante su valor se fuerza para obtener Vcu aproximadamente igual a 82.607 kN.
MzDato= 0 # Momento para comprobar el cortante.
VDato= 125e3 # Momento para comprobar el cortante.

# Armadura longitudinal
areaBarra= areaBarrasEHE.Fi20
numBarras= 3

# Armadura transversal
diamATrsv= 6e-3
numRamas= 4

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
# Materials definition
materiales= mdlr.getMaterialLoader

HA30= hormigonesEHE.HA30
tagHA30= HA30.defDiagD(mdlr)
B500S= EHE_reinforcing_steel.B500S
tagB500S= B500S.defDiagD(mdlr)

respT= typical_materials.defElasticMaterial(mdlr,"respT",1e10) # Respuesta de la sección a torsión.
respVy= typical_materials.defElasticMaterial(mdlr,"respVy",1e6) # Respuesta de la sección a cortante según y.
respVz= typical_materials.defElasticMaterial(mdlr,"respVz",1e3) # Respuesta de la sección a cortante según y.
geomSecHA= mdlr.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
rg= regiones.newQuadRegion(hormigonesEHE.HA30.nmbDiagD)
rg.nDivIJ= 10
rg.nDivJK= 10
rg.pMin= geom.Pos2d(-ancho/2.0,-canto/2.0)
rg.pMax= geom.Pos2d(ancho/2.0,canto/2.0)
armaduras= geomSecHA.getReinfLayers
armaduraInf= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraInf.numReinfBars= numBarras
armaduraInf.barArea= areaBarra
armaduraInf.p1= geom.Pos2d(recub-ancho/2.0,recub-canto/2.0) # Armadura inferior.
armaduraInf.p2= geom.Pos2d(ancho/2.0-recub,recub-canto/2.0)
armaduraSup= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraSup.numReinfBars= numBarras
armaduraSup.barArea= areaBarra
armaduraSup.p1= geom.Pos2d(recub-ancho/2.0,canto/2.0-recub) # Armadura superior.
armaduraSup.p2= geom.Pos2d(ancho/2.0-recub,canto/2.0-recub)

secHA= materiales.newMaterial("fiberSectionShear3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()
secHA.setRespVyByName("respVy")
secHA.setRespVzByName("respVz")
secHA.setRespTByName("respT")

banco_pruebas_scc3d.modeloSecc3d(mdlr,"secHA")

# Constraints
coacciones= mdlr.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)

# Loads definition
cargas= mdlr.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,VDato,0,MyDato,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(10)


secHAParamsCortante= cortanteEHE.ParamsCortante()

secHAParamsCortante.AsTrsv= areaBarrasEHE.Fi6*numRamas/0.2 # Área de la armadura transversal
secHAParamsCortante.theta= math.radians(45)
secHAParamsCortante.alpha= math.radians(90)


secHAParamsTorsion= torsionEHE.calcParamsSeccionHuecaEficaz(geomSecHA,canto/2.0,recub)


elementos= mdlr.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
N= scc.getStressResultantComponent("N")
My= scc.getStressResultantComponent("My")
Vy= scc.getStressResultantComponent("Vy")
Mz= scc.getStressResultantComponent("Mz")
Vz= scc.getStressResultantComponent("Vz")
NTmp= N
MTmp= math.sqrt((My)**2+(Mz)**2)
VTmp= math.sqrt((Vy)**2+(Vz)**2)
TTmp= scc.getStressResultantComponent("Mx")
secHAParamsCortante.calcVuEHE08(mdlr,scc,secHAParamsTorsion,HA30,B500S,NTmp,MTmp,VTmp,TTmp)



Vu1A= secHAParamsCortante.Vu1
Vu1ATeor= 1.40337e6
VcuA= secHAParamsCortante.Vcu
VcuATeor= 82.607e3
VsuA= secHAParamsCortante.Vsu
Vu2A= secHAParamsCortante.Vu2

ratio1= ((Vu1A-Vu1ATeor)/Vu1ATeor)
ratio2= ((VcuA-VcuATeor)/VcuATeor)
ratio3= ((VsuA-111.966e3)/111.966e3)
ratio4= ((Vu2A-194.574e3)/194.574e3)

'''
print "Vu1A= ",Vu1A/1e3," kN"
print "Vu1ATeor= ",Vu1ATeor/1e3," kN"
print "ratio1= ",ratio1
print "VcuA= ",VcuA/1e3," kN"
print "VcuATeor= ",VcuATeor/1e3," kN"
print "ratio2= ",ratio2
print "VsuA= ",VsuA/1e3," kN"
print "Vu2A= ",Vu2A/1e3," kN"
print "ratio3= ",ratio3
print "ratio4= ",ratio4
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-4) & (abs(ratio4)<1e-4):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
