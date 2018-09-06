# -*- coding: utf-8 -*-
''' Reactions on nodes.'''
import xc_base
import geom

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

class Reactions(object):
  def __init__(self,preprocessor,supportNodes):
    self.forces= dict()
    self.moments= dict()
    self.positions= dict()
    self.svdReac= geom.SlidingVectorsSystem3d()
    meshNodes= preprocessor.getNodeHandler
    meshNodes.calculateNodalReactions(True, 1e-7)
    f3d= geom.Vector3d(0.0,0.0,0.0)
    m3d= geom.Vector3d(0.0,0.0,0.0)
    for n in supportNodes:
      tag= n.tag
      tmp= n.getReaction
      if(meshNodes.numDOFs==3):
        force= geom.Vector2d(tmp[0],tmp[1])
        f3d= geom.Vector3d(tmp[0],tmp[1],0.0)
        moment= geom.Vector3d(0.0,0.0,tmp[2])
        m3d= moment
      else:
        force= geom.Vector3d(tmp[0],tmp[1],tmp[2])
        f3d= force
        moment= geom.Vector3d(tmp[3],tmp[4],tmp[5])
        m3d= moment
      self.forces[tag]= force
      self.moments[tag]= moment
      pos= n.getInitialPos3d
      self.positions[tag]= pos
      self.svdReac+= geom.SlidingVectorsSystem3d(pos,f3d,m3d)
  def getReactionForces(self):
    '''Returns a dictionary with the reactions forces at the nodes.
       The key of the map is the node tag.''' 
    return self.forces

  def getReactionMoments(self):
    '''Returns a dictionary with the reactions moments at the nodes.
       The key of the map is the node tag.''' 
    return self.moments

  def getResultant(self):
    return self.svdReac


