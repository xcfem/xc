# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

testQuadRegion= preprocessor.getMaterialHandler.newSectionGeometry("testQuadRegion")
y1= width/2.0
z1= depth/2.0
regions= testQuadRegion.getRegions
steelRegion= regions.newQuadRegion("steel")
steelRegion.nDivIJ= nDivIJ
steelRegion.nDivJK= nDivJK
steelRegion.pMin= geom.Pos2d(y0-y1,z0-z1)
steelRegion.pMax= geom.Pos2d(y0+y1,z0+z1)
