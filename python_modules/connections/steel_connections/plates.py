# -*- coding: utf-8 -*-
''' Naive bolted plate model.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2020, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import json
import importlib
import xc_base
import geom
from import_export import block_topology_entities as bte
from misc_utils import log_messages as lmsg
  
class Plate(object):
    ''' Base class for plates. This class must be code agnostic
        i.e. no AISC, EC3, EAE clauses here.

    :ivar width: plate width.
    :ivar length: plate length.
    :ivar thickness: plate thickness.
    :ivar steelType: steel type.
    '''
    def __init__(self, width, length, thickness= 10e-3, steelType= None):
        ''' Constructor.

        :param width: plate width.
        :param length: plate length.
        :param thickness: plate thickness.
        :param steelType: steel type.
        '''
        self.width= width
        self.length= length
        self.thickness= thickness
        self.steelType= steelType
        self.refSys= None
        self.weldLines= None
        self.attachedMemberCenter= None

    def setWidth(self, w):
        ''' Set the plate width.

        :param w: new width.
        '''
        self.width= w
        
    def getWidth(self):
        ''' Return the plate width.'''
        return self.width
        
    def setLength(self, l):
        ''' Set the plate length.

        :param l: new length.
        '''
        self.length= l
        
    def getLength(self):
        ''' Return the plate length.'''
        return self.length
    
    def getGrossArea(self):
        ''' Return the gross area of the plate.'''
        return self.width*self.thickness
    
    def getPlateDimensions(self):
        ''' Return the width, length and thickness of the the 
            plate in a tuple.'''
        return (self.width, self.length, self.thickness)

    def __str__(self):
        return 'width: '+ str(self.width) + ' length: '+ str(self.length) + ' thickness: '+ str(self.thickness)
    
    def getCoreContour2d(self):
        ''' Return the contour points of the plate core 
            in local coordinates.'''
        l2= self.length/2.0
        w2= self.width/2.0
        return [geom.Pos2d(-l2,-w2), geom.Pos2d(l2,-w2), geom.Pos2d(l2,w2), geom.Pos2d(-l2,w2)]

    def getCoreContour3d(self):
        ''' Return the contour points of the plate core.

        :param refSys: 3D reference system used to perform local
                       to global coordinate transformation.
        '''
        localPos= self.getCoreContour2d()
        retval= list()
        for p in localPos:
            p3d= geom.Pos3d(p.x,p.y,0.0)
            retval.append(self.refSys.getGlobalPosition(p3d))
        return retval
    
    def setRefSys(self, refSys):
        ''' Set the 3D reference system used to perform local
                       to global coordinate transformation.

        :param refSys: 3D reference system.
        '''
        self.refSys= refSys

    def setWeldLines(self, weldLines):
        ''' Set the lines that weld the plate to the structure.

        :param weldLines: weld lines dictionary {partName:line,...}
        '''
        self.weldLines= weldLines

    def getWeldLinesCenter(self):
        ''' Return the center of mass of the weld lines.'''
        v= geom.Vector3d()
        for key in self.weldLines:
            wl= self.weldLines[key]
            center= wl.getCenterOfMass()
            v+= wl.getLength()*center.getPositionVector()
        v*= (1.0/len(self.weldLines))
        return geom.Pos3d(v.x,v.y,v.z)

    def getWeldLinesVertices(self):
        ''' Return the vertices of the weld lines.'''
        retval= list() 
        for key in self.weldLines:
            wl= self.weldLines[key]
            pA= wl.getFromPoint()
            pB= wl.getToPoint()
            retval.extend([pA,pB])
        return retval
    
    def getWeldLinesVertices2d(self):
        ''' Return the vertices of the weld lines expressed
            in local coordinates.'''
        retval= list()
        ptList= self.getWeldLinesVertices(self)
        for p in ptList:
            retval.append(geom.Pos2d(p.x, p.y))
        return retval

    def getContour(self):
        ''' Return the contour points of the plate.
        '''
        coreContour3d= self.getCoreContour3d()
        contourPlg= geom.Polygon3d(coreContour3d)
        distalEdgeIndex= contourPlg.getIndexOfDistalEdge(self.attachedMemberCenter)
        distalEdge= contourPlg.getEdge(distalEdgeIndex)
        retval= list()
        fromPoint= distalEdge.getFromPoint() # contour first point.
        toPoint= distalEdge.getToPoint() # contour last point.
        retval.append(fromPoint)
        numWeldLines= len(self.weldLines)
        if(numWeldLines==1):
            key= list(self.weldLines.keys())[0]
            wl= self.weldLines[key]
            p1= wl.getFromPoint()
            d1= fromPoint.dist2(p1)
            p2= wl.getToPoint()
            d2= fromPoint.dist2(p2)
            if(d1<d2): # p1 is closer.
                p1New= wl.getProjection(fromPoint)
                p2New= wl.getProjection(toPoint)
                retval.extend([p1New,p2New]) # set contour.
                # Modify weld line.
                self.weldLines[key]= geom.Segment3d(p1New,p2New)
            else: # p2 is closer.
                p2New= wl.getProjection(fromPoint)
                p1New= wl.getProjection(toPoint)
                retval.extend([p2New,p1New]) # set contour.
                # Modify weld line.
                self.weldLines[key]= geom.Segment3d(p2New,p1New)
        else:
            weldPline= geom.get_3d_polylines(list(self.weldLines.values()),1e-3)[0]
            weldVertices= weldPline.getVertexList()
            p1= weldVertices[0]
            d1= fromPoint.dist2(p1)
            p2= weldVertices[-1]
            d2= fromPoint.dist2(p2)
            if(d2<d1): # p2 is closer, so reverse the polyline.
                weldVertices.reverse()
            retval.extend(weldVertices) # set contour.
        retval.append(toPoint) # close contour.            
        return retval

    def getObjectTypeAttr(self):
        ''' Return the object type attribute (used in getBlocks).'''
        return 'plate'

    def getBlocks(self, blockProperties= None, loadTag= None, loadDirI= None, loadDirJ= None, loadDirK= None):
        ''' Return the blocks that define the plate for the
            diagonal argument.

        :param blockProperties: labels and attributes to assign to the newly created blocks.
        :param loadTag: tag of the applied loads in the internal forces file.
        :param loadDirI: I vector of the original element. Vector that 
                         points to the loaded side of the plate.
        :param loadDirJ: J vector of the of the original element.
        :param loadDirK: K vector of the of the original element.
        '''
        retval= bte.BlockData()
        plateProperties= bte.BlockProperties.copyFrom(blockProperties)
        plateProperties.appendAttribute('objType', self.getObjectTypeAttr())
        if(loadTag):
            plateProperties.appendAttribute('loadTag', loadTag)
            plateProperties.appendAttribute('loadDirI', [loadDirI.x, loadDirI.y, loadDirI.z])
            plateProperties.appendAttribute('loadDirJ', [loadDirJ.x, loadDirJ.y, loadDirJ.z])
            plateProperties.appendAttribute('loadDirK', [loadDirK.x, loadDirK.y, loadDirK.z])
        # Get the plate contour
        contourVertices= self.getContour()
        blk= retval.blockFromPoints(contourVertices, plateProperties, thickness= self.thickness, matId= self.steelType.name)
        return retval
    
    def getDict(self):
        ''' Returns a dictionary populated with the member values.'''
        retval= {'width':self.width, 'length':self.length, 'thickness':self.thickness, 'steelType':self.steelType.getDict(), 'refSys':self.refSys}
        if(self.weldLines):
            wlDict= dict()
            for key in self.weldLines.keys:
                wl= self.weldLines[key]
                wlDict[key]= wl.getDict()
            retval['weldLines']= wlDict
        else:
            retval['weldLines']= None
        if(self.attachedMemberCenter):
            retval['attachedMemberCenter']= attachedMemberCenter.getDict()
        else:
            retval['attachedMemberCenter']= None
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: dictionary to read values from.
        '''
        self.width= dct['width']
        self.length= dct['length']
        self.thickness= dct['thickness']
        self.steelType.setFromDict(dct['steelType'])
        tmp= dct['refSys']
        if(tmp):
            self.refSys= geom.Ref3d3d()
            self.refSys.setFromDict(dct['refSys'])
        wlDict= dct['weldLines']
        if(wlDict):
            self.weldLines= dict()
            for key in wlDict:
                wl= geom.Segment3d()
                wl.setFromDict(wlDict[key])
                self.weldLines[key]= wl
        cDict= dct['attachedMemberCenter']
        if(cDict):
            self.attachedMemberCenter= geom.Pos3d()
            self.attachedMemberCenter.setFromDict(cDict)
            
    def jsonRead(self, inputFileName):
        ''' Read object from JSON file.'''

        with open(inputFileName) as json_file:
            plateDict= json.load(json_file)
        self.setFromDict(plateDict)
        json_file.close()

    def jsonWrite(self, outputFileName):
        ''' Write object to JSON file.

        :param outputFileName: name of the output file.
        '''
        outputDict= self.getDict()
        with open(outputFileName, 'w') as outfile:
            json.dump(outputDict, outfile)
        outfile.close()
