# -*- coding: utf-8 -*-
''' Test displaying strain loads on trust elements. Home made test.

    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc
from model import predefined_spaces
from materials import typical_materials


L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
AT= 10 # Temperature increment (Celsius degrees)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nod01= nodes.newNodeXY(0.0,0.0)
nod02= nodes.newNodeXY(L,0.0)
nod11= nodes.newNodeXY(0.0,L)
nod12= nodes.newNodeXY(L,L)
nod21= nodes.newNodeXY(0.0,2*L)
nod22= nodes.newNodeXY(L,2*L)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast.name
elements.dimElem= 2 # Dimension of element space
truss1= elements.newElement("Truss",xc.ID([nod01.tag,nod02.tag]))
truss1.sectionArea= A
truss2= elements.newElement("Truss",xc.ID([nod11.tag,nod12.tag]))
truss2.sectionArea= A
truss3= elements.newElement("Truss",xc.ID([nod21.tag,nod22.tag]))
truss3.sectionArea= A

# Constraints
for n in [nod01, nod02, nod11, nod12, nod21, nod22]:
    modelSpace.fixNode('00', n.tag)
    
# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad1= lp0.newElementalLoad("truss_strain_load")
eleLoad1.elementTags= xc.ID([truss1.tag])
eleLoad1.eps1= alpha*AT
eleLoad1.eps2= alpha*AT
eleLoad2= lp0.newElementalLoad("truss_strain_load")
eleLoad2.elementTags= xc.ID([truss2.tag])
eleLoad2.eps1= alpha*AT*2
eleLoad2.eps2= alpha*AT*2
eleLoad3= lp0.newElementalLoad("truss_strain_load")
eleLoad3.elementTags= xc.ID([truss3.tag])
eleLoad3.eps1= alpha*AT*2
eleLoad3.eps2= -alpha*AT*2

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

# for strainLabel in ['epsilon_xx']:
#     oh.displayLoads(elLoadComp= strainLabel)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(elLoadComp= 'epsilon_xx', fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)
