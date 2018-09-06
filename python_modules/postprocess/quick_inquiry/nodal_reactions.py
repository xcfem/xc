# -*- coding: utf-8 -*-
import xc_base
import geom
import xc

def vectorReacUVW(preprocessor,idNode):
  '''X, Y and Z components of the reaction in the node.

  Parameters:
  :param idNode: node tag.
  '''
  nod= preprocessor.getNodeHandler.getNode(idNode)
  reac= nod.getReaction
  return xc.Vector([reac[0],reac[1],reac[2]])

def getSlidingVectorsSystemfromVDesliz(DOFs,coo,v):
  '''Returns a sliding vector system that represents the vector.

  Parameters:
  :param DOFs: degrees of freedom.
  :param coo: coordinates of a point in the vector line of action.
  :param v: vector.
  '''
  if(DOFs=="UV"):
    return geom.SlidingVectorsSystem2d(geom.Pos2d(coo[0],coo[1]),geom.Vector2d(v[0],v[1]))
  elif(DOFs=="UVR"):
    return geom.SlidingVectorsSystem2d(geom.Pos2d(coo[0],coo[1]),geom.Vector2d(v[0],v[1]),v[2])
  elif(DOFs=="UVW"):
    return geom.SlidingVectorsSystem3d(geom.Pos3d(coo[0],coo[1],coo[2]),geom.Vector3d(v[0],v[1],v[2]))
  elif(DOFs=="UVWRxRyRz"):
    return geom.SlidingVectorsSystem3d(geom.Pos3d(coo[0],coo[1],coo[2]),geom.Vector3d(v[0],v[1],v[2]),geom.Vector3d(v[3],v[4],v[5]))

def getReactionFromSetOfNodes(DOFs,nodeSet):
  nodes= nodeSet.getNodes
  retval= geom.SlidingVectorsSystem3d()
  for n in nodes:
    retval+= getSlidingVectorsSystemfromVDesliz(DOFs,n.get3dCoo,n.getReaction)
  return retval

def getReactionFromNodes(modelNodes,DOFs,nodeTags):
  retval= geom.SlidingVectorsSystem3d()
  for tag in nodeTags:
    n= modelNodes.getNode(tag)
    retval+= getSlidingVectorsSystemfromVDesliz(DOFs,n.get3dCoo,n.getReaction)
  return retval
