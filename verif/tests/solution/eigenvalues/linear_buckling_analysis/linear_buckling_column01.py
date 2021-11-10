# -*- coding: utf-8 -*-
''' Linear buckling analysis of a column under axial load.
    Taken from SOLVIA Verification Manual example B46

    We use 6 elements instead of 2 to capture the P-d (small delta) effect.
    To capture the geometric non-linearity "inside" the element you need to 
    use a different element formulation, e.g., nonlinear strain in the 
    displacement-based formulation or curvature-based displacement 
    interpolation (CBDI) in the force-based formulation.

    See the article 'Meshing for Column Loads <https://portwooddigital.com/2020/05/10/meshing-for-column-loads/amp/>'
  '''
from __future__ import print_function
from __future__ import division
import xc_base
import geom
import xc

from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 4.0 # Column length in meters
b= 0.2 # Cross section width in meters
h= 0.2 # Cross section depth in meters
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Moment of inertia in m4
E=30E9 # Elastic modulus en N/m2
P= -100 # Carga vertical sobre la columna.

NumDiv= 6

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

nodes.newSeedNode(2,3)

# Geometric transformation(s)
lin= modelSpace.newPDeltaCrdTransf("lin")


# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= scc.name
seedElemHandler.defaultTransformation= lin.name
beam2d= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))
beam2d.h= h
beam2d.rho= 0.0

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(1,geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(2,geom.Pos3d(0.0,L,0.0))
lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newLine(pt1.tag,pt2.tag)
l.nDiv= NumDiv


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)
n1= pt1.getNode() # Back end node.
n2= pt2.getNode() # Front end node.
# Constraints
constraints= preprocessor.getBoundaryCondHandler

#
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 2,gdl 0 # Back end node.
spc= constraints.newSPConstraint(n1.tag,1,0.0) # Node 2,gdl 1
spc= constraints.newSPConstraint(n2.tag,0,0.0) # Node 2,gdl 0 # Front end node.

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([0,P,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
# print("pth= ", pth)
exec(open(pth+"/../../../aux/solu_linear_buckling.py").read())



eig1= analysis.getEigenvalue(1)
deltay= n2.getDisp[1] 

deltayTeor= P*L/(E*A)
ratio1= abs(deltay-deltayTeor)/deltayTeor
blCalc= eig1*P
blTeor= -1*math.pi**2*E*I/(L**2)
ratio2= (blCalc-blTeor)/blTeor

''' 
print("deltay= ",(deltay))
print("deltayTeor= ",(deltayTeor))
print("eig1= ",(eig1))
print("ratio1= ",(ratio1))
print("blCalc= ",(blCalc/1e6)," MN")
print("blTeor= ",(blTeor/1e6)," MN")
print("ratio2= ",(ratio2))
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<0.03):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
