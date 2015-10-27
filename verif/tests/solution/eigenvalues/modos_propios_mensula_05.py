# -*- coding: utf-8 -*-
# Tomado del Ejemplo B46 del SOLVIA Verification Manual
import xc_base
import geom
import xc

from model import fix_node_3dof
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math

L= 1 # Longitud de la mensula en metros
b= 0.05 # Ancho de la sección en metros
h= 0.1 # Canto de la sección en metros
nuMat= 0.3 # Coeficiente de Poisson.
EMat= 2.0E11 # Módulo de Young en N/m2.
espChapa= h # Espesor en m.
area= b*espChapa # Área de la sección en m2
inercia1= 1/12.0*espChapa*b**3 # Momento de inercia en m4
inercia2= 1/12.0*b*espChapa**3 # Momento de inercia en m4
dens= 7800 # Densidad del acero en kg/m3
m= b*h*dens

NumDiv= 10

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader

predefined_spaces.gdls_resist_materiales3D(nodos)
# Definimos materiales
elast= typical_materials.defElasticMembranePlateSection(preprocessor, "elast",EMat,nuMat,espChapa*dens,espChapa)


puntos= preprocessor.getCad.getPoints
pt1= puntos.newPntIDPos3d(1, geom.Pos3d(0.0,0.0,0.0) )
pt2= puntos.newPntIDPos3d(2, geom.Pos3d(b,0.0,0.0) )
pt3= puntos.newPntIDPos3d(3, geom.Pos3d(b,L,0.0) )
pt4= puntos.newPntIDPos3d(4, geom.Pos3d(0,L,0.0) )
surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 1
s= surfaces.newQuadSurfacePts(1,2,3,4)
s.nDivI= 1
s.nDivJ= NumDiv


nodos.newSeedNode()

seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.defaultTag= 1
elem= seedElemLoader.newElement("shell_mitc4",xc.ID([0,0,0,0]))


f1= preprocessor.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)
# Constraints


ln= preprocessor.getCad.getLineWithEndPoints(pt1.tag,pt2.tag)
lNodes= ln.getNodes()
for n in lNodes:
  n.fix(xc.ID([0,1,2,3,4,5]),xc.Vector([0,0,0,0,0,0])) # UX,UY,UZ,RX,RY,RZ

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

#soe= smt.newSystemOfEqn("sym_band_eigen_soe")
#solver= soe.newSolver("sym_band_eigen_solver")
soe= smt.newSystemOfEqn("band_arpackpp_soe")
soe.shift= 0.0
solver= soe.newSolver("band_arpackpp_solver")
solver.tol= 1e-3
solver.maxNumIter= 5

analysis= solu.newAnalysis("eigen_analysis","smt","")


analOk= analysis.analyze(2)
eig1= analysis.getEigenvalue(1)
eig2= analysis.getEigenvalue(2)




omega1= math.sqrt(eig1)
T1= 2*math.pi/omega1
f1calc= 1.0/T1
omega2= math.sqrt(eig2)
T2= 2*math.pi/omega2
f2calc= 1.0/T2



Lambda= 1.87510407
f1teor= Lambda**2/(2*math.pi*L**2)*math.sqrt(EMat*inercia1/m)
ratio1= abs(f1calc-f1teor)/f1teor
f2teor= Lambda**2/(2*math.pi*L**2)*math.sqrt(EMat*inercia2/m)
ratio2= abs(f2calc-f2teor)/f2teor

'''
print "omega1= ",omega1
print "T1= ",T1
print "f1calc= ",f1calc
print "f1teor= ",f1teor
print "ratio1= ",ratio1
print "omega2= ",omega2
print "T2= ",T2
print "f2calc= ",f2calc
print "f2teor= ",f2teor
print "ratio2= ",ratio2
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio2)<1e-3):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
