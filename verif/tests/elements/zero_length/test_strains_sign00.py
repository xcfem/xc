# -*- coding: utf-8 -*-
# home made test

# ZeroLengthSection elements sign criteria.

#     Axial and shear forces have the same direction and sense that
#     the local axes.
#     Mx torque and My bending moment have the same direction and sense
#     that the local axes.
#     Bending moment Mz has the same direction and its sense is the OPPOSITE to local Z axis.

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from postprocess import prop_statistics

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Material properties
E= 2.1e6 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties
y0= 0
z0= 0
widthOverZ= 2
depthOverY= 1
nDivIJ= 20
nDivJK= 20
A= widthOverZ*depthOverY # Cross section area (m2)
Iy= 1/12.0*widthOverZ*depthOverY**3 # Cross section moment of inertia (m4)
Iz= 1/12.0*depthOverY*widthOverZ**3 # Cross section moment of inertia (m4)


# Geometry
L= 0 # Length expressed in meters

# Load
F= 1e3 # Load magnitude (kN)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(0.0+L,0.0,0.0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
respT= typical_materials.defElasticMaterial(preprocessor, "respT",E) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",E) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",E) # Shear response in y direction.
# Secciones
testQuadRegion= preprocessor.getMaterialHandler.newSectionGeometry("testQuadRegion")
y1= widthOverZ/2.0
z1= depthOverY/2.0
regions= testQuadRegion.getRegions
elast= regions.newQuadRegion("elast")
elast.nDivIJ= nDivIJ
elast.nDivJK= nDivJK
elast.pMin= geom.Pos2d(y0-y1,z0-z1)
elast.pMax= geom.Pos2d(y0+y1,z0+z1)
rectang= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= rectang.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("testQuadRegion")
rectang.setupFibers()
fibras= rectang.getFibers()


# Torsion and shear responses.
materiales= preprocessor.getMaterialHandler
agg= materiales.newMaterial("section_aggregator","sa")
agg.setSection("quadFibers")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])



# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "sa"
elements.dimElem= 1 # Dimension of element space
zl= elements.newElement("ZeroLengthSection",xc.ID([1,2]))

# Constraints
modelSpace.fixNode000_000(1)

# Loads definition
cargas= preprocessor.getLoadHandler
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


# Solution procedure
solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("penalty_constraint_handler")
cHandler.alphaSP= 1.0e15
cHandler.alphaMP= 1.0e15
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
analysisAggregations= solCtrl.getAnalysisAggregationContainer
analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
solAlgo= analysisAggregation.newSolutionAlgorithm("linear_soln_algo")
integ= analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
soe= analysisAggregation.newSystemOfEqn("band_spd_lin_soe")
solver= soe.newSolver("band_spd_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","analysisAggregation","")


def solve():
  return analysis.analyze(1)


#numHipotesis= listaHipotesis.size
i= 0.0
epsMy= 0.0
MyTot= 0.0
fibraEpsZMax= 0.0
epsilonZPos= 0.0
epsZMax= 0.0
yEpsZMax= 0.0
zEpsZMax= 0.0
epsMz= 0.0
MzTot= 0.0
fibraEpsYMax= 0.0
fibraEpsYMin= 0.0
epsilonYPos= 0.0
epsilonYPosTeor= 0.0
epsYMax= 0.0
yEpsYMax= 0.0
zEpsYMax= 0.0
epsYMin= 0.0
yEpsYMin= 0.0
zEpsYMin= 0.0

for key in casos.getKeys():
  lp= casos[key]
  casos.addToDomain(key)
  ok= solve()
  if(ok==0):
    ele1= elements.getElement(0)
    ele1.getResistingForce()
    scc= ele1.getSection()
    My= scc.getStressResultantComponent("My")
    MyTot+= My
    if(abs(My)>1):
      epsilonZPos= scc.getSection().getStrain(0.0,1.0)
      fibraEpsZMax= prop_statistics.getItemWithMaxProp(scc.getSection().getFibers(),"getMaterial.getStrain")
      epsZMax= fibraEpsZMax.getMaterial().getStrain()
      yEpsZMax= fibraEpsZMax.getPos().x
      zEpsZMax= fibraEpsZMax.getPos().y
      epsMy= scc.getSectionDeformationByName("defMy")#defMy
    Mz= scc.getStressResultantComponent("Mz")
    MzTot+= Mz
    if(abs(Mz)>1):
      epsilonYPos= scc.getSection().getStrain(1.0,0.0)
      fibraEpsYMax= prop_statistics.getItemWithMaxProp(scc.getSection().getFibers(),"getMaterial.getStrain")
      epsYMax= fibraEpsYMax.getMaterial().getStrain()
      yEpsYMax= fibraEpsYMax.getPos().x
      zEpsYMax= fibraEpsYMax.getPos().y
      fibraEpsYMin= prop_statistics.getItemWithMinProp(scc.getSection().getFibers(),"getMaterial.getStrain")#IMinProp("getMaterial.getStrain")
      epsYMin= fibraEpsYMin.getMaterial().getStrain()
      yEpsYMin= fibraEpsYMin.getPos().x
      zEpsYMin= fibraEpsYMin.getPos().y
      epsilonYPos= scc.getStrain(yEpsYMin,zEpsYMin)
      epsilonYPosTeor= -Mz/(E*Iz)*yEpsYMin
      epsMz= scc.getSectionDeformationByName("defMz")#defMz
  casos.removeFromDomain(key)
  dom= preprocessor.getDomain
  dom.revertToStart()


ratio1= abs((epsilonYPos-epsilonYPosTeor)/epsilonYPosTeor)
ratio2= abs((epsYMin+epsilonYPosTeor)/epsilonYPosTeor)
ratio3= abs((epsilonZPos-5*F/(E*Iy)))
ratio4= abs((epsilonYPos-6*F/(E*Iz)))

''' 
print "My= ",MyTot/1e3," kN m"
print "epsMy= ",epsMy
print "epsilonZPos= ",epsilonZPos
print "Mz= ",MzTot/1e3," kN m"
print "epsMz= ",epsMz
print "epsYMin= ",epsYMin
print "epsilonYPosTeor= ",epsilonYPosTeor
print "epsilonYPos= ",epsilonYPos
print "yEpsYMax= ",yEpsYMax 
print "yEpsYMin= ",yEpsYMin 
print "zEpsYMax= ",zEpsYMax 
print "zEpsYMin= ",zEpsYMin 

print "getStressResultantComponent= ",sR
print "N= ",Ntot/1e3," kN"
print "epsN= ",epsN
print "Qy= ",QyTot/1e3," kN"
print "epsQy= ",epsQy
print "Qz= ",QzTot/1e3," kN"
print "epsQz= ",epsQz
print "Mx= ",MxTot/1e3," kN m"
print "epsT= ",epsT
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 0.003) and (ratio2 < 0.003) and (ratio3 < 1e-4) and (ratio4 < 2e-3):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

