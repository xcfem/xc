# -*- coding: utf-8 -*-
''' Controls that the relative total norm displacement increment
    doesn't crash when when the relative increment is almost zero.'''

from __future__ import print_function
from __future__ import division


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Testing hinge development in a cantilever.

import xc_base
import geom
import xc

from materials.sections.structural_shapes import arcelor_metric_shapes
from materials.ec3 import EC3_materials
from model import predefined_spaces
from solution import predefined_solutions

test= xc.FEProblem()
preprocessor=  test.getPreprocessor

S355JR= EC3_materials.S355JR
S355JR.gammaM= 1.05
epp= S355JR.getDesignElasticPerfectlyPlasticMaterial(preprocessor, "epp")
IPE200= arcelor_metric_shapes.IPEShape(S355JR,'IPE_200')
fs3d= IPE200.getFiberSection3d(preprocessor,'epp')

L= 1.0
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
nodes.defaultTag= 10 # First node number.
nod1= nodes.newNodeXY(0,0.0)
nod2= nodes.newNodeXY(L,0.0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name # Coordinate transformation for the new elements
elements.defaultMaterial= IPE200.fiberSection3dName
beam2d= elements.newElement("ForceBeamColumn2d",xc.ID([nod1.tag,nod2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(nod1.tag)

# Loads definition
WzplTeor= IPE200.get('Wzpl')
M0Teor= -WzplTeor*S355JR.fyd()
F= M0Teor*0.87
# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nod2.tag,xc.Vector([0,F,0]))

lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(nod2.tag,xc.Vector([0,F/2.0,0]))
modelSpace.addLoadCaseToDomain(lp1.name)

# Solution procedure
solProc= predefined_solutions.PenaltyModifiedNewton(test, maxNumIter= 10, convergenceTestTol= 1e-2, printFlag= 0)
solProc.setup()
modelSpace.analysis= solProc.analysis


convergenceTestType= solProc.ctest.type()
modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain(lp0.name)
result= modelSpace.analyze(calculateNodalReactions= False)
# We repeat the analysis to test that the relative total
# norm display increment doesn't crash when the relative
# increment is almost zero.
result= modelSpace.analyze(calculateNodalReactions= True)

'''
print(result)
print(convergenceTestType)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(result==0.0 and (convergenceTestType=='XC::CTestRelativeTotalNormDispIncr')):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
