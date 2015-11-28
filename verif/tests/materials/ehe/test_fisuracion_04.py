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

ancho= 1.70 # Ancho de la sección expresado en metros.
canto= 1.10 # Canto de la sección expresado en metros.
recub= 0.05 # Recubrimiento de la sección expresado en metros.
areaFi20=3.14e-4
areaFi25=4.91e-4
areaFi32=8.04e-4

NDato= 0 # Axil para comprobar fisuración.
MyDato= -195.3*9810 # Momento para comprobar fisuración.
MzDato= 0 # Momento para comprobar fisuración.

numBarras= 16
sepBarras= ((ancho-2*recub)/(numBarras-1))
offsetBarras= ((ancho-(numBarras-1)*sepBarras)/2)

''' 
print "sepBarras= ",sepBarras
print "numBarras= ",numBarras
print "offsetBarras= ",offsetBarras
   '''

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
tagHA25= EHE_concrete.HA25.defDiagK(preprocessor)
tagB400S= EHE_reinforcing_steel.B400S.defDiagK(preprocessor)

geomSecHA= preprocessor.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
hormigon= regiones.newQuadRegion(EHE_concrete.HA25.nmbDiagK)
hormigon.nDivIJ= 10
hormigon.nDivJK= 10
hormigon.pMin= geom.Pos2d(-ancho/2.0,-canto/2.0)
hormigon.pMax= geom.Pos2d(ancho/2.0,canto/2.0)
armaduras= geomSecHA.getReinfLayers
armaduraA= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B400S.nmbDiagK)
armaduraA.numReinfBars= numBarras/2
armaduraA.barArea= areaFi20
armaduraA.p1= geom.Pos2d(recub-ancho/2.0,recub-canto/2.0)
armaduraA.p2= geom.Pos2d(ancho/2.0-recub-sepBarras,recub-canto/2.0)
armaduraB= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B400S.nmbDiagK)
armaduraB.numReinfBars= numBarras/2
armaduraB.barArea= areaFi32
armaduraB.p1= geom.Pos2d(recub+sepBarras-ancho/2.0,recub-canto/2.0)
armaduraB.p2= geom.Pos2d(ancho/2.0-recub,recub-canto/2.0)
armaduraC= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B400S.nmbDiagK)
armaduraC.numReinfBars= 13
armaduraC.barArea= areaFi25
armaduraC.p1= geom.Pos2d(recub-ancho/2.0,canto/2.0-recub) # Armadura superior.
armaduraC.p2= geom.Pos2d(ancho/2.0-recub,canto/2.0-recub)

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


secHAParamsFis= fisuracionEHE.ParamsFisuracionEHE()



elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
secHAParamsFis.calcApertCaracFis(scc,EHE_concrete.HA25.tagDiagK,EHE_reinforcing_steel.B400S.tagDiagK,EHE_concrete.HA25.fctm())

ratio1= ((secHAParamsFis.sepBarrasTracc-0.105)/0.105)
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
