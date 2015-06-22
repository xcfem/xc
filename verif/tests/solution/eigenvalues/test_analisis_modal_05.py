# -*- coding: utf-8 -*-
''' Test para comprobar el análisis modal espectral
tomado de la publicación de Andrés Sáez Pérez: «Estructuras III»
 E.T.S. de Arquitectura de Sevilla (España). En este test empleamos
la biblioteca Arpack. '''
from __future__ import division
import xc_base
import geom
import xc

from model import fix_node_3dof
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math


masaPorPlanta= 134.4e3
matrizMasasNodo= xc.Matrix([[masaPorPlanta,0,0],[0,masaPorPlanta,0],[0,0,0]])
Ehorm= 200000*1e5 # Módulo elástico del hormigón.

Bbaja= 0.45 # Escuadría de los pilares.
Ibaja= 1/12.0*Bbaja**4 # Inercia de la sección.
Hbaja= 4 # Altura de la planta baja.
B1a= 0.40 # Escuadría de los pilares.
I1a= 1/12.0*B1a**4 # Inercia de la sección.
H= 3 # Altura del resto de plantas.
B3a= 0.35 # Escuadría de los pilares.
I3a= 1/12.0*B3a**4 # Inercia de la sección.


kPlBaja= 20*12*Ehorm*Ibaja/(Hbaja**3)
kPl1a= 20*12*Ehorm*I1a/(H**3)
kPl2a= kPl1a
kPl3a= 20*12*Ehorm*I3a/(H**3)
kPl4a= kPl3a

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales2D(nodos)
nodos.defaultTag= 0
nod= nodos.newNodeXY(0,0)
nod.mass= matrizMasasNodo
nod.setProp("gdlsCoartados",xc.ID([0,1,2]))
nod= nodos.newNodeXY(0,4)
nod.mass= matrizMasasNodo
nod.setProp("gdlsCoartados",xc.ID([1,2]))
nod= nodos.newNodeXY(0,4+3)
nod.mass= matrizMasasNodo
nod.setProp("gdlsCoartados",xc.ID([1,2]))
nod= nodos.newNodeXY(0,4+3+3)
nod.mass= matrizMasasNodo
nod.setProp("gdlsCoartados",xc.ID([1,2]))
nod= nodos.newNodeXY(0,4+3+3+3)
nod.mass= matrizMasasNodo
nod.setProp("gdlsCoartados",xc.ID([1,2]))
nod= nodos.newNodeXY(0,4+3+3+3+3)
nod.mass= matrizMasasNodo
nod.setProp("gdlsCoartados",xc.ID([1,2]))
setTotal= mdlr.getSets.getSet("total")
nodos= setTotal.getNodes
for n in nodos:
  n.fix(n.getProp("gdlsCoartados"),xc.Vector([0,0,0]))

# Materials definition
materiales= mdlr.getMaterialLoader
#scc= typical_materials.defElasticSection2d(mdlr,"scc",A,E,I)

sccPlBaja= typical_materials.defElasticSection2d(mdlr,"sccPlBaja",20*Bbaja*Bbaja,Ehorm,20*Ibaja)
sccPl1a= typical_materials.defElasticSection2d(mdlr,"sccPl1a",20*B1a*B1a,Ehorm,20*I1a) 
sccPl2a= typical_materials.defElasticSection2d(mdlr,"sccPl2a",20*B1a*B1a,Ehorm,20*I1a) 
sccPl3a= typical_materials.defElasticSection2d(mdlr,"sccPl3a",20*B3a*B3a,Ehorm,20*I3a) 
sccPl4a= typical_materials.defElasticSection2d(mdlr,"sccPl4a",20*B3a*B3a,Ehorm,20*I3a)


# Definimos transformaciones geométricas
trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")


# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "sccPlBaja"
elementos.defaultTag= 1 #Tag for next element.
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([0,1]))
beam2d.h= Bbaja
elementos.defaultMaterial= "sccPl1a" 
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([1,2]))
beam2d.h= B1a
elementos.defaultMaterial= "sccPl2a" 
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([2,3]))
beam2d.h= B1a
elementos.defaultMaterial= "sccPl3a" 
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([3,4]))
beam2d.h= B3a
elementos.defaultMaterial= "sccPl4a" 
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([4,5]))
beam2d.h= B3a



masaTotalTeor= 5*masaPorPlanta

# Procedimiento de solución
solu= prueba.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("transformation_constraint_handler")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("smt","sm")
solAlgo= smt.newSolutionAlgorithm("frequency_soln_algo")
integ= smt.newIntegrator("eigen_integrator",xc.Vector([1.0,1,1.0,1.0]))
soe= smt.newSystemOfEqn("band_arpackpp_soe")
soe.shift= 0.0
solver= soe.newSolver("band_arpackpp_solver")
analysis= solu.newAnalysis("modal_analysis","smt","")
analOk= analysis.analyze(3)
periodos= analysis.getPeriods()
modos= analysis.getNormalizedEigenvectors()
factoresParticipacionModal= analysis.getModalParticipationFactors()
masasModalesEfectivas= analysis.getEffectiveModalMasses()
masaTotal= analysis.getTotalMass()
factoresDistribucion= analysis.getDistributionFactors()



periodosTeor= xc.Vector([0.468,0.177,0.105])
ratio1= (periodos-periodosTeor).Norm()
# Los modos se obtienen con distinto signo.
modosEjemplo= xc.Matrix([[0.323,-0.764,-0.946],
                                      [0.521,-0.941,-0.378],
                                      [0.685,-0.700,0.672],
                                      [0.891,0.241,1.000],
                                      [1.000,1.000,-0.849]])
resta_modos= (modos-modosEjemplo)
ratio2= (resta_modos).rowNorm()


ratio3= abs(masaTotal-masaTotalTeor)/masaTotalTeor
factoresDistribEjemplo= xc.Matrix([[0.419,0.295,0.148],
                                   [0.676,0.363,0.059],
                                   [0.889,0.27,-0.105],
                                   [1.157,-0.093,-0.156],
                                   [1.298,-0.386,0.133]])
resta_fdib= factoresDistribucion-factoresDistribEjemplo
ratio4= (resta_fdib).rowNorm()

'''
print "kPlBaja= ",kPlBaja
print "kPl1a= ",kPl1a
print "kPl3a= ",kPl3a
print "periodos: ",periodos
print "ratio1= ",ratio1
# Los modos se obtienen con distinto signo
print "modos: ",modos
print "resta_modos: ",resta_modos
print "ratio2= ",ratio2

print "factoresParticipacionModal: ",factoresParticipacionModal
print "masasModalesEfectivas: ",masasModalesEfectivas
print "masaTotal: ",masaTotal
print "masaTotalTeor: ",masaTotalTeor
print "ratio3= ",ratio3
print "factoresDistribucion: ",factoresDistribucion
print "ratio4= ",ratio4
'''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-3) & (ratio3<1e-12) & (ratio4<5e-2)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
