# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

testQuadRegion= preprocessor.getMaterialLoader.newSectionGeometry("testQuadRegion")
y1= width/2.0
z1= depth/2.0
regiones= testQuadRegion.getRegions
acero= regiones.newQuadRegion("acero")
acero.nDivIJ= nDivIJ
acero.nDivJK= nDivJK
acero.pMin= geom.Pos2d(y0-y1,z0-z1)
acero.pMax= geom.Pos2d(y0+y1,z0+z1)
