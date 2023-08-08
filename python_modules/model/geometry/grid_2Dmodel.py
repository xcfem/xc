# -*- coding: utf-8 -*-
'''Particular geometry organization, linked to a framework of spaced points, 
usually referenced by their indices (i,j) that point to the global 
coordinates (x,y).

Generally, an ijGrid contains the grid of points itself and all the 
geometric entities (lines, surfaces, ...) attached to it. Dictionaries with 
those entities, identified by specific names, are created, in order to 
facilitate and accelerate their creation, search, grouping in sets, ...

Several grids can coexist in a FE problem.
'''

from __future__ import print_function

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"


import geom
import math
from misc_utils import data_struct_utils as dsu

class IJRange(object):
    '''Range of indexes (i,j) in the 2D-grid that defines a region 
    bounded by the coordinates associated with those indexes.

    :ivar ijMin: tuple (or list) with minimum value of indexes 
          (minIindex,minJindex) that point to (xmin,ymin)
    :ivar ijMax: tuple (or list) with maximum value of indexes 
          (maxIindex,maxJindex) that point to (xmax,ymax)
    '''
    def __init__(self,ijMin,ijMax):
        if type(ijMin)==list:
            self.ijMin=tuple(ijMin)
        else:
            self.ijMin= ijMin
        if type(ijMax)==list:
            self.ijMax=tuple(ijMax)
        else:
            self.ijMax= ijMax
    def getIMin(self):
        '''return the value of the index I minimum in the range '''
        return self.ijMin[0]
    def getIMax(self):
        '''return the value of the index I maximum in the range '''
        return self.ijMax[0]
    def getJMin(self):
        '''return the value of the index J minimum in the range '''
        return self.ijMin[1]
    def getJMax(self):
        '''return the value of the index J maximum in the range '''
        return self.ijMax[1]
    def getRange(self,index):
        '''return a list with the range of indexes between minIindex and maxIindex '''
        mn= self.ijMin[index]
        mx= self.ijMax[index]+1 #Last index included
        return range(mn,mx)
    def getIRange(self):
        '''return a list with the range of indexes between minIindex
        and maxIindex '''
        return self.getRange(0)
    def getJRange(self):
        '''return a list with the range of index between minJindex and
          maxJindex'''
        return self.getRange(1)

    def __str__(self):
        return 'IRange: ' + str(self.getIRange()) + ' JRange: ' + str(self.getJRange()) 


    def extractIncludedIranges(self,stepJ=1):
        '''return a list with all the sub-ranges included in the IJRange 
        for which the index J is a constant. Graphically 
        it can be seen as the set of lines 'parallel' to global X 
        axis included in the region defined by the IJRange 

        :param stepJ: step to select the lines in Y direction (defaults to 1)
        '''
        (imin,jmin)=self.ijMin
        (imax,jmax)=self.ijMax
        lstIrang=[IJRange((imin,j),(imax,j)) for j in range(jmin,jmax+1,stepJ)]
        return lstIrang

    def extractIncludedJranges(self,stepI=1):
        '''return a list with all the sub-ranges included in the IJRange 
        for which the index I is constant. Graphically it can be seen as
        the set of lines 'parallel' to global Y axis included in the region defined
        by the IJRange 

        :param stepI: step to select the lines in X direction (defaults to 1)
        '''
        (imin,jmin)=self.ijMin
        (imax,jmax)=self.ijMax
        lstJrang=[IJRange((i,jmin),(i,jmax)) for i in range(imin,imax+1,stepI)]
        return lstJrang



