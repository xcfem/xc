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
import geom
from import_export import block_topology_entities as bte
from misc_utils import log_messages as lmsg
  
class SteelPanel(object):
    ''' Steel panel. This class must be code agnostic
        i.e. no AISC, EC3, EAE clauses here.

    :ivar thickness: plate thickness.
    :ivar steelType: steel type.
    :ivar weldLines: dictionary containing weld lines.
    '''
    def __init__(self, thickness= 10e-3, steelType= None):
        ''' Constructor.

        :param thickness: plate thickness.
        :param steelType: steel type.
        '''
        self.thickness= thickness
        self.steelType= steelType
        self.weldLines= None

    def getThickness(self):
        ''' Return the panel thickness.'''
        return self.thickness
    
    def getFilletWeldLegSize(self, otherThickness, factor= 0.75):
        ''' Return the leg size of the fillet welds that connect the gusset plate
            to the flange.

        :param otherThickness: column member (web, flange,...) thickness.
        :param factor: ratio between the minimum and the maximum thicknesses.
        :return: linear interpolation between the minimum and maximum values:
                        minThickness+factor*(maxThickness-minThickness)
        '''
        minThickness= self.getFilletMinimumLeg(otherThickness)
        maxThickness= self.getFilletMaximumLeg(otherThickness)
        return minThickness+factor*(maxThickness-minThickness)
        
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
        ptList= self.getWeldLinesVertices()
        for p in ptList:
            retval.append(geom.Pos2d(p.x, p.y))
        return retval

    def getObjectTypeAttr(self):
        ''' Return the object type attribute (used in getBlocks).'''
        return 'steel_panel'
    
    def getWeldBlocks(self, ownerId, blockProperties= None):
        ''' Return the blocks representing the welds.

        :param ownerId: identifier of the plate to be welded.
        :param blockProperties: labels and attributes to assign to the 
                                newly created blocks.
        '''
        retval= None
        if(self.weldLines): # it's a welded plate
            retval= bte.BlockData()
            weldProperties= bte.BlockProperties.copyFrom(blockProperties)
            weldProperties.appendAttribute('objType', 'weld')
            weldProperties.appendAttribute('ownerId', ownerId) # Weld owner id.
            weldProperties.appendAttribute('legSize', self.weldLegSize) # Weld size.
            for key in self.weldLines:
                wl= self.weldLines[key]
                pA= wl.getFromPoint()
                pB= wl.getToPoint()
                weldBlk= retval.blockFromPoints(points= [pA, pB], blockProperties= weldProperties, thickness= None)
                if __debug__:
                    if not weldBlk:
                        AssertionError('Can\'t create the block.')
        return retval

    def getDict(self):
        ''' Returns a dictionary populated with the member values.'''
        retval= {'thickness':self.thickness, 'steelType':self.steelType.getDict()}
        if(self.weldLines):
            wlDict= dict()
            for key in self.weldLines.keys:
                wl= self.weldLines[key]
                wlDict[key]= wl.getDict()
            retval['weldLines']= wlDict
        else:
            retval['weldLines']= None
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: dictionary to read values from.
        '''
        self.thickness= dct['thickness']
        self.steelType.setFromDict(dct['steelType'])
        wlDict= dct['weldLines']
        if(wlDict):
            self.weldLines= dict()
            for key in wlDict:
                wl= geom.Segment3d()
                wl.setFromDict(wlDict[key])
                self.weldLines[key]= wl
            
    def jsonRead(self, inputFileName):
        ''' Read object from JSON file.'''

        with open(inputFileName) as json_file:
            steelPanelDict= json.load(json_file)
        self.setFromDict(steelPanelDict)
        json_file.close()

    def jsonWrite(self, outputFileName):
        ''' Write object to JSON file.

        :param outputFileName: name of the output file.
        '''
        outputDict= self.getDict()
        with open(outputFileName, 'w') as outfile:
            json.dump(outputDict, outfile)
        outfile.close()

    def report(self, outputFile):
        ''' Reports connection design values.'''
        # plate dimensions and material.
        outputFile.write(' steel panel:\n')
        outputFile.write('      plate thickness: '+str(self.thickness*1000)+' mm\n')
        outputFile.write('      steel type: '+str(self.steelType.name)+'\n')
        # report welds
        if(hasattr(self, 'weldLegSize')):
            outputFile.write('      weld leg size: '+str(int(self.weldLegSize*1e3))+' mm\n')
            outputFile.write('      weld segments: '+str(len(self.weldLines))+'\n')
        
class Plate(SteelPanel):
    ''' Base class for plates. This class must be code agnostic
        i.e. no AISC, EC3, EAE clauses here.

    :ivar width: plate width.
    :ivar length: plate length.
    :ivar notched:  if true use notchs when appropriate.  
    :ivar contour: list of vertices that form the contour of the plate.
    '''
    def __init__(self, width, length, thickness= 10e-3, steelType= None, notched= False):
        ''' Constructor.

        :param width: plate width.
        :param length: plate length.
        :param thickness: plate thickness.
        :param steelType: steel type.
        :param notched: if true use notchs when appropriate.
        '''
        super(Plate, self).__init__(thickness, steelType)
        self.width= width
        self.length= length
        self.refSys= None
        self.attachedMemberCenter= None
        self.notched= notched
        self.contour= None

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

    def computeContour(self):
        ''' Compute the contour points of the plate. '''
        coreContour3d= self.getCoreContour3d()
        if(self.attachedMemberCenter): # there is an attached member.
            contourPlg= geom.Polygon3d(coreContour3d)
            # Get distal edge.
            distalEdgeIndex= contourPlg.getIndexOfDistalEdge(self.attachedMemberCenter)
            distalEdge= contourPlg.getEdge(distalEdgeIndex)
            centroid= contourPlg.getCenterOfMass()
            distalEdgeAngle= distalEdge.getAngle(geom.Line3d(self.attachedMemberCenter, centroid))
            angleTol= 5/180.0*math.pi # five degrees.
            if((abs(distalEdgeAngle)<angleTol) or (abs(distalEdgeAngle-math.pi)<angleTol)):
                # Distal side is almost parallel to the forces in the connection;
                # we take the next one.
                distalEdgeIndex-= 1
                distalEdge= contourPlg.getEdge(distalEdgeIndex)
                distalEdgeAngle= distalEdge.getAngle(geom.Line3d(self.attachedMemberCenter, centroid))
                
            self.contour= list()
            fromPoint= distalEdge.getFromPoint() # contour first point.
            toPoint= distalEdge.getToPoint() # contour last point.
            self.contour.append(fromPoint)
            if(self.weldLines):
                weldPline= geom.get_3d_polylines(list(self.weldLines.values()),1e-3)[0]
                weldVertices= weldPline.getVertexList()
                weldPline.simplify(0.01)
                weldPlineVertices= weldPline.getVertexList() # Vertices after simplification.
                numWeldLines= len(weldPlineVertices)-1
                if(numWeldLines==1): # only one weld line.
                    # compute the vertex that is closer to fromPoint
                    p1= weldPlineVertices[0]
                    d1= fromPoint.dist2(p1)
                    p2= weldPlineVertices[1]
                    d2= fromPoint.dist2(p2)
                    if(d1>d2): # p2 is closer.
                        p1, p2= p2, p1
                        d1, d2= d2, d1
                    wl= geom.Segment3d(p1,p2) # weld segment.
                    # Clip the weld segments.
                    p1New= wl.getProjection(fromPoint)
                    p2New= wl.getProjection(toPoint)
                    newLength= p2New.dist(p1New)
                    if(newLength<1e-3):
                        lmsg.warning('weld line normal to distal edge. No changes made.')
                        p1New= wl.getFromPoint()
                        p2New= wl.getToPoint()
                    self.contour.extend([p1New,p2New]) # set contour.
                    # New weld vertices.
                    if(len(weldVertices)>2): # two weld segments.
                        newWeldVertices= [p1New, weldVertices[1], p2New]                    
                    else: # one weld segment.0.0198
                        newWeldVertices= [p1New, p2New]
                    # Update weld lines.
                    vertexCount= 0
                    numNewWeldVertices= len(newWeldVertices)
                    for key in self.weldLines.keys():
                        if(vertexCount<numNewWeldVertices-1):
                            self.weldLines[key]= geom.Segment3d(newWeldVertices[vertexCount],newWeldVertices[vertexCount+1])
                        else:
                            lmsg.error('not enough vertices for '+str(len(self.weldLines))+' weld lines.')
                        vertexCount+= 1
                else: # three weld lines.
                    p1= weldPlineVertices[0]
                    d1= fromPoint.dist2(p1)
                    p2= weldPlineVertices[-1]
                    d2= fromPoint.dist2(p2)
                    if(d2<d1): # p2 is closer, so reverse the polyline.
                        weldPlineVertices.reverse()
                    if(self.notched): # use notches
                        plateHeight= fromPoint.dist(toPoint)
                        limitLineHeight= weldPlineVertices[-1].dist(weldPlineVertices[0])
                        # Compute corner positions.
                        limitLine= geom.Line3d(weldPlineVertices[0],weldPlineVertices[-1])                    
                        p1New= limitLine.getProjection(fromPoint)
                        p2New= limitLine.getProjection(toPoint)
                        # Compute chamfers
                        chamfer= 20e-3
                        chamferDir1= chamfer*(fromPoint-p1New).normalized()
                        if(limitLineHeight>(2*chamfer+plateHeight)):
                            chamferDir2= chamfer*limitLine.getVDir().normalized()
                        else:
                            cD2= (limitLineHeight-plateHeight)/2.0
                            chamferDir2= cD2*limitLine.getVDir().normalized()
                        p1NewA= p1New+chamferDir1
                        p1NewB= p1New-chamferDir2
                        self.contour.extend([p1NewA, p1NewB]) # chamfer
                        self.contour.extend(weldPlineVertices) # welded contour.
                        p2NewB= p2New+chamferDir2
                        p2NewA= p2New+chamferDir1
                        self.contour.extend([p2NewB, p2NewA]) # chamfer      
                    else:    
                        self.contour.extend(weldPlineVertices) # welded contour.
            else:
                lmsg.error("undefined weld lines, can't compute contour")
            self.contour.append(toPoint) # close contour.            
        else: # no attached member.
            self.contour= coreContour3d

    def getContour(self):
        ''' Return the contour points of the plate. '''
        if(not self.contour): # Contour not computed yet.
            self.computeContour()
        return self.contour

    def getMidPlane(self):
        ''' Return the mid-plane of the plate.'''
        contourPoints= self.getContour()
        return geom.Plane3d(contourPoints[0],contourPoints[1],contourPoints[3])

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
        :param loadDirJ: J vector of the original element.
        :param loadDirK: K vector of the original element.
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
        ownerId= 'f'+str(blk.id)
        # Get the weld blocks for the plate
        wBlocks= self.getWeldBlocks(ownerId, blockProperties) # Get the weld blocks for the plate
        if(wBlocks):
            blk.weldBlocks= wBlocks 
            retval.extend(blk.weldBlocks)
        return retval
    
    def getDict(self):
        ''' Returns a dictionary populated with the member values.'''
        retval= super(Plate, self).getDict()
        retval.update({'width':self.width, 'length':self.length, 'refSys':self.refSys})
        if(self.attachedMemberCenter):
            retval['attachedMemberCenter']= self.attachedMemberCenter.getDict()
        else:
            retval['attachedMemberCenter']= None
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: dictionary to read values from.
        '''
        super(Plate, self).setFromDict(dct) 
        self.width= dct['width']
        self.length= dct['length']
        tmp= dct['refSys']
        if(tmp):
            self.refSys= geom.Ref3d3d()
            self.refSys.setFromDict(dct['refSys'])
        cDict= dct['attachedMemberCenter']
        if(cDict):
            self.attachedMemberCenter= geom.Pos3d()
            self.attachedMemberCenter.setFromDict(cDict)
            
    def report(self, outputFile):
        ''' Reports connection design values.'''
        # plate dimensions and material.
        outputFile.write(' steel plate:\n')
        if(self.width):
            outputFile.write('      chamfer width: '+str(self.width*1000)+' mm\n')
        outputFile.write('      plate thickness: '+str(self.thickness*1000)+' mm\n')
        outputFile.write('      steel type: '+str(self.steelType.name)+'\n')
        # report welds
        if(hasattr(self, 'weldLegSize')):
            outputFile.write('      weld leg size: '+str(int(self.weldLegSize*1e3))+' mm\n')
            outputFile.write('      weld segments: '+str(len(self.weldLines))+'\n')
