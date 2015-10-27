# -*- coding: utf-8 -*-
# home made test
# Criterio de signos elementos ZeroLengthSection.
# El axil y los cortantes tienen la misma direccion y sentido que los ejes locales.
#     El torsor Mx y el flector My tienen las direcciones y sentido de los ejes X e Y locales.
#     El flector Mz tiene la misma dirección y sentido CONTRARIO al del eje Z local.

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from actions import utilsAcciones

# Propiedades del material
E= 2.1e6 # Módulo elástico en Pa
nu= 0.3 # Coeficiente de Poisson
G= E/(2.0*(1+nu)) # Módulo de elasticidad a cortante

# Propiedades de la sección
y0= 0
z0= 0
ancho= 1
canto= 1
nDivIJ= 20
nDivJK= 20
A= ancho*canto # Área de la sección expresada en m2
Iy= 1/12.0*ancho*canto**3 # Momento de inercia de la sección expresada en m4
Iz= 1/12.0*canto*ancho**3 # Momento de inercia de la sección expresada en m4

# Geometry
L= 0 # Longitud expresada en metros

# Load
F= 1e3 # Magnitud de la carga en kN

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0,0.0,0.0)
nod= nodos.newNodeXYZ(0.0+L,0.0,0.0)

# Materials definition

elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
respT= typical_materials.defElasticMaterial(preprocessor, "respT",E) # Respuesta de la sección a torsión.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",E) # Respuesta de la sección a cortante según y.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",E) # Respuesta de la sección a cortante según y.
# Secciones
geomCuadFibrasTN= preprocessor.getMaterialLoader.newSectionGeometry("geomCuadFibrasTN")
y1= ancho/2.0
z1= canto/2.0
regiones= geomCuadFibrasTN.getRegions
elast= regiones.newQuadRegion("elast")
elast.nDivIJ= nDivIJ
elast.nDivJK= nDivJK
elast.pMin= geom.Pos2d(y0-y1,z0-z1)
elast.pMax= geom.Pos2d(y0+y1,z0+z1)
materiales= preprocessor.getMaterialLoader
cuadFibrasTN= materiales.newMaterial("fiber_section_3d","cuadFibrasTN")
fiberSectionRepr= cuadFibrasTN.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomCuadFibrasTN")
cuadFibrasTN.setupFibers()

sa= materiales.newMaterial("section_aggregator","sa")
sa.setSection("cuadFibrasTN")
sa.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "sa"
elementos.dimElem= 1
elementos.defaultTag= 1
#  sintaxis: zero_length[<tag>] 
zl= elementos.newElement("zero_length_section",xc.ID([1,2]))

# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp1= casos.newLoadPattern("default","1")
lp2= casos.newLoadPattern("default","2")
lp3= casos.newLoadPattern("default","3")
lp4= casos.newLoadPattern("default","4")
lp5= casos.newLoadPattern("default","5")
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,0,0]))
lp1.newNodalLoad(2,xc.Vector([0,2*F,0,0,0,0]))
lp2.newNodalLoad(2,xc.Vector([0,0,3*F,0,0,0]))
lp3.newNodalLoad(2,xc.Vector([0,0,0,4*F,0,0]))
lp4.newNodalLoad(2,xc.Vector([0,0,0,0,5*F,0]))
lp5.newNodalLoad(2,xc.Vector([0,0,0,0,0,6*F]))


listaHipotesis= []
for key in casos.getKeys():
  listaHipotesis.append(key)

# Procedimiento de solución
solu= prueba.getSoluProc
solCtrl= solu.getSoluControl


solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")


cHandler= sm.newConstraintHandler("penalty_constraint_handler")
cHandler.alphaSP= 1.0e15
cHandler.alphaMP= 1.0e15
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")

solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("smt","sm")
solAlgo= smt.newSolutionAlgorithm("linear_soln_algo")
integ= smt.newIntegrator("load_control_integrator",xc.Vector([]))
soe= smt.newSystemOfEqn("band_spd_lin_soe")
solver= soe.newSolver("band_spd_lin_lapack_solver")





def solve():
  analysis= solu.newAnalysis("static_analysis","smt","")
  result= analysis.analyze(1)
  return result


