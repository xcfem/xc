# -*- coding: utf-8 -*-
''' Check method getValuesAtNodes in ZeroLengthSection elements.'''

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
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
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(0.0+L,0.0,0.0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
respT= typical_materials.defElasticMaterial(preprocessor, "respT",E) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",E) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",E) # Shear response in y direction.
# Sections
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
fiberSectionRepr.setGeomNamed(testQuadRegion.name)
rectang.setupFibers()
fibras= rectang.getFibers()

# Torsion and shear responses.
materialHandler= preprocessor.getMaterialHandler
agg= materialHandler.newMaterial("section_aggregator","agg")
agg.setSection("quadFibers")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= agg.name
elements.dimElem= 1 # Dimension of element space
zl= elements.newElement("ZeroLengthSection",xc.ID([n1.tag,n2.tag]))

# Constraints
modelSpace.fixNode000_000(n1.tag)

# Loads definition
# Load case definition.
loadPatterns= list()
for i in range(0,6):
  lp= modelSpace.newLoadPattern(name= str(i))
  loadVector= [0,0,0,0,0,0]
  loadVector[i]= (i+1)*F
  lp.newNodalLoad(n2.tag, xc.Vector(loadVector))
  loadPatterns.append(lp)

# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem, maxNumIter= 10, convergenceTestTol= 1e-5)

def solve():
  return analysis.analyze(1)

error= 0.0
lPatterns= preprocessor.getLoadHandler.getLoadPatterns
internalForceCodes= ['N', 'Vy', 'Vz', 'T', 'My', 'Mz']
internalForceOrder= ['N', 'Mz', 'My', 'T', 'Vy', 'Vz']
for i, key in enumerate(lPatterns.getKeys()):
    lp= lPatterns[key]
    modelSpace.addLoadCaseToDomain(key)
    ok= solve()
    if(ok==0):
        zl.getResistingForce()
        scc= zl.getSection()
        ifCode= internalForceCodes[i]
        ifValue= scc.getStressResultantComponent(ifCode)
        ifOrder= internalForceOrder.index(ifCode)
        F= zl.getValuesAtNodes("stress", False)
        error+= (F[0][ifOrder]-ifValue)**2
        error+= (F[1][ifOrder]+ifValue)**2
    lPatterns.removeFromDomain(key)
    modelSpace.revertToStart()
error= math.sqrt(error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error < 1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

