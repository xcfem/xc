# -*- coding: utf-8 -*-
''' Check the methods that retrieve the section material from the element.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
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
sry= 2.42 # Shear coefficient.
Ay= A/sry
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu))

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Materials definition
scc= typical_materials.defElasticShearSection2d(preprocessor, name= "scc",A= A,E= E, G= G, I= I, alpha= Ay/A)
sccType= scc.tipo()

# Problem geometry
n1= modelSpace.newNode(0,0)
n2= modelSpace.newNode(l,0)

# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

## Seed element
modelSpace.setElementDimension(2) # Bars defined in a three dimensional space.
modelSpace.setDefaultMaterial(scc)
modelSpace.setDefaultCoordTransf(lin)
beam2d= modelSpace.newElement("ElasticBeam2d", [n1.tag, n2.tag])
beam2dSection= beam2d.getSection(0)
beam2dSectionType= beam2dSection.tipo()
beam2dSectionB= beam2d.physicalProperties.getVectorMaterials[0]
beam2dSectionTypeB= beam2dSectionB.tipo()

# print(sccType, beam2dSectionType, beam2dSectionTypeB)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((sccType==beam2dSectionType) and (sccType==beam2dSectionType)):
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
