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
  def __init__(self,preprocessor,supportNodes, inclInertia= False):
      ''' Constructor.

      :param preprocessor: XC preprocessor of the problem.
      :param supportNodes: nodes to compute reactions.
      :param inclInertia: include inertia effects (defaults to false).
      '''
      self.forces= dict()
      self.moments= dict()
      self.positions= dict()
      self.svdReac= geom.SlidingVectorsSystem3d()
      nodeHandler= preprocessor.getNodeHandler
      nodeHandler.calculateNodalReactions(inclInertia, 1e-7)
      f3d= geom.Vector3d(0.0,0.0,0.0)
      m3d= geom.Vector3d(0.0,0.0,0.0)
      for n in supportNodes:
          tag= n.tag
          tmp= n.getReaction
          f3d= n.getReactionForce3d
          m3d= n.getReactionMoment3d
          if(nodeHandler.numDOFs==3):
              force= geom.Vector2d(f3d.x,f3d.y)
          else:
              force= f3d
          self.forces[tag]= force
          self.moments[tag]= m3d
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

  def getResultantSVS(self):
      return self.svdReac


