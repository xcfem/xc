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
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= 1e9,nu= 0.3, rho= 0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nod0= nodes.newNodeXY(0,0)
nod1= nodes.newNodeXY(0.5,0)
nod2= nodes.newNodeXY(1,0)
nod3= nodes.newNodeXY(0,0.5)
nod4= nodes.newNodeXY(0.5,0.5)
nod5= nodes.newNodeXY(1,0.5)
nod6= nodes.newNodeXY(0,1)
nod7= nodes.newNodeXY(0.5,1)
nod8= nodes.newNodeXY(1,1)

elements= preprocessor.getElementHandler
elements.defaultMaterial= elast2d.name


quad0= elements.newElement('FourNodeQuad',xc.ID([nod0.tag, nod1.tag, nod4.tag, nod3.tag]))
quad1= elements.newElement('FourNodeQuad',xc.ID([nod1.tag, nod2.tag, nod5.tag, nod4.tag]))
quad2= elements.newElement('FourNodeQuad',xc.ID([nod3.tag, nod4.tag, nod7.tag, nod6.tag]))
quad3= elements.newElement('FourNodeQuad',xc.ID([nod4.tag, nod5.tag, nod8.tag, nod7.tag]))
quads= [quad0, quad1, quad2, quad3]

# totalSet= preprocessor.getSets.getSet('total')
# for e in totalSet.getElements:
#     print(e.tag, 'K= ', e.getTangentStiff().Norm())
nod0.fix(xc.ID([0,1]),xc.Vector([0,0]) )
nod1.fix(xc.ID([1]),xc.Vector([0]) )
nod2.fix(xc.ID([1]),xc.Vector([0]) )

# Load definition
P= 10e3*2/3 # punctual load.
## Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
nod2.newLoad(xc.Vector([P,0]))
nod3.newLoad(xc.Vector([-P,0]))
nod5.newLoad(xc.Vector([P,0]))
nod6.newLoad(xc.Vector([-P,P]))
nod7.newLoad(xc.Vector([0,P]))
nod8.newLoad(xc.Vector([P,P]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff. Uncomment to get graphics working.
#oh= output_handler.OutputHandler(modelSpace)

# Uncomment to display the mesh
# oh.displayFEMesh()

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)


sXnodes= list()
sYnodes= list()
sX= 0.0
sY= 0.0
sXY= 0.0
count= 0.0
for q in quads:    
    elementStresses= q.physicalProperties.getVectorMaterials.getValues('stress')
    nodeStresses= q.getExtrapolatedValues(elementStresses)
    #vMises= q.physicalProperties.getVectorMaterials.getValues('von_mises_stress')
    # print('element stresses: ', elementStresses)
    # print('node stresses: ', nodeStresses)

    sz= nodeStresses.noRows
    for i in range(0,sz):
        sX+= nodeStresses(i,0)
        sY+= nodeStresses(i,1)
        sXY+= nodeStresses(i,2)/2.0
        sYnodes.append(sY)
        sXnodes.append(sX)
        count+= 1

sX/= count
sY/= count
sXY/= count
sXerr= (sX-3*P)/3/P
sYerr= (sY-3*P)/3/P
tauErr= sXY

# oh.displayLoads()
# oh.displayDispRot('uY')
# oh.displayStresses('sigma_11')
# oh.displayVonMisesStresses()
# oh.displayStresses('sigma_22')
# oh.displayStresses('sigma_12')
# oh.displayStrains('epsilon_xx')
# oh.displayStrains('epsilon_yy')

'''
print('sigma_11 on nodes: ', sXnodes)
print('sigma_22 on nodes: ', sYnodes)
print('sigma_11: ', sX)
print('sigma_22: ', sY)
print('sigma_12: ', sXY)
print('error in sigma_11: ', sXerr)
print('error in sigma_22: ', sYerr)
print('error in sigma_12: ', tauErr)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(sXerr<1e-10 and sYerr<1e-10 and tauErr<1e-10):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
