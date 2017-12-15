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

nodes= preprocessor.getNodeLoader 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod1= nodes.newNodeXYZ(0.249,0.342,0.192)
nod2= nodes.newNodeXYZ(0.826,0.288,0.288)
nod3= nodes.newNodeXYZ(0.850,0.649,0.263)
nod4= nodes.newNodeXYZ(0.273,0.750,0.230)
nod5= nodes.newNodeXYZ(0.320,0.186,0.643)
nod6= nodes.newNodeXYZ(0.677,0.305,0.683)
nod7= nodes.newNodeXYZ(0.788,0.693,0.644)
nod8= nodes.newNodeXYZ(0.165,0.745,0.702)
nod9= nodes.newNodeXYZ(0,0,0)
nod10= nodes.newNodeXYZ(1,0,0)
nod11= nodes.newNodeXYZ(1,1,0)
nod12= nodes.newNodeXYZ(0,1,0)
nod13= nodes.newNodeXYZ(0,0,1)
nod14= nodes.newNodeXYZ(1,0,1)
nod15= nodes.newNodeXYZ(1,1,1)
nod16= nodes.newNodeXYZ(0,1,1)

elements= preprocessor.getElementLoader
elements.defaultMaterial= "elast3d"
brick1= elements.newElement("Brick",xc.ID([nod16.tag,nod13.tag,nod9.tag,nod12.tag,nod8.tag,nod5.tag,nod1.tag,nod4.tag]))
brick2= elements.newElement("Brick",xc.ID([nod13.tag,nod14.tag,nod10.tag,nod9.tag,nod5.tag,nod6.tag,nod2.tag,nod1.tag]))
brick3= elements.newElement("Brick",xc.ID([nod5.tag,nod6.tag,nod2.tag,nod1.tag,nod8.tag,nod7.tag,nod3.tag,nod4.tag]))
brick4= elements.newElement("Brick",xc.ID([nod7.tag,nod3.tag,nod4.tag,nod8.tag,nod15.tag,nod11.tag,nod12.tag,nod16.tag]))
brick5= elements.newElement("Brick",xc.ID([nod14.tag,nod10.tag,nod11.tag,nod15.tag,nod6.tag,nod2.tag,nod3.tag,nod7.tag]))
brick6= elements.newElement("Brick",xc.ID([nod14.tag,nod13.tag,nod16.tag,nod15.tag,nod6.tag,nod5.tag,nod8.tag,nod7.tag]))
brick7= elements.newElement("Brick",xc.ID([nod2.tag,nod3.tag,nod4.tag,nod1.tag,nod10.tag,nod11.tag,nod12.tag,nod9.tag]))

# elements= preprocessor.getSets.getSet('total').getElements
# for e in elements:
#     print e.tag, 'K= ', e.getTangentStiff().Norm()
constraints= preprocessor.getConstraintLoader

nod1.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod2.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod3.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod4.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod5.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod6.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod7.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod8.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
#nod10.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
#nod11.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
#nod12.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
#nod14.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
#nod15.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )

# nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
# nod13.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
# nod16.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )


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
