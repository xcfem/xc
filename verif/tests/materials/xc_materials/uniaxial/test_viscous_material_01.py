# -*- coding: utf-8 -*-
''' Trivial test to check the Python exposure of viscous materials.'''

from __future__ import print_function

import xc
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"



# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
nodes.dimSpace= 1 # One coordinate for each node.
nodes.numDOFs= 1 # One degree of freedom for each node.

n1= nodes.newNodeX(0)
n2= nodes.newNodeX(0)

# Check viscous material definition
baseArea= 1219631.1
dashpotCoeff= 1750.00
viscousMaterial= typical_materials.defViscousMaterial(preprocessor, name= "viscousMaterial01", C= dashpotCoeff*baseArea, Alpha= 1.0)

# Check assingning material to element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= viscousMaterial.name
elements.dimElem= 1 # Dimension of element space
spring= elements.newElement("ZeroLength",xc.ID([n1.tag, n2.tag]))

# Check attributes.
refDampingCoeff= dashpotCoeff*baseArea
ratio1= abs((refDampingCoeff-viscousMaterial.dampingCoeff)/refDampingCoeff)
ratio2= abs(1.0-viscousMaterial.alpha)

'''
print('refDampingCoeff= ', refDampingCoeff)
print('B viscousMaterial.dampingCoeff= ', viscousMaterial.dampingCoeff)
print("ratio1= ", ratio1)
print("ratio2= ", ratio2) 
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-10) & (ratio2<1e-10)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
