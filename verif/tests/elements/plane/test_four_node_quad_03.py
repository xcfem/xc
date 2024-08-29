# -*- coding: utf-8 -*-
''' Test made from the example 3-001 of the SAP2000 software
    verification revision 2.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import typical_materials

# Units
inch= 25.4e-3
pound= 4.44822

# Material properties.
E= 1e6*pound/inch**2 # Young modulus (psi)
nu= 0.25 # Poisson's ratio
rho= 0.0 # Density
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

## Define material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= E, nu= nu, rho= rho)

## Define nodes.
n1= modelSpace.newNodeXY(0.00, 0.00)
n2= modelSpace.newNodeXY(0.00, 0.12*inch)
n3= modelSpace.newNodeXY(0.04*inch, 0.02*inch)
n4= modelSpace.newNodeXY(0.08*inch, 0.08*inch)
n5= modelSpace.newNodeXY(0.18*inch, 0.03*inch)
n6= modelSpace.newNodeXY(0.16*inch, 0.08*inch)
n7= modelSpace.newNodeXY(0.24*inch, 0.00)
n8= modelSpace.newNodeXY(0.24*inch, 0.12*inch)

## Define elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast2d.name
q1= elements.newElement("FourNodeQuad",xc.ID([n1.tag, n3.tag, n4.tag, n2.tag]))
q2= elements.newElement("FourNodeQuad",xc.ID([n1.tag, n7.tag, n5.tag, n3.tag]))
q3= elements.newElement("FourNodeQuad",xc.ID([n3.tag, n5.tag, n6.tag, n4.tag]))
q4= elements.newElement("FourNodeQuad",xc.ID([n2.tag, n4.tag, n6.tag, n8.tag]))
q5= elements.newElement("FourNodeQuad",xc.ID([n5.tag, n7.tag, n8.tag, n6.tag]))
### Set thickness.
for e in [q1, q2, q3, q4, q5]:
    e.physicalProperties.thickness= 0.001*inch

## Constraints.
constraints= modelSpace.constraints
prescribedDisp= list()
for n in [n1, n2, n7, n8]:
    pos= n.getInitialPos3d
    Ux= (pos.x+pos.y/2.0)/1000.0
    prescribedDisp.append(constraints.newSPConstraint(n.tag, 0, Ux)) # Ux
    Uy= (pos.y+pos.x/2.0)/1000.0
    prescribedDisp.append(constraints.newSPConstraint(n.tag, 1, Uy)) # Uy
    
# Solution
analOk= modelSpace.analyze(calculateNodalReactions= True)
if(analOk!=0):
    lmsg.error('Analysis failed.')
    quit()

# Check results
## Reference valuesq
ref_sigma_11= 1333.33*pound/inch**2
ref_sigma_22= 1333.33*pound/inch**2
ref_sigma_12= 400*pound/inch**2

# Compute roor mean square relative errors.
err11= 0.0 # error in sigma_xx computation.
err22= 0.0 # error in sigma_yy computation.
err12= 0.0 # error in sigma_xy computation.
for e in [q1, q2, q3, q4, q5]:
    elementStresses= e.physicalProperties.getVectorMaterials.getValues('stress', False)
    sz= elementStresses.noRows # number of gauss points.
    for igp in range(0, sz): # iterate through gauss points.
        sg11= elementStresses(igp,0)
        sg22= elementStresses(igp,1)
        sg12= elementStresses(igp,2)
        err11+= ((sg11-ref_sigma_11)/ref_sigma_11)**2
        err22+= ((sg22-ref_sigma_22)/ref_sigma_22)**2
        err12+= ((sg12-ref_sigma_12)/ref_sigma_12)**2

n= 5*4
err11/= n
err22/= n
err12/= n
err11= math.sqrt(err11)
err22= math.sqrt(err22)
err12= math.sqrt(err12)

'''
print('ref_sigma_11= ', ref_sigma_11/1e6, ' MPa', err11)
print('ref_sigma_22= ', ref_sigma_22/1e6, ' MPa', err22)
print('ref_sigma_12= ', ref_sigma_12/1e6, ' MPa', err12)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((err11<1e-5) and (err22<1e-5) and (err12<1e-9)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# # oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
# oh.displayStresses(itemToDisp='sigma_11', defFScale= 10.0)
# oh.displayStresses(itemToDisp='sigma_22', defFScale= 10.0)
# oh.displayStresses(itemToDisp='sigma_12', defFScale= 10.0)
