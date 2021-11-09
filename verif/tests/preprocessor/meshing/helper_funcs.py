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