class Grid2DModel(object):
    '''Particular geometry organization, linked to a framework of spaced points, 
    usually referenced by their indices (i,j) that point to the global 
    coordinates (x,y).

    Generally, an ijGrid contains the grid of points itself and all the 
    geometric entities (lines, surfaces, ...) attached to it. Dictionaries with 
    those entities, identified by specific names, are created, in order to 
    facilitate and accelerate their creation, search, grouping in sets, ...

    Several grids can coexist in a FE problem.

    :ivar prep: preprocessor
    :ivar xList: - cartesian coord: ordered list of x-coordinates for the grid 
                 - cylindrical coord: ordered list of radius
                 - elliptical coord: ordered list of semi-axes X
    :ivar yList: - cartesian coord: ordered list of y-coordinates for the grid 
                 - cylindrical and elliptical coord: ordered list of angles 
                   expressed in degrees counterclockwise from the X-axis direction
                 -  elliptical coord: ordered list of semi-axes Y
    :ivar xCentCoo, yCentCoo: x and y coordinates of the center point
          in cylindrical or elliptical coordinates
          (both coordinates default to 0)
    :ivar semiYellipList: (only used for elliptical coordinates) ordered list of 
                    semi-axes Y that correspond with each semi-axis X
                    in xList 
    :ivar dicLin: generated dictionary with all the lines linked to the grid. 
          Each key is the name of a line, the value associated is the line 
          itself.
    :ivar dicQuadSurf: generated dictionary with all the surfaces linked to the grid. 
          Each key is the name of a surface, the value associated is the surface 
          itself.


    '''
    def __init__(self,prep,xList,yList,xCentCoo=0,yCentCoo=0,semiYellipList=None):
        self.gridCoo= list()
        self.gridCoo.append(xList)
        self.gridCoo.append(yList)
        self.prep= prep
        self.indices= self.prep.getMultiBlockTopology.get2DNets.new2DNet()
        self.indices.dim(len(self.gridCoo[0]),len(self.gridCoo[1]))
        self.pointCounter=0
        self.dicQuadSurf=dict()
        self.dicLin=dict()
        self.xCentCoo=xCentCoo
        self.yCentCoo=yCentCoo
        self.semiYellipList=semiYellipList

    def lastXIndex(self):
        return len(self.gridCoo[0])-1
    def lastYIndex(self):
        return len(self.gridCoo[1])-1

    
    def getIJfromXY(self,xy):
        '''Return (i,j) indexes that match the closest with (x,y) coordinates
        '''
        ij=(dsu. get_index_closest_inlist(self.gridCoo[0],xy[0]),
             dsu. get_index_closest_inlist(self.gridCoo[1],xy[1]))
        return ij

    def getPntXY(self,xy):
        '''Return the point of the grid closest to coordinates (x,y)'''
        ij=self.getIJfromXY(xy)
        pnt=self.getPntGrid(ij)
        return pnt
    
    def getIJrangeFromXYrange(self,xyRange):
        '''Return an ijRange that matches with closest coordinates in 
        xyRange defined as:
        xyRange=((xmin,ymin),(xmax,ymax))
        '''
        ijRange=IJRange(
            (dsu. get_index_closest_inlist(self.gridCoo[0],xyRange[0][0]),
             dsu. get_index_closest_inlist(self.gridCoo[1],xyRange[0][1])),
            (dsu. get_index_closest_inlist(self.gridCoo[0],xyRange[1][0]),
             dsu. get_index_closest_inlist(self.gridCoo[1],xyRange[1][1])))
        return ijRange
    
    def getPntGrid(self,indPnt):
        '''Return the point at indPnt=(i,j) index of the grid.

        :param indPnt: grid indices that point to the global (X, Y)
                 coordinates    
        '''
        return self.indices.getPnt(indPnt[0]+1,indPnt[1]+1)

    def getTagPntGrid(self,indPnt):
        '''Return the tag of the point at indPnt=(i,j) index of the grid.

        :param indPnt: grid indices that point to the global (X, Y)
                 coordinates    
        '''
        tagPto= self.indices.getPnt(indPnt[0]+1,indPnt[1]+1).tag
        return tagPto

    def gridSurfName(self,pt1,pt2,pt3,pt4):
        '''Return the name of the quadrangle surface defined by 4 points.

        :param pt1,pt2,pt3,pt4: tags of the points (in right order) 
               that define the surface 
        '''
        return 's'+'%06.0f' % pt1 +'%06.0f' % pt2 +'%06.0f' % pt3 +'%06.0f' % pt4


    def gridLinName(self,pt1,pt2):
        '''Return the name of the line defined by 2 points

        :param pt1,pt2: tags of the points (in right order) that
               define the line

        '''
        return 'l'+'%06.0f' % pt1 +'%06.0f' % pt2

    def getNmSurfInRange(self,ijRange,closeCyl='N'):
        '''Return a list with the names of the surfaces limited by a volume 
        defined by the coordinates that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax). 

        :param ijRange: instance of IJRange class
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        (imin,jmin)=ijRange.ijMin
        (imax,jmax)=ijRange.ijMax
        'surfaces in XY plane'
        indPtSurfXY=[((i,j),(i+1,j),(i+1,j+1),(i,j+1)) for j in range(jmin,jmax) for i in range(imin,imax)]
        if closeCyl.lower()[0]=='y':
            indPtSurfXY+=[((i,jmax),(i+1,jmax),(i+1,jmin),(i,jmin)) for i in range(imin,imax)]
        nmSurfXY=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfXY]
        return nmSurfXY

    def getNmLinInRange(self,ijRange):
        '''Return a list with the names of the surfaces limited by a volume 
        defined by the coordinates that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax). 
        '''
        (imin,jmin)=ijRange.ijMin
        (imax,jmax)=ijRange.ijMax
        'lines parallel to X-axis'
        indPtLinX=[((i,j),(i+1,j)) for  i in range(imin,imax) for j in range(jmin,jmax+1)]
        nmLinX=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinX]
        'lines parallel to Y-axis'
        indPtLinY=[((i,j),(i,j+1)) for j in range(jmin,jmax)  for  i in range(imin,imax+1)]
        nmLinY=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinY]
        return (nmLinX+nmLinY)

    def generatePoints(self):
        '''Point generation in a cartesian coordinate system.'''
        points= self.prep.getMultiBlockTopology.getPoints
        inicTag=points.defaultTag
        lstPt=[(i+1,j+1,self.gridCoo[0][i],self.gridCoo[1][j]) for i in range(len(self.gridCoo[0])) for j in range(len(self.gridCoo[1])) ]
        for p in lstPt:
            (i,j,x,y)=p
            self.pointCounter+=1
            pnt=points.newPoint(self.pointCounter+inicTag,geom.Pos2d(x,y))
            self.indices.setPnt(i,j,pnt.tag)

#     def generateCylZPoints(self):
#         '''Point generation in the following cylindrical coordinate system:

#         - Origin: point of coordinates (xCentCoo,yCentCoo,0)
#         - Longitudinal axis: Z
#         - Azimuth expressed in degrees counterclockwise from the X-axis direction
        
#         Coordinates are defined:
        
#         - x: radial coordinate (radius)
#         - y: angular coordinate (degrees counterclockwise from X-axis)
#         - z: height coordinate (Z axis)
#         '''
#         points= self.prep.getMultiBlockTopology.getPoints
#         inicTag=points.defaultTag
#         lstPt=[(i+1,j+1,k+1,
#                 self.xCentCoo+self.gridCoo[0][i]*math.cos(math.radians(self.gridCoo[1][j])),
#                 self.yCentCoo+self.gridCoo[0][i]*math.sin(math.radians(self.gridCoo[1][j])),
#                 self.gridCoo[2][k]) for i in range(len(self.gridCoo[0]))
#                for j in range(len(self.gridCoo[1]))
#                for k in range(len(self.gridCoo[2])) ]
#         for p in lstPt:
#             (i,j,k,x,y,z)=p
#             self.pointCounter+=1
#             pnt=points.newPoint(self.pointCounter+inicTag,geom.Pos3d(x,y,z))
#             self.indices.setPnt(i,j,k,pnt.tag)
            
