# -*- coding: utf-8 -*-
''' Section geometry related utilities '''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"

import sys
import geom
import xc
from misc_utils import log_messages as lmsg

def gmSquareSection(geomSection, fiberMatName, ld, nD):
    '''returns a square section of fibers of the same material

    :param geomSection:  geometric section
    :param fiberMatName: name of the material that makes up the fibers
    :param ld:           edge length of the square
    :param nD:           number of divisions to be used along the edges
                    of the square
    '''
    regions= geomSection.getRegions
    rg= regions.newQuadRegion(fiberMatName)
    rg.nDivIJ= nD
    rg.nDivJK= nD
    rg.pMin= geom.Pos2d(-ld/2,-ld/2)
    rg.pMax= geom.Pos2d(ld/2,ld/2)
    return rg

def gmRectangSection(geomSection,fiberMatName, h, b, nDIJ, nDIK):
    '''returns a rectangular section of fibers of the same material

    :param geomSection:  geometric section
    :param fiberMatName: name of the material that makes up the fibers
    :param h:            length of the height side of the rectangle
    :param b:            length of the width side of the rectangle
    :param nDIJ:         number of divisions to be used along the IJ (width)
                    direction
    :param nDJK:         number of divisions to be used along the JK (height)
                    direction
    '''
    regions= geomSection.getRegions
    rg= regions.newQuadRegion(fiberMatName)
    rg.nDivIJ= nDIJ
    rg.nDivJK= nDIK
    rg.pMin= geom.Pos2d(-b/2,-h/2)
    rg.pMax= geom.Pos2d(b/2,h/2)
    return rg

def reflect_region(regions, region, symAxis):
    ''' Define a region symmetric to the given one.

    :param regions: region container
    :param region: original region.
    :param symAxis: symmetry axis.
    '''
    retval= None
    vertices= region.getPolygon().getVertices()
    reflection2d= geom.Reflection2d(symAxis)
    sim_vertices= list()
    for v in vertices:
        sv= reflection2d.getTransformed(v)
        sim_vertices.append(sv)
    sim_vertices= list(reversed(sim_vertices))
    if(len(sim_vertices)>4):
        methodName= sys._getframe(0).f_code.co_name
        errMsg= '; not implemented yet for regions with more than 4 vertices.'
        lmsg.error(methodName+errMsg)
        exit(1)
    else:
        retval= regions.newQuadRegion(region.getMaterial().name)
        retval.setVertices(sim_vertices[0], sim_vertices[1], sim_vertices[2], sim_vertices[3])
        retval.setDiscretization(region.nDivIJ, region.nDivJK)
    return retval

def reflect_straight_reinf_layer(reinforcement, reinfLayer, symAxis):
    ''' Define a straight reinforcement layer symmetric to the given one.

    :param reinforcement: reinforcement container
    :param reinfLayer: original reinforcement layer.
    :param symAxis: symmetry axis.
    '''
    retval= reinforcement.newStraightReinfLayer(reinfLayer.getMaterial().name)
    retval.numReinfBars= reinfLayer.numReinfBars
    retval.barDiameter= reinfLayer.barDiameter
    reflection2d= geom.Reflection2d(symAxis)
    retval.p1= reflection2d.getTransformed(reinfLayer.p1)
    retval.p2= reflection2d.getTransformed(reinfLayer.p2)
    return retval

def create_fiber_section(materialHandler, sectionGeometry, fiberSectionType= "fiber_section_3d"):
    ''' Create fiber section from the given geometry.

    :param materialHandler: material handler of the FE problem.
    :param sectionGeometry: geometry of the material distribution in the
                            cross-section.
    '''
    fiberSectionName= sectionGeometry.name+"_"+fiberSectionType
    retval= materialHandler.newMaterial(fiberSectionType, fiberSectionName)
    fiberSectionRepr= retval.getFiberSectionRepr()
    fiberSectionRepr.setGeomNamed(sectionGeometry.name)
    retval.setupFibers()
    return retval

def get_interaction_diagram_parameters(fiberSection):
    ''' Return the interaction diagram parameters for the given fiber section.

    :param fiberSection: fiber section to compute the interaction diagram for.
    '''
    retval= xc.InteractionDiagramParameters()
    sectionGeometry= fiberSection.getSectionGeometry
    baseMaterials= sectionGeometry.getRegionMaterials()
    sz= len(baseMaterials)
    if(sz==1):
        retval.concreteTag= baseMaterials[0].tag
    else:
        methodName= sys._getframe(0).f_code.co_name
        if(sz==0):
            errMsg= '; the list of base materials is empty.'
        elif(sz>1):
            errMsg= '; not implemented for many base materials.'
        lmsg.error(methodName+errMsg)
        exit(1)
    reinforcementMaterials= sectionGeometry.getReinforcementMaterials()
    sz= len(reinforcementMaterials)
    if(sz==1):
        retval.reinforcementTag= reinforcementMaterials[0].tag
    else:
        methodName= sys._getframe(0).f_code.co_name
        if(sz==0):
            errMsg= '; the list of reinforcement materials is empty.'
        elif(sz>1):
            errMsg= '; not implemented for many reinforcement materials.'
        lmsg.error(methodName+errMsg)
        exit(1)
    return retval
    
        
def compute_interaction_diagram(materialHandler, fiberSection):
    ''' Compute the interaction diagram for the given fiber section.

    :param materialHandler: material handler of the FE problem.
    :param fiberSection: fiber section to compute the interaction diagram for.
    '''
    param= get_interaction_diagram_parameters(fiberSection)
    return materialHandler.calcInteractionDiagram(fiberSection.name,param)
