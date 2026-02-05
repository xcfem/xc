# -*- coding: utf-8 -*-
''' Naive approach to calculation of pile cap on two piles. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from misc_utils import log_messages as lmsg
from actions import combinations as combs
from postprocess.config import default_config
from postprocess import limit_state_data as lsd
from materials.strut_and_tie import strut_and_tie_limit_state_checking as st_ls

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Problem geometry.
# 
#        n0
#        +
#       / \
#      /   \    d
#     /     \
#   +--------+
# n1    S    n2
#

s= 1.6
d= 1.0
v= s/2.0

## Define mesh.
n0= modelSpace.newNode(0.0, d)
n1= modelSpace.newNode(-v, 0.0)
n2= modelSpace.newNode(v, 0.0)

## Define materials.
reinfSteel= EC2_materials.S500B
tieArea= 5.8e-4
concrete= EC2_materials.C30
strutArea= 0.25

## Define elements.
### Define tie.
modelSpace.setDefaultMaterial(reinfSteel.defElasticMaterial(preprocessor= preprocessor))
modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
tie= modelSpace.newElement("Truss", [n1.tag, n2.tag])
tie.sectionArea= tieArea
### Define struts.
modelSpace.setDefaultMaterial(concrete.defElasticMaterial(preprocessor= preprocessor))
strut1= modelSpace.newElement("Truss", [n1.tag, n0.tag])
strut1.sectionArea= strutArea
strut2= modelSpace.newElement("Truss", [n2.tag, n0.tag])
strut2.sectionArea= strutArea

## Define constraints
modelSpace.fixNode('00', n1.tag)
modelSpace.fixNode('F0', n2.tag)

# Define loads.
F= 1.5*183e3
lpF= modelSpace.newLoadPattern(name= 'F')
lpF.newNodalLoad(n0.tag,xc.Vector([0,-F]))

# Define load combinations.
combContainer= combs.CombContainer()
for i, f in enumerate([0.25, 0.5, 0.75, 1.0, 2.0]):
    combName= 'ELU0'+str(i)
    combExpr= str(f)+'*F'
    combContainer.ULS.acc.add(combName, combExpr)

# Define set of elements to check.
setCalc= modelSpace.defSet('setCalc', elements=[tie, strut1, strut2])

## Setup working directory.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.setEnvConfig(cfg)

# Define limit states to check.
limitStateToCheck= st_ls.strutAndTieLimitState
# Analyze load combinations.
limitStateToCheck.analyzeLoadCombinations(combContainer,setCalc)
# Perform checking.
limitStateToCheck.check(setCalc= setCalc, concrete= concrete, steel= reinfSteel, appendToResFile='N',listFile='N',calcMeanCF='N')

# Get results
refValues= [0.8708275862068884, 0.070306304127012, 0.070306304127012]
err= 0.0
for e, refValue in zip([tie, strut1, strut2], refValues):
    cv= e.getProp('ULS_StrutAndTie')
    err+= (cv.CF-refValue)**2

#  print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

cfg.cleandirs() # Clean after yourself.

