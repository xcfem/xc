# -*- coding: utf-8 -*-
''' Define cross-section for concrete frame pushover analysis. 

Example based on the OpenSees one:
See https://openseespydoc.readthedocs.io/en/latest/src/RCFramePushOver.html
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
import geom
from materials import typical_materials

def def_rc_column_fiber_section(preprocessor, As):
    ''' Define rectangular RC section for push-over test on RC frame.

    :param As: area of each rebar.
    ''' 
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
    leftBottomCorner= geom.Pos2d(-z1, -y1)
    rightBottomCorner= leftBottomCorner+widthVector
    rightTopCorner= rightBottomCorner+depthVector
    leftTopCorner= rightTopCorner-widthVector
    # Colum core corners.
    coreLeftBottomCorner= geom.Pos2d(cover - z1, cover - y1)
    coreRightBottomCorner= coreLeftBottomCorner+coreWidthVector
    coreRightTopCorner= coreRightBottomCorner+coreDepthVector
    coreLeftTopCorner= coreRightTopCorner-coreWidthVector
    # # Rotate section
    # leftBottomCorner= geom.Pos2d(-leftBottomCorner.y, leftBottomCorner.x)
    # rightBottomCorner= geom.Pos2d(-rightBottomCorner.y, rightBottomCorner.x)
    # rightTopCorner= geom.Pos2d(-rightTopCorner.y, rightTopCorner.x)
    # leftTopCorner= geom.Pos2d(-leftTopCorner.y, leftTopCorner.x)
    # coreLeftBottomCorner= geom.Pos2d(-coreLeftBottomCorner.y, coreLeftBottomCorner.x)
    # coreRightBottomCorner= geom.Pos2d(-coreRightBottomCorner.y, coreRightBottomCorner.x)
    # coreRightTopCorner= geom.Pos2d(-coreRightTopCorner.y, coreRightTopCorner.x)
    # coreLeftTopCorner= geom.Pos2d(-coreLeftTopCorner.y, coreLeftTopCorner.x)

    # Contour of the section core.
    coreContour= geom.Polygon2d([coreLeftBottomCorner, coreRightBottomCorner, coreRightTopCorner, coreLeftTopCorner])
    # Contours of the section cover.
    ## Bottom cover
    bottomCoverContour= geom.Polygon2d([leftBottomCorner, rightBottomCorner, coreRightBottomCorner, coreLeftBottomCorner])
    ## Right cover
    rightCoverContour= geom.Polygon2d([coreRightBottomCorner, rightBottomCorner, rightTopCorner, coreRightTopCorner])
    print(rightCoverContour.getArea())
    ## Top cover
    topCoverContour= geom.Polygon2d([coreLeftTopCorner, coreRightTopCorner, rightTopCorner, leftTopCorner])
    ## Left cover.
    leftCoverContour= geom.Polygon2d([leftBottomCorner, coreLeftBottomCorner, coreLeftTopCorner, leftTopCorner])
    print(leftCoverContour.getArea())

    matHandler= preprocessor.getMaterialHandler
    columnSectionGeometry= matHandler.newSectionGeometry("columnSectionGeometry")
    columnSectionGeometryRegions= columnSectionGeometry.getRegions
    # Create the concrete core fibers
    nDivY= 1
    nDivZ= 10
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
    rightCoverRegion.nDivIJ= int(nDivZ/2.0)
    rightCoverRegion.nDivJK= nDivY
    # Top cover
    topCoverRegion= columnSectionGeometryRegions.newQuadRegion(coverConcrete.name)
    topCoverRegion.setPolygon(topCoverContour)
    topCoverRegion.nDivIJ= nDivZ
    topCoverRegion.nDivJK= nDivY
    # Left cover
    leftCoverRegion= columnSectionGeometryRegions.newQuadRegion(coverConcrete.name)
    leftCoverRegion.setPolygon(leftCoverContour)
    leftCoverRegion.nDivIJ= int(nDivZ/2.0)
    leftCoverRegion.nDivJK= nDivY

    regions_area= columnSectionGeometryRegions.getAreaGrossSection()
    regions_reference_area= colWidth*colDepth
    regions_G= columnSectionGeometryRegions.getCenterOfMassGrossSection()
    regions_yG= regions_G[1]
    regions_zG= regions_G[0]
    print('regions area: ', regions_area)
    print('regions reference area: ', regions_reference_area)
    print('regions yG= ', regions_yG)
    print('regions zG= ', regions_zG)


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

    # Check some cross-section values.
    fiber_section_area= columnFiberSection.getArea()
    fiber_section_reference_area= colWidth*colDepth+8*As
    ratio1= abs(fiber_section_area-fiber_section_reference_area)/fiber_section_reference_area
    fiber_section_yG= columnFiberSection.getCenterOfMassY()
    fiber_section_zG= columnFiberSection.getCenterOfMassZ()

    print('\nfiber section area: ', fiber_section_area)
    print('fiber section reference area: ', fiber_section_reference_area)
    print('ratio1= ', ratio1)
    print('fiber section yG= ', fiber_section_yG)
    print('fiber section zG= ', fiber_section_zG)

    return columnFiberSection

