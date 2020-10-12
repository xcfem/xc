# -*- coding: utf-8 -*-
''' Connecttion design. Obtain connection member data from the 
    model of the whole structure.'''

from __future__ import division
from __future__ import print_function

import enum
import math
import ezdxf
import json
from import_export import block_topology_entities as bte
import xc_base
import geom
from misc_utils import log_messages as lmsg
from import_export import neutral_mesh_description as nmd
from connections import gusset_plate as gp

class MemberSort(enum.IntEnum):
    '''Classification of members by its angle with the horizon.'''
    horizontal= 0
    vertical= 1
    diagonal= 2

class ConnectedMemberMetaData(object):
    ''' Data of a connected member.

    :ivar eTag: element tag.
    :ivar shape: cross section steel shape.
    :ivar iVector: axis direction vector.
    :ivar jVector: J direction vector.
    :ivar kVector: K direction vector.
    :ivar nodePositions: positions of the element nodes.
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
        ''' Return the member type (beam, column or diagonal.'''
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
        d0= origin.dist2Pos3d(self.nodePositions[0])
        d1= origin.dist2Pos3d(self.nodePositions[1])
        if(d0>d1):
            retval= -1.0
        return retval

    def getDirection(self, origin):
        ''' Return the member direction vector with respect
            to the origin (to or from the origin).'''
        retval= self.iVector
        retval*= self.getOrientation(origin)
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
        return self.getJVector3d(False)

    def getWebMidPlane(self, origin, factor):
        ''' Return the mid-plane of the member web.

        :param origin: connection origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        extrusionVector= self.getExtrusionVector(factor)
        return self.shape.getWebMidPlane(org= origin, extrusionVDir= extrusionVector)
    
    def getBottomFlangeMidPlane(self, origin, factor):
        ''' Return the mid-plane of the bottom flange.

        :param origin: connection origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        extrusionVector= self.getExtrusionVector(factor)
        return self.shape.getBottomFlangeMidPlane(org= origin, extrusionVDir= extrusionVector)

    def getTopFlangeMidPlane(self, origin, factor):
        ''' Return the mid-plane of the top flange.

        :param origin: connection origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        extrusionVector= self.getExtrusionVector(factor)
        return self.shape.getTopFlangeMidPlane(org= origin, extrusionVDir= extrusionVector)
    
    def getSegmentOrientation(self, origin, sg):
        ''' Return the diagonal direction vector.

        :param sg: segment to compute the orientation for.
        '''
        retval= 1.0
        d0= origin.dist2Pos3d(sg.getFromPoint())
        d1= origin.dist2Pos3d(sg.getToPoint())
        if(d0>d1):
            retval= -1.0

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
        orientation= self.getSegmentOrientation(origin, sg)
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
                    dA= p1.dist2Pos3d(pIntA)
                    dB= p1.dist2Pos3d(pIntB)
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
        else: # diagonal normal to web
            p0= webPlane.getIntersection(sg)
        return p0
    
    def getMemberLoadLabels(self):
        ''' Return the labels with the loading data
            for this member.
        '''
        loadTag= 'loadTag_'+str(self.eTag)
        loadDirI= 'loadDirI_'+str(self.iVector)
        loadDirJ= 'loadDirJ_'+str(self.jVector)
        loadDirK= 'loadDirK_'+str(self.kVector)
        return [loadTag, loadDirI, loadDirJ, loadDirK]
    
    def getShapeBlocks(self, origin, factor, lbls= None):
        ''' Return the faces of the member.

        :param origin: connection origin.
        :param factor: factor that multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        labels= list()
        if(lbls):
            labels.extend(lbls)
        labels.append(self.getMemberType())
        labels.extend(self.getMemberLoadLabels())
        extrusionVector= self.getExtrusionVector(factor)
        return self.shape.getBlockData(org= origin, extrusionVDir= extrusionVector, lbls= labels)
    
    def getFrontalWeldBlocks(self, flangeWeldLegSize, webWeldLegSize, lbls= None):
        ''' Return the lines corresponding to weld beads with a frontal plate.

        :param flangeWeldLegSize: leg size for the weld to the flange.
        :param webWeldLegSize: leg size for the weld to the web.
        :param lbls: labels for the created blocks.
        '''
        labels=[self.getMemberType(),'welds']
        if(lbls):
            labels.extend(lbls)
        return self.shape.getWeldBlockData(flangeWeldLegSize, webWeldLegSize, lbls= labels)
    
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
    '''
    def __init__(self, originNode, column, beams, diagonals):
        ''' Constructor.

        :param connectionData: connection origin node and elements connected 
                               to it classified as column or diagonals.
        '''
        self.originNode= originNode
        self.column= column
        self.beams= beams
        self.diagonals= diagonals
        
    def getOrigin(self):
        ''' Return the connection origin.'''
        return self.originNode.getInitialPos3d
    
    def getColumnShape(self):
        ''' Return the shape of the column shaft.'''
        return self.column.shape
    
    def getBeamShapes(self):
        ''' Return the shapes of the joint beams.'''
        retval= list()
        for b in self.beams:
            retval.append(b.shape)
        return retval
    
    def getDiagonalShapes(self):
        ''' Return the shapes of the diagonals.'''
        retval= list()
        for d in self.diagonals:
            retval.append(d.shape)
        return retval
    
    def getColumnShapeBlocks(self, factor, lbls= None):
        ''' Return the faces of the column shaft.

        :param factor: factor multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and lenght.
        '''
        return self.column.getShapeBlocks(self.getOrigin(),factor, lbls)
    
    def getColumnWebDirection(self):
        ''' Return the direction of the column web.'''
        return self.column.getJVector3d(False)

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

    
def getConnectedMembers(xcSet):
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
            cm= ConnectedMemberMetaData(e)
            memberSort= cm.getMemberSort()
            print(e.tag, cm.shape.name, memberSort)
            if(memberSort==MemberSort.diagonal):
                diagonals.append(cm)
            elif(memberSort==MemberSort.horizontal):
                beams.append(cm)
            elif(cm.shape.name.startswith('W')):
                column= cm
        retval[n.tag]= ConnectionMetaData(n,column,beams,diagonals)
    return retval
