# -*- coding: utf-8 -*-
''' Define piers section geometry.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
from materials.sections.fiber_section import geom_fiber_sect as gfs

def define_pier_section_geometry(preprocessor, name, concrete, steel, p4i, p5i, p6, p6i, p7, p8, p9, p9i, p10, p10i, r1Reinf, r2Reinf, r3Reinf, r9Reinf, r10Reinf, r11Reinf):
    ''' Define the geometry of a pier section.

    :param preprocessor: FE problem preprocessor.
    :param name: section name.
    :param p4i: point defining the section geometry.
    :param p10i: point defining the section geometry.
    :param r1Reinf: tuple defining number of rebars and its diameter.
    :param r2Reinf: tuple defining number of rebars and its diameter.
    :param r3Reinf: tuple defining number of rebars and its diameter.
    :param r9Reinf: tuple defining number of rebars and its diameter.
    :param r10Reinf: tuple defining number of rebars and its diameter.
    :param r11einf: tuple defining number of rebars and its diameter.
    '''
    # Section geometry
    tileSize= 0.05
    xAxis= geom.Line2d(geom.Pos2d(0,0), geom.Vector2d(1, 0))
    yAxis= geom.Line2d(geom.Pos2d(0,0), geom.Vector2d(0, 1))
    ## creation
    matHandler= preprocessor.getMaterialHandler
    retval= matHandler.newSectionGeometry("P32_01")
    ## filling with regions
    regions= retval.getRegions
    ## generation of a quadrilateral regions
    ### Top region.
    topRegion= regions.newQuadRegion(concrete.getDDiagName()) # name of material strain-stress diagram.
    topRegion.pMin= geom.Pos2d(p10i.x, p10i.y)
    topRegion.pMax= geom.Pos2d(p9.x, p9.y)
    topRegion.setTileSize(tileSize, tileSize)
    ### Bottom region
    bottomRegion= gfs.reflect_region(regions= regions, region= topRegion, symAxis= xAxis)
    ### Upper right spall region.
    urSpallRegion= regions.newQuadRegion(concrete.getDDiagName()) # name of material strain-stress diagram.
    urSpallRegion.setVertices(p9i, p6i, p8, p9)
    urSpallRegion.setTileSize(tileSize, tileSize)
    ### Lower right spall region.
    lrSpallRegion= gfs.reflect_region(regions= regions, region= urSpallRegion, symAxis= xAxis)
    ### Lower left spall region.
    llSpallRegion= gfs.reflect_region(regions= regions, region= lrSpallRegion, symAxis= yAxis)
    ### Upper left spall region.
    ulSpallRegion= gfs.reflect_region(regions= regions, region= urSpallRegion, symAxis= yAxis)
    ### Upper right chamfer region.
    urChamferRegion= regions.newQuadRegion(concrete.getDDiagName())
    urChamferRegion.setVertices(p8, p7, p5i, p6i)
    urChamferRegion.swap()
    urChamferRegion.setTileSize(tileSize, tileSize)
    ### Lower right chamfer region.
    lrChamferRegion= gfs.reflect_region(regions= regions, region= urChamferRegion, symAxis= xAxis)
    ### Lower left chamfer region.
    llChamferRegion= gfs.reflect_region(regions= regions, region= lrChamferRegion, symAxis= yAxis)
    ### Upper left chamfer region.
    ulChamferRegion= gfs.reflect_region(regions= regions, region= urChamferRegion, symAxis= yAxis)
    ### Right side region.
    rightSideRegion= regions.newQuadRegion(concrete.getDDiagName())
    rightSideRegion.setVertices(p7, p6, p4i, p5i)
    rightSideRegion.swap()
    rightSideRegion.setTileSize(tileSize, tileSize)
    ### Left side region
    leftSideRegion= gfs.reflect_region(regions= regions, region= rightSideRegion, symAxis= yAxis)

    # Section reinforcement.
    cover= 35e-3
    steelDiagTag= steel.defDiagD(preprocessor)
    steelDiag= steel.getDiagD(preprocessor)

    reinforcement= retval.getReinfLayers
    ## R1
    r1= reinforcement.newStraightReinfLayer(steelDiag.name)
    r1.numReinfBars= r1Reinf[0]
    r1.barDiameter= r1Reinf[1]
    r1CoverVector= geom.Vector2d(0.0, -cover)
    r1.setP1P2(p10+r1CoverVector, p9+r1CoverVector)
    ## R2
    r2= reinforcement.newStraightReinfLayer(steelDiag.name)
    r2.numReinfBars= r2Reinf[0]
    r2.barDiameter= r2Reinf[1]
    chamferVector= (p8-p7).normalized()
    chamferNormal= geom.Vector2d(-chamferVector.y, chamferVector.x)
    r2.setP1P2(p8-cover*chamferVector+cover*chamferNormal, p7+cover*chamferVector+cover*chamferNormal)
    ## R3
    r3= reinforcement.newStraightReinfLayer(steelDiag.name)
    r3.numReinfBars= r3Reinf[0]
    r3.barDiameter= r3Reinf[1]
    r3TangentVector= (p6-p7).normalized()
    r3NormalVector= geom.Vector2d(-cover, 0.0).normalized()
    r3.setP1P2(p7+cover*r3TangentVector+cover*r3NormalVector, p6-cover*r3TangentVector+cover*r3NormalVector)
    ## R4
    r4= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r2, symAxis= xAxis)
    ## R5
    r5= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r1, symAxis= xAxis)
    ## R6
    r6= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r4, symAxis= yAxis)
    ## R7
    r7= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r3, symAxis= yAxis)
    ## R8
    r8= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r2, symAxis= yAxis)
    ## R9
    r9= reinforcement.newStraightReinfLayer(steelDiag.name)
    r9.numReinfBars= r9Reinf[0]
    r9.barDiameter= r9Reinf[1]
    r9CoverVector= geom.Vector2d(0.0, cover)
    r9.setP1P2(p10i+r9CoverVector, p9i+r9CoverVector)
    ## R10
    r10= reinforcement.newStraightReinfLayer(steelDiag.name)
    r10.numReinfBars= r10Reinf[0]
    r10.barDiameter= r10Reinf[1]
    chamferVector= (p6i-p5i).normalized()
    chamferNormal= geom.Vector2d(-chamferVector.y, chamferVector.x)
    r10.setP1P2(p6i-cover*chamferVector-cover*chamferNormal, p5i+cover*chamferVector-cover*chamferNormal)
    ## R11
    r11= reinforcement.newStraightReinfLayer(steelDiag.name)
    r11.numReinfBars= r11Reinf[0]
    r11.barDiameter= r11Reinf[1]
    r11TangentVector= (p5i-p4i).normalized()
    r11NormalVector= geom.Vector2d(-cover, 0.0).normalized()
    r11.setP1P2(p5i-cover*r11TangentVector-cover*r11NormalVector, p4i+cover*r11TangentVector-cover*r11NormalVector)
    ## R12
    r12= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r10, symAxis= xAxis)
    ## R13
    r13= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r9, symAxis= xAxis)
    ## R14
    r14= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r12, symAxis= yAxis)
    ## R15
    r15= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r11, symAxis= yAxis)
    ## R16
    r16= gfs.reflect_straight_reinf_layer(reinforcement, reinfLayer= r10, symAxis= yAxis)
    return retval
