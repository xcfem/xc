# -*- coding: utf-8 -*-
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "


def getTagNodoPunto(preprocessor,idPunto):
  ''':returns: the tag of the node associated with a point

  :param preprocessor:  preprocessor name
  :param idPunto: tag that identifies the point 
  '''
  return preprocessor.getCad.getPoints.get(idPunto).getTagNode


def getTagNodosPuntos(lstPuntos):
  ''':returns: the tag of the node associated with a point

  :param preprocessor:  preprocessor name
  '''
  retva=[] 
  for p in lstPuntos:
    retval.append(p.getTagNodo())
  return retval

def dist(preprocessor,a,b):
  ''':returns: the distance between two 3D points

  :param preprocessor:  preprocessor name
  :param a: tag that identifies the first point 
  :param b: tag that identifies the second point 
  '''
  puntos= preprocessor.getCad.getPoints
  pA= puntos.get(a).getPos
  pB= puntos.get(b).getPos
  return pA.distPos3d(pB)
