# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test
#    Shrinking load pattern test.

import math
import xc_base
import geom
import xc
from materials.ehe import EHE_materials
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Ec= 2e5*9.81/1e-4 # Concrete Young modulus (Pa).
nuC= 0.2 # Concrete Poisson's ratio.
thk= 0.2 # Thickness.
densLosa= 2500*thk # Deck density kg/m2.


# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(-1,-1,0)
n2= nodes.newNodeXYZ(1,-1,0)
n3= nodes.newNodeXYZ(1,1,0)
n4= nodes.newNodeXYZ(-1,1,0)


# Materials definition
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",Ec,nuC,densLosa,thk)


elements= preprocessor.getElementHandler
elements.defaultMaterial= mat.name
elem= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))


# Node positions.
n1Pos= elem.getCartesianCoordinates(xc.ParticlePos3d(geom.Pos3d(-1,-1,0)), True)  
n2Pos= elem.getCartesianCoordinates(xc.ParticlePos3d(geom.Pos3d(1,-1,0)), True)
n3Pos= elem.getCartesianCoordinates(xc.ParticlePos3d(geom.Pos3d(1,1,0)), True)
n4Pos= elem.getCartesianCoordinates(xc.ParticlePos3d(geom.Pos3d(-1,1,0)), True)

err= n1Pos.dist2(n1.getInitialPos3d)
err+= n2Pos.dist2(n2.getInitialPos3d)
err+= n3Pos.dist2(n3.getInitialPos3d)
err+= n4Pos.dist2(n4.getInitialPos3d)

# Gauss points positions
gaussPoints= elem.getGaussModel().getGaussPoints()
for p in gaussPoints:
    gpPos= elem.getCartesianCoordinates(p, True)
    err+= (p.r-gpPos.x)**2+(p.s-gpPos.y)**2+(p.t-gpPos.z)**2

err= math.sqrt(err)

'''
print('n1 position: ', n1Pos)
print('n2 position: ', n2Pos)
print('n3 position: ', n3Pos)
print('n4 position: ', n4Pos)
print('err: ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
