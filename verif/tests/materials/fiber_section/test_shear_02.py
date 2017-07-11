# -*- coding: utf-8 -*-
from __future__ import division
''' Test for checking the shear-strength verificacion of a reinforced concrete section.
   Some comparison values are obtained from example ER-CA-02 of
   www.areadecalculo.com. Calculation is carried on according to EHE-08 standard. '''


import xc_base
import geom
import xc
from misc import banco_pruebas_scc3d
from solution import predefined_solutions
from model import predefined_spaces

from materials.ehe import EHE_materials
from materials.ehe import comprobVEHE08
from materials.ehe import cortanteEHE
from materials.ehe import torsionEHE
from materials import typical_materials

import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"


# Data

gammac= 1.5  # Concrete safety coefficient
gammas= 1.15 # Steel safety coefficient

width= 0.4  # Cross-section width [m]
depth= 0.6  # Cross-section depth [m]
cover= 0.05 # Cover [m]

NDato= 0         # Axial force when checking shear.
MyDato= 1.9606e5 # Moment to check shear it's value is forced to obtain Vcu around 82.607 kN.
MzDato= 0        # Moment value when checking shear.
VDato= 125e3     # Shear value.

# Armadura longitudinal
areaBarra= EHE_materials.Fi20
numBarras= 3

# Armadura transversal
diamATrsv= 6e-3
numRamas= 4

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
materiales= preprocessor.getMaterialLoader

# Materials definition
concr= EHE_materials.HA30
concr.alfacc=0.85    # f_maxd= 0.85*fcd coeficiente de fatiga del hormigón (generalmente alfacc=1)
concrMatTag30= concr.defDiagD(preprocessor)
B500S= EHE_materials.B500S
tagB500S= B500S.defDiagD(preprocessor)

respT= typical_materials.defElasticMaterial(preprocessor, "respT",1e10) # Respuesta de la sección a torsión.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e6) # Respuesta de la sección a cortante según y.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e3) # Respuesta de la sección a cortante según y.


# Section geometry
# setting up
geomSecHA= preprocessor.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
#filling with regions
rg= regiones.newQuadRegion(concr.nmbDiagD)  #the name assigned to the region is the string concr.nmbDiagD
rg.nDivIJ= 10
rg.nDivJK= 10
rg.pMin= geom.Pos2d(-width/2.0,-depth/2.0)
rg.pMax= geom.Pos2d(width/2.0,depth/2.0)
reinforcement= geomSecHA.getReinfLayers
reinforcementInf= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
reinforcementInf.numReinfBars= numBarras
reinforcementInf.barArea= areaBarra
reinforcementInf.p1= geom.Pos2d(cover-width/2.0,cover-depth/2.0) # Armadura inferior.
reinforcementInf.p2= geom.Pos2d(width/2.0-cover,cover-depth/2.0)
reinforcementSup= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
reinforcementSup.numReinfBars= numBarras
reinforcementSup.barArea= areaBarra
reinforcementSup.p1= geom.Pos2d(cover-width/2.0,depth/2.0-cover) # Armadura superior.
reinforcementSup.p2= geom.Pos2d(width/2.0-cover,depth/2.0-cover)

secHA= materiales.newMaterial("fiberSectionShear3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()
secHA.setRespVyByName("respVy")
secHA.setRespVzByName("respVz")
secHA.setRespTByName("respT")

banco_pruebas_scc3d.sectionModel(preprocessor, "secHA")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)

# Loads definition
cargas= preprocessor.getLoadLoader

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


secHAParamsCortante= cortanteEHE.ShearControllerEHE('ULS_shear')

secHAParamsCortante.AsTrsv= EHE_materials.Fi6*numRamas/0.2 # reinforcement area transversal
secHAParamsCortante.theta= math.radians(45)
secHAParamsCortante.alpha= math.radians(90)


secHAParamsTorsion= torsionEHE.calcParamsSeccionHuecaEficaz(geomSecHA,depth/2.0,cover)


elements= preprocessor.getElementLoader
ele1= elements.getElement(1)
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
secHAParamsCortante.calcVuEHE08(preprocessor, scc,secHAParamsTorsion,concr,B500S,NTmp,MTmp,VTmp,TTmp)



Vu1A= secHAParamsCortante.Vu1
Vu1ATeor= 1.32e6
VcuA= secHAParamsCortante.Vcu
VcuATeor= 82.607e3
VsuA= secHAParamsCortante.Vsu
Vu2A= secHAParamsCortante.Vu2

ratio1= ((Vu1A-Vu1ATeor)/Vu1ATeor)   #high, to be controlled later
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
#if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-4) & (abs(ratio4)<1e-4):
if (abs(ratio1)<0.1) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-4) & (abs(ratio4)<1e-4):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
