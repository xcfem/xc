# -*- coding: utf-8 -*-

''' Utilities for geometry entities related manipulations. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg
from postprocess import extrapolate_elem_attr as extrap
import numpy as np
import math
import geom
from model.geometry import grid_model as gm

class LineWrapper(object):
    def __init__(self,line):
        self.line= line

    def getOrderedNodes(self):
        '''Return line interior nodes.'''
        nNodes= self.line.getNumNodes
        retval= []
        for i in range(0,nNodes):
            retval.append(self.line.getNodeI(i+1))
        return retval

    def getElements(self):
        return self.line.elements

    def getNodeAbcissae(self):
        '''Return node abcissae; first node x= 0.0 last node x= lengthOfLine.'''
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
        for e in self.elements:
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

# Return line's last node.
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
    pol=geom.Polygon2d()
    pol.appendVertex(geom.Pos2d(xCent-Lx/2.0,yCent-Ly/2.0))
    pol.appendVertex(geom.Pos2d(xCent-Lx/2.0,yCent+Ly/2.0))
    pol.appendVertex(geom.Pos2d(xCent+Lx/2.0,yCent+Ly/2.0))
    pol.appendVertex(geom.Pos2d(xCent+Lx/2.0,yCent-Ly/2.0))
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

def lstP3d_from_lstLns(lstLns):
    '''Return an ordered list of vertex (3DPos) from the ordered list of 
    consecutive lines 'lstLns' given as parameter. 
    '''
    prep=lstLns[0].getPreprocessor
    pointsHandler=prep.getMultiBlockTopology.getPoints
    lstP3d=[pointsHandler.get(l.getKPoints()[0]).getPos for l in lstLns]
    lstP3d.append(pointsHandler.get(lstLns[-1].getKPoints()[1]).getPos)
    return lstP3d

def lstLns_from_lstPnts(lstPnts):
    '''Return an ordered list of consecutive lines  from the ordered list of 
    points 'lstPnts' given as parameter. 
    '''
    prep=lstPnts[0].getPreprocessor
    lstLn=[get_lin_2Pts(lstPnts[i],lstPnts[i+1]) for i in range (len(lstPnts)-1)]
    lstLn.append(get_lin_2Pts(lstPnts[-2],lstPnts[-1]))
    return lstLn


def get_lin_2Pts(pnt1,pnt2,setSrchLin=None):
    '''return the line that belongs to the set `,setSrchLin` and whose
    starting and ending points are `pnt1` and `pnt2` (in this order)
    If no set is defined, total set is adopted
    '''
    broke_out= False
    tgp1=pnt1.tag
    tgp2=pnt2.tag
    if not setSrchLin:
        setSrchLin=pnt1.getPreprocessor.getSets.getSet('total').getLines
    for l in setSrchLin:
        extr= l.getKPoints()
        if (extr[0]== tgp1 and extr[1]== tgp2):
            broke_out= True
            break
    if not broke_out:
#        print "Can't find the line"
        return
    else:
        return l
    
def lstEquPnts_from_polyline(pol,nDiv):
    '''Return a list of nDiv+1 equally spaced points (3dPos) in polyline 'pol'
    
    :param pol: polyline (as defined with geom.Polyline3d())
    :param nDiv: number of divisions to split polyline
    '''
    L=pol.getLength()
    nTotSegm=pol.getNumSegments()
    eqDistCP=L/nDiv         
    nmbCP=nDiv-1            #number of intermediate control points
    cumLength=0             #cumulate length in the polyline
    nmbSegm=1               #starting number of segment
    retval=[pol.getSegment(1).getFromPoint()]
    for i in range(1,nmbCP+1):    #intermediate points
        lengthCP=i*eqDistCP-cumLength
        for j in range (nmbSegm,nTotSegm+1):
            sg=pol.getSegment(j)
            LSegm=sg.getLength()
            if lengthCP<LSegm:
                retval.append(sg.getPoint(lengthCP/LSegm))
                break
            else:
                nmbSegm+=1
                cumLength+=LSegm
                lengthCP-=LSegm
    retval.append(pol.getSegment(nTotSegm).getToPoint())
    return retval
    
  
def lines_on_path(preprocessor, path, elementLength):
    '''Return a list of lines between the points on the path
    
    :param path: point sequence
    :param elementLength: length of the element for the lines
    '''
    retval= []
    lines= preprocessor.getMultiBlockTopology.getLines
    for points in zip(path, path[1:]):
        newLine= lines.newLine(points[0].tag,points[1].tag)
        newLine.setElemSize(elementLength)
        retval.append(newLine)
    return retval

def create_set_lines_from_pairs_points(preprocessor,lstPairPoints,setName):
    '''Return a set of lines created between the pairs of points in list
    'lstPairPoints'

    :param preprocessor: preprocessor
    :param lstPairPoints: list of point pairs [[pnt1,pnt2],[pnt3,pnt4], ...]
    :param setName: name of the set to be created
    '''
    setLin=preprocessor.getSets.defSet(setName)
    lines= preprocessor.getMultiBlockTopology.getLines
    for pairPnt in lstPairPoints:
        l=lines.newLine(pairPnt[0].tag,pairPnt[1].tag)
        setLin.getLines.append(l)
    return setLin
