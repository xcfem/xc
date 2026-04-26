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
    
# Define materials for nonlinear columns
# ------------------------------------------
# CONCRETE                   tag  f'c    ec0    f'cu   ecu
## Core concrete (confined)
coreConcrete= typical_materials.defConcrete01(preprocessor= preprocessor, name= 'coreConcrete', fpc= -6.0, epsc0= -0.004, fpcu= -5.0, epscu= -0.014)
## Cover concrete (unconfined)
coverConcrete= typical_materials.defConcrete01(preprocessor= preprocessor, name= 'coverConcrete', fpc= -5.0, epsc0= -0.002, fpcu= 0.0, epscu= -0.006)
# STEEL
# Reinforcing steel
fy = 60.0;  # Yield stress
E= 30000.0;  # Young's modulus
steel= typical_materials.defSteel01(preprocessor= preprocessor, name= 'steel', fy= fy, E= E, b= 0.01)

# Define cross-section for nonlinear columns
# ------------------------------------------

#  some parameters
colWidth= 15
colDepth= 24

cover= 1.5
As= 0.60  # area of no. 7 bars

# some variables derived from the parameters
widthVector= geom.Vector2d(colWidth, 0)
depthVector= geom.Vector2d(0, colDepth)
coreWidth= colWidth-2*cover
coreDepth= colDepth-2*cover
coreWidthVector= geom.Vector2d(coreWidth, 0)
coreDepthVector= geom.Vector2d(0, coreDepth)
y1= colDepth / 2.0
z1= colWidth / 2.0

# Column corners.
leftBottomCorner= geom.Pos2d(-y1, -z1)
rightBottomCorner= leftBottomCorner+widthVector
rightTopCorner= rightBottomCorner+depthVector
leftTopCorner= rightTopCorner-widthVector
# Colum core corners.
coreLeftBottomCorner= geom.Pos2d(cover - y1, cover - z1)
coreRightBottomCorner= coreLeftBottomCorner+coreWidthVector
coreRightTopCorner= coreRightBottomCorner+coreDepthVector
coreLeftTopCorner= coreRightTopCorner-coreWidthVector
# Contour of the section core.
coreContour= geom.Polygon2d([coreLeftBottomCorner, coreRightBottomCorner, coreRightTopCorner, coreLeftTopCorner])
# Contours of the section cover.
## Bottom cover
bottomCoverContour= geom.Polygon2d([leftBottomCorner, rightBottomCorner, coreRightBottomCorner, coreLeftBottomCorner])
## Right cover
rightCoverContour= geom.Polygon2d([coreRightBottomCorner, rightBottomCorner, rightTopCorner, coreRightTopCorner])
## Top cover
topCoverContour= geom.Polygon2d([coreLeftTopCorner, coreRightTopCorner, rightTopCorner, leftTopCorner])
## Left cover.
leftCoverContour= geom.Polygon2d([leftBottomCorner, coreLeftBottomCorner, coreLeftTopCorner, leftTopCorner])

matHandler= preprocessor.getMaterialHandler
columnSectionGeometry= matHandler.newSectionGeometry("columnSectionGeometry")
columnSectionGeometryRegions= columnSectionGeometry.getRegions
# Create the concrete core fibers
nDivY= 10
nDivZ= 1
coreRegion= columnSectionGeometryRegions.newQuadRegion(coreConcrete.name)
coreRegion.setPolygon(coreContour)
coreRegion.nDivIJ= nDivZ
coreRegion.nDivJK= nDivY
# Bottom cover
bottomCoverRegion= columnSectionGeometryRegions.newQuadRegion(coverConcrete.name)
bottomCoverRegion.setPolygon(bottomCoverContour)
bottomCoverRegion.nDivIJ= nDivZ
bottomCoverRegion.nDivJK= nDivY
# Right cover
rightCoverRegion= columnSectionGeometryRegions.newQuadRegion(coverConcrete.name)
rightCoverRegion.setPolygon(rightCoverContour)
rightCoverRegion.nDivIJ= nDivZ
rightCoverRegion.nDivJK= nDivY
# Top cover
topCoverRegion= columnSectionGeometryRegions.newQuadRegion(coverConcrete.name)
topCoverRegion.setPolygon(topCoverContour)
topCoverRegion.nDivIJ= nDivZ
topCoverRegion.nDivJK= nDivY
# # Left cover
leftCoverRegion= columnSectionGeometryRegions.newQuadRegion(coverConcrete.name)
leftCoverRegion.setPolygon(leftCoverContour)
leftCoverRegion.nDivIJ= nDivZ
leftCoverRegion.nDivJK= nDivY

# Create the reinforcing fibers (left, middle, right)
reinforcement= columnSectionGeometry.getReinfLayers
## Left reinforcement.
leftReinforcement= reinforcement.newStraightReinfLayer(steel.name)
leftReinforcement.numReinfBars= 3
leftReinforcement.barArea= As
leftReinforcement.p1= coreLeftBottomCorner
leftReinforcement.p2= coreLeftTopCorner
## Middle reinforcement.
middleReinforcement= reinforcement.newStraightReinfLayer(steel.name)
middleReinforcement.numReinfBars= 2
middleReinforcement.barArea= As
middleReinforcement.p1= geom.Segment2d(coreLeftBottomCorner, coreRightBottomCorner).getMidPoint()
middleReinforcement.p2= geom.Segment2d(coreLeftTopCorner, coreRightTopCorner).getMidPoint()
## Right reinforcement.
rightReinforcement= reinforcement.newStraightReinfLayer(steel.name)
rightReinforcement.numReinfBars= 3
rightReinforcement.barArea= As
rightReinforcement.p1= coreRightBottomCorner
rightReinforcement.p2= coreRightTopCorner

# Create fiber section.
columnFiberSection= matHandler.newMaterial("fiber_section_2d","columnFiberSection")
columnFiberSectionRepr= columnFiberSection.getFiberSectionRepr()
columnFiberSectionRepr.setGeomNamed(columnSectionGeometry.name)
columnFiberSection.setupFibers()

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
analysis= predefined_solutions.transformation_newton_raphson_band_gen(feProblem, maxNumIter= 10, convergenceTestTol= 1e-12, printFlag= 0, numSteps= numSteps, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test')
# perform the gravity load analysis, requires 10 steps to reach the load level
analysis.analyze(numSteps)

# Get results.
u3= n3.getDisp[1]
ratio1= abs(u3 + 0.0183736)
u4= n4.getDisp[1]
ratio2= abs(u4 + 0.0183736)

testOK= testOK and (ratio1<1e-6) and (ratio2<1e-6)

# Compute reference values.
# XXX Continue here.
coreEc= coreConcrete.getTangent()

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

