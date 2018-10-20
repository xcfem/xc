# -*- coding: utf-8 -*-
# Pre-design of diaphragm walls according to Geojuanjo's recomendations 
# (see document predisenio_muros_pantalla_Geojuanjo.pdf)
# http://estructurando.net/2015/11/23/prediseno-de-muros-pantalla-un-primer-paso-imprescindible/

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2018, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es" 


def side_line(pnt1,pnt2,pnt):
    '''function to determine which side of the line from pnt1=(x1,y1) to
    pnt2=(x2,y2) a point pnt=(x,y) falls on.

    All the points that return a value <0 lie on one side of the line and 
    those that return a value >0 lie in the other side (if =0 -> in the line)
    '''
    d=(pnt[0]-pnt1[0])*(pnt2[1]-pnt1[1])-(pnt[1]-pnt1[1])*(pnt2[0]-pnt1[0])
    return d

# Points (x,y) to delimit wall thickness (45-60-80-100 cm) when phreatic level 
# is bellow maximum digging height
# x= maximum distance between anchors
# y= maximum effective digging height
points_60_NW=[(3,18.5),(12,3)]
points_80_NW=[(3,21),(12,6)]
points_100_NW=[(3,27.5),(12,14)]

# Points (x,y) to delimit wall thickness (45-60-80-100 cm) when phreatic level 
# is above maximum digging height
# x= maximum distance between anchors
# y= maximum effective digging height
points_60_W=[(3,12.5),(12,3)]
points_80_W=[(3,17),(12,6)]
points_100_W=[(3,21),(12,11)]

class DiaphragmWallRoughModel(object):
    '''Class intended to pre-design a diaphragm wall.

    :ivar digHeight: maximum digging height [m]
    :ivar phreaticLevel: phreatic level above maximum effective digging height
          [m]. It will be =0 or negative if phreatic level below  
          maximum excavation level
    :ivar distAnch: maximum distance between anchors [m] (= 0 or negative
          if no anchors)
    :ivar firstExcavHeight: height of the first phase of excavation (cantilever) [m]
    :ivar distLastAnchExc: distance from the last anchor to the bottom of excavation [m]
    :ivar unifLoad: uniform load on the ground [kN/m2]
    :ivar distLoadWall: distance from the back of the wall to the starting 
          point where uniform load in applied [m]

    '''
    def __init__(self, digHeight,phreaticLevel,distAnch,firstExcavHeight,distLastAnchExc,unifLoad,distLoadWall):
        self.digHeight= digHeight
        self.phreaticLevel= phreaticLevel
        self.distAnch= distAnch
        self.firstExcavHeight= firstExcavHeight
        self.distLastAnchExc= distLastAnchExc
        self.unifLoad= unifLoad
        self.distLoadWall= distLoadWall

    def getEffectDigHeight(self):
        '''Return the maximum effective digging height'''
        if self.distLoadWall <= 0.75*self.digHeight:
            coeff=1-(1-0.5)/(0.75*self.digHeight)*self.distLoadWall
        else:
            coeff=0
        fictH=coeff*self.unifLoad/20  #fictitious digging height
        effH=self.digHeight+fictH
        return effH

    def getFffectDistAnch(self):
        ''' Return effective distance between anchors'''
        if self.phreaticLevel <=0:
            effDistAnc=max(self.distAnch,1.5*self.firstExcavHeight,self.distLastAnchExc+1)
        else:
            effDistAnc=max(self.distAnch,1.5*self.firstExcavHeight,self.distLastAnchExc+2)
        return effDistAnc

    def getWallThickness(self):
        '''Return a pre-designed diaphragm wall thickness [m]'''
        effDigH=self.getEffectDigHeight()
        effDistAnch=self.getFffectDistAnch()
        wallTh=0.45
        if self.phreaticLevel<=0:
            if side_line(points_60_NW[0],points_60_NW[1],(effDistAnch,effDigH)) < 0:
                if side_line(points_80_NW[0],points_80_NW[1],(effDistAnch,effDigH)) < 0:
                    if side_line(points_100_NW[0],points_100_NW[1],(effDistAnch,effDigH)) < 0:
                        wallTh=1.00
                    else:
                        wallTh=0.80
                else:
                    wallTh=0.60
        else:
            if side_line(points_60_W[0],points_60_W[1],(effDistAnch,effDigH)) < 0:
                if side_line(points_80_W[0],points_80_W[1],(effDistAnch,effDigH)) < 0:
                    if side_line(points_100_W[0],points_100_W[1],(effDistAnch,effDigH)) < 0:
                        wallTh=1.00
                    else:
                        wallTh=0.80
                else:
                    wallTh=0.60
        return wallTh

    def getWallEmbedmentDepth(self):
        '''Return a pre-designed embedment depth [m]
        '''
        if self.distAnch > 0:
            embdt=max(3,self.digHeight/3.,3/2.*self.phreaticLevel,2+self.unifLoad/40)
        else:
            embdt=max(3,2*self.digHeight,3/2.*self.phreaticLevel,2+self.unifLoad/40)
        return embdt

    def printResult(self):
        print 'Maximum effective digging height = ', self.getEffectDigHeight(), ' m '
        print 'Maximum effective interdistance anchoring = ', self.getFffectDistAnch(), ' m '
        print 'Embedment depth of diaphragm wall = ',self.getWallEmbedmentDepth() , ' m '
        print 'Diaphragm wall thickness= ', self.getWallThickness(),  ' m '
        


#      Examples
print '** Example 1'
example1=DiaphragmWallRoughModel(digHeight=9,phreaticLevel=2.4,distAnch=9,firstExcavHeight=1,distLastAnchExc=9,unifLoad=60,distLoadWall=0)
example1.printResult()
print '** Example 2'
example2=DiaphragmWallRoughModel(digHeight=15,phreaticLevel=3.0,distAnch=15,firstExcavHeight=1,distLastAnchExc=15,unifLoad=100,distLoadWall=0)
example2.printResult()
print '** Example 3'
example3=DiaphragmWallRoughModel(digHeight=9,phreaticLevel=2,distAnch=3,firstExcavHeight=1,distLastAnchExc=0,unifLoad=60,distLoadWall=0)
example3.printResult()
print '** Example 4'
example4=DiaphragmWallRoughModel(digHeight=19,phreaticLevel=-2,distAnch=8,firstExcavHeight=1,distLastAnchExc=0,unifLoad=(10*5+100*(0.75*19-5))/(0.75*19),distLoadWall=0)
example4.printResult()
