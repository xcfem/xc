# -*- coding: utf-8 -*-
from __future__ import division

'''Testing the database as combination results storage to accelerate computation. Home made test.'''

from materials.ehe import EHE_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

concrHA30= EHE_materials.HA30
concrHA30.cemType='N'
fckHA30= 30e6 # Concrete characteristic compressive strength HA-30.
Hrel= 0.8 # Relative humidity of the air.
Ec= 2e5*9.81/1e-4 # Concrete Young modulus (Pa).
nuC= 0.2 # Concrete Poisson's ratio EHE-08.
hLosa= 0.2 # Thickness.
densLosa= 2500*hLosa # Deck density kg/m2.
t0= 28.0 # Concrete age at loading
tFin= 10000.0 # Service life.


# Load
F= 5.5e4 # Load magnitude en N

# Concrete shrinkage
tS= 7 # Inicio del secado.

# active reinforcement
Ep= 190e9 # Elastic modulus expressed in MPa
Ap= 140e-6 # bar area expressed in square meters
fMax= 1860e6 # Maximum unit load of the material expressed in MPa.
fy= 1171e6 # Yield stress of the material expressed in Pa.
tInic= 0.75**2*fMax # Effective prestress (0.75*P0 y 25% prestress losses).

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from solution import database_helper as dbHelper

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0,0)
nod= nodes.newNodeXYZ(1,0,0)
nod= nodes.newNodeXYZ(2,0,0)
nod= nodes.newNodeXYZ(3,0,0)
nod= nodes.newNodeXYZ(0,1,0)
nod= nodes.newNodeXYZ(1,1,0)
nod= nodes.newNodeXYZ(2,1,0)
nod= nodes.newNodeXYZ(3,1,0)
nod= nodes.newNodeXYZ(0,2,0)
nod= nodes.newNodeXYZ(1,2,0)
nod= nodes.newNodeXYZ(2,2,0)
nod= nodes.newNodeXYZ(3,2,0)


# Materials definition

hLosa= typical_materials.defElasticMembranePlateSection(preprocessor, "hLosa",Ec,nuC,densLosa,hLosa)

typical_materials.defSteel02(preprocessor, "prestressingSteel",Ep,fy,0.001,tInic)

elements= preprocessor.getElementHandler
# Reinforced concrete deck
elements.defaultMaterial= "hLosa"
elements.defaultTag= 1
elem= elements.newElement("ShellMITC4",xc.ID([1,2,6,5]))

elem= elements.newElement("ShellMITC4",xc.ID([2,3,7,6]))
elem= elements.newElement("ShellMITC4",xc.ID([3,4,8,7]))
elem= elements.newElement("ShellMITC4",xc.ID([5,6,10,9]))
elem= elements.newElement("ShellMITC4",xc.ID([6,7,11,10]))
elem= elements.newElement("ShellMITC4",xc.ID([7,8,12,11]))

# active reinforcement
elements.defaultMaterial= "prestressingSteel"
elements.dimElem= 3 # Dimension of element space
truss= elements.newElement("Truss",xc.ID([1,2]));
truss.area= Ap
truss= elements.newElement("Truss",xc.ID([2,3]));
truss.area= Ap
truss= elements.newElement("Truss",xc.ID([3,4]));
truss.area= Ap
truss= elements.newElement("Truss",xc.ID([5,6]));
truss.area= Ap
truss= elements.newElement("Truss",xc.ID([6,7]));
truss.area= Ap
truss= elements.newElement("Truss",xc.ID([7,8]));
truss.area= Ap
truss= elements.newElement("Truss",xc.ID([9,10]));
truss.area= Ap
truss= elements.newElement("Truss",xc.ID([10,11]));
truss.area= Ap
truss= elements.newElement("Truss",xc.ID([11,12]));
truss.area= Ap

# Constraints

modelSpace.fixNode000_000(1)
modelSpace.fixNode000_000(5)
modelSpace.fixNode000_000(9)

# Loads definition
cargas= preprocessor.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"

lpSHRINKAGE= casos.newLoadPattern("default","SHRINKAGE")
lpFLU= casos.newLoadPattern("default","FLU")

lpG= casos.newLoadPattern("default","G")
lpSC= casos.newLoadPattern("default","SC")
lpVT= casos.newLoadPattern("default","VT")
lpNV= casos.newLoadPattern("default","NV")

