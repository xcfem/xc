# -*- coding: utf-8 -*-
''' Trivial check of the Concrete02IS material.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

l= 1e-7 # Distance between nodes

import xc
from solution import predefined_solutions
from materials import typical_materials
from materials.ec2 import EC2_materials
# import matplotlib.pyplot as plt
import numpy as np

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
nodes.dimSpace= 1 # One coordinate for each node.
nodes.numDOFs= 1 # One degree of freedom for each node.

n1= nodes.newNodeX(1)
n2= nodes.newNodeX(1.0+l)

# Materials definition
concrAux= EC2_materials.C25
epsc0= concrAux.epsilon0()
fpc= concrAux.fmaxK()
Ec0= 2.0*fpc/epsc0
concrete= typical_materials.defConcrete02IS(preprocessor=preprocessor,name='concrete', Ec0= Ec0, epsc0= epsc0, fpc= fpc, fpcu= 0.85*concrAux.fmaxK(), epscu= concrAux.epsilonU(), ratioSlope= 0.1, ft= concrAux.fctk(), Ets= concrAux.E0()/19.0)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial='concrete'
elements.dimElem= 1 # Dimension of element space
elem1= elements.newElement("ZeroLength",xc.ID([n1.tag, n2.tag]))

   
# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(n1.tag, 0,0.0) # Node 1


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
#We add the load case to domain.
#lPatterns.addToDomain(lp0.name)

# Solution
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
dom= preprocessor.getDomain

strainData= [-0.0014517255054275048, -0.0012950647995898898, -0.0011771607360655028, -0.0010769605147439268, -0.000994164434068782, -0.0009152654474126228, -0.0008469605993237639, -0.0007803305574086666, -0.0007214112867197361, -0.0006653605008990896, -0.0006118455255762576, -0.000560558859581081, -0.0005104647548656792, -0.0004631570115009628, -0.00041717711637729796, -0.0003723706244344146, -0.00032858986556887407, -0.0002856946662170974, -0.00024355265797323854, -0.00020198019801980199, -0.0001610174880763116, -0.0001204304503786369, -8.012779552715656e-05, -4.0015993602558976e-05, 0.0, 3.9999999999999996e-05, 6e-05, 6.8e-05, 7.219053709429341e-05]
stressData= [-23769526.89452975, -22811710.597085666, -21857123.845715567, -20871959.01879276, -19929673.314080067, -18919391.37806733, -17954190.160818744, -16930892.09817331, -15958893.497465627, -14976474.048593784, -13987102.85209974, -12993265.281878645, -11981125.618300915, -10989569.986983366, -9994572.202418111, -8997371.976836976, -7998835.473570751, -6999538.547498543, -5999841.947359786, -4998524.993595367, -3999513.72232689, -2999884.0598165416, -1999984.680959728, -999999.5203199998, -0.0, 999999.9999999999, 1500000.0, 1700000.0, 1794985.865939647]

stressInput= [-24e6,-23e6,-22e6,-21e6,-20e6,-19e6,-18e6,-17e6,-16e6,-15e6,-14e6,-13e6,-12e6,-11e6,-10e6,-9e6,-8e6,-7e6,-6e6,-5e6,-4e6,-3e6,-2e6,-1e6,0.0,1e6,1.5e6,1.7e6,1.8e6]
calculatedStresses= []
calculatedStrains= []

for stress in stressInput:
    dom.revertToStart()
    lPatterns.removeFromDomain(lp0.name)
    lp0.clearLoads()
    nl= lp0.newNodalLoad(n2.tag, xc.Vector([stress]))
    lPatterns.addToDomain(lp0.name)
    result= analysis.analyze(1)
    calculatedStresses.append(-elem1.getResistingForce()[0])
    calculatedStrains.append(elem1.getMaterials()[0].getStrain())

residStresses= (np.array(calculatedStresses) - np.array(stressData))
residStrains= (np.array(calculatedStrains) - np.array(strainData))

ratio1= np.linalg.norm(residStresses)/-concrAux.fmaxK()
ratio2= np.linalg.norm(residStrains)/3.5e-3

'''
print('E0= ', concrete.getInitialTangent()/1e9, 2*concrete.fpc/concrete.epsc0/1e9)
print('epsc0= ', concrete.epsc0)
print('fmaxK= ', concrete.fpc/1e6)
print('epscu= ', concrete.epscu)
print('ft= ', concrete.ft/1e6)
print('Ets= ', concrete.Ets/1e9, concrete.getInitialTangent()/1e9)
print('calculatedStresses= ', calculatedStresses)
print('error in stress calculation: ', residStresses)
print('calculatedStrains= ', calculatedStrains)
print(residStrains)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
from postprocess.reports import graph_material
import matplotlib.pyplot as plt
retval= graph_material.UniaxialMaterialDiagramGraphic(epsMin=-0.0035,epsMax=0.0016,title=' concrete02 characteristic stress-strain diagram')
retval.setupGraphic(plt,concrete)
plt.plot(strainData, stressData, '-')
retval.savefig(plt,'concrete02is_diagram.jpeg')
'''
  
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-5)) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
