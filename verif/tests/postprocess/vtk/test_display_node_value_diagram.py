# -*- coding: utf-8 -*-
'''
Check that the Mz bending moment diagram is oriented towards the the "belly" 
due that belly.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from actions import loads
from actions import load_cases as lcases
from postprocess import control_vars as cv

E= 2.0e11 # Young modulus.
A= 31e-4 # Beam cross-section area.
h= math.sqrt(A) # Beam cross-section depth.
I= 2810e-8 # Inertia of the beam section.
Iy= I
Iz= I
sry= 2.42 # Shear coefficient.
Ay= A/sry
Az= A-Ay
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu))
p= 1e5 # Transverse load.

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Problem geometry.
p1= modelSpace.newKPoint(0,0,0)
p2= modelSpace.newKPoint(0,0,4*math.pi)
l= modelSpace.newLine(p1,p2)
l.setNDiv(100)

# Materials definition
scc= typical_materials.defElasticShearSection3d(preprocessor, name= "scc",A= A,E= E,G= G,Iy= Iy, Iz= Iz,J= Iy/100.0, alpha_y= Ay/A, alpha_z= Az/A)
# Geometric transformation
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
# Mesh generation
modelSpace.setElementDimension(3)
modelSpace.setDefaultMaterial(scc)
modelSpace.setDefaultCoordTransf(lin)
modelSpace.newSeedElement("ElasticBeam3d")
l.genMesh(xc.meshDir.I)

for n in l.nodes:
    pos= n.getInitialPos3d
    z= pos.z
    n.setProp('sin_z', math.sin(z))

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
fname= os.path.basename(__file__)

outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayNodeValueDiagram(itemToDisp= 'sin_z', caption= 'Test', setToDisplay= None, defaultDirection= 'X', defaultValue= 0.0, fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)
