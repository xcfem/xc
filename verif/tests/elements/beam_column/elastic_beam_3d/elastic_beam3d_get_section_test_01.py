# -*- coding: utf-8 -*-
''' Test 3D Timoshenko’s beam bending - SSLL02/89 from Autodesk Robot
Structural Analysis Professional - Verification Manual AFNOR benchmarks
SSLL02 Poutre courte sur deux appuis articulés
Test of the elastic beam 3D element (WITH shear deformation)
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import xc

from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

E= 2.0e11 # Young modulus.
l= 1.44 # Bar length in inches.
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

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
scc= typical_materials.defElasticShearSection3d(preprocessor, name= "scc",A= A,E= E,G= G,Iy= Iy, Iz= Iz,J= Iy/100.0, alpha_y= Ay/A, alpha_z= Az/A)
sccType= scc.tipo()

# Problem geometry
n1= modelSpace.newNode(0,0,0)
n2= modelSpace.newNode(l,0,0)

# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

## Seed element
modelSpace.setElementDimension(3) # Bars defined in a three dimensional space.
modelSpace.setDefaultMaterial(scc)
modelSpace.setDefaultCoordTransf(lin)
beam3d= modelSpace.newElement("ElasticBeam3d", [n1.tag, n2.tag])
beam3dSection= beam3d.getSection(0)
beam3dSectionType= beam3dSection.tipo()
physicalProperties= beam3d.physicalProperties
beam3dSectionB= beam3d.physicalProperties.getVectorMaterials[0]
beam3dSectionTypeB= beam3dSectionB.tipo()

# print(sccType, beam3dSectionType, beam3dSectionTypeB)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((sccType==beam3dSectionType) and (sccType==beam3dSectionType)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# # oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
# oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayIntForcDiag('N')
# # oh.displayIntForcDiag('M')
# # oh.displayLocalAxes()
