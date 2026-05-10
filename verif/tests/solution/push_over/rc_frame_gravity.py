# -*- coding: utf-8 -*-
''' Reinforced concrete frame pushover analysis example migrated from OpenSees.

See https://openseespydoc.readthedocs.io/en/latest/src/RCFramePushOver.html
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
import geom
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Import local modules.
import rc_column_fiber_section

# Define problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Create nodes
## Set parameters for overall model geometry
width = 360.0
height = 144.0

# Create nodes
#    tag, X, Y
n1= modelSpace.newNode(0.0, 0.0)
n2= modelSpace.newNode(width, 0.0)
n3= modelSpace.newNode(0.0, height)
n4= modelSpace.newNode(width, height)

# Constraints.
# Fix supports at base of columns
#   tag, DX, DY, RZ
for n in [n1, n2]:
    modelSpace.fixNode('000', n.tag)
    
# Some parameters
colWidth= 15
colDepth= 24
cover= 1.5
As= 0.60  # area of no. 7 bars    
columnFiberSection= rc_column_fiber_section.def_rc_column_fiber_section(preprocessor, colWidth= colWidth, colDepth= colDepth, cover= cover, As= As)

# Extract concrete materials.
sectionGeometry= columnFiberSection.getSectionGeometry
regions= sectionGeometry.getRegions
coreConcrete= None
coverConcrete= None
coreArea= 0.0
coverArea= 0.0
for r in regions:
    area= r.getArea()
    if(area>100.0):
        coreConcrete= r.getMaterial()
        coreArea= area
    else:
        coverConcrete= r.getMaterial()
        coverArea+= area
# Extract steel material.
reinforcement= sectionGeometry.getReinfLayers
steel= reinforcement.getMaterials()[0]


# Define column elements
# ----------------------

# Geometry of column elements
#                tag
pDelta= modelSpace.newPDeltaCrdTransf("pDelta")

# Lobatto integration
beamIntegratorHandler= preprocessor.getBeamIntegratorHandler
lobattoBeamIntegration= beamIntegratorHandler.newBeamIntegrator('Lobatto','lobattoBeamIntegration')

# Create the columns using Beam-column elements
modelSpace.setDefaultCoordTransf(pDelta) # Coordinate transformation for the new elements.
modelSpace.setDefaultMaterial(columnFiberSection) # Section material for the new elements.
modelSpace.setNumSections(5) # Number of integration points along length of element.
modelSpace.setDefaultIntegrator(lobattoBeamIntegration)
column1= modelSpace.newElement("ForceBeamColumn2d", [n1.tag, n3.tag])
column2= modelSpace.newElement("ForceBeamColumn2d", [n2.tag, n4.tag])
weights1= column1.getSectionWeights()
weights2= column1.getSectionWeights()
integratorType1= str(type(column1.getIntegrator()))
integratorType2= str(type(column2.getIntegrator()))
testOK= ('LobattoBeamIntegration' in integratorType1)
testOK= testOK and (len(weights1)== 5)
testOK= testOK and ('LobattoBeamIntegration' in integratorType2)
testOK= testOK and (len(weights2)== 5)

# Define beam element
## Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin")
## Linear elastic 2D section.
A= 360
E= 4030
I= 8640
nu= 0.3
G= E/(2.0*(1+nu))
beamSection= typical_materials.defElasticShearSection2d(preprocessor, "section",A= A, E= E, G= G, I= I, alpha= 1.0)
modelSpace.setDefaultCoordTransf(lin) # Coordinate transformation for the new elements.
modelSpace.setDefaultMaterial(beamSection) # Section material for the new elements.
beam= modelSpace.newElement("ElasticBeam2d", [n3.tag, n4.tag]) # Create the beam element

# Define gravity loads
# --------------------
#  a parameter for the axial load
P= 180.0;  # 10% of axial capacity of columns
# Create a Plain load pattern with a Linear TimeSeries
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
glp= modelSpace.newLoadPattern(name= 'glp', setCurrent= True)
# Create nodal loads at nodes 3 & 4
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

# Compute reference values.
# XXX Continue here.
coreEc= coreConcrete.getTangent()
coreStiffness= coreEc*coreArea
coverEc= coreConcrete.getTangent()
coverStiffness= coverEc*coverArea
steelEs= steel.getTangent()
steelArea= 8*As
steelStiffness= steelEs*steelArea
totalStiffness= steelStiffness+coverStiffness+coreStiffness
epsilon= P/totalStiffness
vertDisplacement= epsilon*height

# Get results.
u3= n3.getDisp[1]
ratio1= abs(u3 + vertDisplacement)
u4= n4.getDisp[1]
ratio2= abs(u4 + vertDisplacement)

testOK= testOK and (ratio1<5e-3) and (ratio2<5e-3)


print('core concrete Ec= ', coreEc)
print('cover concrete Ec= ', coverEc)
print('steel Es= ', steelEs)
print('epsilon= ', epsilon*1e3)
print('vertical displacement: ', vertDisplacement*1e3, 'mm') 
print('u3= ', u3)
print('ratio1= ', ratio1)
print('u4= ', u4)
print('ratio2= ', ratio2)

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

