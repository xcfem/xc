# -*- coding: utf-8 -*-
''' Taken from figure 6.22 of "Finite Element Procedures"
     by "Klaus Jurgen Bathe".

    We use 6 elements instead of 2 to capture the P-d (small delta) effect.
    To capture the geometric non-linearity "inside" the element you need to 
    use a different element formulation, e.g., nonlinear strain in the 
    displacement-based formulation or curvature-based displacement 
    interpolation (CBDI) in the force-based formulation.

    See the article 'Meshing for Column Loads <https://portwooddigital.com/2020/05/10/meshing-for-column-loads/amp/>'
 '''

from __future__ import print_function

import xc_base
import geom
import xc

from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math
from postprocess import output_handler

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 10 # Column length in meters
b= 0.2 # Cross section width in meters
h= 0.2 # Cross section depth in meters
A= b*h # Cross section area en m2
Iz= 1/12.0*b*h**3 # Moment of inertia in m4
Iy= Iz # Moment of inertia in m4
E=1e4/Iz # Elastic modulus in N/m2
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
J= 0.001 # Cross section torsion constant (m4) (ES IRRELEVANTE)
P= -100 # Carga vertical sobre la columna.

NumDiv= 6

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
scc= typical_materials.defElasticShearSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J,1)


# Geometric transformation(s)
lin= modelSpace.newPDeltaCrdTransf("lin",xc.Vector([0,1,0]))
# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= scc.name
seedElemHandler.defaultTransformation= lin.name
beam3d= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))
beam3d.rho= 0.0


points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntIDPos3d(2,geom.Pos3d(0.0,0.0,L))
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
spc= constraints.newSPConstraint(n1.tag,2,0.0) # Node 2,gdl 2
spc= constraints.newSPConstraint(n1.tag,5,0.0) # Node 2,gdl 5
spc= constraints.newSPConstraint(n2.tag,0,0.0) # Node 2,gdl 0 # Front end node.
spc= constraints.newSPConstraint(n2.tag,1,0.0) # Node 2,gdl 1

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([0,0,P,0,0,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
# analysis= predefined_solutions.simple_static_linear(feProblem)
# result= analysis.analyze(1)
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
# print("pth= ", pth)
exec(open(pth+"/../../aux/solu_linear_buckling.py").read())

eig1= analysis.getEigenvalue(1)

deltay= n2.getDisp[2] 
  
deltayTeor= P*L/(E*A)
ratio1= abs(deltay-deltayTeor)/deltayTeor
blCalc= eig1*P
blTeor= -1.0*math.pi**2*E*Iz/(L**2)
ratio2= (blCalc-blTeor)/blTeor

''' 
print("deltay= ",(deltay))
print("deltayTeor= ",(deltayTeor))
print("eig1= ",eig1)
print("ratio1= ",(ratio1))
print("blCalc= ",(blCalc/1e3)," kN")
print("blTeor= ",(blTeor/1e3)," kN")
print("ratio2= ",(ratio2))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<0.03):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
  
# Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayEigenvectors(1)
# oh.displayEigenResult(1, defFScale= 10.0)
# oh.displayLocalAxes()
# oh.displayReactions()
