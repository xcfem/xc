# -*- coding: utf-8 -*-
'''Beam under movable load.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials.ec3 import EC3_materials # Steel shapes.
from misc_utils import log_messages as lmsg
from rough_calculations import ng_simple_beam as sb

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry
span= 12
pt1= modelSpace.newKPoint(0,0)
pt2= modelSpace.newKPoint(span,0)

ln= modelSpace.newLine(pt1, pt2)
ln.nDiv= 4

# Material
steelType= EC3_materials.S235JR # steel type
shape= EC3_materials.IPEShape(steel= steelType, name= 'IPE_200')
scc= shape.defElasticShearSection2d(preprocessor, "scc")
linearRho= shape.getRho()
E= shape.steelType.E
I= shape.Iz()
sBeam= sb.SimpleBeam(E= E, I= I, l= span)
f0ref= sBeam.getFundamentalBendingFrequencies(1, linearRho)[0]
w0ref= 2*math.pi*f0ref

# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

## Seed element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 2 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= scc.name
seedElemHandler.defaultTransformation= lin.name
beam2d= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))

ln.genMesh(xc.meshDir.I)

nA= pt1.getNode()
nB= pt2.getNode()

# Constraints
modelSpace.fixNode00F(nA.tag)
modelSpace.fixNodeF0F(nB.tag)

### Eigen analysis.
solProc= predefined_solutions.FrequencyAnalysis(feProblem,systemPrefix='full_gen')
if(solProc.solve()!=0):
    lmsg.error('Eigen analysis failed.')
    quit()
eig0= solProc.analysis.getEigenvalue(1)
omega0= math.sqrt(eig0)
T0= 2*math.pi/omega0
f0calc= 1.0/T0
error= abs(f0calc-f0ref)/f0ref

''' 
print('w0ref= ', w0ref, 'rad/s')
print(f0ref, f0calc)
print("error= ",error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-3:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

