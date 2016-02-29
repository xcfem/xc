# -*- coding: utf-8 -*-

import xc_base
import geom

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def getReactionForces(preprocessor,supportNodes):
  '''Returns a dictionary with the reactions forces at the nodes.
     The key of the map is the node tag.''' 
  reactions= dict()
  meshNodes= preprocessor.getNodeLoader
  print 'meshNodes.numGdls= ', meshNodes.numGdls
  meshNodes.calculateNodalReactions(True)
  for n in supportNodes:
    tmp= n.getReaction
    if(meshNodes.numGdls==3):
      reac= geom.Vector2d(tmp[0],tmp[1])
    else:
      reac= geom.Vector3d(tmp[0],tmp[1],tmp[2])
    reactions[n.tag]= reac
  return reactions

def getReactionMoments(preprocessor,supportNodes):
  '''Returns a dictionary with the reactions moments at the nodes.
     The key of the map is the node tag.''' 
  reactions= dict()
  meshNodes= preprocessor.getNodeLoader
  meshNodes.calculateNodalReactions(True)
  for n in supportNodes:
    tmp= n.getReaction
    if(meshNodes.numGdls==3):
      reac= geom.Vector3d(0.0,0.0,tmp[2])
    else:
      reac= geom.Vector3d(tmp[3],tmp[4],tmp[5])
    reactions[n.tag]= reac
  return reactions


