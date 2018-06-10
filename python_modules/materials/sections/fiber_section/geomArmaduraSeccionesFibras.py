# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"


def gmHorizRowRebars(sectionGeom, fiberMatName, nRebars, areaRebar, depth, width, cover, h):
  '''Horizontal row of reinforcement bars, placed at a distance h 
  from the median plane (h + towards the top face)
  Parameters:
      sectionGeom:   geometric section
      fiberMatName:  name identifying the material that makes up the fibers
      nRebars:       number of reinforcing bars to be placed in the row
      areaRebar:     cross-sectional area of each reinforcing bar
      depth:         depth (vertical direction) of the rectangular section
      width:         width (horizontal direction) of the rectangular section
      cover:         effective cover
      h:             vertical distance from the row of rebars to the horizontal
                     simetry axis of the section
  '''
  reinforcement= sectionGeom.getReinfLayers
  reinfH= reinforcement.newStraightReinfLayer(fiberMatName)
  reinfH.numReinfBars= nRebars
  reinfH.barArea= areaRebar
  reinfH.p1= geom.Pos2d(h,cover-width/2) # top layer.
  reinfH.p2= geom.Pos2d(h-cover,width/2-cover)
  return reinfH

def gmBottomRowRebars(sectionGeom,fiberMatName, nRebars, areaRebar, depth, width, cover):
  '''Horizontal row of reinforcement bars in the bottom face '''
  return gmHorizRowRebars(sectionGeom, fiberMatName, nRebars, areaRebar, depth, width, cover, cover-depth/2)

def ggmTopRowRebars(sectionGeom, fiberMatName, nRebars, areaRebar, depth, width, cover):
  '''Horizontal row of reinforcement bars in the top face '''
  return gmHorizRowRebars(sectionGeom, fiberMatName, nRebars, areaRebar, depth, width, cover, depth/2-cover)

def gmSideFaceRebars(fiberMatName, areaRebar, depth, width, cover, h):
  '''Side face row of reinforcement, placed at a distance h 
  from the median plane (h + towards the top face) 
  '''
  return gmHorizRowRebars(sectionGeom, fiberMatName, 2, areaRebar, depth, width, cover, h)

