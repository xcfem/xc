# -*- coding: utf-8 -*-
'''8 node brick element Patch test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod9= nodes.newNodeXYZ(0,0,0)
nod10= nodes.newNodeXYZ(1,0,0)
nod11= nodes.newNodeXYZ(1,1,0)
nod12= nodes.newNodeXYZ(0,1,0)
nod13= nodes.newNodeXYZ(0,0,1)
nod14= nodes.newNodeXYZ(1,0,1)
nod15= nodes.newNodeXYZ(1,1,1)
nod16= nodes.newNodeXYZ(0,1,1)

elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast3d"
brick1= elements.newElement("Brick",xc.ID([nod9.tag,nod10.tag,nod11.tag,nod12.tag,nod13.tag,nod14.tag,nod15.tag,nod16.tag]))

constraints= preprocessor.getBoundaryCondHandler

nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod10.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod11.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod12.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )

nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod10.fix(xc.ID([0,1,2]),xc.Vector([0.001,0.0005,0.0005]) )
nod11.fix(xc.ID([0,1,2]),xc.Vector([0.0015,0.0015,0.001]) )
nod12.fix(xc.ID([0,1,2]),xc.Vector([0.0005,0.001,0.0005]) )
nod13.fix(xc.ID([0,1,2]),xc.Vector([0.0005,0.0005,0.001]) )
nod14.fix(xc.ID([0,1,2]),xc.Vector([0.0015,0.001,0.0015]) )
nod15.fix(xc.ID([0,1,2]),xc.Vector([0.002,0.002,0.002]) )
nod16.fix(xc.ID([0,1,2]),xc.Vector([0.001,0.0015,0.0015]) )

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

avgStress11= brick1.getAvgStressIJ(1,1)

print 'average stress: ', avgStress11