numHipotesis= len(listaHipotesis)
i= 0.0
ok= 0.0
epsN= 0.0
Ntot= 0.0
epsQy= 0.0
QyTot= 0.0
epsQz= 0.0
QzTot= 0.0
epsT= 0.0
MxTot= 0.0
epsMy= 0.0
MyTot= 0.0
epsilonZPos= 0.0
epsMz= 0.0
MzTot= 0.0
fibraEpsYMax= 0.0
epsilonYPos= 0.0
epsYMax= 0.0
yEpsYMax= 0.0
zEpsYMax= 0.0
sR= xc.Vector([0,0,0,0,0,0])
vI= xc.Vector([0,0,0])
vJ= xc.Vector([0,0,0])
vK= xc.Vector([0,0,0])

for hip in listaHipotesis:
  cargas= preprocessor.getLoadLoader
  cargas.addToDomain(hip)
  ok= solve()
  if(ok==0):
    ele1= elementos.getElement(1)
    vI= ele1.getIVector
    vJ= ele1.getJVector
    vK= ele1.getKVector
    ele1.getResistingForce()
    scc0= ele1.getSection()
    sR= sR + scc0.getStressResultant()
    N= scc0.getStressResultantComponent("N")
    Ntot+= N
    if(abs(N)>1):
      epsN= scc0.getSectionDeformationByName("defN")
    Vy= scc0.getStressResultantComponent("Vy")
    QyTot= QyTot+Vy
    if(abs(Vy)>1):
      epsQy= scc0.getSectionDeformationByName("defVy")
    Vz= scc0.getStressResultantComponent("Vz")
    QzTot= QzTot+Vz
    if(abs(Vz)>1):
      epsQz= scc0.getSectionDeformationByName("defVz")
    T= scc0.getStressResultantComponent("T")
    MxTot= MxTot+T
    if(abs(T)>1):
      epsT= scc0.getSectionDeformationByName("defT")
    My= scc0.getStressResultantComponent("My")
    MyTot= MyTot+My
    if(abs(My)>1):
      epsilonZPos= scc0.getStrain(0.0,1.0)
    epsMy= scc0.getSectionDeformationByName("defMy")
    Mz= scc0.getStressResultantComponent("Mz")
    MzTot= MzTot+Mz
    if(abs(Mz)>1):
      epsilonYPos= scc0.getStrain(1.0,0.0)
    epsMz= scc0.getSectionDeformationByName("defMz")
    cargas.removeFromDomain(hip)
    dom= preprocessor.getDomain
    dom.revertToStart()

fuerzasEnGlobales= Ntot*vI+QyTot*vJ+QzTot*vK
fuerzasEnGlobalesTeor= F*xc.Vector([1,2,3])
momentosEnGlobales= MxTot*vI+MyTot*vJ+MzTot*vK
momentosEnGlobalesTeor= F*xc.Vector([4,5,6])

ratio1= (fuerzasEnGlobales-fuerzasEnGlobalesTeor).Norm()/fuerzasEnGlobalesTeor.Norm()
ratio2= (momentosEnGlobales-momentosEnGlobalesTeor).Norm()/momentosEnGlobalesTeor.Norm()
ratio3= abs((epsilonZPos-5*F/(E*Iy)))
ratio4= abs((epsilonYPos+6*F/(E*Iz)))

''' 
print "vI= ",vI
print "vJ= ",vJ
print "vK= ",vK
print "getStressResultant= ",sR
print "N= ",Ntot/1e3," kN\n"
print "epsN= ",epsN
print "Qy= ",QyTot/1e3," kN\n"
print "epsQy= ",epsQy
print "Qz= ",QzTot/1e3," kN\n"
print "epsQz= ",epsQz
print "Mx= ",MxTot/1e3," kN m\n"
print "epsT= ",epsT
print "My= ",MyTot/1e3," kN m\n"
print "epsMy= ",epsMy
print "epsilonZPos= ",epsilonZPos
print "Mz= ",MzTot/1e3," kN m\n"
print "epsMz= ",epsMz
print "epsilonYPos= ",epsilonYPos
print "fuerzasEnGlobales= ",fuerzasEnGlobales
print "fuerzasEnGlobales= ",fuerzasEnGlobalesTeor
print "momentosEnGlobales= ",momentosEnGlobales
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
'''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-4) & (ratio4 < 1e-4):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."


