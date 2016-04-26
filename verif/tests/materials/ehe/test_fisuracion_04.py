# -*- coding: utf-8 -*-
''' Test de funcionamiento de la comprobación a fisuración de una sección de hormigón armado.
   Los resultados se comparan con los obtenidos del problema 9 del
   libro «El hormigón armado en problemas. Según la norma EHE». '''

import xc_base
import geom
import xc
# Macros
from misc import banco_pruebas_scc3d
from solution import predefined_solutions # Procedimiento de solución


from materials.ehe import EHE_concrete
from materials.ehe import EHE_reinforcing_steel
from materials.ehe import fisuracionEHE
from model import fix_node_6dof

width= 1.70 # Ancho de la sección expresado en metros.
depth= 1.10 # Canto de la sección expresado en metros.
cover= 0.05 # Recubrimiento de la sección expresado en metros.
areaFi20=3.14e-4
areaFi25=4.91e-4
areaFi32=8.04e-4

NDato= 0 # Axil para comprobar fisuración.
MyDato= -195.3*9810 # Momento para comprobar fisuración.
MzDato= 0 # Momento para comprobar fisuración.

numBarras= 16
rebarsSpacing= ((width-2*cover)/(numBarras-1))
offsetBarras= ((width-(numBarras-1)*rebarsSpacing)/2)

''' 
print "rebarsSpacing= ",rebarsSpacing
print "numBarras= ",numBarras
print "offsetBarras= ",offsetBarras
   '''

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
concrMatTag25= EHE_concrete.HA25.defDiagK(preprocessor)
tagB400S= EHE_reinforcing_steel.B400S.defDiagK(preprocessor)

geomSecHA= preprocessor.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
hormigon= regiones.newQuadRegion(EHE_concrete.HA25.nmbDiagK)
hormigon.nDivIJ= 10
hormigon.nDivJK= 10
hormigon.pMin= geom.Pos2d(-width/2.0,-depth/2.0)
hormigon.pMax= geom.Pos2d(width/2.0,depth/2.0)
reinforcement= geomSecHA.getReinfLayers
reinforcementA= reinforcement.newStraightReinfLayer(EHE_reinforcing_steel.B400S.nmbDiagK)
reinforcementA.numReinfBars= numBarras/2
reinforcementA.barArea= areaFi20
reinforcementA.p1= geom.Pos2d(cover-width/2.0,cover-depth/2.0)
reinforcementA.p2= geom.Pos2d(width/2.0-cover-rebarsSpacing,cover-depth/2.0)
reinforcementB= reinforcement.newStraightReinfLayer(EHE_reinforcing_steel.B400S.nmbDiagK)
reinforcementB.numReinfBars= numBarras/2
reinforcementB.barArea= areaFi32
reinforcementB.p1= geom.Pos2d(cover+rebarsSpacing-width/2.0,cover-depth/2.0)
reinforcementB.p2= geom.Pos2d(width/2.0-cover,cover-depth/2.0)
reinforcementC= reinforcement.newStraightReinfLayer(EHE_reinforcing_steel.B400S.nmbDiagK)
reinforcementC.numReinfBars= 13
reinforcementC.barArea= areaFi25
reinforcementC.p1= geom.Pos2d(cover-width/2.0,depth/2.0-cover) # Armadura superior.
reinforcementC.p2= geom.Pos2d(width/2.0-cover,depth/2.0-cover)

materiales= preprocessor.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()

banco_pruebas_scc3d.modeloSecc3d(preprocessor, "secHA")
# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.Nodo6DOFMovXGirosYZLibres(coacciones,2)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,MyDato,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")



# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(10)


secHAParamsFis= fisuracionEHE.CrackControl('SLS_crack')



elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
secHAParamsFis.calcApertCaracFis(scc,EHE_concrete.HA25.matTagK,EHE_reinforcing_steel.B400S.matTagK,EHE_concrete.HA25.fctm())

ratio1= ((secHAParamsFis.rebarsSpacingTracc-0.105)/0.105)
ratio2= ((secHAParamsFis.Wk-0.3e-3)/0.3e-3)


'''
secHAParamsFis.printParams()
print "ratio1= ",ratio1
print "ratio2= ",ratio2
'''


import os
fname= os.path.basename(__file__)
if (abs(ratio1)<0.05) & (abs(ratio2)<0.1):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