#     def generateEllipZPoints(self):
#         '''Point generation in the following elliptical coordinate system:

#         - Origin: point of coordinates (xCentCoo,yCentCoo,0)
#         - Longitudinal axis: Z
#         - Azimuth expressed in degrees counterclockwise from the X-axis direction
        
#         Coordinates are defined:
        
#         - x: semi-axis X of the ellipse
#         - y: angular coordinate (degrees counterclockwise from X-axis)
#         - z: height coordinate (Z axis)
#         - semiYellipList contains a list of semi-axis Y of the ellipse
#           that correspond element to element with the semi-axis X in xList
#         '''
#         points= self.prep.getMultiBlockTopology.getPoints
#         inicTag=points.defaultTag
# #        lstPt=[(i+1,j+1,k+1,
# #                self.xCentCoo+self.gridCoo[0][i]*math.cos(math.radians(self.gridCoo[1][j])),
# #                self.yCentCoo+self.gridCoo[0][i]*math.sin(math.radians(self.gridCoo[1][j])),
# #                self.gridCoo[2][k]) for i in range(len(self.gridCoo[0]))
# #               for j in range(len(self.gridCoo[1]))
# #               for k in range(len(self.gridCoo[2])) ]
#         lstPt=list()
#         for i in range(len(self.gridCoo[0])):
#             sx=self.gridCoo[0][i]
#             sy=self.semiYellipList[i]
#             for j in range(len(self.gridCoo[1])):
#                 ang=math.radians(self.gridCoo[1][j])
#                 paramCoo=math.sqrt((math.cos(ang)/sx)**2+(math.sin(ang)/sy)**2)
#                 x=self.xCentCoo+1/paramCoo*math.cos(ang)
#                 y=self.yCentCoo+1/paramCoo*math.sin(ang)
#                 for k in range(len(self.gridCoo[2])):
#                     z=self.gridCoo[2][k]
#                     lstPt.append((i+1,j+1,k+1,x,y,z))
#         for p in lstPt:
#             (i,j,k,x,y,z)=p
#             self.pointCounter+=1
#             pnt=points.newPoint(self.pointCounter+inicTag,geom.Pos3d(x,y,z))
#             self.indices.setPnt(i,j,k,pnt.tag)

    def movePointsRange(self,ijRange,vDisp):
        '''Move points  in a 2D grid-region limited by 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax), to a new position 
        by applying a 2D displacement.

        :param ijRange: range for the search
        :param vDisp: xc vector displacement
        '''
        sPtMove=self.getSetPntRange(ijRange,'sPtMove')
        for p in sPtMove.getPoints:
          p.getPos.x+= vDisp[0]
          p.getPos.y+= vDisp[1]
        sPtMove.clear()

    def moveAllPoints(self,vDisp):
        '''Move all points in the grid to a new position
        by pplying a 2D displacement.

        :param vDisp: xc vector displacement
        '''
        ijRange=IJRange([0,0,0],[self.lastXIndex(),self.lastYIndex()])
        self.movePointsRange(ijRange,vDisp)

    def movePointsXYRange(self,XYrange,vDisp):
        '''Move points  in a 2D grid-region limited by an
        xyRange=((xmin,ymin),(xmax,ymax))
        to a new position by applying a 2D displacement.

        :param ijRange: range for the search
        :param vDisp: xc vector displacement
        '''
        ijRange=self.getIJrangeFromXYrange(XYrange)
        return self.movePointsRange(ijRange,vDisp)

    def movePointXY(self,xy,vDisp):
        '''Move the nearest point in grid xy=(x,y) coordinates
        to a new position by applying a 2D displacement.

        :param xy: grid (x,y) coordinates of the point
        :param vDisp: xc vector displacement xc.Vector([vx,vy])
        '''
        p=self.getPntXY(xy)
        p.getPos.x+= vDisp[0]
        p.getPos.y+= vDisp[1]

    # def slopePointsRange(self,ijRange,slopeX=0,xZeroSlope=0,slopeY=0,yZeroSlope=0):
    #     '''Applies one or two slopes (in X and Y directions) 
    #     to points in a 2D grid-region limited by 
    #     ijRange.ijMin=(indXmin,indYmin) and
    #     ijRange.ijMax=(indXmax,indYmax). Only Z coordinate
    #     of points is modified.

    #     :param ijRange: range for the search.
    #     :param slopeX: slope in X direction, expressed as deltaZ/deltaX 
    #                    (defaults to 0 = no slope applied)
    #     :param xZeroSlope: coordinate X of the "rotation axis".
    #     :param slopeY: slope in Y direction, expressed as deltaZ/deltaY)
    #                    (defaults to 0 = no slope applied)
    #     :param yZeroSlope: coordinate Y of the "rotation axis".
    #     '''
    #     sPtMove=self.getSetPntRange(ijRange,'sPtMove')
    #     for p in sPtMove.getPoints:
    #       p.getPos.z+= slopeX*(p.getPos.x-xZeroSlope)+slopeY*(p.getPos.y-yZeroSlope)
    #     sPtMove.clear()

    # def slopePointsXYrange(self,XYrange,slopeX=0,xZeroSlope=0,slopeY=0,yZeroSlope=0):
    #     '''Applies one or two slopes (in X and Y directions) 
    #     to points in a 3D grid-region limited by an
    #     xyRange=((xmin,ymin),(xmax,ymax))
    #     Only Z coordinate of points is modified.

    #     :param XYrange: range for the search.
    #     :param slopeX: slope in X direction, expressed as deltaZ/deltaX 
    #                    (defaults to 0 = no slope applied)
    #     :param xZeroSlope: coordinate X of the "rotation axis".
    #     :param slopeY: slope in Y direction, expressed as deltaZ/deltaY)
    #                    (defaults to 0 = no slope applied)
    #     :param yZeroSlope: coordinate Y of the "rotation axis".
    #     '''

    #     ijRange=self.getIJrangeFromXYrange(XYrange)
    #     return self.slopePointsRange(ijRange,slopeX,xZeroSlope,slopeY,yZeroSlope)
        
    def rotPntsZAxis(self,ijRange,angle,xyRotCent):
        '''Rotates points in ijRange around a Z axis passing by xyRotCent.

        :param ijRange: range for the search.
        :param angle: rotation angle (degrees)
        :param xyRotCent: coordinates [x,y] of the axis Z of rotation
        '''
        theta=math.radians(angle)
        sinTheta=math.sin(theta)
        cosTheta=math.cos(theta)
        sPtMove=self.getSetPntRange(ijRange,'sPtMove')
        for p in sPtMove.getPoints:
            xp=p.getPos.x
            yp=p.getPos.y
            p.getPos.x= xyRotCent[0]+cosTheta*(xp-xyRotCent[0])-sinTheta*(yp-xyRotCent[1])
            p.getPos.y= xyRotCent[1]+sinTheta*(xp-xyRotCent[0])+cosTheta*(yp-xyRotCent[1])
        sPtMove.clear()

    def rotAllPntsZAxis(self,angle,xyRotCent):
        '''Rotates all points in the grid around a Z axis passing by xyRotCent.

        :param angle: rotation angle (degrees)
        :param xyRotCent: coordinates [x,y] of the axis Z of rotation
        '''
        ijRange=IJRange([0,0,0],[self.lastXIndex(),self.lastYIndex()])
        self.rotPntsZAxis(ijRange,angle,xyRotCent)
    
    # def scaleCoorXPointsRange(self,ijRange,xOrig,scale):
    #     '''Applies a scale in X with origin xOrig (fixed axis: X=xOrig) 
    #     to the points in a 3D grid-region limited by 
    #     ijRange.ijMin=(indXmin,indYmin) and
    #     ijRange.ijMax=(indXmax,indYmax. Only X coordinate
    #     of points is modified in the following way:
    #     x_scaled=xOrig+scale*(x_inic-xOrig)

    #     :param ijRange: range for the search.
    #     :param xOrig: origin X to apply scale (point in axis X=xOrig)
    #                   are not affected by the transformation 
    #     :param scale: scale to apply to X coordinate

    #     '''
    #     sPtXscale=self.getSetPntRange(ijRange,'sPtXscale')
    #     for p in sPtXscale.getPoints:
    #         xpt=p.getPos.x
    #         p.getPos.x= xOrig+scale*(xpt-xOrig)
    #     sPtXscale.clear()

    # def scaleCoorXPointsXYRange(self,XYrange,xOrig,scale):
    #     '''Applies a scale in X with origin xOrig (fixed axis: X=xOrig) 
    #     to the points in a 3D grid-region limited by
    #     one XYrange defined as ((xmin,ymin),(xmax,ymax))
    #     Only X coordinate of points is modified in the following way:
    #     x_scaled=xOrig+scale*(x_inic-xOrig)

    #     :param XYrange:((xmin,ymin,zmin),(xmax,ymax)) range for the search.
    #     :param xOrig: origin X to apply scale (point in axis X=xOrig)
    #                   are not affected by the transformation 
    #     :param scale: scale to apply to X coordinate

    #     '''
    #     IJrange=self.getIJrangeFromXYrange(XYrange)
    #     self.scaleCoorXPointsRange(IJrange,xOrig,scale)
        
    # def scaleCoorYPointsRange(self,ijRange,yOrig,scale):
    #     '''Applies a scale in Y with origin yOrig (fixed axis: y=yOrig) 
    #     to the points in a 3D grid-region limited by 
    #     ijRange.ijMin=(indXmin,indYmin,indZmin) and
    #     ijRange.ijMax=(indXmax,indYmax). Only Y coordinate
    #     of points is modified in the following way:
    #     y_scaled=yOrig+scale*(y_inic-yOrig)

    #     :param ijRange: range for the search.
    #     :param yOrig: origin Y to apply scale (point in axis Y=yOrig)
    #                   are not affected by the transformation 
    #     :param scale: scale to apply to Y coordinate
    #     '''
    #     sPtYscale=self.getSetPntRange(ijRange,'sPtYscale')
    #     for p in sPtYscale.getPoints:
    #         ypt=p.getPos.y
    #         p.getPos.y= yOrig+scale*(ypt-yOrig)
    #     sPtYscale.clear()

    # def scaleCoorYPointsXYrange(self,XYrange,yOrig,scale):
    #     '''Applies a scale in Y with origin yOrig (fixed axis: y=yOrig) 
    #     to the points in a 3D grid-region limited by 
    #     an XYrange defined as ((xmin,ymin,zmin),(xmax,ymax))
    #     Only Y coordinate of points is modified in the following way:
    #     y_scaled=yOrig+scale*(y_inic-yOrig)

    #     :param XYrange:((xmin,ymin,zmin),(xmax,ymax)) range for the search.
    #     :param yOrig: origin Y to apply scale (point in axis Y=yOrig)
    #                   are not affected by the transformation 
    #     :param scale: scale to apply to Y coordinate
    #     '''
    #     IJrange=self.getIJrangeFromXYrange(XYrange)
    #     self.scaleCoorYPointsRange(IJrange,yOrig,scale)
        
    # def scaleCoorZPointsRange(self,ijRange,zOrig,scale):
    #     '''Applies a scale in Z with origin zOrig (fixed axis: z=zOrig) 
    #     to the points in a 3D grid-region limited by 
    #     ijRange.ijMin=(indXmin,indYmin,indZmin) and
    #     ijRange.ijMax=(indXmax,indYmax). Only Z coordinate
    #     of points is modified in the following way:
    #     z_scaled=zOrig+scale*(z_inic-zOrig)

    #     :param ijRange: range for the search.
    #     :param zOrig: origin Z to apply scale (point in axis Z=zOrig)
    #                   are not affected by the transformation 
    #     :param scale: scale to apply to Z coordinate
    #     '''
    #     sPtZscale=self.getSetPntRange(ijRange,'sPtZscale')
    #     for p in sPtZscale.getPoints:
    #         zpt=p.getPos.z
    #         p.getPos.z= zOrig+scale*(zpt-zOrig)
    #     sPtZscale.clear()

    # def scaleCoorZPointsXYrange(self,XYrange,zOrig,scale):
    #     '''Applies a scale in Z with origin zOrig (fixed axis: z=zOrig) 
    #     to the points in a 3D grid-region limited by 
    #     one XYrange defined as ((xmin,ymin,zmin),(xmax,ymax))
    #     Only Z coordinate of points is modified in the following way:
    #     z_scaled=zOrig+scale*(z_inic-zOrig)

    #     :param XYrange:((xmin,ymin,zmin),(xmax,ymax)) range for the search.
    #     :param zOrig: origin Z to apply scale (point in axis Z=zOrig)
    #                   are not affected by the transformation 
    #     :param scale: scale to apply to Z coordinate
    #     '''
    #     IJrange=self.getIJrangeFromXYrange(XYrange)
    #     self.scaleCoorZPointsRange(IJrange,zOrig,scale)
        
    # def moveCylPointsRadius(self,ijRange,radius):
    #     '''Move points in a 3D grid-region limited by the ijRange 
    #     in the cylindrical coordinate system to radius coordinate 
    #     given as parameter
    #     '''
    #     sPtMove=self.getSetPntRange(ijRange,'sPtMove')
    #     for p in sPtMove.getPoints:
    #         vdir=geom.Vector2d(p.getPos.x-self.xCentCoo,p.getPos.y-self.yCentCoo,).normalized()
    #         p.getPos.x= self.xCentCoo+radius*vdir.x
    #         p.getPos.y= self.yCentCoo+radius*vdir.y
            
    #     sPtMove.clear()

    # def movePointsRangeToZcylinder(self,ijRange,xCent,yCent,R):
    #     '''Moves the points in the range to make them belong to 
    #     a cylinder with radius R and axis parallel to global Z passing 
    #     through the point (xCent,yCent,0).
    #     '''
    #     vCent=geom.Vector2d(xCent,yCent)
    #     setPnt=self.getSetPntRange(ijRange,'setPnt')
    #     for p in setPnt.getPoints:
    #         v=geom.Vector2d(p.getPos.x,p.getPos.y)-vCent
    #         vdir= v.normalized()
    #         p.getPos.x=xCent+R*vdir.x
    #         p.getPos.y=yCent+R*vdir.y
    #     setPnt.clear()

    # def movePointsRangeToXcylinder(self,ijRange,yCent,zCent,R):
    #     '''Moves the points in the range to make them belong to 
    #     a cylinder with radius R and axis parallel to global X passing 
    #     through the point (0,yCent,zCent)
    #     '''
    #     vCent=geom.Vector2d(yCent,zCent)
    #     setPnt=self.getSetPntRange(ijRange,'setPnt')
    #     for p in setPnt.getPoints:
    #         v=geom.Vector2d(p.getPos.y,p.getPos.z)-vCent
    #         vdir=v.normalized()
    #         p.getPos.y=yCent+R*vdir.x
    #         p.getPos.z=zCent+R*vdir.y
    #     setPnt.clear()

    # def movePointsRangeToYcylinder(self,ijRange,xCent,zCent,R):
    #     '''Moves the points in the range to make them belong to 
    #     a cylinder with radius R and axis parallel to global Y passing 
    #     through the point (xCent,0,zCent)
    #     '''
    #     vCent=geom.Vector2d(xCent,zCent)
    #     setPnt=self.getSetPntRange(ijRange,'setPnt')
    #     for p in setPnt.getPoints:
    #         v=geom.Vector2d(p.getPos.x,p.getPos.z)-vCent
    #         vdir=v.normalized()
    #         p.getPos.x=xCent+R*vdir.x
    #         p.getPos.z=zCent+R*vdir.y
    #     setPnt.clear()

    # def movePointsEllipse(self,ijRange,semiAxX,semiAXY):
    #     ''''Move points in a 3D grid-region limited by the ijRange 
    #     in the elliptical coordinate system to an ellipse of
    #     semi-axes semiAxX and semiAXY given as parameters
    #     '''
    #     sPtMove=self.getSetPntRange(ijRange,'sPtMove')
    #     for p in sPtMove.getPoints:
    #         ang=math.atan2(p.getPos.y-self.yCentCoo,p.getPos.x-self.xCentCoo)
    #         paramCoo=math.sqrt((math.cos(ang)/semiAxX)**2+(math.sin(ang)/semiAXY)**2)
    #         p.getPos.x=self.xCentCoo+1/paramCoo*math.cos(ang)
    #         p.getPos.y= self.yCentCoo++1/paramCoo*math.sin(ang)
    #     sPtMove.clear()
            
    def newQuadGridSurface(self,surfName):
        '''Generate the quadrangle surface defined by the 4 vertex whose tags
        are implicit in the name of the surface.

        :param surfName: name given to the grid surface
        return the quadrangle surface
        '''
        # points= self.prep.getMultiBlockTopology.getPoints
        (tgPt1,tgPt2,tgPt3,tgPt4)=(int(surfName[1:7]),int(surfName[7:13]),int(surfName[13:19]),int(surfName[19:25]))
        surfaces= self.prep.getMultiBlockTopology.getSurfaces
        qs= surfaces.newQuadSurfacePts(tgPt1,tgPt2,tgPt3,tgPt4)
        qs.name= surfName
        qs.nDivI=1 #initialization values of number of divisions
        qs.nDivJ=1 
        return qs

    def getNameQuadGridSurface(self,ind4Pnt):
        '''Return the name of the quadrangle surface defined by the 4 vertices 
        whose indices in the grid are passed as parameters. 

        :param ind4Pnt: tuple of ordered points defined by their grid indices 
                        (i,j)
        :return: the quadrangle surface
        '''
        (pto1,pto2,pto3,pto4)=tuple([self.getTagPntGrid(ind4Pnt[i]) for i in range(4)])
        nameSurf= self.gridSurfName(pto1,pto2,pto3,pto4)
        return nameSurf

    def getNameGridLine(self,ind2Pnt):
        '''Return the name of the line defined by the 2 vertices 
        whose indices in the grid are passed as parameters. 

        :param ind2Pnt: tuple of ordered points defined by their grid indices 
                        (i,j)
        return the line
        '''
        (pto1,pto2)=tuple([self.getTagPntGrid(ind2Pnt[i]) for i in range(2)])
        nameLin= self.gridLinName(pto1,pto2)
        return nameLin

    def newGridLine(self,linName):
        '''Generate the line defined by the 2 end-points whose tags
        are implicit in the name of the line.

        :param linName: name given to the grid line
        return the line
        '''
        #points= self.prep.getMultiBlockTopology.getPoints
        (tgPt1,tgPt2)=(int(linName[1:7]),int(linName[7:15]))
        lines= self.prep.getMultiBlockTopology.getLines
        ln= lines.newLine(tgPt1,tgPt2)
        ln.name= linName
        ln.nDiv=1 #initialization value
        return ln

    def appendSurfRange(self,ijRange,setSurf,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        and append them to the set named 'setName'.
        Add those surfaces to the dictionary dicQuadSurf.

        :param ijRange: instance of IJRange class
        :param setSurf: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        
        '''
        nmSurfinRang=self.getNmSurfInRange(ijRange,closeCyl)
        for nameSurf in nmSurfinRang:
            s= self.newQuadGridSurface(nameSurf)
            self.dicQuadSurf[nameSurf]= s
            setSurf.getSurfaces.append(s)

    def genSurfOneRegion(self,ijRange,setName,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax).
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param ijRange: instance of IJRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
 
        '''
        setSurf= self.prep.getSets.defSet(setName)
        self.appendSurfRange(ijRange,setSurf,closeCyl)
        return setSurf
    
    def genSurfOneXYRegion(self,xyRange,setName,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        defined in the range  xyRange=((xmin,ymin),(xmax,ymax))
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param xyRange: range xy
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
         '''
        ijRange=self.getIJrangeFromXYrange(xyRange)
        return self.genSurfOneRegion(ijRange,setName,closeCyl)

    def genSurfMultiRegion(self,lstIJRange,setName,closeCyl='N'):
        '''generate the surfaces limited by all the regions included in the 
        list of ijRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax). 
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param lstIJRange: list of instances of IJRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(setName)
        for rg in lstIJRange:
            self.appendSurfRange(rg,setSurf,closeCyl)
        return setSurf

    def genSurfMultiXYRegion(self,lstXYRange,setName,closeCyl='N'):
        '''generate the surfaces limited by all the regions included in the 
        list of xyRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the coordinates in ranges xyRange=((xmin,ymin),(xmax,ymax))
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param lstXYRange: list of ranges xy
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
         '''
        lstIJRange=list()
        for rg in lstXYRange:
            lstIJRange.append(self.getIJrangeFromXYrange(rg))
        return self.genSurfMultiRegion(lstIJRange,setName,closeCyl)
        
    def appendLinRange(self,ijRange,setName):
        '''generate the lines limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        and append them to the set named 'setName'. 
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.getSet(setName)
        nmLinInRang=self.getNmLinInRange(ijRange)
        for nameLin in nmLinInRang:
            l=self.newGridLine(nameLin)
            self.dicLin[nameLin]=l
            setLin.getLines.append(l)
       
    def genLinOneRegion(self,ijRange,setName): 
        '''generate the lines limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.defSet(setName)
        self.appendLinRange(ijRange,setName)
        return setLin

    def genLinOneXYRegion(self,xyRange,setName): 
        '''generate the lines limited by a region defined by the coordinates
        defined in range xyRange=((xmin,ymin),(xmax,ymax)
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        ijRange=self.getIJrangeFromXYrange(xyRange)
        return self.genLinOneRegion(ijRange,setName)
        
    def genLinMultiRegion(self,lstIJRange,setName):
        '''generate the lines limited by all the regions included in the 
        list of ijRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.defSet(setName)
        for rg in lstIJRange:
            self.appendLinRange(rg,setName)
        return setLin

    def genLinMultiXYRegion(self,lstXYRange,setName):
        '''generate the lines limited by all the regions included in the 
        list of coordinate passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the ranges xyRange=((xmin,ymin),(xmax,ymax)
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        lstIJRange=list()
        for rg in lstXYRange:
            lstIJRange.append(self.getIJrangeFromXYrange(rg))
        return self.genLinMultiRegion(lstIJRange,setName)
        
    def getSetSurfOneRegion(self,ijRange,setName,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)

        :param ijRange: instance of IJRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(setName)
        nmSurfinRang=self.getNmSurfInRange(ijRange,closeCyl)
        for nameSurf in nmSurfinRang:
            if nameSurf in self.dicQuadSurf:
                setSurf.getSurfaces.append(self.dicQuadSurf[nameSurf])
        setSurf.fillDownwards()    
        return setSurf

    def getSubsetSurfOneRegion(self,superset,ijRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfOneRegion(ijRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
        
    def getSetSurfOneXYRegion(self,xyRange,setName,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        ijRange=self.getIJrangeFromXYrange(xyRange)
        return self.getSetSurfOneRegion(ijRange,setName,closeCyl)
        
    def getSubsetSurfOneXYRegion(self,superset,xyRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfOneXYRegion(xyRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset

    
    def getSetSurfMultiRegion(self,lstIJRange,setName,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of ijRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)

        :param lstIJRange: list of instances of IJRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(setName)
        for rg in lstIJRange:
            nmSurfinRang=self.getNmSurfInRange(rg,closeCyl)
            for nameSurf in nmSurfinRang:
                if nameSurf in self.dicQuadSurf:
                    setSurf.getSurfaces.append(self.dicQuadSurf[nameSurf])
        setSurf.fillDownwards()    
        return setSurf

    def getSubsetSurfMultiRegion(self,superset,lstIJRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of ijRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfMultiRegion(lstIJRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetSurfMultiXYRegion(self,lstXYRange,setName,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of xyRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to coordinates in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        lstIJRange=list()
        for rg in lstXYRange:
            lstIJRange.append(self.getIJrangeFromXYrange(rg))
        return self.getSetSurfMultiRegion(lstIJRange,setName,closeCyl)
        
    def getSubsetSurfMultiXYRegion(self,superset,lstXYRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of xyRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to coordinates in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfMultiXYRegion(lstXYRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinOneRegion(self,ijRange,setName):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        '''
        setLin= self.prep.getSets.defSet(setName)
        nmLininRang=self.getNmLinInRange(ijRange)
        for nameLin in nmLininRang:
            if nameLin in self.dicLin:
                setLin.getLines.append(self.dicLin[nameLin])
        setLin.fillDownwards()    
        return setLin
        
    def getSubsetLinOneRegion(self,superset,ijRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinOneRegion(ijRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinOneXYRegion(self,xyRange,setName):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        defined in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        ijRange=self.getIJrangeFromXYrange(xyRange)
        return self.getSetLinOneRegion(ijRange,setName)

    def getSubsetLinOneXYRegion(self,superset,xyRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        defined in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinOneXYRegion(xyRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinMultiRegion(self,lstIJRange,setName):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of ijRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        '''
        setLin= self.prep.getSets.defSet(setName)
        for rg in lstIJRange:
            nmLininRang=self.getNmLinInRange(rg)
            for nameLin in nmLininRang:
                if nameLin in self.dicLin:
                    setLin.getLines.append(self.dicLin[nameLin])
        setLin.fillDownwards()    
        return setLin

    def getSubsetLinMultiRegion(self,superset,lstIJRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of ijRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        '''
        subset=self.prep.getSets.defSet(nameSubset)
        if self.prep.getSets.exists('auxSet'):
            self.prep.getSets.removeSet('auxSet') 
        auxSet= self.getSetLinMultiRegion(lstIJRange,'auxSet')
        subset= getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinMultiXYRegion(self,lstXYRange,setName):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of xyRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        defined in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        lstIJRange=list()
        for rg in lstXYRange:
            lstIJRange.append(self.getIJrangeFromXYrange(rg))
        return self.getSetLinMultiRegion(lstIJRange,setName)
        
    def getSubsetLinMultiXYRegion(self,superset,lstXYRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of xyRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        defined in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        subset=self.prep.getSets.defSet(nameSubset)
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinMultiXYRegion(lstXYRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getLstLinRange(self,ijRange):
        '''return a list of lines in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        '''
        retval=list()
        nmLinInRang=self.getNmLinInRange(ijRange)
        for nameLin in nmLinInRang:
            if nameLin in self.dicLin:
                retval.append(self.dicLin[nameLin])
        return retval

    def getLstLinXYRange(self,xyRange):
        '''return a list of lines in a region limited by the coordinates
        defined in range xyRange=((xmin,ymin),(xmax,ymax)
        '''
        ijRange=self.getIJrangeFromXYrange(xyRange)
        return self.getLstLinRange(ijRange)

    def getLstPntRange(self,ijRange):
        '''return the ordered list points in a 3D grid-region limited by 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)

        :param ijRange: range for the search
        '''
        retval=list()
        (imin,jmin)=ijRange.ijMin
        (imax,jmax)=ijRange.ijMax
        lstTagPnt=[self.getTagPntGrid(indPnt=(i,j)) for i in range(imin,imax+1) for j in range(jmin,jmax+1) ]
        points= self.prep.getMultiBlockTopology.getPoints
        for tg in lstTagPnt:
          pnt=points.get(tg)
          retval.append(pnt)
        return retval
    
    def getLstPntXYRange(self,xyRange):
        '''return the ordered list points in a 3D grid-region limited by 
        coordinates defined in xyRange=((xmin,ymin),(xmax,ymax)
        '''
        ijRange=self.getIJrangeFromXYrange(xyRange)
        return self.getLstPntRange(ijRange)
        
    def getSetPntRange(self,ijRange,setName):
        '''return the set of points in a 3D grid-region limited by 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)

        :param ijRange: range for the search
        :param setName: name of the new set of points
        '''
        lstPnt=self.getLstPntRange(ijRange)
        retval= self.prep.getSets.defSet(setName)
        pntsSet=retval.getPoints
        for pnt in lstPnt:
          pntsSet.append(pnt)
        return retval

    def getSetPntXYRange(self,xyRange,setName):
        '''return the set of points in a 3D grid-region limited by 
        coordinates in xyRange=((xmin,ymin),(xmax,ymax)
        '''
        ijRange=self.getIJrangeFromXYrange(xyRange)
        return self.getSetPntRange(ijRange,setName)
        
    def getSetPntMultiRegion(self,lstIJRange,setName):
        '''return the set of points in a all the 3D grid-regions
        included in the list of ijRanges passed as parameter
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijRange.ijMin=(indXmin,indYmin) and
        ijRange.ijMax=(indXmax,indYmax)
        '''
        retval= self.prep.getSets.defSet(setName)
        pntsSet=retval.getPoints
        for rg in lstIJRange:
            lstPnt=self.getLstPntRange(rg)
            for pnt in lstPnt:
                pntsSet.append(pnt)
        return retval
    
    def getSetPntMultiXYRegion(self,lstXYRange,setName):
        '''return the set of points in a all the 3D grid-regions
        included in the list of xyRanges passed as parameter
        Each region defines a volume limited by the coordinates    
        defined in xyRange=((xmin,ymin),(xmax,ymax)
        '''
        lstIJRange=list()
        for rg in lstXYRange:
            lstIJRange.append(self.getIJrangeFromXYrange(rg))
        return self.getSetPntMultiRegion(lstIJRange,setName)

    def genSetLinFromLstGridPnt(self,lstGridPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'lstGridPnt' by their indexes in the grid.

        :param lstGridPnt: list of successive points [(i1,j1),(i2,j2), ...]
        :param setName: name of the set.
        '''
        if self.prep.getSets.exists(setName):
            setLin= self.prep.getSets.getSet(setName)
        else:
            setLin= self.prep.getSets.defSet(setName)
        lines= self.prep.getMultiBlockTopology.getLines
        pntTags=[self.getPntGrid(pind).tag for pind in lstGridPnt]
        for i in range(len(pntTags)-1):
            l=lines.newLine(pntTags[i],pntTags[i+1])
            setLin.getLines.append(l)
            nameLin=self.gridLinName(pntTags[i],pntTags[i+1])
            self.dicLin[nameLin]=l
        return setLin

    def getSetLinFromLstGridPnt(self,lstGridPnt,setName):
        '''return the set of lines that joint the successive points defined 
        in 'lstGridPnt' by their indexes in the grid.

        :param lstGridPnt: list of successive points [(i1,j1),(i2,j2), ...]
        :param setName: name of the set.
        '''
        setLin= self.prep.getSets.defSet(setName)
        for i in range(len(lstGridPnt)-1):
            nameLin=self.getNameGridLine((lstGridPnt[i],lstGridPnt[i+1]))
            setLin.getLines.append(self.dicLin[nameLin])
        return setLin
             
    def genSetLinFromMultiLstGridPnt(self,multiLstGridPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'multiLstGridPnt' by their indexes in the grid.

        :param multiLstGridPnt: list of lists of successive points 
        [[(i1,j1),(i2,j2), ...],[],...]
        :param setName: name of the set.
        '''
        for lstp in  multiLstGridPnt:
            setLin=self.genSetLinFromLstGridPnt(lstp,setName)
        return setLin
    
    def getSetLinFromMultiLstGridPnt(self,multiLstGridPnt,setName):
        '''retur the set of lines that joint the successive points defined 
        in 'multiLstGridPnt' by their indexes in the grid.

        :param multiLstGridPnt: list of lists of successive points 
        [[(i1,j1),(i2,j2), ...],[],...]
        :param setName: name of the set.
        '''
        for lstp in  multiLstGridPnt:
            setLin=self.getSetLinFromLstGridPnt(lstp,setName)
        return setLin
    
    def genSetLinFromLstXYPnt(self,lstXYPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'lstXYPnt' by their coordinates (x,y,z).

        :param lstXYPnt: list of successive points [(x1,y1,z1),(x2,y2,z2), ...]
        :param setName: name of the set.
        '''
        lstGridPnt=[self.getIJfromXY(coord) for coord in lstXYPnt]
        setLin=self.genSetLinFromLstGridPnt(lstGridPnt,setName)
        return setLin
    
    def getSetLinFromLstXYPnt(self,lstXYPnt,setName):
        '''return the set of lines that joint the successive points defined 
        in 'lstXYPnt' by their coordinates (x,y,z).

        :param lstXYPnt: list of successive points [(x1,y1,z1),(x2,y2,z2), ...]
        :param setName: name of the set.
        '''
        lstGridPnt=[self.getIJfromXY(coord) for coord in lstXYPnt]
        setLin=self.getSetLinFromLstGridPnt(lstGridPnt,setName)
        return setLin
    
    def genSetLinFromMultiLstXYPnt(self,multiLstXYPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'multiLstXYPnt' by their coordinates (x,y,z)

        :param multiLstXYPnt: list of lists of successive points 
        [[(x1,y1,z1),(x2,y2,z2), ...],[],...]
        :param setName: name of the set.
        '''
        multiLstGridPnt=list()
        for xyLst in multiLstXYPnt:
            indLst=[self.getIJfromXY(coord) for coord in xyLst]
            multiLstGridPnt.append(indLst)
        setLin=self.genSetLinFromMultiLstGridPnt(multiLstGridPnt,setName)
        return setLin
    
    def getSetLinFromMultiLstXYPnt(self,multiLstXYPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'multiLstXYPnt' by their coordinates (x,y,z)

        :param multiLstXYPnt: list of lists of successive points 
        [[(x1,y1,z1),(x2,y2,z2), ...],[],...]
        :param setName: name of the set.
        '''
        multiLstGridPnt=list()
        for xyLst in multiLstXYPnt:
            indLst=[self.getIJfromXY(coord) for coord in xyLst]
            multiLstGridPnt.append(indLst)
        setLin=self.getSetLinFromMultiLstGridPnt(multiLstGridPnt,setName)
        return setLin

