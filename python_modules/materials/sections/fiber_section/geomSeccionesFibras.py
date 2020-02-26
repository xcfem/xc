# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"

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
