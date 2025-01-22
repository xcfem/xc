# -*- coding: utf-8 -*-
''' Skew constraint trivial test.

See section 4.2 of the book: Finite Element Procedures Klaus-Jurgen Bathe, 2006
 ISBN 097900490X, 9780979004902.  Page 190 et seq.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Material properties.
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
# Define mesh.

#   n4
#   +--------------+ n3
#   |              |
#   |              |
#   |    quad      |
#   |              |
#   |              |
#   |              |
#   +--------------+
#  n1              n2


## Define nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(1,0)
n3= nodes.newNodeXY(1,1)
n4= nodes.newNodeXY(0,1)

## Define material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
## Define elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast2d.name
quad4n= elements.newElement("FourNodeQuad",xc.ID([n1.tag, n2.tag, n3.tag, n4.tag]))
quad4n.physicalProperties.thickness= 0.01

# Constraints
modelSpace.fixNode00(n1.tag)
vertLine= geom.Line2d(geom.Pos2d(0,0), geom.Pos2d(0,1)) # Vertical line at origin.
skew4= modelSpace.constraints.newSkewPlane(n4.tag, vertLine, 0.0, 0.0)
horizLine= geom.Line2d(geom.Pos2d(0,0), geom.Pos2d(1,0)) # Horizontal line at origin.
skew2= modelSpace.constraints.newSkewPlane(n2.tag, horizLine, 0.0, 0.0)


# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
fX= 1e3
fY= 1e3
loadVector= xc.Vector([fX,fY])
lp0.newNodalLoad(n3.tag, loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solve.
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
if(result!=0):
  errMsg= 'Solution algorithm failed.'
  lmsg.error(errMsg)
  exit(1)

# Check results.
## Check displacement results (verify symmetry).
uX_n4= n4.getDisp[0]
uY_n4= n4.getDisp[1]
uX_n2= n2.getDisp[0]
uY_n2= n2.getDisp[1]

error= math.sqrt(uX_n4**2+uY_n2**2+(uY_n4-uX_n2)**2)

## Check reaction results (verify symmetry).
modelSpace.calculateNodalReactions()

r_n1= n1.getReaction
rX_n1= r_n1[0]
rY_n1= r_n1[1]
r_n2= n2.getReaction
rX_n2= r_n2[0]
rY_n2= r_n2[1]
r_n4= n4.getReaction
rX_n4= r_n4[0]
rY_n4= r_n4[1]

eqX= abs(fX+rX_n4+rX_n1+rX_n2)
eqY= abs(fY+rY_n4+rY_n1+rY_n2)


'''
print(uX_n4, uY_n4)
print(uX_n2, uY_n2)
print(error)
print(rX_n4, rY_n4)
print(rX_n2, rY_n2)
print(eqX)
print(eqY)
'''

import os
fname= os.path.basename(__file__)
if (abs(error)<1e-10) and (eqX<1e-10) and (eqY<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX')
# oh.displayDispRot(itemToDisp='uY')
