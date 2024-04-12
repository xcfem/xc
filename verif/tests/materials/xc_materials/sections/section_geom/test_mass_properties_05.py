#!/usr/bin/env python
''' Build a rectangular hollow section and check its mass properties computed from its regios and from its fibers. Home made test.'''

import math
import geom
import xc
from materials.ec2 import EC2_materials


# MATERIAL parameters
concrete= EC2_materials.C30
steel= EC2_materials.S500B

# Define XC problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

## Define section geometry.
### Section dimensions.
b= 4.0 # section width.
d= 5.6 # section depth.
bh= 3.0 # width of the hole
dh= 4.6 # depth of the hole

### Exterior contour (strong axis: Z, weak axis: Y).
leftBottomCorner= geom.Pos2d(-d/2.0,-b/2.0)
rightBottomCorner= leftBottomCorner+geom.Vector2d(d,0)
rightTopCorner= geom.Pos2d(d/2.0,b/2.0)
leftTopCorner= rightTopCorner-geom.Vector2d(d,0)
### Interior contour.
leftBottomHoleCorner= geom.Pos2d(-dh/2.0,-bh/2.0)
rightBottomHoleCorner= leftBottomHoleCorner+geom.Vector2d(dh,0)
rightTopHoleCorner= geom.Pos2d(dh/2.0,bh/2.0)
leftTopHoleCorner= rightTopHoleCorner-geom.Vector2d(dh,0)
### Wings and webs contours.
bottomWingContour= geom.Polygon2d([leftBottomCorner, rightBottomCorner, rightBottomHoleCorner, leftBottomHoleCorner])
leftWebContour= geom.Polygon2d([leftBottomCorner, leftBottomHoleCorner, leftTopHoleCorner, leftTopCorner])
rightWebContour= geom.Polygon2d([rightBottomHoleCorner, rightBottomCorner, rightTopCorner, rightTopHoleCorner])
topWingContour= geom.Polygon2d([leftTopHoleCorner, rightTopHoleCorner, rightTopCorner, leftTopCorner])

### Define section geometry and its regions.
xc_concrete= concrete.defDiagE(preprocessor= preprocessor) # Use an uniaxial elastic material for testing purposes.
rectangularHollowSectionGeometry= preprocessor.getMaterialHandler.newSectionGeometry("rectangularHollowSectionGeometry")
regions= rectangularHollowSectionGeometry.getRegions
bottomWingRegion= regions.newQuadRegion(xc_concrete.name)
bottomWingRegion.setPolygon(bottomWingContour)
topWingRegion= regions.newQuadRegion(xc_concrete.name)
topWingRegion.setPolygon(topWingContour)
leftWebRegion= regions.newQuadRegion(xc_concrete.name)
leftWebRegion.setPolygon(leftWebContour)
rightWebRegion= regions.newQuadRegion(xc_concrete.name)
rightWebRegion.setPolygon(rightWebContour)

# Get some mass properties from the section geometry.
area= rectangularHollowSectionGeometry.getAreaHomogenizedSection(xc_concrete.E)
refArea= b*d-bh*dh
ratio1= abs(area-refArea)/refArea
G= rectangularHollowSectionGeometry.getCenterOfMassHomogenizedSection(xc_concrete.E)
ratio2= math.sqrt(G[0]**2+G[1]**2)
Iy= rectangularHollowSectionGeometry.getIyHomogenizedSection(xc_concrete.E)
refIy= 1/12.0*(d*b**3-dh*bh**3)
ratio3= abs(Iy-refIy)/refIy
Iz= rectangularHollowSectionGeometry.getIzHomogenizedSection(xc_concrete.E)
refIz= 1/12.0*(b*d**3-bh*dh**3)
ratio4= abs(Iz-refIz)/refIz
Pyz= rectangularHollowSectionGeometry.getPyzHomogenizedSection(xc_concrete.E)
ratio5= abs(Pyz)

# Create fiber section.
tileSize= 0.05
## Set tile size.
for region in [bottomWingRegion, topWingRegion, leftWebRegion, rightWebRegion]:
    region.setTileSize(tileSize, tileSize)
materialHandler= preprocessor.getMaterialHandler
rectangularHollowRCSection= materialHandler.newMaterial("fiber_section_3d","rectangularHollowRCSection")
fiberSectionRepr= rectangularHollowRCSection.getFiberSectionRepr() # Get the fiber section representation.
fiberSectionRepr.setGeomNamed(rectangularHollowSectionGeometry.name) # Assign the geometry.
rectangularHollowRCSection.setupFibers() # Create fibers corresponding to the geometry.
fibers= rectangularHollowRCSection.getFibers()

## Compute the same properties from the fibers object and compare them
## with the reference values.
fibersArea= fibers.getArea(1.0)
fibersIz= fibers.getIz(1.0, 0.0) # getIz(factor, y0)
fibersIy= fibers.getIy(1.0, 0.0) # getIz(factor, z0)
fibersPyz= fibers.getPyz(1.0, 0.0, 0.0) # getPyz(factor, y0, z0)
fibersCenterOfMassZ= fibers.getCenterOfMassZ()
fibersCenterOfMassY= fibers.getCenterOfMassY()
ratio6= abs(area-refArea)/refArea
ratio7= math.sqrt(fibersCenterOfMassY**2+fibersCenterOfMassZ**2)
ratio8= abs(fibersIy-refIy)/refIy
ratio9= abs(fibersIz-refIz)/refIz
ratio10= abs(fibersPyz)


'''
areaFmt= "{:.2f}"
iFmt= "{:.3f}"
print('Mass properties obtained from regions.')
print('  area= '+areaFmt.format(area), ' refArea= '+areaFmt.format(refArea)+' ratio1= '+str(ratio1))
print('  G= '+str(G)+' ratio5= '+str(ratio2))
print('  Iy= '+iFmt.format(Iy), ' refIy= '+iFmt.format(refIy)+' ratio2= '+str(ratio3))
print('  Iz= '+iFmt.format(Iz), ' refIz= '+iFmt.format(refIz)+' ratio3= '+str(ratio4))
print('  Pyz= '+iFmt.format(Pyz), ' refPyz= '+iFmt.format(0.0)+' ratio4= '+str(ratio5))
print('\nMass properties obtained from fibers.')
print('  area= '+areaFmt.format(fibersArea), ' refArea= '+areaFmt.format(refArea)+' ratio1= '+str(ratio6))
print('  G= '+str(fibersCenterOfMassY)+' '+str(fibersCenterOfMassZ)+' ratio5= '+str(ratio7))
print('  Iy= '+iFmt.format(fibersIy), ' refIy= '+iFmt.format(refIy)+' ratio2= '+str(ratio8))
print('  Iz= '+iFmt.format(fibersIz), ' refIz= '+iFmt.format(refIz)+' ratio3= '+str(ratio9))
print('  Pyz= '+iFmt.format(fibersPyz), ' refPyz= '+iFmt.format(0.0)+' ratio4= '+str(ratio10))
'''
maxRatioRegions= max(abs(ratio1), abs(ratio2), abs(ratio3), abs(ratio4), abs(ratio5))
maxRatioFibers= max(abs(ratio6), abs(ratio7), abs(ratio8), abs(ratio9), abs(ratio10))

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if maxRatioRegions<1e-10 and maxRatioFibers<1e-4:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
