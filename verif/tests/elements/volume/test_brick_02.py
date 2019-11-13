# -*- coding: utf-8 -*-
'''8 node brick element Patch test.

Reference: MacNeal & Harder, “A Proposed Standard Set of Problems to Test Finite Element Accuracy”, Finite Elements in Analysis and Design, 1 (1985) 3-20
'''

from __future__ import division
from __future__ import print_function

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
from postprocess import output_handler

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,0.0)

nodes= preprocessor.getNodeHandler 
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

elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast3d"


brick1= elements.newElement("Brick",xc.ID([nod1.tag, nod2.tag, nod3.tag, nod4.tag, nod5.tag, nod6.tag, nod7.tag, nod8.tag]))
brick2= elements.newElement("Brick",xc.ID([nod4.tag, nod3.tag, nod11.tag, nod12.tag, nod8.tag, nod7.tag, nod15.tag, nod16.tag]))
brick3= elements.newElement("Brick",xc.ID([nod9.tag, nod10.tag, nod2.tag, nod1.tag, nod13.tag, nod14.tag, nod6.tag, nod5.tag]))
brick4= elements.newElement("Brick",xc.ID([nod2.tag, nod10.tag, nod11.tag, nod3.tag, nod6.tag, nod14.tag, nod15.tag, nod7.tag]))
brick5= elements.newElement("Brick",xc.ID([nod9.tag, nod1.tag, nod4.tag, nod12.tag, nod13.tag, nod5.tag, nod8.tag, nod16.tag]))
brick6= elements.newElement("Brick",xc.ID([nod9.tag, nod10.tag, nod11.tag, nod12.tag, nod1.tag, nod2.tag, nod3.tag, nod4.tag]))
brick7= elements.newElement("Brick",xc.ID([nod5.tag, nod6.tag, nod7.tag, nod8.tag, nod13.tag, nod14.tag, nod15.tag, nod16.tag]))

# totalSet= preprocessor.getSets.getSet('total')
# for e in totalSet.getElements:
#     print(e.tag, 'K= ', e.getTangentStiff().Norm())

constraints= preprocessor.getBoundaryCondHandler
nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod10.fix(xc.ID([0,1,2]),xc.Vector([0.001,0.0005,0.0005]) )
nod11.fix(xc.ID([0,1,2]),xc.Vector([0.0015,0.0015,0.001]) )
nod12.fix(xc.ID([0,1,2]),xc.Vector([0.0005,0.001,0.0005]) )
nod13.fix(xc.ID([0,1,2]),xc.Vector([0.0005,0.0005,0.001]) )
nod14.fix(xc.ID([0,1,2]),xc.Vector([0.0015,0.001,0.0015]) )
nod15.fix(xc.ID([0,1,2]),xc.Vector([0.002,0.002,0.002]) )
nod16.fix(xc.ID([0,1,2]),xc.Vector([0.001,0.0015,0.0015]) )

# Graphic stuff.
oh= output_handler.OutputHandler(modelSpace)

## Uncomment to display the mesh
#oh.displayFEMesh()

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

disp1= xc.Vector([5.16E-04,5.63E-04,4.88E-04])
disp2= xc.Vector([1.11E-03,8.45E-04,8.45E-04])
disp3= xc.Vector([1.31E-03,1.21E-03,1.01E-03])
disp4= xc.Vector([7.63E-04,1.00E-03,7.42E-04])
disp5= xc.Vector([7.35E-04,6.68E-04,8.96E-04])
disp6= xc.Vector([1.17E-03,9.85E-04,1.17E-03])
disp7= xc.Vector([1.46E-03,1.41E-03,1.38E-03])

disp8= xc.Vector([8.89E-04,1.18E-03,1.16E-03])

internalNodes= [nod1, nod2, nod3, nod4, nod5, nod6, nod7, nod8]
theoreticalDisp= [disp1, disp2, disp3, disp4, disp5, disp6, disp7, disp8]
xcDisp= list()
for n in internalNodes:
    xcDisp.append(n.getDisp)
    
avgErr= 0.0
for theorD, xcD in zip(theoreticalDisp, xcDisp):
    avgErr+= (theorD-xcD).Norm()
avgErr/=len(xcDisp)

#print('avgErr= ',avgErr)

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(avgErr<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
