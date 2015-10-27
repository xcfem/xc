# -*- coding: utf-8 -*-
geomCuadFibrasTN= preprocessor.getMaterialLoader.newSectionGeometry("geomCuadFibrasTN")
y1= ancho/2.0
z1= canto/2.0
regiones= geomCuadFibrasTN.getRegions
acero= regiones.newQuadRegion("acero")
acero.nDivIJ= nDivIJ
acero.nDivJK= nDivJK
acero.pMin= geom.Pos2d(y0-y1,z0-z1)
acero.pMax= geom.Pos2d(y0+y1,z0+z1)
