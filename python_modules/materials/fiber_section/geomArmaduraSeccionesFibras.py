# -*- coding: utf-8 -*-


def gmHorizRowRebars(sectionGeom, fiberMatName, nRebars, areaRebar, depth, width, cover, h):
  '''Horizontal row of reinforcement bars, placed at a distance h 
  from the median plane (h + towards the top face)
  '''
  reinforcement= sectionGeom.getReinfLayers
  reinfH= reinforcement.newStraightReinfLayer(fiberMatName)
  reinfH.numReinfBars= nRebars
  reinfH.barArea= areaRebar
  reinfH.p1= geom.Pos2d(h,cover-width/2) # Armadura superior.
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

