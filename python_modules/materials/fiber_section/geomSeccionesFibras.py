# -*- coding: utf-8 -*-
# Definición de un cuadrado.
def gmSeccCuadrado(geomSection, fiberMatName, ld, nD):
  regiones= geomSection.getRegions
  rg= regiones.newQuadRegion(fiberMatName)
  rg.nDivIJ= nD
  rg.nDivJK= nD
  rg.pMin= geom.Pos2d(-ld/2,-ld/2)
  rg.pMax= geom.Pos2d(ld/2,ld/2)
  return rg

# Definición de un rectángulo.
def gmSeccRectangulo(geomSection,fiberMatName, h, b, nDIJ, nDIK):
  regiones= geomSection.getRegions
  rg= regiones.newQuadRegion(fiberMatName)
  rg.nDivIJ= nDIJ
  rg.nDivJK= nDIK
  rg.pMin= geom.Pos2d(-b/2,-h/2)
  rg.pMax= geom.Pos2d(b/2,h/2)
  return rg
