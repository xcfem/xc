# -*- coding: utf-8 -*-
''' Connecttion design. Obtain connection member data from the 
    model of the whole structure.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2020, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import enum
import math
import ezdxf
import json
from import_export import block_topology_entities as bte
import xc_base
import geom
from misc_utils import log_messages as lmsg
from import_export import neutral_mesh_description as nmd

class MemberSort(enum.IntEnum):
    '''Classification of members by its angle with the horizon.'''
    horizontal= 0
    vertical= 1
    diagonal= 2

def getSegmentOrientation(origin, sg):
    ''' Return the orientation of the segment with respect to the origin.

    :param origin: point to compute the orientation with respect to.
    :param sg: segment to compute the orientation for.
    :return: 1 if the segment from point is the nearest to origin
            -1 if the segment to point is nearest to origin
    '''
    retval= 1.0
    d0= origin.dist2(sg.getFromPoint())
    d1= origin.dist2(sg.getToPoint())
#    if(d0>d1):
    if(d1>d0):
       retval= -1.0
    return retval

class ConnectedMemberMetaData(object):
    ''' Data of a connected member.

    :ivar eTag: element tag.
    :ivar shape: cross section steel shape.
    :ivar iVector: axis direction vector.
    :ivar jVector: J direction vector.
    :ivar kVector: K direction vector.
    :ivar nodePositions: positions of the element nodes.
    :ivar memberOrigin: origin for this member.
    '''
    def __init__(self, element):
        ''' Extracts member data from element.

        :param element: element to extract the data from.
        '''
        self.eTag= element.tag
        self.shape= element.getProp('crossSection')
        self.iVector= element.getIVector3d(False)
        self.jVector= element.getJVector3d(False)
        self.kVector= element.getKVector3d(False)
        self.nodePositions= list()
        nodes= element.getNodes
        for n in nodes:
            self.nodePositions.append(n.getInitialPos3d)
        self.memberOrigin= None

    def getReferenceSystem(self, origin= None):
        ''' Return the member reference system.

        :param origin: member origin.
        '''
        if(not origin):
            origin= self.memberOrigin
        return geom.Ref2d3d(origin, self.kVector, -self.jVector) # right-handed

    def getMidPoints(self, origin= None):
        ''' Return the position expressed in global coordinates
            of the pointsat the middle of the shape.

        :param origin: member origin.
        '''
        return self.shape.getMidPointsGlobalPos(self.getReferenceSystem(origin))
    
    def getMemberSort(self, tol= math.radians(5)):        
        ''' Return the member sort: horizontal, vertical or diagonal.

        :param tol: tolerance.
        '''
        retval= MemberSort.diagonal
        polarAngle= self.iVector.getPolarAngle()
        if(abs(polarAngle)<tol): # vertical
            retval= MemberSort.vertical
        elif(abs(polarAngle-math.pi)<tol): # vertical
            retval= MemberSort.vertical
        elif(abs(polarAngle-math.pi/2.0)<tol): # horizontal
            retval= MemberSort.horizontal
        elif(abs(polarAngle+math.pi/2.0)<tol): # horizontal
            retval= MemberSort.horizontal
        return retval

    def getMemberType(self):
        ''' Return the member type (beam, column or diagonal).'''
        retval= None
        memberSort= self.getMemberSort()
        if(memberSort==MemberSort.vertical):
            retval= 'column'
        elif(memberSort==MemberSort.horizontal):
            retval= 'beam'
        elif(memberSort==MemberSort.diagonal):
            retval= 'diagonal'
        return retval
        
    def getOrientation(self, origin):
        ''' Return the member orientation with respect to the
            origin.

        :param origin: connection origin.
        '''
        retval= 1.0
        d0= origin.dist2(self.nodePositions[0])
        d1= origin.dist2(self.nodePositions[1])
        if(d0>d1):
            retval= -1.0
        return retval

    def getDirection(self, origin):
        ''' Return the local base vectors with respect
            to the origin (to or from the origin).'''
        orientation= self.getOrientation(origin)
        retval= [orientation*self.iVector, orientation*self.jVector, self.kVector]
        return retval

    def getExtrusionVector(self, factor):
        ''' Return the extrusion vector for this member.

        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        return factor*self.iVector
    
    def getWebDirection(self):
        ''' Return the direction of the member web.'''
        return self.jVector

    def getWebMidPlane(self, origin, factor):
        ''' Return the mid-plane of the member web.

        :param origin: connection origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        extrusionVector= self.getExtrusionVector(factor)
        return self.shape.getWebMidPlane(org= origin, extrusionVDir= extrusionVector, weakAxisVDir= self.jVector)
    
    def getBottomFlangeMidPlane(self, origin, factor):
        ''' Return the mid-plane of the bottom flange.

        :param origin: connection origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        extrusionVector= self.getExtrusionVector(factor)
        return self.shape.getBottomFlangeMidPlane(org= origin, extrusionVDir= extrusionVector, weakAxisVDir= self.jVector)

    def getTopFlangeMidPlane(self, origin, factor):
        ''' Return the mid-plane of the top flange.

        :param origin: connection origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        extrusionVector= self.getExtrusionVector(factor)
        return self.shape.getTopFlangeMidPlane(org= origin, extrusionVDir= extrusionVector, weakAxisVDir= self.jVector)
    
    def getIntersectionPoint(self, origin, factor, sg):
        ''' Get the intersection of the segment with the member
            mid planes.

        :param origin: connection origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        :param sg: segment to intersect.
        '''
        webPlane= self.getWebMidPlane(origin, factor)
        angleWithWeb= webPlane.getAngleWithVector3d(sg.getVDir())
        orientation= getSegmentOrientation(origin, sg)
        if(orientation<0.0):
            sg.swap()
        p1= sg.getToPoint() # Farthest point from origin.
        p0= sg.getFromPoint()
        if(abs(angleWithWeb)<1e-3): # segment parallel to web.
            bottomFlangeMidPlane= self.getBottomFlangeMidPlane(origin, factor)
            pIntA= bottomFlangeMidPlane.getIntersection(sg)
            topFlangeMidPlane= self.getTopFlangeMidPlane(origin, factor)
            pIntB= topFlangeMidPlane.getIntersection(sg)
            if(pIntA.exists): # segment intersects bottom flange.
                if(pIntB.exists): # segment intersects top flange.
                    dA= p1.dist2(pIntA)
                    dB= p1.dist2(pIntB)
                    if(dA<dB):
                        p0= pIntA
                    else:
                        p0= pIntB
                else: # pIntB doesn't exists.
                    p0= pIntA
            else:
                if(pIntB.exists): # segment intersects top flange.
                    p0= pIntB
                else:
                    lmsg.error('Intersection point not found.')
        else: # segment normal to web
            p0= webPlane.getIntersection(sg)
        return p0
    
    def getMemberLoadAttributes(self):
        ''' Return the attirbutes with the loading data
            for this member.
        '''
        retval= dict()
        retval['loadTag']= self.eTag
        retval['loadDirI']= [self.iVector.x, self.iVector.y, self.iVector.z]
        retval['loadDirJ']= [self.jVector.x, self.jVector.y, self.jVector.z]
        retval['loadDirK']= [self.kVector.x, self.kVector.y, self.kVector.z]
        return retval
    
    def getMemberBlocks(self, connectionOrigin, memberOrigin, factor, blockProperties= None):
        ''' Return the faces of the member.

        :param connectionOrigin: connection origin.
        :param memberOrigin: member origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        self.memberOrigin= memberOrigin
        memberProperties= bte.BlockProperties.copyFrom(blockProperties)
        memberProperties.appendAttribute('objType', self.getMemberType())
        memberProperties.extendAttributes(self.getMemberLoadAttributes())
        f= factor*self.getOrientation(connectionOrigin)
        extrusionVector= self.getExtrusionVector(f)
        return self.shape.getBlockData(org= self.memberOrigin, extrusionVDir= extrusionVector, weakAxisVDir= self.jVector, blockProperties= memberProperties)
    
    def getFrontalWeldBlocks(self, flangeWeldLegSize, webWeldLegSize, blockProperties= None):
        ''' Return the lines corresponding to weld beads with a frontal plate.

        :param flangeWeldLegSize: leg size for the weld to the flange.
        :param webWeldLegSize: leg size for the weld to the web.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        frontalWeldProperties= bte.BlockProperties.copyFrom(blockProperties)
        frontalWeldProperties.appendAttribute('objType', 'weld')
        frontalWeldProperties.appendLabel('welds')
        return self.shape.getWeldBlockData(flangeWeldLegSize, webWeldLegSize, blockProperties= frontalWeldProperties)
    
    def getLoadData(self, internalForces, origin):
        ''' Extracts the internal forces for this member from the
            argument.

        :param internalForces: internal forces of the connected elements.
        :param origin: connection origin.
        '''
        originPos= origin.getInitialPos3d
        orientation= self.getOrientation(originPos)
        self.internalForces= dict()
        for comb in internalForces:
            loads= internalForces[comb]
            for elemId in loads:
                if(int(elemId)==self.eTag): # loads for this member
                    elemLoads= loads[elemId]
                    elemInternalForces= elemLoads['internalForces']
                    iForces= None
                    if(orientation>0):
                        iForces= elemInternalForces['0']
                    else:
                        iForces= elemInternalForces['1']
                    loadPos= [originPos.x,originPos.y,originPos.z]
                    self.internalForces[comb]= (loadPos, iForces)
        if(len(self.internalForces)==0):
            lmsg.warning('No internal forces found for element: '+str(self.eTag))           
        return self.internalForces

class ConnectionMetaData(object):
    ''' Data of a connection.

    :ivar originNode: origin node of the connection.
    :ivar column: column member.
    :iver beams: beam members.
    :ivar diagonals: diagonal members.
    :ivar boltSteel: steel type for bolts.
    '''
    def __init__(self, originNode, column, beams, diagonals, boltSteel= None):
        ''' Constructor.

        :param connectionData: connection origin node and elements connected 
                               to it classified as column or diagonals.
        '''
        self.originNode= originNode
        self.column= column
        self.beams= beams
        self.diagonals= diagonals
        self.boltSteel= boltSteel
        
    def getOrigin(self):
        ''' Return the connection origin.'''
        return self.originNode.getInitialPos3d

    def getReferenceSystem(self):
        ''' Return the connection reference system.'''
        origin= self.getOrigin()
        #plane= geom.Plane3d(origin, self.column.getDirection(origin))
        return geom.Ref3d3d(origin, self.column.jVector, self.column.kVector)
    
    def getColumnShape(self):
        ''' Return the shape of the column shaft.'''
        return self.column.shape
    
    def getBeamShapes(self):
        ''' Return the shapes of the joint beams.'''
        retval= list()
        for b in self.beams:
            retval.append(b.shape)
        return retval

    def getBeamsTop(self):
        ''' Return the maximum local z coordinate of the beams in the
            connection.'''
        retval= 0 # if no beams, no top.
        origin= self.getOrigin()
        ref= self.getReferenceSystem()
        for b in self.beams:
            bottomFlangePts, topFlangePts= b.getMidPoints(origin)
            beamPoints= bottomFlangePts+topFlangePts
            for p in beamPoints:
                lp= ref.getPosLocal(p)
                retval= max(retval,lp.z)
        return retval
                    
    def getDiagonalShapes(self):
        ''' Return the shapes of the diagonals.'''
        retval= list()
        for d in self.diagonals:
            retval.append(d.shape)
        return retval
    
    def getColumnShapeBlocks(self, factor, blockProperties= None):
        ''' Return the faces of the column shaft.

        :param factor: factor multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        beamsTop= self.getBeamsTop()
        ref= self.getReferenceSystem()
        origin= self.getOrigin()
        columnOrigin= origin+1.25*beamsTop*self.column.iVector
        return self.column.getMemberBlocks(origin,columnOrigin,factor, blockProperties)

    def getBeamShapeBlocks(self, factor, blockProperties= None):
        ''' Return the faces of the beams.

        :param factor: factor multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        retval= bte.BlockData()
        beamShapeProperties= bte.BlockProperties.copyFrom(blockProperties)
        beamShapeProperties.appendAttribute('objType', 'beam')
        originNodeTag= str(self.originNode.tag)
        beamShapeProperties.appendAttribute('jointId', originNodeTag)
        for b in self.beams:
            webPlane= self.getColumnWebMidPlane()
            angleWithWeb= webPlane.getAngleWithVector3d(b.iVector)
            columnShape= self.getColumnShape()
            columnHalfB= columnShape.getFlangeWidth()/2.0
            columnHalfH= columnShape.h()/2.0
            origin= self.getOrigin()
            beamOrientation= b.getOrientation(origin)
            if(abs(angleWithWeb)<1e-3): # beam parallel to web => flange beam.
                beam_label= 'flange_beam'
                b.connectedTo= 'flange'
                tf= columnShape.getFlangeThickness()
                offset= (25e-3+columnHalfH-tf/2.0)*beamOrientation
            else: # beam normal to web  => web beam
                beam_label= 'web_beam'
                b.connectedTo= 'web'
                offset= (20e-3+columnHalfB)*beamOrientation
            beamShapeProperties.appendAttribute('beamLabel', beam_label)
            beamOrigin= origin+offset*b.iVector # beam direction
            retval.extend(b.getMemberBlocks(origin, beamOrigin, factor, beamShapeProperties))
        return retval
    
    def getColumnWebDirection(self):
        ''' Return the direction of the column web.'''
        return self.column.getWebDirection(False)

    def getColumnWebMidPlane(self):
        ''' Return the mid-plane of the column web.'''
        return self.column.getWebMidPlane(origin= self.getOrigin(), factor= self.columnLengthFactor)

    def getColumnBottomFlangeMidPlane(self):
        ''' Return the mid-plane of the column bottom flange.'''
        return self.column.getBottomFlangeMidPlane(origin= self.getOrigin(), factor= self.columnLengthFactor)

    def getColumnTopFlangeMidPlane(self):
        ''' Return the mid-plane of the column bottom flange.'''
        return self.column.getTopFlangeMidPlane(origin= self.getOrigin(), factor= self.columnLengthFactor)

    def getColumnIntersectionPoint(self, sg):
        ''' Get the intersection of the segment with the column
            mid planes.

        :param sg: segment to intersect.
        '''
        return self.column.getIntersectionPoint(origin= self.getOrigin(), factor= self.columnLengthFactor, sg= sg)

    def getLoadData(self, internalForces):
        ''' Extracts the internal forces for each member from the
            argument.

        :param internalForces: internal forces of the connected elements.
        '''
        retval= dict()
        retval[self.column.eTag]= self.column.getLoadData(internalForces, self.originNode)
        for d in self.diagonals:
            retval[d.eTag]= d.getLoadData(internalForces, self.originNode)
        return retval

def getConnectedMembers(xcSet, ConnectedMemberType= ConnectedMemberMetaData):
    '''Return the node, and the data of the members
       connected to the nodes of the argument set.

    :param xcSet: set containing the nodes.
    '''
    retval= dict()
    for n in xcSet.getNodes:
        elements= n.getConnectedElements()
        diagonals= list()
        beams= list()
        column= None
        for e in elements:
            cm= ConnectedMemberType(e)
            memberSort= cm.getMemberSort()
            if(memberSort==MemberSort.diagonal):
                diagonals.append(cm)
            elif(memberSort==MemberSort.horizontal):
                beams.append(cm)
            elif(cm.shape.name.startswith('W')):
                column= cm
        retval[n.tag]= ConnectionMetaData(n,column,beams,diagonals)
    return retval