nod4= nodes.getNode(4)
nod8= nodes.getNode(8)
nod12= nodes.getNode(12)
lpG.newNodalLoad(nod4.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpG.newNodalLoad(nod8.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpG.newNodalLoad(nod12.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

lpSC.newNodalLoad(nod4.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpSC.newNodalLoad(nod8.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpSC.newNodalLoad(nod12.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

lpVT.newNodalLoad(nod4.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpVT.newNodalLoad(nod8.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpVT.newNodalLoad(nod12.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

lpNV.newNodalLoad(nod4.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpNV.newNodalLoad(nod8.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpNV.newNodalLoad(nod12.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

# Combinaciones
combs= cargas.getLoadCombinations
comb= combs.newLoadCombination("ELU001","1.00*G")
comb= combs.newLoadCombination("ELU002","1.35*G")
comb= combs.newLoadCombination("ELU003","1.00*G + 1.50*SC")
comb= combs.newLoadCombination("ELU004","1.00*G + 1.50*SC + 0.90*NV")
comb= combs.newLoadCombination("ELU005","1.00*G + 1.50*SC + 0.90*VT")
comb= combs.newLoadCombination("ELU006","1.00*G + 1.50*SC + 0.90*VT + 0.90*NV")
comb= combs.newLoadCombination("ELU007","1.00*G + 1.50*VT")
comb= combs.newLoadCombination("ELU008","1.00*G + 1.50*VT + 0.90*NV")
comb= combs.newLoadCombination("ELU009","1.00*G + 1.05*SC + 1.50*VT")
comb= combs.newLoadCombination("ELU010","1.00*G + 1.05*SC + 1.50*VT + 0.90*NV")
comb= combs.newLoadCombination("ELU011","1.00*G + 1.50*NV")
comb= combs.newLoadCombination("ELU012","1.00*G + 0.90*VT + 1.50*NV")
comb= combs.newLoadCombination("ELU013","1.00*G + 1.05*SC + 1.50*NV")
comb= combs.newLoadCombination("ELU014","1.00*G + 1.05*SC + 0.90*VT + 1.50*NV")
comb= combs.newLoadCombination("ELU015","1.35*G + 1.50*SC")
comb= combs.newLoadCombination("ELU016","1.35*G + 1.50*SC + 0.90*NV")
comb= combs.newLoadCombination("ELU017","1.35*G + 1.50*SC + 0.90*VT")
comb= combs.newLoadCombination("ELU018","1.35*G + 1.50*SC + 0.90*VT + 0.90*NV")
comb= combs.newLoadCombination("ELU019","1.35*G + 1.50*VT")
comb= combs.newLoadCombination("ELU020","1.35*G + 1.50*VT + 0.90*NV")
comb= combs.newLoadCombination("ELU021","1.35*G + 1.05*SC + 1.50*VT")
comb= combs.newLoadCombination("ELU022","1.35*G + 1.05*SC + 1.50*VT + 0.90*NV")
comb= combs.newLoadCombination("ELU023","1.35*G + 1.50*NV")
comb= combs.newLoadCombination("ELU024","1.35*G + 0.90*VT + 1.50*NV")
comb= combs.newLoadCombination("ELU025","1.35*G + 1.05*SC + 1.50*NV")
comb= combs.newLoadCombination("ELU026","1.35*G + 1.05*SC + 0.90*VT + 1.50*NV")



printFlag= 0
analysis= predefined_solutions.penalty_newton_raphson(feProblem)

from solution import database_helper


def resuelveCombEstatLin(preprocessor, comb,tagSaveFase0,dbHelp):
  preprocessor.resetLoadCase()
  db.restore(tagSaveFase0)

  dbHelp.helpSolve(comb)

  ''' 
    print "nombrePrevia= ",nombrePrevia
    print "tag= ",tagComb
    print "tagPrevia= ",tagPrevia
    print "descomp previa= ",getDescompCombPrevia
    print "resto sobre previa= ",getDescompRestoSobrePrevia
  '''

  comb.addToDomain()
  analOk= analysis.analyze(1)
  db.save(comb.tag*100)
  comb.removeFromDomain()


dXMin=1e9
dXMax=-1e9
dYMin=1e9
dYMax=-1e9

def procesResultVerif(preprocessor,  comb):
  tagComb= comb.tag
  nmbComb= comb.getName
  nodes= preprocessor.getNodeHandler
  nod8= nodes.getNode(8)
  global deltaX
  deltaX= nod8.getDisp[0]
  global deltaY
  deltaY= nod8.getDisp[1]
  global dXMin
  dXMin= min(dXMin,deltaX)
  global dXMax
  dXMax= max(dXMax,deltaX)
  global dYMin
  dYMin= min(dYMin,deltaY)
  global dYMax
  dYMax= max(dYMax,deltaY)

  ''' 
  print "nmbComb=  ",nmbComb
  print "tagComb=  ",tagComb
  print "descomp=  ",getDescomp("%3.1f")
  print "dXMin=  ",(dXMin*1e3)," mm\n"
  print "dXMax=  ",(dXMax*1e3)," mm\n"
  '''

import os
os.system("rm -r -f /tmp/test_fluencia_03.db")
db= feProblem.newDatabase("BerkeleyDB","/tmp/test_fluencia_03.db")

helper= dbHelper.DatabaseHelperSolve(db)

# Fase 0: prestressing, shrinking
tagSaveFase0= -1



# Shrinking strains
setTotal= preprocessor.getSets["total"]
setShells= preprocessor.getSets.defSet("shells")
for e in setTotal.getElements:
  if(e.getDimension==2):
    setShells.getElements.append(e)

for e in setShells.getElements:
  averageSideLength= e.getPerimeter(True)/4.0
  mat= e.getPhysicalProperties.getVectorMaterials[0]
  grueso= mat.h
  Ac= averageSideLength*grueso
  u= 2*averageSideLength+grueso
  espMedio= 2*Ac/u
  e.setProp("epsShrinkage",concrHA30.getShrEpscs(tFin,tS,Hrel*100,espMedio*1000))

cargas= preprocessor.getLoadHandler
casos= cargas.getLoadPatterns
#casos.setCurrentLoadPattern("SHRINKAGE")

for e in setShells.getElements:
  eleLoad= lpSHRINKAGE.newElementalLoad("shell_strain_load")
  eleLoad.elementTags= xc.ID([e.tag])
  epsShrinkage= e.getProp("epsShrinkage")
  eleLoad.setStrainComp(0,0,epsShrinkage) #(id of Gauss point, id of component, value)
  eleLoad.setStrainComp(1,0,epsShrinkage)
  eleLoad.setStrainComp(2,0,epsShrinkage)
  eleLoad.setStrainComp(3,0,epsShrinkage)
  eleLoad.setStrainComp(0,1,epsShrinkage)
  eleLoad.setStrainComp(1,1,epsShrinkage)
  eleLoad.setStrainComp(2,1,epsShrinkage)
  eleLoad.setStrainComp(3,1,epsShrinkage)


preprocessor.resetLoadCase()

comb= combs.newLoadCombination("FASE0","1.00*SHRINKAGE")
tagSaveFase0= comb.tag*100
comb.addToDomain()
result= analysis.analyze(1)
#comb.removeFromDomain()
combs.remove("FASE0")  # Avoid adding shrinking on each computation.
db.save(tagSaveFase0)



# Deformaciones de fluencia.
for e in setShells.getElements:
  tension1Media= e.getMeanInternalForce("n1")/Ac
  tension2Media= e.getMeanInternalForce("n2")/Ac
#  fi1= shrinkage_fluencia.getPhiFluencia(fckHA30,tFin,t0,Hrel,u,Ac)
  epsFluencia1=concrHA30.getCreepDeformation(tFin,t0,Hrel*100,espMedio*1000,tension1Media)
  epsFluencia2=concrHA30.getCreepDeformation(tFin,t0,Hrel*100,espMedio*1000,tension2Media)


cargas= preprocessor.getLoadHandler
casos= cargas.getLoadPatterns
#cargas.setCurrentLoadPattern("FLU")
for e in setShells.getElements:
  eleLoad= lpFLU.newElementalLoad("shell_strain_load")
  eleLoad.elementTags= xc.ID([e.tag]) 
  eleLoad.setStrainComp(0,0,epsFluencia1) #(id of Gauss point, id of component, value)
  eleLoad.setStrainComp(1,0,epsFluencia1)
  eleLoad.setStrainComp(2,0,epsFluencia1)
  eleLoad.setStrainComp(3,0,epsFluencia1)
  eleLoad.setStrainComp(0,1,epsFluencia2) 
  eleLoad.setStrainComp(1,1,epsFluencia2)
  eleLoad.setStrainComp(2,1,epsFluencia2)
  eleLoad.setStrainComp(3,1,epsFluencia2)

preprocessor.resetLoadCase()
cargas= preprocessor.getLoadHandler

comb= combs.newLoadCombination("FASE0","1.00*FLU")
comb.tagSaveFase0= comb.tag*100
comb.addToDomain()
result= analysis.analyze(1)
#comb.removeFromDomain()
combs.remove("FASE0")  # Avoid adding shrinking on each computation.
db.save(tagSaveFase0)



db.restore(tagSaveFase0)

nombrePrevia="" 
tagPrevia= 0 
tagSave= 0
for key in combs.getKeys():
  comb= combs[key]
  resuelveCombEstatLin(preprocessor, comb,tagSaveFase0,helper)
  procesResultVerif(preprocessor, comb)

# 2015.07.06 Values changed when zeroed initial strains in revertToStart
dXMaxTeor= 0.115734009591e-3 #-1.04244692895e-3
dXMinTeor= -0.0872327603415e-3 #-1.24522898798e-3
# End of change

dYMaxTeor= 0
dYMinTeor= 0
ratio1= abs(((dXMax-dXMaxTeor)/dXMaxTeor))
ratio2= abs(((dXMin-dXMinTeor)/dXMinTeor))
ratio3= abs(((dYMax-dYMaxTeor)))
ratio4= abs(((dYMin-dYMinTeor)))

''' 
print "dXMax= ",dXMax*1e3," mm"
print "dXMaxTeor= ",dXMaxTeor*1e3," mm"
print "ratio1= ",ratio1
print "dXMin= ",dXMin*1e3," mm"
print "dXMinTeor= ",dXMinTeor*1e3," mm"
print "ratio2= ",ratio2
print "dYMin= ",(dYMin*1e3)," mm"
print "dYMinTeor= ",(dYMin*1e3)," mm"
print "ratio3= ",ratio3
print "dYMax= ",(dYMax*1e3)," mm"
print "dYMaxTeor= ",(dYMaxTeor*1e3)," mm"
print "ratio4= ",ratio4
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-5) & (ratio2<1e-5) & (ratio3<1e-5) & (ratio4<1e-5)  :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

os.system("rm -r -f /tmp/test_fluencia_03.db") # Your garbage you clean it
