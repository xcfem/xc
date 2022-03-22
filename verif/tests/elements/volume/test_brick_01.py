# -*- coding: utf-8 -*-
from __future__ import print_function
''' Test made from the example 9-1 of the book: 
    Introducción Al Estudio Del Elemento Finito en Ingeniería. 
    Segunda Edició!n. Tirupathi R. Chandrupatla, Ashok D. Belegundu. 
    Pearson Educación, 1999
    isbn={9789701702604},
    url={https://books.google.ch/books?id=auQwAAAACAAJ},
'''

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
# from postprocess import output_handler

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",200000,0.3,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
n1= nodes.newNodeXYZ(100,0,100)
n2= nodes.newNodeXYZ(0,0,100)
n3= nodes.newNodeXYZ(0,0,200)
n4= nodes.newNodeXYZ(100,0,200)
n5= nodes.newNodeXYZ(100,100,100)
n6= nodes.newNodeXYZ(0,100,100)
n7= nodes.newNodeXYZ(0,100,200)
n8= nodes.newNodeXYZ(100,100,200)

n9= nodes.newNodeXYZ(100,200,100)
n10= nodes.newNodeXYZ(0,200,100)
n11= nodes.newNodeXYZ(0,200,200)
n12= nodes.newNodeXYZ(100,200,200)
n13= nodes.newNodeXYZ(100,300,100)
n14= nodes.newNodeXYZ(0,300,100)
n15= nodes.newNodeXYZ(0,300,200)

n16= nodes.newNodeXYZ(100,300,200)
n17= nodes.newNodeXYZ(100,200,0)
n18= nodes.newNodeXYZ(100,300,0)
n19= nodes.newNodeXYZ(0,300,0)
n20= nodes.newNodeXYZ(0,200,0)

elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name

brick1= elements.newElement("Brick",xc.ID([n1.tag, n2.tag, n3.tag, n4.tag, n5.tag, n6.tag, n7.tag, n8.tag]))
brick2= elements.newElement("Brick",xc.ID([n5.tag, n6.tag, n7.tag, n8.tag, n9.tag, n10.tag, n11.tag, n12.tag]))
brick3= elements.newElement("Brick",xc.ID([n9.tag, n10.tag, n11.tag, n12.tag, n13.tag, n14.tag, n15.tag, n16.tag]))
brick4= elements.newElement("Brick",xc.ID([n9.tag, n10.tag, n14.tag, n13.tag, n17.tag, n20.tag, n19.tag, n18.tag]))

n17.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n18.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n19.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n20.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n4.tag,xc.Vector([0,0,-80000]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# # Output stuff.
# oh= output_handler.OutputHandler(modelSpace)

# ## Uncomment to display the mesh
# oh.displayFEMesh()
# oh.displayLoads()

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

dN1Teor= xc.Vector([-2.1569e-2,-3.7891e-3,-4.0982e-1])
dN1= n1.getDisp

ratio= (dN1-dN1Teor).Norm()

''' 
print("dN1= ",dN1)
print("dN1Teor= ",dN1Teor)
print("rario= ",ratio)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
