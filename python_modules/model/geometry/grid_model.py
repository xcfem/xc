from __future__ import print_function
# -*- coding: utf-8 -*-
'''Particular geometry organization, linked to a framework of spaced points, 
usually referenced by their indices (i,j,k) that point to the global 
coordinates (x,y,z).

Generally, an ijkGrid contains the grid of points itself and all the 
geometric entities (lines, surfaces, ...) attached to it. Dictionaries with 
those entities, identified by specific names, are created, in order to 
facilitate and accelerate their creation, search, grouping in sets, ...

Several grids can coexist in a FE problem.
'''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"


import xc_base
import geom
import xc
import math

class IJKRange(object):
    '''Range of indexes (i,j,k) in the 3D-grid that defines a region 
    bounded by the coordinates associated with those indexes.

    :ivar ijkMin: tuple (or list) with minimum value of indexes 
          (minIindex,minJindex,minKindex) that point to (xmin,ymin,zmin)
    :ivar ijkMax: tuple (or list) with maximum value of indexes 
          (maxIindex,maxJindex,maxKindex) that point to (xmax,ymax,zmax)
    '''
    def __init__(self,ijkMin,ijkMax):
        if type(ijkMin)==list:
            self.ijkMin=tuple(ijkMin)
        else:
            self.ijkMin= ijkMin
        if type(ijkMax)==list:
            self.ijkMax=tuple(ijkMax)
        else:
            self.ijkMax= ijkMax
    def getIMin(self):
        '''return the value of the index I minimum in the range '''
        return self.ijkMin[0]
    def getIMax(self):
        '''return the value of the index I maximum in the range '''
        return self.ijkMax[0]
    def getJMin(self):
        '''return the value of the index J minimum in the range '''
        return self.ijkMin[1]
    def getJMax(self):
        '''return the value of the index J maximum in the range '''
        return self.ijkMax[1]
    def getKMin(self):
        '''return the value of the index K minimum in the range '''
        return self.ijkMin[2]
    def getKMax(self):
      '''return the value of the index K maximum in the range '''
      return self.ijkMax[2]
    def getRange(self,index):
        '''return a list with the range of indexes between minIindex and maxIindex '''
        mn= self.ijkMin[index]
        mx= self.ijkMax[index]+1 #Last index included
        return range(mn,mx)
    def getIRange(self):
        '''return a list with the range of indexes between minIindex
        and maxIindex '''
        return self.getRange(0)
    def getJRange(self):
        '''return a list with the range of index between minJindex and
          maxJindex'''
        return self.getRange(1)

    def getKRange(self):
        '''return a list with the range of indexes between minKindex
        and maxKindex '''
        return self.getRange(2)

    def __str__(self):
        return 'IRange: ' + str(self.getIRange()) + ' JRange: ' + str(self.getJRange()) + ' KRange: ' + str(self.getKRange())

    def extractIncludedIJranges(self,step=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the index K is a constant. Graphically it can be seen as
        the set of planes 'parallel' to global XY included in the region defined
        by the IJKRange 

        :ivar step: K step to select the XY planes (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstIJrang=[IJKRange((imin,jmin,k),(imax,jmax,k)) for k in range(kmin,kmax+1,step)]
        return lstIJrang

    def extractIncludedIKranges(self,step=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the index J is a constant. Graphically it can be seen as
        the set of planes 'parallel' to global XZ included in the region defined
        by the IJKRange 

        :param step: J step to select the XZ planes (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstIKrang=[IJKRange((imin,j,kmin),(imax,j,kmax)) for j in range(jmin,jmax+1,step)]
        return lstIKrang

    def extractIncludedJKranges(self,step=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the index I is a constant. Graphically it can be seen as
        the set of planes 'parallel' to global YZ included in the region defined
        by the IJKRange 

        :param step: I step to select the XZ planes (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstJKrang=[IJKRange((i,jmin,kmin),(i,jmax,kmax)) for i in range(imin,imax+1,step)]
        return lstJKrang

    def extractIncludedIranges(self,stepJ=1,stepK=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the indices J and K are a constant. Graphically 
        it can be seen as the set of lines 'parallel' to global X 
        axis included in the region defined by the IJKRange 

        :param stepJ: step to select the lines in Y direction (defaults to 1)
        :param stepK step to select the lines in Z direction (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstIrang=[IJKRange((imin,j,k),(imax,j,k)) for j in range(jmin,jmax+1,stepJ) for k in range(kmin,kmax+1,stepK)]
        return lstIrang

    def extractIncludedJranges(self,stepI=1,stepK=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the indices I and K are a constant. Graphically it can be seen as
        the set of lines 'parallel' to global Y axis included in the region defined
        by the IJKRange 

        :param stepI: step to select the lines in X direction (defaults to 1)
        :param stepK step to select the lines in Z direction (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstJrang=[IJKRange((i,jmin,k),(i,jmax,k)) for i in range(imin,imax+1,stepI) for k in range(kmin,kmax+1,stepK)]
        return lstJrang

    def extractIncludedKranges(self,stepI=1,stepJ=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the indices I and J are a constant. Graphically it can be seen as
        the set of lines 'parallel' to global Z axis included in the region defined
        by the IJKRange 

        :param stepI: step to select the lines in X direction (defaults to 1)
        :param stepJ step to select the lines in Y direction (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstKrang=[IJKRange((i,j,kmin),(i,j,kmax)) for i in range(imin,imax+1,stepI) for j in range(jmin,jmax+1,stepJ) ]
        return lstKrang



class GridModel(object):
    '''Particular geometry organization, linked to a framework of spaced points, 
    usually referenced by their indices (i,j,k) that point to the global 
    coordinates (x,y,z).

    Generally, an ijkGrid contains the grid of points itself and all the 
    geometric entities (lines, surfaces, ...) attached to it. Dictionaries with 
    those entities, identified by specific names, are created, in order to 
    facilitate and accelerate their creation, search, grouping in sets, ...

    Several grids can coexist in a FE problem.

    :ivar prep: preprocessor
    :ivar xList: ordered list of x-coordinates for the grid 
                 (radius in cylindrical coordinate system)
    :ivar yList: ordered list of y-coordinates for the grid 
                 (angle in degrees counterclockwise from the X-axis directio for cylindrical coordinate system)
    :ivar xList: ordered list of z-coordinates for the grid.
    :ivar xCentCyl, yCentCy: x and y coordinates of the points of the
          Z axis for the cylindrical coordinate system 
          (both coordinates default to 0)
    :ivar dicLin: dictionary with all the lines linked to the grid. 
          Each key is the name of a line, the value associated is the line 
          itself.
    :ivar dicQuadSurf: dictionary with all the surfaces linked to the grid. 
          Each key is the name of a surface, the value associated is the surface 
          itself.


    '''
    def __init__(self,prep,xList,yList,zList,xCentCyl=0,yCentCyl=0):
        self.gridCoo= list()
        self.gridCoo.append(xList)
        self.gridCoo.append(yList)
        self.gridCoo.append(zList)
        self.prep= prep
        self.indices= self.prep.getMultiBlockTopology.get3DNets.new3DNet()
        self.indices.dim(len(self.gridCoo[0]),len(self.gridCoo[1]),len(self.gridCoo[2]))
        self.pointCounter=0
        self.dicQuadSurf=dict()
        self.dicLin=dict()
        self.xCentCyl=xCentCyl
        self.yCentCyl=yCentCyl

    def lastXIndex(self):
        return len(self.gridCoo[0])-1
    def lastYIndex(self):
        return len(self.gridCoo[1])-1
    def lastZIndex(self):
        return len(self.gridCoo[2])-1

    def getIJKfromXYZ(self,xyz):
        '''Return (i,j,k) indexes that match with (x,y,z) coordinates
        '''
        ijk=(self.gridCoo[0].index(xyz[0]),self.gridCoo[1].index(xyz[1]),self.gridCoo[2].index(xyz[2]))
        return ijk

    def getPntXYZ(self,xyz):
        '''Return the point of the grid with coordinates (x,y,z)'''
        ijk=self.getIJKfromXYZ(xyz)
        pnt=self.getPntGrid(ijk)
        return pnt
    
    def getIJKrangeFromXYZrange(self,xyzRange):
        '''Return an ijkRange that matches with coordinates in xyzRange defined as:
        xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Coordinates must be previously defined in the grid. 
        '''
        ijkRange=IJKRange((self.gridCoo[0].index(xyzRange[0][0]),self.gridCoo[1].index(xyzRange[0][1]),self.gridCoo[2].index(xyzRange[0][2])),
                          (self.gridCoo[0].index(xyzRange[1][0]),self.gridCoo[1].index(xyzRange[1][1]),self.gridCoo[2].index(xyzRange[1][2])))
        return ijkRange
    
    def getPntGrid(self,indPnt):
        '''Return the point at indPnt=(i,j,k) index of the grid.

        :param indPnt: grid indices that point to the global (X, Y, Z)
                 coordinates    
        '''
        return self.indices.getPnt(indPnt[0]+1,indPnt[1]+1,indPnt[2]+1)

    def getTagPntGrid(self,indPnt):
        '''Return the tag of the point at indPnt=(i,j,k) index of the grid.

        :param indPnt: grid indices that point to the global (X, Y, Z)
                 coordinates    
        '''
        tagPto= self.indices.getPnt(indPnt[0]+1,indPnt[1]+1,indPnt[2]+1).tag
        return tagPto

    def gridSurfName(self,pt1,pt2,pt3,pt4):
        '''Return the name of the quadrangle surface defined by 4 points.

        :param pt1,pt2,pt3,pt4: tags of the points (in right order) 
               that define the surface 
        '''
        return 's'+'%04.0f' % pt1 +'%04.0f' % pt2 +'%04.0f' % pt3 +'%04.0f' % pt4


    def gridLinName(self,pt1,pt2):
        '''Return the name of the line defined by 2 points

        :param pt1,pt2: tags of the points (in right order) that
               define the line

        '''
        return 'l'+'%04.0f' % pt1 +'%04.0f' % pt2

    def getNmSurfInRange(self,ijkRange,closeCyl='N'):
        '''Return a list with the names of the surfaces limited by a volume 
        defined by the coordinates that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 

        :param ijkRange: instance of IJKRange class
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        (imin,jmin,kmin)=ijkRange.ijkMin
        (imax,jmax,kmax)=ijkRange.ijkMax
        'surfaces in XY plane'
        indPtSurfXY=[((i,j,k),(i+1,j,k),(i+1,j+1,k),(i,j+1,k)) for j in range(jmin,jmax) for i in range(imin,imax) for k in range (kmin,kmax+1)]
        if closeCyl.lower()[0]=='y':
            indPtSurfXY+=[((i,jmax,k),(i+1,jmax,k),(i+1,jmin,k),(i,jmin,k)) for i in range(imin,imax) for k in range (kmin,kmax+1)]
        nmSurfXY=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfXY]
        'surfaces in XZ plane'
        indPtSurfXZ=[((i,j,k),(i,j,k+1),(i+1,j,k+1),(i+1,j,k)) for k in range(kmin,kmax) for i in range(imin,imax) for j in range(jmin,jmax+1)]
        nmSurfXZ=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfXZ]
        'surfaces in YZ plane'
        indPtSurfYZ=[((i,j,k),(i,j+1,k),(i,j+1,k+1),(i,j,k+1)) for k in range(kmin,kmax) for j in range(jmin,jmax) for i in range(imin,imax+1)]
        if closeCyl.lower()[0]=='y':
            indPtSurfYZ+=[((i,jmax,k),(i,jmin,k),(i,jmin,k+1),(i,jmax,k+1)) for k in range(kmin,kmax) for i in range(imin,imax+1)]
        nmSurfYZ=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfYZ]
        return (nmSurfXY+nmSurfXZ+nmSurfYZ)

    def getNmLinInRange(self,ijkRange):
        '''Return a list with the names of the surfaces limited by a volume 
        defined by the coordinates that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        (imin,jmin,kmin)=ijkRange.ijkMin
        (imax,jmax,kmax)=ijkRange.ijkMax
        'lines parallel to X-axis'
        indPtLinX=[((i,j,k),(i+1,j,k)) for  i in range(imin,imax) for j in range(jmin,jmax+1) for k in range(kmin,kmax+1)]
        nmLinX=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinX]
        'lines parallel to Y-axis'
        indPtLinY=[((i,j,k),(i,j+1,k)) for j in range(jmin,jmax)  for  i in range(imin,imax+1) for k in range(kmin,kmax+1)]
        nmLinY=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinY]
        'lines parallel to Z-axis'
        indPtLinZ=[((i,j,k),(i,j,k+1)) for  k in range(kmin,kmax) for i in range(imin,imax+1) for j in range(jmin,jmax+1)]
        nmLinZ=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinZ]
        return (nmLinX+nmLinY+nmLinZ)

    def generatePoints(self):
        '''Point generation in a cartesian coordinate system.'''
        points= self.prep.getMultiBlockTopology.getPoints
        inicTag=points.defaultTag
        lstPt=[(i+1,j+1,k+1,self.gridCoo[0][i],self.gridCoo[1][j],self.gridCoo[2][k]) for i in range(len(self.gridCoo[0])) for j in range(len(self.gridCoo[1])) for k in range(len(self.gridCoo[2])) ]
        for p in lstPt:
            (i,j,k,x,y,z)=p
            self.pointCounter+=1
            pnt=points.newPntIDPos3d(self.pointCounter+inicTag,geom.Pos3d(x,y,z))
            self.indices.setPnt(i,j,k,pnt.tag)

    def generateCylZPoints(self):
        '''Point generation in the following cylindrical coordinate system:

        - Origin: point of coordinates (xCentCyl,yCentCyl,0)
        - Longitudinal axis: Z
        - Azimuth expressed in degrees counterclockwise from the X-axis direction
        
        Coordinates are defined:
        
        - x: radial coordinate (radius)
        - y: angular coordinate (degrees counterclockwise from X-axis)
        - z: height coordinate (Z axis)
        '''
        points= self.prep.getMultiBlockTopology.getPoints
        inicTag=points.defaultTag
        lstPt=[(i+1,j+1,k+1,
                self.xCentCyl+self.gridCoo[0][i]*math.cos(math.radians(self.gridCoo[1][j])),
                self.yCentCyl+self.gridCoo[0][i]*math.sin(math.radians(self.gridCoo[1][j])),
                self.gridCoo[2][k]) for i in range(len(self.gridCoo[0]))
               for j in range(len(self.gridCoo[1]))
               for k in range(len(self.gridCoo[2])) ]
        for p in lstPt:
            (i,j,k,x,y,z)=p
            self.pointCounter+=1
            pnt=points.newPntIDPos3d(self.pointCounter+inicTag,geom.Pos3d(x,y,z))
            self.indices.setPnt(i,j,k,pnt.tag)

    def moveCylPointsRadius(self,ijkRange,radius):
        '''Move points in a 3D grid-region limited by the ijkRange 
        in the cylindrical coordinate system to radius coordinate 
        given as parameter
        '''
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
            vdir=geom.Vector2d(p.getPos.x-self.xCentCyl,p.getPos.y-self.yCentCyl,).normalized()
            p.getPos.x= radius*vdir.x
            p.getPos.y= radius*vdir.y
        sPtMove.clear()

    def movePointsRange(self,ijkRange,vDisp):
        '''Move points  in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax), to a new position 
        by applying a 3D displacement.

        :param ijkRange: range for the search
        :param vDisp: xc vector displacement
        '''
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
          p.getPos.x+= vDisp[0]
          p.getPos.y+= vDisp[1]
          p.getPos.z+= vDisp[2]
        sPtMove.clear()

    def slopePointsRange(self,ijkRange,slopeX=0,xZeroSlope=0,slopeY=0,yZeroSlope=0):
        '''Applies one or two slopes (in X and Y directions) 
        to points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). Only Z coordinate
        of points is modified.

        :param ijkRange: range for the search.
        :param slopeX: slope in X direction, expressed as deltaZ/deltaX 
                       (defaults to 0 = no slope applied)
        :param xZeroSlope: coordinate X of the "rotation axis".
        :param slopeY: slope in Y direction, expressed as deltaZ/deltaY)
                       (defaults to 0 = no slope applied)
        :param yZeroSlope: coordinate Y of the "rotation axis".
        '''
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
          p.getPos.z+= slopeX*(p.getPos.x-xZeroSlope)+slopeY*(p.getPos.y-yZeroSlope)
        sPtMove.clear()


    def rotPntsZAxis(self,ijkRange,angle,xyRotCent):
        '''Rotates points in ijkRange around a Z axis passing by xyRotCent.

        :param ijkRange: range for the search.
        :param angle: rotation angle (degrees)
        :param xyRotCent: coordinates [x,y] of the axis Z of rotation
        '''
        theta=math.radians(angle)
        sinTheta=math.sin(theta)
        cosTheta=math.cos(theta)
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
            xp=p.getPos.x
            yp=p.getPos.y
            p.getPos.x= xyRotCent[0]+cosTheta*(xp-xyRotCent[0])-sinTheta*(yp-xyRotCent[1])
            p.getPos.y= xyRotCent[1]+sinTheta*(xp-xyRotCent[0])+cosTheta*(yp-xyRotCent[1])
        sPtMove.clear()
        
    def scaleCoorXPointsRange(self,ijkRange,xOrig,scale):
        '''Applies a scale in X with origin xOrig (fixed axis: X=xOrig) 
        to the points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). Only X coordinate
        of points is modified in the following way:
        x_scaled=xOrig+scale*(x_inic-xOrig)

        :param ijkRange: range for the search.
        :param xOrig: origin X to apply scale (point in axis X=xOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to X coordinate

        '''
        sPtXscale=self.getSetPntRange(ijkRange,'sPtXscale')
        for p in sPtXscale.getPoints:
            xpt=p.getPos.x
            p.getPos.x= xOrig+scale*(xpt-xOrig)
        sPtXscale.clear()

    def scaleCoorYPointsRange(self,ijkRange,yOrig,scale):
        '''Applies a scale in Y with origin yOrig (fixed axis: y=yOrig) 
        to the points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). Only Y coordinate
        of points is modified in the following way:
        y_scaled=yOrig+scale*(y_inic-yOrig)

        :param ijkRange: range for the search.
        :param yOrig: origin Y to apply scale (point in axis Y=yOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to Y coordinate
        '''
        sPtYscale=self.getSetPntRange(ijkRange,'sPtYscale')
        for p in sPtYscale.getPoints:
            ypt=p.getPos.y
            p.getPos.y= yOrig+scale*(ypt-yOrig)
        sPtYscale.clear()

    def scaleCoorZPointsRange(self,ijkRange,zOrig,scale):
        '''Applies a scale in Z with origin zOrig (fixed axis: z=zOrig) 
        to the points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). Only Z coordinate
        of points is modified in the following way:
        z_scaled=zOrig+scale*(z_inic-zOrig)

        :param ijkRange: range for the search.
        :param zOrig: origin Z to apply scale (point in axis Z=zOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to Z coordinate
        '''
        sPtZscale=self.getSetPntRange(ijkRange,'sPtZscale')
        for p in sPtZscale.getPoints:
            zpt=p.getPos.z
            p.getPos.z= zOrig+scale*(zpt-zOrig)
        sPtZscale.clear()

    def movePointsRangeToZcylinder(self,ijkRange,xCent,yCent,R):
        '''Moves the points in the range to make them belong to 
        a cylinder with radius R and axis parallel to global Z passing 
        through the point (xCent,yCent,0).
        '''
        vCent=geom.Vector2d(xCent,yCent)
        setPnt=self.getSetPntRange(ijkRange,'setPnt')
        for p in setPnt.getPoints:
            v=geom.Vector2d(p.getPos.x,p.getPos.y)-vCent
            vdir= v.normalized()
            p.getPos.x=xCent+R*vdir.x
            p.getPos.y=yCent+R*vdir.y
        setPnt.clear()

    def movePointsRangeToXcylinder(self,ijkRange,yCent,zCent,R):
        '''Moves the points in the range to make them belong to 
        a cylinder with radius R and axis parallel to global X passing 
        through the point (0,yCent,zCent)
        '''
        vCent=geom.Vector2d(yCent,zCent)
        setPnt=self.getSetPntRange(ijkRange,'setPnt')
        for p in setPnt.getPoints:
            v=geom.Vector2d(p.getPos.y,p.getPos.z)-vCent
            vdir=v.normalized()
            p.getPos.y=yCent+R*vdir.x
            p.getPos.z=zCent+R*vdir.y
        setPnt.clear()

    def movePointsRangeToYcylinder(self,ijkRange,xCent,zCent,R):
        '''Moves the points in the range to make them belong to 
        a cylinder with radius R and axis parallel to global Y passing 
        through the point (xCent,0,zCent)
        '''
        vCent=geom.Vector2d(xCent,zCent)
        setPnt=self.getSetPntRange(ijkRange,'setPnt')
        for p in setPnt.getPoints:
            v=geom.Vector2d(p.getPos.x,p.getPos.z)-vCent
            vdir=v.normalized()
            p.getPos.x=xCent+R*vdir.x
            p.getPos.z=zCent+R*vdir.y
        setPnt.clear()

    def newQuadGridSurface(self,surfName):
        '''Generate the quadrangle surface defined by the 4 vertex whose tags
        are implicit in the name of the surface.

        :param surfName: name given to the grid surface
        return the quadrangle surface
        '''
        points= self.prep.getMultiBlockTopology.getPoints
        (tgPt1,tgPt2,tgPt3,tgPt4)=(int(surfName[1:5]),int(surfName[5:9]),int(surfName[9:13]),int(surfName[13:17]))
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
                        (i,j,k)
        :return: the quadrangle surface
        '''
        (pto1,pto2,pto3,pto4)=tuple([self.getTagPntGrid(ind4Pnt[i]) for i in range(4)])
        nameSurf= self.gridSurfName(pto1,pto2,pto3,pto4)
        return nameSurf

    def getNameGridLine(self,ind2Pnt):
        '''Return the name of the line defined by the 2 vertices 
        whose indices in the grid are passed as parameters. 

        :param ind2Pnt: tuple of ordered points defined by their grid indices 
                        (i,j,k)
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
        points= self.prep.getMultiBlockTopology.getPoints
        (tgPt1,tgPt2)=(int(linName[1:5]),int(linName[5:9]))
        lines= self.prep.getMultiBlockTopology.getLines
        ln= lines.newLine(tgPt1,tgPt2)
        ln.name= linName
        ln.nDiv=1 #initialization value
        return ln


    def appendSurfRange(self,ijkRange,setSurf,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)
        and append them to the set named 'nameSet'.
        Add those surfaces to the dictionary dicQuadSurf.

        :param ijkRange: instance of IJKRange class
        :param setSurf: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        
        '''
        nmSurfinRang=self.getNmSurfInRange(ijkRange,closeCyl)
        for nameSurf in nmSurfinRang:
            s= self.newQuadGridSurface(nameSurf)
            self.dicQuadSurf[nameSurf]= s
            setSurf.getSurfaces.append(s)

    def genSurfOneRegion(self,ijkRange,nameSet,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax).
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param ijkRange: instance of IJKRange class
        :param nameSet: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
 
        '''
        setSurf= self.prep.getSets.defSet(nameSet)
        self.appendSurfRange(ijkRange,setSurf,closeCyl)
        return setSurf
    
    def genSurfOneXYZRegion(self,xyzRange,nameSet,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        defined in the range  xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param xyzRange: range xyz
        :param nameSet: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
         '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.genSurfOneRegion(ijkRange,nameSet,closeCyl)

    def genSurfMultiRegion(self,lstIJKRange,nameSet,closeCyl='N'):
        '''generate the surfaces limited by all the regions included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param lstIJKRange: list of instances of IJKRange class
        :param nameSet: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(nameSet)
        for rg in lstIJKRange:
            self.appendSurfRange(rg,setSurf,closeCyl)
        return setSurf

    def genSurfMultiXYZRegion(self,lstXYZRange,nameSet,closeCyl='N'):
        '''generate the surfaces limited by all the regions included in the 
        list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the coordinates in ranges xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param lstXYZRange: list of ranges xyz
        :param nameSet: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
         '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.genSurfMultiRegion(lstIJKRange,nameSet,closeCyl)
        
    def appendLinRange(self,ijkRange,nameSet):
        '''generate the lines limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)
        and append them to the set named 'nameSet'. 
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.getSet(nameSet)
        nmLinInRang=self.getNmLinInRange(ijkRange)
        for nameLin in nmLinInRang:
            l=self.newGridLine(nameLin)
            self.dicLin[nameLin]=l
            setLin.getLines.append(l)
       
    def genLinOneRegion(self,ijkRange,nameSet): 
        '''generate the lines limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.defSet(nameSet)
        self.appendLinRange(ijkRange,nameSet)
        return setLin

    def genLinOneXYZRegion(self,xyzRange,nameSet): 
        '''generate the lines limited by a region defined by the coordinates
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.genLinOneRegion(ijkRange,nameSet)
        
    def genLinMultiRegion(self,lstIJKRange,nameSet):
        '''generate the lines limited by all the regions included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.defSet(nameSet)
        for rg in lstIJKRange:
            self.appendLinRange(rg,nameSet)
        return setLin

    def genLinMultiXYZRegion(self,lstXYZRange,nameSet):
        '''generate the lines limited by all the regions included in the 
        list of coordinate passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the ranges xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.genLinMultiRegion(lstIJKRange,nameSet)
        
    def getSetSurfOneRegion(self,ijkRange,nameSet,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax).

        :param ijkRange: instance of IJKRange class
        :param nameSet: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(nameSet)
        nmSurfinRang=self.getNmSurfInRange(ijkRange,closeCyl)
        for nameSurf in nmSurfinRang:
            if nameSurf in self.dicQuadSurf:
                setSurf.getSurfaces.append(self.dicQuadSurf[nameSurf])
        setSurf.fillDownwards()    
        return setSurf

    def getSubsetSurfOneRegion(self,superset,ijkRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfOneRegion(ijkRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
        
    def getSetSurfOneXYZRegion(self,xyzRange,nameSet,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getSetSurfOneRegion(ijkRange,nameSet,closeCyl)
        
    def getSubsetSurfOneXYZRegion(self,superset,xyzRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfOneXYZRegion(xyzRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset

    
    def getSetSurfMultiRegion(self,lstIJKRange,nameSet,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 

        :param lstIJKRange: list of instances of IJKRange class
        :param nameSet: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(nameSet)
        for rg in lstIJKRange:
            nmSurfinRang=self.getNmSurfInRange(rg,closeCyl)
            for nameSurf in nmSurfinRang:
                if nameSurf in self.dicQuadSurf:
                    setSurf.getSurfaces.append(self.dicQuadSurf[nameSurf])
        setSurf.fillDownwards()    
        return setSurf

    def getSubsetSurfMultiRegion(self,superset,lstIJKRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfMultiRegion(lstIJKRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetSurfMultiXYZRegion(self,lstXYZRange,nameSet,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to coordinates in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.getSetSurfMultiRegion(lstIJKRange,nameSet,closeCyl)
        
    def getSubsetSurfMultiXYZRegion(self,superset,lstXYZRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to coordinates in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfMultiXYZRegion(lstXYZRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinOneRegion(self,ijkRange,nameSet):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        setLin= self.prep.getSets.defSet(nameSet)
        nmLininRang=self.getNmLinInRange(ijkRange)
        for nameLin in nmLininRang:
            if nameLin in self.dicLin:
                setLin.getLines.append(self.dicLin[nameLin])
        setLin.fillDownwards()    
        return setLin
        
    def getSubsetLinOneRegion(self,superset,ijkRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinOneRegion(ijkRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinOneXYZRegion(self,xyzRange,nameSet):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getSetLinOneRegion(ijkRange,nameSet)

    def getSubsetLinOneXYZRegion(self,superset,xyzRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinOneXYZRegion(xyzRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinMultiRegion(self,lstIJKRange,nameSet):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        setLin= self.prep.getSets.defSet(nameSet)
        for rg in lstIJKRange:
            nmLininRang=self.getNmLinInRange(rg)
            for nameLin in nmLininRang:
                if nameLin in self.dicLin:
                    setLin.getLines.append(self.dicLin[nameLin])
        setLin.fillDownwards()    
        return setLin

    def getSubsetLinMultiRegion(self,superset,lstIJKRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        subset=self.prep.getSets.defSet(nameSubset)
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinMultiRegion(lstIJKRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinMultiXYZRegion(self,lstXYZRange,nameSet):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.getSetLinMultiRegion(lstIJKRange,nameSet)
        
    def getSubsetLinMultiXYZRegion(self,superset,lstXYZRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        subset=self.prep.getSets.defSet(nameSubset)
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinMultiXYZRegion(lstXYZRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getLstLinRange(self,ijkRange):
        '''return a list of lines in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        retval=list()
        nmLinInRang=self.getNmLinInRange(ijkRange)
        for nameLin in nmLinInRang:
            if nameLin in self.dicLin:
                retval.append(self.dicLin[nameLin])
        return retval

    def getLstLinXYZRange(self,xyzRange):
        '''return a list of lines in a region limited by the coordinates
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getLstLinRange(ijkRange)

    def getLstPntRange(self,ijkRange):
        '''return the ordered list points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)

        :param ijkRange: range for the search
        '''
        retval=list()
        (imin,jmin,kmin)=ijkRange.ijkMin
        (imax,jmax,kmax)=ijkRange.ijkMax
        lstTagPnt=[self.getTagPntGrid(indPnt=(i,j,k)) for i in range(imin,imax+1) for j in range(jmin,jmax+1) for k in range(kmin,kmax+1)] #list of point tags to include in the set
        points= self.prep.getMultiBlockTopology.getPoints
        for tg in lstTagPnt:
          pnt=points.get(tg)
          retval.append(pnt)
        return retval
    
    def getLstPntXYZRange(self,xyzRange):
        '''return the ordered list points in a 3D grid-region limited by 
        coordinates defined in xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getLstPntRange(ijkRange)
        
    def getSetPntRange(self,ijkRange,setName):
        '''return the set of points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)

        :param ijkRange: range for the search
        :param setName: name of the new set of points
        '''
        lstPnt=self.getLstPntRange(ijkRange)
        retval= self.prep.getSets.defSet(setName)
        pntsSet=retval.getPoints
        for pnt in lstPnt:
          pntsSet.append(pnt)
        return retval

    def getSetPntXYZRange(self,xyzRange,setName):
        '''return the set of points in a 3D grid-region limited by 
        coordinates in xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getSetPntRange(ijkRange,setName)
        
    def getSetPntMultiRegion(self,lstIJKRange,setName):
        '''return the set of points in a all the 3D grid-regions
        included in the list of ijkRanges passed as parameter
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        retval= self.prep.getSets.defSet(setName)
        pntsSet=retval.getPoints
        for rg in lstIJKRange:
            lstPnt=self.getLstPntRange(rg)
            for pnt in lstPnt:
                pntsSet.append(pnt)
        return retval
    
    def getSetPntMultiXYZRegion(self,lstXYZRange,setName):
        '''return the set of points in a all the 3D grid-regions
        included in the list of xyzRanges passed as parameter
        Each region defines a volume limited by the coordinates    
        defined in xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.getSetPntMultiRegion(lstIJKRange,setName)

    def genSetLinFromLstGridPnt(self,lstGridPnt,nameSet):
        '''generate the set of lines that joint the successive points defined 
        in 'lstGridPnt' by their indexes in the grid.

        :param lstGridPnt: list of successive points [(i1,j1,k1),(i2,j2,k2), ...]
        :param nameSet: name of the set.
        '''
        if self.prep.getSets.exists(nameSet):
            setLin= self.prep.getSets.getSet(nameSet)
        else:
            setLin= self.prep.getSets.defSet(nameSet)
        lines= self.prep.getMultiBlockTopology.getLines
        pntTags=[self.getPntGrid(pind).tag for pind in lstGridPnt]
        for i in range(len(pntTags)-1):
            l=lines.newLine(pntTags[i],pntTags[i+1])
            setLin.getLines.append(l)                
        return setLin
    
    def genSetLinFromMultiLstGridPnt(self,multiLstGridPnt,nameSet):
        '''generate the set of lines that joint the successive points defined 
        in 'multiLstGridPnt' by their indexes in the grid.

        :param multiLstGridPnt: list of lists of successive points 
        [[(i1,j1,k1),(i2,j2,k2), ...],[],...]
        :param nameSet: name of the set.
        '''
        for lstp in  multiLstGridPnt:
            setLin=self.genSetLinFromLstGridPnt(lstp,nameSet)
        return setLin
    
    def genSetLinFromLstXYZPnt(self,lstXYZPnt,nameSet):
        '''generate the set of lines that joint the successive points defined 
        in 'lstXYZPnt' by their coordinates (x,y,z).

        :param lstXYZPnt: list of successive points [(x1,y1,z1),(x2,y2,z2), ...]
        :param nameSet: name of the set.
        '''
        lstGridPnt=[self.getIJKfromXYZ(coord) for coord in lstXYZPnt]
        setLin=self.genSetLinFromLstGridPnt(lstGridPnt,nameSet)
        return setLin
    
    def genSetLinFromMultiLstXYZPnt(self,multiLstXYZPnt,nameSet):
        '''generate the set of lines that joint the successive points defined 
        in 'multiLstXYZPnt' by their coordinates (x,y,z)

        :param multiLstXYZPnt: list of lists of successive points 
        [[(x1,y1,z1),(x2,y2,z2), ...],[],...]
        :param nameSet: name of the set.
        '''
        multiLstGridPnt=list()
        for xyzLst in multiLstXYZPnt:
            indLst=[self.getIJKfromXYZ(coord) for coord in xyzLst]
            multiLstGridPnt.append(indLst)
        setLin=self.genSetLinFromMultiLstGridPnt(multiLstGridPnt,nameSet)
        return setLin
    
def getSetIntersSurf(set1,set2,nameSetInter):
    '''Return a set of surfaces intersection of those in 'set1' and 'set2'

    :param set1, set2: sets containing surfaces to search intersection
    :param nameSetInter: name of the set of surfaces to return
    '''
    prep=set1.getPreprocessor
    if prep.getSets.exists(nameSetInter): prep.getSets.removeSet(nameSetInter) 
    setInters=prep.getSets.defSet(nameSetInter)
    surfSet1=set1.getSurfaces
    surfSet2=set2.getSurfaces
    surfSetInters=setInters.getSurfaces
    for s in surfSet2:
        if s in surfSet1: surfSetInters.append(s)
    return setInters

def getSetIntersLin(set1,set2,nameSetInter):
    '''Return a set of lines intersection of those in 'set1' and 'set2'

    :param set1, set2: sets containing lines to search intersection
    :param nameSetInter: name of the set of lines to return
    '''
    prep=set1.getPreprocessor
    if prep.getSets.exists(nameSetInter): prep.getSets.removeSet(nameSetInter) 
    setInters=prep.getSets.defSet(nameSetInter)
    linSet1=set1.getLines
    linSet2=set2.getLines
    linSetInters=setInters.getLines
    for s in linSet2:
        if s in linSet1: linSetInters.append(s)
    return setInters
