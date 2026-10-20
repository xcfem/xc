# -*- coding: utf-8 -*-
''' Test of TzSimple1 uniaxial material object.

TzSimple1 is a uniaxial material model used to simulate the load-transfer behavior (t-z curves) for the skin friction/axial shear resistance of deep foundation piles.
'''

__author__= "Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es "

import os
import xc
import math
import numpy as np
from model import predefined_spaces
from materials import soil_structure_interaction as ssi
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

silent= True

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
## Problem type
modelSpace= predefined_spaces.SolidMechanics1D(preprocessor.getNodeHandler)

# Define mesh.
n1= modelSpace.newNode(0.0) # Fixed node.
n2= modelSpace.newNode(0.0) # Free node.
# Define constraints.
modelSpace.fixNode('0', n1.tag)

# Define material.
tzS1= ssi.def_tzsimple1_material(preprocessor,
                                 matName= 'tzS1',
                                 tzType= 2, # Mosher relation.
                                 tult= 50.0, # Ultimate skin friction capacity.
                                 z50= 0.05, # 50% capacity reached at 0.05 units of displacement
                                 c= 0.0)

# Define "spring" element.
modelSpace.setDefaultMaterial(tzS1)
modelSpace.setElementDimension(1)
soilSpring= modelSpace.newElement("ZeroLength", [n1.tag,n2.tag])

# Define load.
## Linear time series.
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts', setCurrent= True)
lp= modelSpace.newLoadPattern(name= 'lp', setCurrent= True)
lp.newNodalLoad(n2.tag, xc.Vector([-60.0]))
modelSpace.addLoadCaseToDomain(lp.name)

# Solution procedure.
solProc= predefined_solutions.PlainNewtonRaphson(prb= feProblem, numSteps= 0, maxNumIter= 10, convTestType= 'norm_unbalance_conv_test', printFlag= 0)
solProc.setup()
# Set the load factor increment between steps.
solProc.setDeltaLambda(0.1)
analysis= solProc.getAnalysis()

uxi= list()
fi= list()
for step in range(8):
    analysis.analyze(1)
    uX= n2.getDisp[0]
    modelSpace.calculateNodalReactions()
    f= n1.getReaction[0]
    uxi.append(uX)
    fi.append(f)

# Check results.
errUX= 0.0
refUXi= [-0.00779495845761738, -0.0172849611431117, -0.0294947540364051, -0.0464543168295277, -0.0727942124667418, -0.121686822787245, -0.249573829441827, -1.31700945847032]
for ux, rUX in zip(uxi, refUXi):
    errUX+= (ux-rUX)**2
errUX= math.sqrt(errUX)

errF= 0.0
refFi= [6*i for i in range(1,9)]
for f, rF in zip(fi, refFi):
    errF+= (f-rF)**2
errF= math.sqrt(errF)

if(not silent):
    print('errUX= ', errUX)
    print('errF= ', errF)
    
import os
fname= os.path.basename(__file__)
if((errUX<1e-12) and (errF<1e-8)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Plot the loading response.
if(not silent):
    import matplotlib.pyplot as plt
    plt.figure(figsize=(8, 5))
    plt.plot(uxi, fi, color='b', linewidth=1.5)
    plt.title('Pile skin friction (TzSimple1)')
    plt.xlabel('Pile displacement (m)')
    plt.ylabel('Soil reaction (kN)')
    plt.grid(True)
    plt.show()

