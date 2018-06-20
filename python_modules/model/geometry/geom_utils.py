# -*- coding: utf-8 -*-

''' Utilities for geometry entities related manipulations. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg
from postprocess import extrapolate_elem_attr as extrap
import numpy as np
import math
import geom
from model.geometry import grid_model as gm

class LineWrapper(object):
  def __init__(self,line):
    self.line= line

  def getOrderedNodes(self):
    '''Returns line's interior nodes.'''
    nNodes= self.line.getNumNodes
    retval= []
    for i in range(0,nNodes):
      retval.append(self.line.getNodeI(i+1))
    return retval

  def getElements(self):
    return self.line.getElements()

  def getNodeAbcissae(self):
    '''Returns node's abcissae; first node x= 0.0 last node x= lengthOfLine.'''
    retval= list()
    nodes= self.getOrderedNodes()
    for n in nodes:
      retval.append(self.line.getLambda(n.getInitialPos3d))
    return retval

  def getNodeAttrOrdinates(self,attributeName):
    '''Return a list with the values of attribute at each node.'''
    retval= list()
    nodes= self.getOrderedNodes()
    for n in nodes:
      retval.append(n.getProp(attributeName))
    return retval

  def getElementFunctionOrdinates(self,factor0, funcName0, factor1, funcName1):
    '''Return a list with the values of attribute extrapolated at each node.'''
    retval= list()
    indices= dict()
    nodes= self.getOrderedNodes()
    count= 0
    for n in nodes:
      retval.append(0.0) #Initialize values
      indices[n.tag]= count
      count+= 1
    for e in self.getElements():
      f0= factor0*getattr(e, funcName0)
      f1= factor1*getattr(e, funcName1)
      i0= indices[e.getNodes[0].tag]
      retval[i0]+= 0.5*f0
      i1= indices[e.getNodes[1].tag]
      retval[i1]+= 0.5*f1
    retval[0]*=2 #Extreme nodes.
    retval[-1]*=2
    return retval

  def getInteriorNodes(self):
    '''Returns line's interior nodes.'''
    nNodes= self.line.getNumNodes
    retval= []
    for i in range(2,nNodes):
      n= self.line.getNodeI(i)
      retval.append(n)
    return retval

  def getFirstNode(self):
    '''Returns line's first node.'''
    return self.line.getNodeI(1)

  def getLastNode(self):
    '''Return line's last node.'''
    nNodes= self.line.getNumNodes
    return self.line.getNodeI(nNodes-1)
  

def getInteriorNodes(line):
  '''DEPRECATED; returns line's interior nodes.'''
  lmsg.warning('getInteriorNodes DEPRECATED; use LineWrapper.getInteriorNodes.')
  lw= LineWrapper(line)
  return lw.getInteriorNodes()

def getFirstNode(line):
  '''DEPRECATED; returns line's first node.'''
  lmsg.warning('getFirstNode DEPRECATED; use LineWrapper.getFirstNode.')
  lw= LineWrapper(line)
  return lw.getFirstNode()

# Returns line's last node.
def getLastNode(line):
  '''DEPRECATED; returns line's last node.'''
  lmsg.warning('getLastNode DEPRECATED; use LineWrapper.getLastNode.')
  lw= LineWrapper(line)
  return lw.getLastNode()

#Exact parabola
def fit_parabola(x, y):
    '''Fits the equation "y = ax^2 + bx + c" given exactly 3 points as two
    lists or arrays of x & y coordinates
    '''
    A = np.zeros((3,3), dtype=np.float)
    A[:,0] = x**2
    A[:,1] = x
    A[:,2] = 1
    a, b, c = np.linalg.solve(A, y)
    return a, b, c

def eq_points_parabola(startS,stopS,numPts,a,b,c,angSX):
    '''Returns equispaced nPts points of the parabola "z=as^2 + bs + c" 
    in the range [startS,stopS].
    S is a coordinate by the straight line projection of the parabola on the XY plane.
    angSX is the angle (in rad) between that line and the X-axis
    '''
    s_parab=np.linspace(startS,stopS,numPts)
    x_parab=np.linspace(startS*math.cos(angSX),stopS*math.cos(angSX),numPts)
    y_parab=np.linspace(startS*math.sin(angSX),stopS*math.sin(angSX),numPts)
    z_parab=a*s_parab**2+b*s_parab+c
    return x_parab,y_parab,z_parab

def rect2DPolygon(xCent,yCent,Lx,Ly):
    '''Rectangular polygon
    '''
    pol=geom.Poligono2d()
    pol.agregaVertice(geom.Pos2d(xCent-Lx/2.0,yCent-Ly/2.0))
    pol.agregaVertice(geom.Pos2d(xCent-Lx/2.0,yCent+Ly/2.0))
    pol.agregaVertice(geom.Pos2d(xCent+Lx/2.0,yCent+Ly/2.0))
    pol.agregaVertice(geom.Pos2d(xCent+Lx/2.0,yCent-Ly/2.0))
    return pol

def def_rg_cooLim(XYZLists,Xcoo,Ycoo,Zcoo):
  '''Return an XYZ range given the three lists of coordinates of the grid
  and the limit coordinates.

  :param XYZList: (xList,yList,zList)
  :param Xcoo: (xmin,xmax)
  :param Ycoo: (ymin,ymax)
  :param Zcoo: (zmin,zmax)
  '''
  xLst=XYZLists[0]
  yLst=XYZLists[1]
  zLst=XYZLists[2]
  rg=gm.IJKRange((xLst.index(Xcoo[0]),yLst.index(Ycoo[0]),zLst.index(Zcoo[0])),(xLst.index(Xcoo[1]),yLst.index(Ycoo[1]),zLst.index(Zcoo[1])))
  return rg
                                                                                
