# -*- coding: utf-8 -*-
''' Home made test.'''

# ZeroLengthSection elements sign criteria.
# Axial and shear forces have the same direction and sense that
#     the local axes.
#     Mx torque and My bending moment have the same direction and sense
#     that the local axes.
#     Bending moment Mz has the same direction and its sense is the OPPOSITE to local Z axis.

from __future__ import print_function
from __future__ import division
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Material properties
E= 2.1e6 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2.0*(1+nu)) # Shear modulus

# Cross section properties
y0= 0
z0= 0
width= 1
depth= 1
nDivIJ= 20
nDivJK= 20
A= width*depth # Cross section area (m2)
Iy= 1/12.0*width*depth**3 # Cross section moment of inertia (m4)
Iz= 1/12.0*depth*width**3 # Cross section moment of inertia (m4)

# Geometry
L= 0 # Length expressed in meters

# Load
F= 1e3 # Load magnitude (kN)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(0.0+L,0.0,0.0)

# Materials definition

elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
respT= typical_materials.defElasticMaterial(preprocessor, "respT",E) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",E) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",E) # Shear response in y direction.
# Sections
testQuadRegion= preprocessor.getMaterialHandler.newSectionGeometry("testQuadRegion")
y1= width/2.0
z1= depth/2.0
regions= testQuadRegion.getRegions
elast= regions.newQuadRegion("elast")
elast.nDivIJ= nDivIJ
elast.nDivJK= nDivJK
elast.pMin= geom.Pos2d(y0-y1,z0-z1)
elast.pMax= geom.Pos2d(y0+y1,z0+z1)
materialHandler= preprocessor.getMaterialHandler
quadFibers= materialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(testQuadRegion.name)
quadFibers.setupFibers()

sa= materialHandler.newMaterial("section_aggregator","sa")
sa.setSection("quadFibers")
sa.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= sa.name
elements.dimElem= 1 # Dimension of element space
#  sintaxis: zero_length[<tag>] 
zl= elements.newElement("ZeroLengthSection",xc.ID([n1.tag,n2.tag]))

# Constraints
modelSpace.fixNode000_000(n1.tag)

# Loads definition
loadPatterns= list()
for i in range(0,6):
    lp= modelSpace.newLoadPattern(name= str(i))
    loadVector= [0,0,0,0,0,0]
    loadVector[i]= (i+1)*F
    lp.newNodalLoad(n2.tag, xc.Vector(loadVector))
    loadPatterns.append(lp)

lPatterns= preprocessor.getLoadHandler.getLoadPatterns
listaHipotesis= []
for key in lPatterns.getKeys():
    listaHipotesis.append(key)

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

solutionStrategies= solCtrl.getSolutionStrategyContainer
solutionStrategy= solutionStrategies.newSolutionStrategy("solutionStrategy","sm")
solAlgo= solutionStrategy.newSolutionAlgorithm("linear_soln_algo")
integ= solutionStrategy.newIntegrator("load_control_integrator",xc.Vector([]))
soe= solutionStrategy.newSystemOfEqn("band_spd_lin_soe")
solver= soe.newSolver("band_spd_lin_lapack_solver")

def solve():
  analysis= solu.newAnalysis("static_analysis","solutionStrategy","")
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
    loadHandler= preprocessor.getLoadHandler
    loadHandler.addToDomain(hip)
    ok= solve()
    if(ok==0):
        vI= zl.getIVector
        vJ= zl.getJVector
        vK= zl.getKVector
        zl.getResistingForce()
        scc0= zl.getSection()
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
        loadHandler.removeFromDomain(hip)
        dom= preprocessor.getDomain
        dom.revertToStart()

forcesInGlobalCoord= Ntot*vI+QyTot*vJ+QzTot*vK
forcesInGlobalCoordTeor= F*xc.Vector([1,2,3])
momentsInGlobalCoord= MxTot*vI+MyTot*vJ+MzTot*vK
momentsInGlobalCoordTeor= F*xc.Vector([4,5,6])

ratio1= (forcesInGlobalCoord-forcesInGlobalCoordTeor).Norm()/forcesInGlobalCoordTeor.Norm()
ratio2= (momentsInGlobalCoord-momentsInGlobalCoordTeor).Norm()/momentsInGlobalCoordTeor.Norm()
ratio3= abs((epsilonZPos-5*F/(E*Iy)))
ratio4= abs((epsilonYPos+6*F/(E*Iz)))

''' 
print("vI= ",vI)
print("vJ= ",vJ)
print("vK= ",vK)
print("getStressResultant= ",sR)
print("N= ",Ntot/1e3," kN\n")
print("epsN= ",epsN)
print("Qy= ",QyTot/1e3," kN\n")
print("epsQy= ",epsQy)
print("Qz= ",QzTot/1e3," kN\n")
print("epsQz= ",epsQz)
print("Mx= ",MxTot/1e3," kN m\n")
print("epsT= ",epsT)
print("My= ",MyTot/1e3," kN m\n")
print("epsMy= ",epsMy)
print("epsilonZPos= ",epsilonZPos)
print("Mz= ",MzTot/1e3," kN m\n")
print("epsMz= ",epsMz)
print("epsilonYPos= ",epsilonYPos)
print("forcesInGlobalCoord= ",forcesInGlobalCoord)
print("forcesInGlobalCoord= ",forcesInGlobalCoordTeor)
print("momentsInGlobalCoord= ",momentsInGlobalCoord)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-4) & (ratio4 < 1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


