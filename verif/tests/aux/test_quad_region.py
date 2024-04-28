# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom

def get_test_quad_region(preprocessor, y0, z0, width, depth, nDivIJ, nDivJK):
    ''' Return a quad region for test purposes.

    :param preprocessor: pre-processor for the finite element problem.
    :param y0: "y" coordinate of the region center.
    :param y0: "z" coordinate of the region center.
    '''
    retval= preprocessor.getMaterialHandler.newSectionGeometry("testQuadRegion")
    y1= width/2.0
    z1= depth/2.0
    regions= retval.getRegions
    steelRegion= regions.newQuadRegion("steel")
    steelRegion.nDivIJ= nDivIJ
    steelRegion.nDivJK= nDivJK
    steelRegion.pMin= geom.Pos2d(y0-y1,z0-z1)
    steelRegion.pMax= geom.Pos2d(y0+y1,z0+z1)
    return retval
