# -*- coding: utf-8 -*-
import xc_base
import geom
import xc

def vectorReacUVW(preprocessor,idNodo):
  # Reacciones según los ejes XYZ del nodo cuyo código se pasa como parámetro.
  nod= preprocessor.getNodeLoader.getNode(idNodo)
  reac= nod.getReaction
  return xc.Vector([reac[0],reac[1],reac[2]])

def getSVDfromVDesliz(DOFs,coo,v):
  # Returns a SVD which represents vector.
  if(DOFs=="UV"):
    return geom.SVD2d(geom.Pos2d(coo[0],coo[1]),geom.Vector2d(v[0],v[1]))
  elif(DOFs=="UVR"):
    return geom.SVD2d(geom.Pos2d(coo[0],coo[1]),geom.Vector2d(v[0],v[1]),v[2])
  elif(DOFs=="UVW"):
    return geom.SVD3d(geom.Pos3d(coo[0],coo[1],coo[2]),geom.Vector3d(v[0],v[1],v[2]))
  elif(DOFs=="UVWRxRyRz"):
    return geom.SVD3d(geom.Pos3d(coo[0],coo[1],coo[2]),geom.Vector3d(v[0],v[1],v[2]),geom.Vector3d(v[3],v[4],v[5]))

def getReactionFromSetOfNodes(DOFs,nodeSet):
  nodes= nodeSet.getNodes
  retval= geom.SVD3d()
  for n in nodes:
    retval+= getSVDfromVDesliz(DOFs,n.get3dCoo,n.getReaction)
  return retval

def getReactionFromNodes(modelNodes,DOFs,nodeTags):
  retval= geom.SVD3d()
  for tag in nodeTags:
    n= modelNodes.getNode(tag)
    retval+= getSVDfromVDesliz(DOFs,n.get3dCoo,n.getReaction)
  return retval
