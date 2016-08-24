# -*- coding: utf-8 -*-
''' Test para comprobar el análisis modal espectral
tomado de la publicación de Andrés Sáez Pérez: «Estructuras III»
 E.T.S. de Arquitectura de Sevilla (España). '''
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
Ehorm= 200000*1e5 # Elastic modulus del hormigón.

Bbaja= 0.45 # Escuadría de los pilares.
Ibaja= 1/12.0*Bbaja**4 # Cross section moment of inertia.
Hbaja= 4 # Altura de la planta baja.
B1a= 0.40 # Escuadría de los pilares.
I1a= 1/12.0*B1a**4 # Cross section moment of inertia.
H= 3 # Altura del resto de plantas.
B3a= 0.35 # Escuadría de los pilares.
I3a= 1/12.0*B3a**4 # Cross section moment of inertia.


kPlBaja= 20*12*Ehorm*Ibaja/(Hbaja**3)
kPl1a= 20*12*Ehorm*I1a/(H**3)
kPl2a= kPl1a
kPl3a= 20*12*Ehorm*I3a/(H**3)
kPl4a= kPl3a

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodes)

nodes= preprocessor.getNodeLoader
nodes.defaultTag= 0; 
nod0= nodes.newNodeXY(0,0) 
nod0.mass= matrizMasasNodo
nod0.setProp("gdlsCoartados",xc.ID([0,1,2]))
nod1= nodes.newNodeXY(0,4)
nod1.mass= matrizMasasNodo
nod1.setProp("gdlsCoartados",xc.ID([1,2]))
nod2= nodes.newNodeXY(0,4+3)
nod2.mass= matrizMasasNodo
nod2.setProp("gdlsCoartados",xc.ID([1,2]))
nod3= nodes.newNodeXY(0,4+3+3)
nod3.mass= matrizMasasNodo
nod3.setProp("gdlsCoartados",xc.ID([1,2]))
nod4= nodes.newNodeXY(0,4+3+3+3)
nod4.mass= matrizMasasNodo
nod4.setProp("gdlsCoartados",xc.ID([1,2]))
nod5= nodes.newNodeXY(0,4+3+3+3+3)
nod5.mass= matrizMasasNodo
nod5.setProp("gdlsCoartados",xc.ID([1,2]))
setTotal= preprocessor.getSets.getSet("total")
nodes= setTotal.getNodes
for n in nodes:
  n.fix(n.getProp("gdlsCoartados"),xc.Vector([0,0,0]))

# Materials definition
materiales= preprocessor.getMaterialLoader
#scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

sccPlBaja= typical_materials.defElasticSection2d(preprocessor, "sccPlBaja",20*Bbaja*Bbaja,Ehorm,20*Ibaja)
sccPl1a= typical_materials.defElasticSection2d(preprocessor, "sccPl1a",20*B1a*B1a,Ehorm,20*I1a) 
sccPl2a= typical_materials.defElasticSection2d(preprocessor, "sccPl2a",20*B1a*B1a,Ehorm,20*I1a) 
sccPl3a= typical_materials.defElasticSection2d(preprocessor, "sccPl3a",20*B3a*B3a,Ehorm,20*I3a) 
sccPl4a= typical_materials.defElasticSection2d(preprocessor, "sccPl4a",20*B3a*B3a,Ehorm,20*I3a)


# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")


# Elements definition
elementos= preprocessor.getElementLoader
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
soe= smt.newSystemOfEqn("sym_band_eigen_soe")
solver= soe.newSolver("sym_band_eigen_solver")
analysis= solu.newAnalysis("modal_analysis","smt","")
analOk= analysis.analyze(5)
periodos= analysis.getPeriods()
modos= analysis.getNormalizedEigenvectors()
factoresParticipacionModal= analysis.getModalParticipationFactors()
masasModalesEfectivas= analysis.getEffectiveModalMasses()
masaTotal= analysis.getTotalMass()
factoresDistribucion= analysis.getDistributionFactors()



periodosTeor= xc.Vector([0.468,0.177,0.105,0.084,0.065])
ratio1= (periodos-periodosTeor).Norm()
modosEjemplo= xc.Matrix([[0.323,-0.764,0.946,0.897,-0.623],[0.521,-0.941,0.378,-0.251,1.000],[0.685,-0.700,-0.672,-0.907,-0.658],[0.891,0.241,-1.000,1.000,0.195],[1.000,1.000,0.849,-0.427,-0.042]])
resta= (modos-modosEjemplo)
ratio2= resta.Norm()
ratio3= abs(masaTotal-5*masaPorPlanta)/5/masaPorPlanta
''' Los valores de los 3 primeros factores de distribución (3 primeras columnas)
   se tomaron del ejemplo de referencia. Los otros dos (que no se dan en el
   ejemplo) son los que se obtienen del programa. ''' 
factoresDistribEjemplo= xc.Matrix([[0.419,0.295,0.148,0.0966714,0.0429946],
                         [0.676,0.363,0.059,-0.0270432,-0.0689994],
                         [0.889,0.27,-0.105,-0.0978747,0.0453662],
                         [1.157,-0.093,-0.156,0.1078,-0.0134259],
                         [1.298,-0.386,0.133,-0.0461473,0.00292086]])
resta= factoresDistribucion-factoresDistribEjemplo
ratio4= resta.Norm()

''' 
print "kPlBaja= ",kPlBaja
print "kPl1a= ",kPl1a
print "kPl3a= ",kPl3a
print "periodos: ",periodos
print "ratio1= ",ratio1
print "modos: ",modos
print "resta: ",resta
print "ratio2= ",ratio2
print "factoresParticipacionModal: ",factoresParticipacionModal
print "masasModalesEfectivas: ",masasModalesEfectivas
print "masaTotal: ",masaTotal
print "ratio3= ",ratio3
print "factoresDistribucion: ",factoresDistribucion
print "ratio4= ",ratio4
 '''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-3) & (ratio2<5e-2) & (ratio3<1e-12) & (ratio4<5e-2)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

