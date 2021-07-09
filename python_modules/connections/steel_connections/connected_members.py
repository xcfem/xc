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

import sys
import enum
import math
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

    def getAxis(self, origin= None):
        ''' Return the member axis.

        :param origin: member origin.
        '''
        if(not origin):
            origin= self.memberOrigin
        return geom.Line3d(origin, self.iVector)
    
    def getMidPoints(self, origin= None):
        ''' Return the position expressed in global coordinates
            of the points at the middle of the shape.

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

    def getWebMidPlaneContourPoints(self):
        ''' Return the contour points at the mid-plane of the member web.'''
        return self.shape.getWebMidPlaneContourPoints(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection())
    
    def getWebMidPlane(self):
        ''' Return the mid-plane of the member web. '''
        return self.shape.getWebMidPlane(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection())
    
    def getClosingPlateContourPoints(self, positiveSide: bool):
        ''' Return the contour points of the plate that closes the shape
            connection the borders of the top and bottom flanges
            at the "positive" or "negative" sides of the web.

        :param positiveSide: if true return the half at the positive side of 
                             the web, otherwise return the half at the negative side.
        '''
        return self.shape.getClosingPlateContourPoints(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection(), positiveSide= positiveSide)
    
    def getClosingPlateMidPlane(self, positiveSide: bool):
        ''' Return the mid plane of the plate that closes the shape
            connection the borders of the top and bottom flanges
            at the "positive" or "negative" sides of the web.

        :param positiveSide: if true return the half at the positive side of 
                             the web, otherwise return the half at the negative side.
        '''
        return self.shape.getClosingPlateMidPlane(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection(), positiveSide= positiveSide)
    
    def getHalfBottomFlangeMidPlaneContourPoints(self, positiveSide: bool):
        ''' Return the contour points at the mid-plane of HALF the bottom flange.

        :param positiveSide: if true return the half at the positive side of 
                             the web, otherwise return the half at the negative side.
        '''
        return self.shape.getHalfBottomFlangeMidPlaneContourPoints(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection(), positiveSide= positiveSide)
    
    def getBottomFlangeMidPlaneContourPoints(self):
        ''' Return the contour points at the mid-plane of the bottom flange.'''
        return self.shape.getBottomFlangeMidPlaneContourPoints(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection())

    def getBottomFlangeMidPlane(self):
        ''' Return the mid-plane of the bottom flange.'''
        return self.shape.getBottomFlangeMidPlane(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection())

    def getHalfTopFlangeMidPlaneContourPoints(self, positiveSide: bool):
        ''' Return the contour points at the mid-plane of HALF the top flange.

        :param positiveSide: if true return the half at the positive side of 
                             the web, otherwise return the half at the negative side.
        '''
        return self.shape.getHalfTopFlangeMidPlaneContourPoints(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection(), positiveSide= positiveSide)
    
    def getTopFlangeMidPlaneContourPoints(self):
        ''' Return the contour points at the mid-plane of the top flange.
        '''
        return self.shape.getTopFlangeMidPlaneContourPoints(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.getWebDirection())
    
    def getTopFlangeMidPlane(self):
        ''' Return the mid-plane of the top flange.'''
        return self.shape.getTopFlangeMidPlane(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.jVector)

    def getWebIntersectionPoint(self,sg: geom.Segment3d):
        ''' Get the intersection of the segment with the member
            mid planes.

        :param sg: segment to intersect.
        '''
        webPlane= self.getWebMidPlane()
        ray= geom.Ray3d(sg.getFromPoint(), sg.getToPoint())
        retval= webPlane.getIntersection(ray)
        return retval
    
    def getIntersectionPoint(self, sg: geom.Segment3d):
        ''' Get the intersection of the segment with the member
            mid planes.

        :param sg: segment to intersect.
        '''
        webPlane= self.getWebMidPlane()
        angleWithWeb= webPlane.getAngle(sg.getVDir())
        orientation= getSegmentOrientation(self.memberOrigin, sg)
        if(orientation<0.0):
            sg.swap()
        p1= sg.getToPoint() # Farthest point from self.memberOrigin.
        p0= sg.getFromPoint()
        retval= None
        ray= geom.Ray3d(p0, p1)
        if(abs(angleWithWeb)<1e-3): # segment parallel to web.
            bottomFlangeMidPlane= self.getBottomFlangeMidPlane()
            pIntA= bottomFlangeMidPlane.getIntersection(ray)
            topFlangeMidPlane= self.getTopFlangeMidPlane()
            pIntB= topFlangeMidPlane.getIntersection(ray)
            if(pIntA.exists): # segment intersects bottom flange.
                if(pIntB.exists): # segment intersects both flanges.
                    dA= p0.dist2(pIntA)
                    dB= p0.dist2(pIntB)
                    if(dA<dB):
                        retval= pIntA
                    else:
                        retval= pIntB
                else: # pIntB doesn't exists.
                    retval= pIntA
            else:
                if(pIntB.exists): # segment intersects top flange.
                    retval= pIntB
                # else: # no intersection.
                #     className= type(self).__name__
                #     methodName= sys._getframe(0).f_code.co_name
                #     lmsg.error(className+'.'+methodName+': intersection point not found for segment: '+str(sg))
            # if(p1.dist2(retval)>sg.getLength()):
            #     className= type(self).__name__
            #     methodName= sys._getframe(0).f_code.co_name
            #     lmsg.warning(className+'.'+methodName+': intersection point outside the segment: '+str(sg))
        else: # segment normal to web
            retval= webPlane.getIntersection(sg)
        return retval
    
    def getMemberLoadAttributes(self, connectionOrigin):
        ''' Return the attributes with the loading data
            for this member.

        :param connectionOrigin: connection origin.
        '''
        orientation= self.getOrientation(connectionOrigin)
        loadDirI= orientation*self.iVector
        loadDirJ= orientation*self.jVector
        loadDirK= orientation*self.kVector
        retval= dict()
        retval['loadTag']= self.eTag
        retval['loadDirI']= [loadDirI.x, loadDirI.y, loadDirI.z]
        retval['loadDirJ']= [loadDirJ.x, loadDirJ.y, loadDirJ.z]
        retval['loadDirK']= [loadDirK.x, loadDirK.y, loadDirK.z]
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
        memberProperties.extendAttributes(self.getMemberLoadAttributes(connectionOrigin))
        f= factor*self.getOrientation(connectionOrigin)
        self.extrusionVector= self.getExtrusionVector(f)
        return self.shape.getBlockData(org= self.memberOrigin, extrusionVDir= self.extrusionVector, weakAxisVDir= self.jVector, blockProperties= memberProperties)
    
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

    def getFlangeBoltedPlateRefSys(self, connectionOrigin, flangeBoltedPlate, topFlange):
        ''' Return the position of the center for the top flange
            bolted plate.

        :param connectionOrigin: origin for the connection.
        :param flangeBoltedPlate: bolted plate attached to flange.
        :param topFlange: true if it's the top flange.
        '''
        baseVectors= self.getDirection(connectionOrigin)
        flangeThickness= self.shape.getFlangeThickness()
        platesThickness= flangeThickness+flangeBoltedPlate.thickness
        halfHFlange= (self.shape.h()-flangeThickness)/2.0
        halfHPlate= halfHFlange+platesThickness/2.0
        halfD= flangeBoltedPlate.length/2.0
        ## Compute position of the plate center.
        if(topFlange): # Top flange reference system
            plateCenter= self.memberOrigin + halfHPlate*baseVectors[1] + halfD*baseVectors[0]
        else: # Bottom flange reference system
            plateCenter= self.memberOrigin - halfHPlate*baseVectors[1] + halfD*baseVectors[0]
        return geom.Ref3d3d(plateCenter, baseVectors[0], baseVectors[2])

    def getTopFlangeBoltedPlateRefSys(self, connectionOrigin, topFlangeBoltedPlate):
        ''' Return the position of the center for the top flange
            bolted plate.

        :param connectionOrigin: origin for the connection.
        :param topFlangeBoltedPlate: top flange bolted plate.
        '''
        return self.getFlangeBoltedPlateRefSys(connectionOrigin, topFlangeBoltedPlate, topFlange= True)
    
    def getBottomFlangeBoltedPlateRefSys(self, connectionOrigin, bottomFlangeBoltedPlate):
        ''' Return the position of the center for the bottom flange
            bolted plate.

        :param connectionOrigin: origin for the connection.
        :param bottomFlangeBoltedPlate: bottom flange bolted plate.
        '''
        return self.getFlangeBoltedPlateRefSys(connectionOrigin, bottomFlangeBoltedPlate, topFlange= False)
    
    def getShearTabRefSys(self, connectionOrigin, shearTab, positiveSide):
        ''' Return the position of the center for the shear tab.

        :param connectionOrigin: origin for the connection.
        :param shearTab: shear tab attached to web.
        :param positiveSide: true if it's the positive side of the web.
        '''
        baseVectors= self.getDirection(connectionOrigin)
        webThickness= self.shape.getWebThickness()
        platesThickness= webThickness+shearTab.thickness
        halfWPlate= platesThickness/2.0
        halfD= shearTab.width/2.0
        ## Compute position of the plate center.
        if(positiveSide): # positive side of the web
            plateCenter= self.memberOrigin + halfWPlate*baseVectors[2] + halfD*baseVectors[0]
        else: # negative side of the web
            plateCenter= self.memberOrigin - halfWPlate*baseVectors[2] + halfD*baseVectors[0]
        return geom.Ref3d3d(plateCenter, baseVectors[1], baseVectors[0])
    
    def computeWeldLines(self, column, plate):
        ''' Return the lines of the plate that will be welded with the column and
            refines the "connectedTo" property of the plate by specifying the side
            of the web or the flage (top or bottom) to which the plate is attached.

        :param column: column to which this member is attached to.
        :param plate: plate to connect.
        '''
        weldDict= dict() # Weld line container.
        weldLegSize= 0.0 # Weld size.
        plateOrigin= plate.refSys.getOrg()
        platePlane= plate.refSys.getXYPlane()
        angle= platePlane.getAngle(column.iVector)
        angleTol= math.radians(1) # one degrees
        # Compute contours of the column flanges and web.
        columnTopFlangeContour= geom.Polygon3d(column.getTopFlangeMidPlaneContourPoints())
        columnBottomFlangeContour= geom.Polygon3d(column.getBottomFlangeMidPlaneContourPoints())
        columnWebContour= geom.Polygon3d(column.getWebMidPlaneContourPoints())
        columnWebMidPlane= columnWebContour.getPlane()
        if((abs(angle)<angleTol) or (abs(angle-math.pi)<angleTol)): # beam shear tab plate
            if(plate.connectedTo=='column_web'): # plate connected to the column web
                # Compute the side of the column web to which
                # the shear tab will be attached.
                positiveSide= columnWebMidPlane.positiveSide(plateOrigin)
                if(positiveSide):
                    plate.connectedTo= 'column_web+'
                else:
                    plate.connectedTo= 'column_web-'
                # Get the beam flange plates.
                beamTopPlate= column.connectedPlates[plate.connectedTo+','+'beam_top_flange']
                beamBottomPlate= column.connectedPlates[plate.connectedTo+','+'beam_bottom_flange']
                # Compute intersections with the column web
                # and the flange plates.
                ## Web.
                columnWebLine= columnWebContour.getIntersection(platePlane)
                ## Top plate.
                topPlatePlane= beamTopPlate.refSys.getXYPlane()
                plateHalfSpace= geom.HalfSpace3d(topPlatePlane, plateOrigin)
                columnWebLine= plateHalfSpace.clip(columnWebLine)
                ## Bottom plate.
                bottomPlatePlane= beamBottomPlate.refSys.getXYPlane()
                plateHalfSpace= geom.HalfSpace3d(bottomPlatePlane, plateOrigin)
                columnWebLine= plateHalfSpace.clip(columnWebLine)
                # Compute intersections with the top and bottom plates.
                topPlateLine= platePlane.getIntersection(topPlatePlane)
                bottomPlateLine= platePlane.getIntersection(bottomPlatePlane)
                ## Clip by the plane of the column web.
                topPlateLineFromPoint= columnWebMidPlane.getIntersection(topPlateLine)
                bottomPlateLineFromPoint= columnWebMidPlane.getIntersection(bottomPlateLine)
                ## Clip by the virtual "closing" plate.
                closingPlateMidPlanePlus= column.getClosingPlateMidPlane(True)
                distPlus2= closingPlateMidPlanePlus.dist2(plateOrigin)
                closingPlateMidPlaneMinus= column.getClosingPlateMidPlane(False)
                distMinus2= closingPlateMidPlaneMinus.dist2(plateOrigin)
                if(distPlus2<distMinus2):
                    closingPlateMidPlane= closingPlateMidPlanePlus
                else:
                    closingPlateMidPlane= closingPlateMidPlaneMinus                    
                topPlateLineToPoint= closingPlateMidPlane.getIntersection(topPlateLine)
                topPlateLine= geom.Segment3d(topPlateLineFromPoint, topPlateLineToPoint)
                bottomPlateLineToPoint= closingPlateMidPlane.getIntersection(bottomPlateLine)
                bottomPlateLine= geom.Segment3d(bottomPlateLineFromPoint, bottomPlateLineToPoint)
                weldDict['topPlateWeld']= topPlateLine
                weldDict['columnWebWeld']= columnWebLine
                weldDict['bottomPlateWeld']= bottomPlateLine
                ## Compute weld size.
                wls1= plate.getFilletWeldLegSize(otherThickness= column.shape.getWebThickness())
                wls2= plate.getFilletWeldLegSize(otherThickness= beamTopPlate.thickness)
                wls3= plate.getFilletWeldLegSize(otherThickness= beamBottomPlate.thickness)
                weldLegSize= (wls1+wls2+wls3)/3.0
            else: # plate connected to one flange
                columnTopFlangeLine= columnTopFlangeContour.getIntersection(platePlane)
                dTop= columnTopFlangeLine.dist(plateOrigin)
                columnBottomFlangeLine= columnBottomFlangeContour.getIntersection(platePlane)
                dBottom= columnBottomFlangeLine.dist(plateOrigin)
                if(dBottom<dTop): # column bottom flange is closer.
                    plate.connectedTo= 'column_bottom_flange'
                    weldDict['columnBottomFlangeWeld']= columnBottomFlangeLine
                else: # column top flage is closer.
                    plate.connectedTo= 'column_top_flange' 
                    weldDict['columnTopFlangeWeld']= columnTopFlangeLine
                ## Compute weld size.
                weldLegSize= plate.getFilletWeldLegSize(otherThickness= column.shape.getFlangeThickness())
        else: # beam flange plate
            # Compute contours for the column flanges.
            columnTopFlangeLine= columnTopFlangeContour.getIntersection(platePlane)
            columnBottomFlangeLine= columnBottomFlangeContour.getIntersection(platePlane)
            if(plate.connectedTo=='column_web'): # plate connected to the column web
                if(columnWebMidPlane.positiveSide(plateOrigin)):
                    plate.connectedTo= 'column_web+'
                else:
                    plate.connectedTo= 'column_web-'
                columnWebLine= columnWebContour.getIntersection(platePlane)
                plateHalfSpace= geom.HalfSpace3d(columnWebMidPlane, plateOrigin)
                halfTopFlange= plateHalfSpace.clip(columnTopFlangeLine)
                halfBottomFlange= plateHalfSpace.clip(columnBottomFlangeLine)
                weldDict['columnBottomFlangeWeld']= halfBottomFlange
                weldDict['columnWebWeld']= columnWebLine
                weldDict['columnTopFlangeWeld']= halfTopFlange
                ## Compute weld size.
                weldLegSize= (plate.getFilletWeldLegSize(otherThickness= column.shape.getWebThickness())+2*plate.getFilletWeldLegSize(otherThickness= column.shape.getFlangeThickness()))/3.0
            else: # plate connected to one flange
                dTop= columnTopFlangeLine.dist(plateOrigin)
                dBottom= columnBottomFlangeLine.dist(plateOrigin)
                if(dBottom<dTop):
                    plate.connectedTo= 'column_bottom_flange'
                    # Split the line by the web plane to make two weld segments
                    # that lie on each half of the flange.
                    pInt= columnWebMidPlane.getIntersection(columnBottomFlangeLine)
                    if pInt.notAPoint():
                        lmsg.error('intersection point not found.')
                    columnBottomFlangeLineA= geom.Segment3d(columnBottomFlangeLine.getFromPoint(), pInt)
                    columnBottomFlangeLineB= geom.Segment3d(pInt, columnBottomFlangeLine.getToPoint())
                    weldDict['columnBottomFlangeWeldA']= columnBottomFlangeLineA
                    weldDict['columnBottomFlangeWeldB']= columnBottomFlangeLineB
                else:
                    plate.connectedTo= 'column_top_flange' 
                    # Split the line by the web plane to make two weld segments
                    # that lie on each half of the flange.
                    pInt= columnWebMidPlane.getIntersection(columnTopFlangeLine)
                    if pInt.notAPoint():
                        lmsg.error('intersection point not found.')
                    columnTopFlangeLineA= geom.Segment3d(columnTopFlangeLine.getFromPoint(), pInt)
                    columnTopFlangeLineB= geom.Segment3d(pInt, columnTopFlangeLine.getToPoint())
                    weldDict['columnTopFlangeWeldA']= columnTopFlangeLineA
                    weldDict['columnTopFlangeWeldB']= columnTopFlangeLineB
                ## Compute weld size.
                weldLegSize= plate.getFilletWeldLegSize(otherThickness= column.shape.getFlangeThickness())
        # Define welds to the connected member.
        plate.setWeldLines(weldDict)
        plate.weldLegSize= weldLegSize
        # Update the plates connected to the column
        if(not hasattr(column,'connectedPlates')):
            column.connectedPlates= dict()
        key= plate.connectedTo +','+plate.location
        column.connectedPlates[key]= plate
    
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

    def getBeamsTopBottom(self):
        ''' Return the maximum and minimum local z coordinate of the beams
            in the connection.'''
        top= 0; bottom= 0; # if no beams, no top, no bottom.
        origin= self.getOrigin()
        ref= self.getReferenceSystem()
        for b in self.beams:
            bottomFlangePts, topFlangePts= b.getMidPoints(origin)
            beamPoints= bottomFlangePts+topFlangePts
            for p in beamPoints:
                lp= ref.getLocalPosition(p)
                top= max(top,lp.z)
                bottom= min(bottom, lp.z)
        return top, bottom
                    
    def getDiagonalShapes(self):
        ''' Return the shapes of the diagonals.'''
        retval= list()
        for d in self.diagonals:
            retval.append(d.shape)
        return retval
    
    def getColumnShapeBlocks(self, factor, intermediateJoint, blockProperties= None):
        ''' Return the faces of the column shaft.

        :param factor: factor multiplies the unary direction vector
                       of the member to define its extrusion 
                       direction and length.
        :param intermediateJoint: true if the joint doesn't correspond
                                  to an extremity of the column (i.e.
                                  a base plate or the top end of the column).
        :param blockProperties: labels and attributes to assign to the newly 
                                created blocks.
        '''
        beamsTop, beamsBottom= self.getBeamsTopBottom()
        # Compute the position of the column origin to be sure
        # that the element connections are "covered".
        connectionOrigin= self.getOrigin()
        columnOrientation= self.column.getOrientation(connectionOrigin)
        if(intermediateJoint):
            ff= 6.0
        else:
            ff= 1.25
        if(columnOrientation<0):
            columnOrigin= connectionOrigin+ff*beamsTop*self.column.iVector
        else:
            columnOrigin= connectionOrigin+ff*beamsBottom*self.column.iVector
        return self.column.getMemberBlocks(connectionOrigin, columnOrigin, factor, blockProperties)

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
            # Angle of the beam axis with the column web.
            columnWebPlane= self.getColumnWebMidPlane()
            angleWithColumnWeb= columnWebPlane.getAngle(b.iVector)
            # Column dimensions.
            columnShape= self.getColumnShape()
            columnHalfB= columnShape.getFlangeWidth()/2.0
            columnHalfH= columnShape.h()/2.0
            origin= self.getOrigin()
            beamOrientation= b.getOrientation(origin)
            lbl= None
            if(abs(angleWithColumnWeb)<1e-3): # beam parallel to column web => flange beam.
                lbl= 'column_flange'
                tf= columnShape.getFlangeThickness()
                offset= (25e-3+columnHalfH-tf/2.0)*beamOrientation # 25 mm gap from flange mid-plane
            else: # beam normal to column web  => web beam
                lbl= 'column_web'
                offset= (20e-3+columnHalfB)*beamOrientation # 20 mm gap from column "envelope"
            beam_label= lbl+'_beam'
            b.connectedTo= lbl
            beamShapeProperties.appendAttribute('beamLabel', beam_label)
            beamOrigin= origin+offset*b.iVector # beam direction
            retval.extend(b.getMemberBlocks(origin, beamOrigin, factor, beamShapeProperties))
        return retval
    
    def getColumnWebDirection(self):
        ''' Return the direction of the column web.'''
        return self.column.getWebDirection(False)

    def getColumnWebMidPlane(self):
        ''' Return the mid-plane of the column web.'''
        return self.column.getWebMidPlane()

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
        return self.column.getIntersectionPoint(sg= sg)

    def getLoadData(self, internalForces):
        ''' Extracts the internal forces for each member from the
            argument.

        :param internalForces: internal forces of the connected elements.
        '''
        retval= dict()
        # Column internal forces.
        retval[self.column.eTag]= self.column.getLoadData(internalForces, self.originNode)
        # Beam internal forces.
        for b in self.beams:
            retval[b.eTag]= b.getLoadData(internalForces, self.originNode)
        # Diagonal internal forces.
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

