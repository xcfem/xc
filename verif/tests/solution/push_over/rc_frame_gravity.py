# -*- coding: utf-8 -*-
''' Reinforced concrete frame pushover analysis example migrated from OpenSees.

See https://openseespydoc.readthedocs.io/en/latest/src/RCFramePushOver.html
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import xc
import geom
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Import RC section module.
pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
sys.path.append(pth+"/../../aux/sections/")
import core_cover_rc_section

import rc_push_over_frame

# Define problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# RC section with confined concrete.
colWidth= 15
colDepth= 24
cover= 1.5
As= 0.60  # area of no. 7 bars    
columnFiberSection= core_cover_rc_section.def_core_cover_rc_section(preprocessor, colWidth= colWidth, colDepth= colDepth, cover= cover, As= As)


# Create nodes
## Set parameters for overall model geometry
width = 360.0
height = 144.0

frameItems= rc_push_over_frame.def_rc_push_over_frame(modelSpace= modelSpace, columnFiberSection= columnFiberSection, width= width, height= height)

column1= frameItems['column1']
column2= frameItems['column2']

weights1= column1.getSectionWeights()
weights2= column1.getSectionWeights()
integratorType1= str(type(column1.getIntegrator()))
integratorType2= str(type(column2.getIntegrator()))
testOK= ('LobattoBeamIntegration' in integratorType1)
testOK= testOK and (len(weights1)== 5)
testOK= testOK and ('LobattoBeamIntegration' in integratorType2)
testOK= testOK and (len(weights2)== 5)

print('XXX create a module that can be loades from this script and from rc_frame_push_over.py.')

# Define gravity loads
# --------------------
#  a parameter for the axial load
P= 180.0;  # 10% of axial capacity of columns
# Create a Plain load pattern with a Linear TimeSeries
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
glp= modelSpace.newLoadPattern(name= 'glp', setCurrent= True)
# Create nodal loads at nodes 3 & 4
n3= frameItems['n3']
n4= frameItems['n4']
glp.newNodalLoad(n3.tag, xc.Vector([0,-P,0]))
glp.newNodalLoad(n4.tag, xc.Vector([0,-P,0]))
modelSpace.addLoadCaseToDomain(glp.name)

# Solve
numSteps= 10
solProc= predefined_solutions.TransformationNewtonRaphsonBandGen(feProblem, maxNumIter= 10, convergenceTestTol= 1e-12, printFlag= 0, numSteps= numSteps, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test')
solProc.setup()
analysis= solProc.getAnalysis()
# perform the gravity load analysis, requires 10 steps to reach the load level
analysis.analyze(numSteps)

# Compute the stiffness of the column sections.
totalStiffness= core_cover_rc_section.get_core_cover_rc_section_stiffness(columnFiberSection, As)
epsilon= P/totalStiffness
refVertDisplacement= epsilon*height

# Get results.
u3= n3.getDisp[1]
ratio1= abs(u3 + refVertDisplacement)
u4= n4.getDisp[1]
ratio2= abs(u4 + refVertDisplacement)

testOK= testOK and (ratio1<1e-3) and (ratio2<1e-3)

'''
print('reference vertical displacement: ', refVertDisplacement*1e3) 
print('u3= ', u3*1e3)
print('ratio1= ', ratio1)
print('u4= ', u4*1e3)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Section output
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section
# fig = plt.figure()
# ax= fig.add_subplot(111)
# plot_fiber_section.mplot_section_geometry(ax, sectionGeometry= columnSectionGeometry)
# plt.show()

