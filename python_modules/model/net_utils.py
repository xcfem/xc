# -*- coding: utf-8 -*-

import geom

def net2DfromXYlists(preprocessor,abscissae,ordinates):
  puntos= preprocessor.getCad.getPoints
  nets= preprocessor.getCad.get2DNets
  retval= nets.new2DNet()
  retval.dim(len(ordinates),len(abscissae))

  i= 1
  for x in abscissae:
    j= 1
    for y in ordinates:
      pnt= puntos.newPntFromPos3d(geom.Pos3d(x,y,0.0))
      retval.setPnt(j,i,pnt.tag)
      j+= 1
    i+= 1
  return retval


def createSurfacesNet2D(preprocessor,net2D,iSize,jSize):
  surfaces= preprocessor.getCad.getSurfaces
  m= net2D.nRow
  n= net2D.nCol
  for i in range(1,m):
    for j in range(1,n):
      p1= net2D.getPnt(i,j)
      p2= net2D.getPnt(i+1,j)
      p3= net2D.getPnt(i+1,j+1)
      p4= net2D.getPnt(i,j+1)
      s= surfaces.newQuadSurfacePts(p1.tag,p2.tag,p3.tag,p4.tag)
      s.setElemSizeIJ(iSize,jSize)

class Net2DHelper(object):
  net= None #
  def __init__(self, net2D):
    self.net= net2D

  def assignZ(self,zFunc):
    m= self.net.nRow
    n= self.net.nCol
    for i in range(1,m+1):
      for j in range(1,n+1):
        p= self.net.getPnt(i,j)
        pos= p.getPos
        pos.z= zFunc(pos.x,pos.y)

  def createSurfaces(self,preprocessor,iSize,jSize):
    return createSurfacesNet2D(preprocessor,self.net,iSize,jSize)
