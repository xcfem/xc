# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import xc
from materials.astm_aisc import ASTM_materials


def getHoleAsPolygonalSurface(
	bolt_fastener: ASTM_materials.BoltFastener,
    surfaces: xc.SurfaceMap,
    points: xc.PntMap
    ) -> xc.PolygonalFace:
    ''' Create the polygonal surface that represents the hole.

    :param bolt_fastener: ASTM bolt fastener material to create the hole surface for.
    :param surfaces: XC surface handler.
    :param points: XC point handler
    '''
    plg= bolt_fastener.getHoleAsPolygon()
    vertices= plg.getVertexList()
    newPnts = [points.newPoint(v) for v in vertices]
    pntTags= [p.tag for p in newPnts]
    pFace= surfaces.newPolygonalFacePts(pntTags)
    return pFace
