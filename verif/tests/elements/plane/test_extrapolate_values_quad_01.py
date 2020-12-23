# -*- coding: utf-8 -*-
'''Test extrapolation from Gauss points. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
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
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= 1e6,nu= 0.25, rho= 0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nod0= nodes.newNodeXY(0,0)
nod1= nodes.newNodeXY(1,0)
nod2= nodes.newNodeXY(0,1)
nod3= nodes.newNodeXY(1,1)

elements= preprocessor.getElementHandler
elements.defaultMaterial= elast2d.name


quad0= elements.newElement('FourNodeQuad',xc.ID([nod0.tag, nod1.tag, nod3.tag, nod2.tag]))
quad0.thickness= 1

# totalSet= preprocessor.getSets.getSet('total')
# for e in totalSet.getElements:
#     print(e.tag, 'K= ', e.getTangentStiff().Norm())

constraints= preprocessor.getBoundaryCondHandler
nod0.fix(xc.ID([0,1]),xc.Vector([0,0]) )
nod1.fix(xc.ID([1]),xc.Vector([0]) )

# Load definition
P= 10e3 # punctual load.
## Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
nod1.newLoad(xc.Vector([P,0]))
nod2.newLoad(xc.Vector([-P,P]))
nod3.newLoad(xc.Vector([P,P]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

# Uncomment to display the mesh
# oh.displayFEMesh()

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

elementStresses= quad0.physicalProperties.getVectorMaterials.getValues('stress', False)
nodeStresses= quad0.getExtrapolatedValues(elementStresses)

sz= nodeStresses.noRows
sXnodes= list()
sYnodes= list()
sXerr= 0.0
sYerr= 0.0
tauErr= 0.0
for i in range(0,sz):
    sX= nodeStresses(i,0)
    sY= nodeStresses(i,1)
    sXY= nodeStresses(i,2)/2.0
    sYnodes.append(sY)
    sXnodes.append(sX)
    sXerr= (sX-2.0*P)**2
    sYerr= (sY-2.0*P)**2
    tauErr= sXY**2

sXerr/= 4.0
sYerr/= 4.0
tauErr/= 4.0

# oh.displayDispRot('uZ')
# oh.displayStresses('sigma_11')
# oh.displayStresses('sigma_22')
# oh.displayStresses('sigma_12')
# oh.displayStrains('epsilon_xx')
# oh.displayStrains('epsilon_yy')

'''
print('element stresses: ', elementStresses)
print('node stresses: ', nodeStresses)
print('sigma_11 on nodes: ', sXnodes)
print('sigma_22 on nodes: ', sYnodes)
print('error in sigma_11: ', sXerr)
print('error in sigma_22: ', sYerr)
print('error in sigma_12: ', tauErr)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(sXerr<1e-20 and sYerr<1e-20 and tauErr<1e-20):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
