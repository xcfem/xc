# -*- coding: utf-8 -*-
''' Code to generate the geometry of a connection from the data
    obtained from the structure model.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2021, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
import math
import json
from misc_utils import log_messages as lmsg
from import_export import block_topology_entities as bte
from import_export import neutral_mesh_description as nmd
import xc_base
import geom
from connections.steel_connections import bolted_plate
from connections.steel_connections import connected_members
from connections.steel_connections import bolts
from connections.steel_connections import stiffeners
from materials import limit_state_checking_base as lsc
from postprocess import limit_state_data
from inspect import getmodule

class Connection(connected_members.ConnectionMetaData):
    ''' Naive connection model.

    :ivar columnLengthFactor: vector that multiplies the column unary
                               direction vector to obtain the length
                               of the column.
    :ivar beamLengthFactor: factor that multiplies the beam unary
                            direction vector to obtain the length
                            of the beam to modelize.
    :ivar gussetLengthFactor: factor that multiplies the bolted plate
                               length to obtain the lenght of the gusset
                               plate.
    :ivar beamsShearEfficiency: ratio between the design shear and 
                                the shear strength for the beams in
                                this connection.
    :ivar boltedPlateTemplate: bolted plate object: dimensions of the bolted 
                               plate and bolt type and arrangement.
    :ivar flangeGussetLegsSlope: tangent of the angle of the flange gusset legs
                                 with its member axis.
    :ivar webGussetBottomLegSlope: definition of the slope of the 
                                   gusset bottom leg.
    :ivar intermediateJoint: true if the joint doesn't correspond
                             to an extremity of the column (i.e.
                             a base plate or the top end of the column).
    '''
    def __init__(self, connectionMetaData, columnLengthFactor, beamLengthFactor, gussetLengthFactor, beamsShearEfficiency, boltedPlateTemplate, intermediateJoint= False):
        ''' Constructor.

        :param connectionMetaData: connection origin node and members 
                                   connected to it.
        :param columnLengthFactor: factor that multiplies the column unary
                                   direction vector to obtain the length
                                   of the column.
        :param beamLengthFactor: factor that multiplies the beam unary
                                 direction vector to obtain the length
                                 of the beam to modelize.
        :param gussetLengthFactor: factor that multiplies the bolted plate
                                   length to obtain the lenght of the gusset
                                   plate.
        :param beamsShearEfficiency: ratio between the design shear and 
                                    the shear strength for the beams in
                                    this connection.
        :param boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                    arrangement.
        :param intermediateJoint: true if the joint doesn't correspond
                                  to an extremity of the column (i.e.
                                  a base plate or the top end of the column).
                                  Defaults to false for backward compatibility
                                  reasons.
        '''
        super(Connection,self).__init__(connectionMetaData.originNode, connectionMetaData.column, connectionMetaData.beams, connectionMetaData.diagonals)
        self.columnLengthFactor= columnLengthFactor
        self.beamLengthFactor= beamLengthFactor
        self.gussetLengthFactor= gussetLengthFactor
        self.beamsShearEfficiency= beamsShearEfficiency
        self.boltedPlateTemplate= boltedPlateTemplate
        self.intermediateJoint= intermediateJoint
        self.flangeGussetLegsSlope= math.tan(math.radians(30))
        self.webGussetBottomLegSlope= 'vertical'

    def getBoltSteel(self):
        ''' Return the steel for the connection bolts.'''
        return self.boltedPlateTemplate.boltArray.bolt.steelType

    def getBoltedPlatesSteel(self):
        ''' Return the steel for the bolted plates.'''
        return self.boltedPlateTemplate.steelType

    def getMaterialModule(self):
        ''' material module used to instantiate some 
            connection elements (ASTM_materials, EC3_materials, ...)
        '''
        return getmodule(self.boltedPlateTemplate.__class__)

    def getSegmentOrientation(self, sg: geom.Segment3d):
        ''' Return the orientation of the segment (>0 => upwards, <0 downwards).

        :param sg: 3D segment.
        '''
        orientation= connected_members.getSegmentOrientation(self.getOrigin(), sg)
        if(sg.downwards()):
            orientation= -orientation
        return orientation
        
    def getFlangeGussetPlate(self, baseVectors, diagSegment: geom.Segment3d, gussetLength, halfChamfer, slope):
        ''' Return the GussetPlate object for the gusset attached to the
            flange.

        :param baseVectors: unary vectors of the gusset plate reference system.
        :param diagSegment: 3D segment on the diagonal axis.
        :param gussetLength: distance from the connection origin to the
                             gusset chamfer.
        :param halfChamfer: 3D vector from the gusset chamfer mid-point
                            to the end of the chamfer.
        :param slope: tangent of the angle of the gusset leg with its member axis.
        '''
        print('**** here getFlangeGussetPlate.')
        p0= self.getColumnIntersectionPoint(diagSegment) # intersection with the column
        gussetTip= p0-gussetLength*diagSegment.getVDir().normalized()
        retval= self.getMaterialModule().GussetPlate(self.boltedPlateTemplate, gussetTip, halfChamfer, ijkVectors= baseVectors)
        # Top leg
        p1, p2= retval.getSloppedTopLeg(slope, gussetLength)
        print('top leg: ', p1, p2)
        ## Clip top leg
        topLegSegment= geom.Segment3d(p1,p2)
        tmp= self.getNearestIntersectionPoint(topLegSegment) # intersection with the nearest member
        if(tmp):
            p2= tmp
        print('intersection: ', p2)
        corner= geom.Pos3d(p0.x, p0.y, p2.z)
        # Bottom leg.
        ## Orientation: upwards or downwards:
        diagonalOrientation= self.getSegmentOrientation(diagSegment)
        if(diagonalOrientation<0): # downwards
            p3, p4, p5= retval.getToColumnBottomLeg(corner, cutKnifePoint= 0.6)
            bottomLegSegment= geom.Segment3d(p3,p4)
            p4a= self.getConnectedPlatesIntersectionPoint(bottomLegSegment)
            if(p4a.z>p4.z): # The leg intersects a plate.
                p4= p4a
                p5= None
                corner= geom.Pos3d(corner.x, corner.y, p4.z)
        else:
            p3, p4= retval.getSloppedBottomLeg(slope, gussetLength)
            bottomLegSegment= geom.Segment3d(p3,p4)
            p4= self.getNearestIntersectionPoint(bottomLegSegment) # intersection with the nearest member
            p5= None
        if(p5): # knife point not cutted
            retval.setContour([p1, p2, corner, p5, p4, p3])
            topLegWeld= geom.Segment3d(p2, corner)
            bottomLegWeld= geom.Segment3d(p5, corner)
        else:
            retval.setContour([p1, p2, corner, p4, p3])
            topLegWeld= geom.Segment3d(p2, corner)
            bottomLegWeld= geom.Segment3d(p4, corner)
        # Weld definition.
        weldDict= dict() # Weld line container.
        ## Compute weld size.
        weldLegSize= retval.getFilletWeldLegSize(otherThickness= self.column.shape.getFlangeThickness()) # Weld size.
        retval.weldLegSize= weldLegSize
        ## Weld lines.
        weldDict['topLegWeld']= topLegWeld
        weldDict['bottomLegWeld']= bottomLegWeld
        retval.setWeldLines(weldDict)
        # Update the plates connected to the column
        if(not hasattr(self.column,'connectedPlates')):
            self.column.connectedPlates= dict()
        retval.connectedTo= 'column_flange'
        if(diagonalOrientation>0):
           retval.location= 'upwards_gusset_plate'
        else:
           retval.location= 'downwards_gusset_plate'
        key= retval.connectedTo +','+retval.location
        self.column.connectedPlates[key]= retval
        return retval;

    def getConnectedPlatesIntersectionPoint(self, sg: geom.Segment3d):
        ''' Get the intersection of the segment with the nearest of the
            plates connected to the column.

        :param sg: segment to intersect. The distances are computed with
                   respect to the first point of the segment.
        '''
        retval= None
        fromPoint= sg.getFromPoint()
        dist2= 6.023e23
        for key in self.column.connectedPlates:
            plate= self.column.connectedPlates[key]
            if(not 'gusset_plate' in key): # Ignore gusset plates.
                midPlane= plate.getMidPlane()                
                tmp= midPlane.getIntersection(sg)
                if(tmp.notAPoint()): #intersection not found
                    continue
                else:
                    d2= fromPoint.dist2(tmp)
                    if(d2<dist2):
                        if(plate.contour): # plate has a contour.
                            contour= geom.Polygon3d(plate.contour)
                            if(contour.In(tmp, .05)): # point inside plate.
                                retval= tmp
                                dist2= d2
                        else: # undefined contour
                            retval= tmp
                            dist2= d2
        if(retval):        
            if(retval.notAPoint()):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+': error computing intersection with connected plates.')
        return retval
    
    def getBeamsIntersectionPoint(self, sg: geom.Segment3d):
        ''' Get the intersection of the segment with the nearest of the
            beams connected to the column.

        :param sg: segment to intersect. The distances are computed with
                   respect to the first point of the segment.
        '''
        retval= None
        fromPoint= sg.getFromPoint()
        dist2= 6.023e23
        for b in self.beams:
            tmp= b.getIntersectionPoint(sg)
            if(tmp): # if intersection exists.
                d2= fromPoint.dist2(tmp)
                if(d2<dist2):
                    retval= tmp
                    dist2= d2
        if(retval):            
            if(math.isnan(retval.x) or math.isnan(retval.y) or math.isnan(retval.z)):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+': error intersecting with connected plates.')
        return retval

    def getNearestIntersectionPoint(self, sg: geom.Segment3d):
        ''' Get the intersection of the segment with the nearest of the
            plates connected to the column.

        :param sg: segment to intersect. The distances are computed with
                   respect to the first point of the segment.
        '''
        fromPoint= sg.getFromPoint()
        dist2= 6.023e23
        # intersection with the column.
        retval= self.getColumnIntersectionPoint(sg)
        if(retval):
            dist2= fromPoint.dist2(retval)
        # intersection with the connected plates.
        tmp= self.getConnectedPlatesIntersectionPoint(sg)
        if(tmp): # if intersection exists.
            d2= fromPoint.dist2(tmp)
            if(d2<dist2): # new intersection point is closer.
                retval= tmp
                dist2= d2
        # intersection with the beams.
        tmp= self.getBeamsIntersectionPoint(sg)
        if(tmp): # if intersection exists.
            d2= fromPoint.dist2(tmp)
            if(d2<dist2): # new intersection point is closer.
                retval= tmp
                dist2= d2
        if(retval):
            if(retval.notAPoint()):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+': error computing intersection with connected plates or members.')
                retval= None
        return retval
        
    def getWebGussetPlate(self, baseVectors, diagSegment: geom.Segment3d, gussetLength, halfChamfer, bottomLegSlope):
        ''' Return the GussetPlate object for the gusset attached to the
            web.

        :param baseVectors: unary vectors of the gusset plate reference system.
        :param diagSegment: 3D segment on the diagonal axis.
        :param gussetLength: distance from the connection origin to the
                             gusset chamfer.
        :param halfChamfer: 3D vector from the gusset chamfer mid-point
                            to the end of the chamfer.
        :param bottomLegSlope: if not 'vertical' tangent of the angle of the 
                               gusset bottom leg with its  member axis.
        '''
        print('**** here getWebGussetPlate.')
        origin= diagSegment.getFromPoint()
        gussetTip= origin+gussetLength*diagSegment.getVDir().normalized()
        retval= self.getMaterialModule().GussetPlate(self.boltedPlateTemplate, gussetTip, halfChamfer, ijkVectors= baseVectors)
        # Diagonal orientation downwards or upwards.
        diagonalOrientation= self.getSegmentOrientation(diagSegment)
        # Top leg.
        if(diagonalOrientation<0): # Downwards diagonal
            p1, p2= retval.getHorizontalTopLeg(origin)
            # Clip top leg with the column web.
            p2= self.column.getWebIntersectionPoint(geom.Segment3d(p1, p2))
        else:
            p1, p2= retval.getVerticalTopLeg(origin)
            ## Clip top leg with the neares member.
            p2= self.getNearestIntersectionPoint(geom.Segment3d(p1,p2)) # intersection with the nearest member.
        # Bottom leg.
        p3= None; p4= None
        if(bottomLegSlope=='vertical'):
            if(diagonalOrientation<0): # downwards diagonal
                p3, p4= retval.getVerticalBottomLeg(origin)
                ## Clip bottom leg segment.
                p4= self.getConnectedPlatesIntersectionPoint(geom.Segment3d(p3,p4)) # intersection with the connected plates.
                corner= geom.Pos3d(p2.x, p2.y, p4.z)
            else: # upwards diagonal
                p3, p4= retval.getHorizontalBottomLeg(origin)
                corner= geom.Pos3d(p4.x, p4.y, p2.z)
        else:
            p3, p4= retval.getSloppedBottomLeg(bottomLegSlope, gussetLength)
            ## Clip bottom leg segment.
            bottomLegSegment= geom.Segment3d(p3,p4)
            p4= self.column.getWebIntersectionPoint(bottomLegSegment) # intersection with the column web
            corner= geom.Pos3d(p4.x, p4.y, p2.z)
        retval.setContour([p1, p2, corner, p4, p3])
        # Weld definition.
        weldDict= dict() # Weld line container.
        ## Compute weld size.
        weldLegSize= retval.getFilletWeldLegSize(otherThickness= self.column.shape.getWebThickness()) # Weld size.
        retval.weldLegSize= weldLegSize
        ## Weld lines.
        weldDict['topLegWeld']= geom.Segment3d(p2, corner)
        weldDict['bottomLegWeld']= geom.Segment3d(p4, corner)
        retval.setWeldLines(weldDict)
        # Update the plates connected to the column
        if(not hasattr(self.column,'connectedPlates')):
            self.column.connectedPlates= dict()
        retval.connectedTo= 'column_web'
        if(diagonalOrientation>0): # diagonal upwards
           retval.location= 'upwards_gusset_plate'
        else:
           retval.location= 'downwards_gusset_plate'
        key= retval.connectedTo +','+retval.location
        self.column.connectedPlates[key]= retval
        return retval

    def getBoltedPlateTemplate(self):
        ''' Return the blocks corresponding to the plate
            bolted to the gusset plate.
        '''
        # Create bolted plate according to the material module.
        boltedPlate= self.getMaterialModule().BoltedPlate()
        # Assign parameters obtained from the template.
        boltedPlate.setFromDict(self.boltedPlateTemplate.getDict())
        boltedPlate.eccentricity= geom.Vector2d(.025,0.0)
        boltedPlate.length+= .05
        return boltedPlate
    
    def getGussetBlocksForDiagonal(self, diagonal, blockProperties= None):
        ''' Return the blocks that define the gusset for the
            diagonal argument.

        :param diagonal: diagonal to get the gusset for.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        def getHalfChamferVector(diagonal):
            ''' Return a vector normal to the diagonal contained in
                a vertical plane.'''
            iVector= diagonal.iVector
            # Horizontal vector perpendicular to the projection
            # of iVector on the horizontal plane.
            perpHoriz= geom.Vector3d(-iVector.y, iVector.x, 0.0)
            return iVector.cross(perpHoriz).normalized()
        print('here getGussetBlocksForDiagonal.')
        retval= bte.BlockData()
        origin= self.getOrigin()
        baseVectors= diagonal.getDirection(origin)
        extrusionLength= self.columnLengthFactor
        p1= origin+extrusionLength*baseVectors[0]
        webPlane= self.getColumnWebMidPlane()
        angleWithWeb= webPlane.getAngle(baseVectors[0])
        dgSegment= geom.Segment3d(origin,p1)
        gussetLength= self.gussetLengthFactor*self.boltedPlateTemplate.length
        halfChamferVector= getHalfChamferVector(diagonal)
        halfChamfer= self.boltedPlateTemplate.width/2.0*halfChamferVector
        angleTolerance= math.pi/180.0 # tolerance: 1 degree.
        if(abs(angleWithWeb)<angleTolerance): # diagonal parallel to web => flange gusset.
            objType= 'flange_gusset'
            gussetPlate= self.getFlangeGussetPlate(baseVectors= baseVectors, diagSegment= dgSegment, gussetLength= gussetLength, halfChamfer= halfChamfer, slope= self.flangeGussetLegsSlope)
        else: # diagonal normal to web  => web gusset
            objType= 'web_gusset'
            gussetPlate= self.getWebGussetPlate(baseVectors= baseVectors, diagSegment= dgSegment, gussetLength= gussetLength, halfChamfer= halfChamfer, bottomLegSlope= self.webGussetBottomLegSlope)
        # Attached plate.
        boltedPlate= self.getBoltedPlateTemplate()
        gussetPlateProperties= bte.BlockProperties.copyFrom(blockProperties)
        gussetPlateProperties.appendAttribute('objType', objType)
        
        gussetPlateBlocks= gussetPlate.getBlocks(boltedPlate, diagonal, self.getOrigin(), blockProperties= gussetPlateProperties)
        retval.extend(gussetPlateBlocks)
        return retval

    def getColumnShapeBlocks(self, blockProperties= None):
        ''' Return the faces of the column shaft.

        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        return super(Connection,self).getColumnShapeBlocks(self.columnLengthFactor, intermediateJoint= self.intermediateJoint, blockProperties= blockProperties)

    def getBoltGroupName(self, beam, plateProperties):
        ''' Return a name for the bolt group that attaches the beam to the 
            plate.

        :param beam: beam to wich the plate is attached.
        :param plateProperties: properties of the plate.
        '''
        retval= 'joint_'+plateProperties.getAttribute('jointId') # Joint id.
        retval+= '_beam_'+str(beam.eTag) # Beam identifier.
        retval+= '_'+plateProperties.getAttribute('objType')
        retval+= '_'+plateProperties.getAttribute('location') # flange location.
        return retval

    def getHolesOnBeamBlocks(self, beam, holesList, beamBlocks, plateProperties):
        ''' Return the blocks corresponding to the holes in the beam web or flanges.

        :param beam: beam to wich the plate is attached.
        :param holesList: holes that will be projected on the
                          beam surfaces (web or flanges).
        :param beamBlocks: surfaces corresponding to the member plates:
                          flanges, web, etc.
        :param plateProperties: properties of the plate.
        '''
        # Setup bolt properties.
        flangeBoltGroupName= self.getBoltGroupName(beam, plateProperties)
        boltProperties= bte.BlockProperties.copyFrom(plateProperties)
        boltProperties.appendAttribute('boltGroup', flangeBoltGroupName)
        ## Create holes.
        return bolts.createHolesOnMemberBlocks(holesList, beamBlocks, boltProperties, self.getMaterialModule().__name__)
    
    def getBeamShapeBlocks(self, blockProperties= None):
        ''' Return the blocks corresponding to the faces of the beams.

        :param blockProperties: labels and attributes to assign to the newly 
                                created blocks.
        '''
        retval= bte.BlockData()
        beamBlocks= super(Connection,self).getBeamShapeBlocks(self.beamLengthFactor)
        retval.extend(beamBlocks)
        flangePlateProperties= bte.BlockProperties.copyFrom(blockProperties)
        flangePlateProperties.appendAttribute('objType', 'flange_plate')
        shearTabProperties= bte.BlockProperties.copyFrom(blockProperties)
        shearTabProperties.appendAttribute('objType', 'shear_tab')
        for b in self.beams:
            # Top plate
            flangePlateProperties.appendAttribute('location', 'top_flange')
            topPlate= b.getTopFlangeBoltedPlate(connectionOrigin= self.getOrigin(), column= self.column, boltSteel= self.getBoltSteel(), plateSteel= self.getBoltedPlatesSteel())
            topPlateBlocks= topPlate.getBlocks(blockProperties= flangePlateProperties)
            retval.extend(topPlateBlocks)
            ## Holes in beam top flange
            holesList= topPlateBlocks.getHoles()
            holeBlocks= self.getHolesOnBeamBlocks(b, holesList, beamBlocks, flangePlateProperties)
            retval.extend(holeBlocks)
            ## Bolts between beam top flange and top plate.
            boltBlocks= bolted_plate.getBoltedPointBlocks(topPlateBlocks, holeBlocks, abs(topPlate.distBetweenPlates), blockProperties) # points linked by bolts.
            retval.extend(boltBlocks)
            # Bottom plate
            flangePlateProperties.appendAttribute('location', 'bottom_flange')
            bottomPlate= b.getBottomFlangeBoltedPlate(connectionOrigin= self.getOrigin(), column= self.column, boltSteel= self.getBoltSteel(), plateSteel= self.getBoltedPlatesSteel())
            bottomPlateBlocks= bottomPlate.getBlocks(blockProperties= flangePlateProperties)
            retval.extend(bottomPlateBlocks)
            ## Holes in beam bottom flange
            holesList= bottomPlateBlocks.getHoles()
            holeBlocks= self.getHolesOnBeamBlocks(b, holesList, beamBlocks, flangePlateProperties)
            retval.extend(holeBlocks)
            ## Bolts between beam bottom flange and bottom plate.
            boltBlocks= bolted_plate.getBoltedPointBlocks(bottomPlateBlocks, holeBlocks, abs(bottomPlate.distBetweenPlates), blockProperties) # points linked by bolts.
            retval.extend(boltBlocks)
            # Shear tab
            shearTabProperties.appendAttribute('location', 'web')
            shearTab= b.getShearTab(connectionOrigin= self.getOrigin(), column= self.column, boltSteel= self.getBoltSteel(), plateSteel= self.getBoltedPlatesSteel(), shearEfficiency= self.beamsShearEfficiency)
            shearTabBlocks= shearTab.getBlocks(blockProperties= shearTabProperties)
            retval.extend(shearTabBlocks)    
            ## Holes in beam web
            holesList= shearTabBlocks.getHoles()
            holeBlocks= self.getHolesOnBeamBlocks(b, holesList, beamBlocks, shearTabProperties)
            retval.extend(holeBlocks)
            ## Bolts between beam web and shear tab.
            boltBlocks= bolted_plate.getBoltedPointBlocks(shearTabBlocks, holeBlocks, abs(shearTab.distBetweenPlates), blockProperties) # points linked by bolts.
            retval.extend(boltBlocks)
        return retval

    def getStiffeners(self):
        ''' Return the stiffeners needed in the faces where there is
            no plate.

        :param blockProperties: labels and attributes to assign to the newly 
                                created blocks.
        '''
        # Detect missing plates.
        missingStiffeners= ['top_column_web+', 'bottom_column_web+', 'top_column_web-', 'bottom_column_web-']
        topPlate= None
        bottomPlate= None
        for key in self.column.connectedPlates:
            plateKeys= key.split(',')
            plate= self.column.connectedPlates[key]
            if(plateKeys[0]=='column_web+'):
                if('_top_' in plateKeys[1]):
                    missingStiffeners.remove('top_column_web+')
                    topPlate= plate
                elif('_bottom_' in plateKeys[1]):
                    missingStiffeners.remove('bottom_column_web+')
                    bottomPlate= plate
            elif(plateKeys[0]=='column_web-'):
                if('_top_' in plateKeys[1]):
                    missingStiffeners.remove('top_column_web-')
                    topPlate= plate
                elif('_bottom_' in plateKeys[1]):
                    missingStiffeners.remove('bottom_column_web-')
                    bottomPlate= plate
        # Create stiffeners where needed.
        retval= list()
        for stiffener in missingStiffeners:
            if('top_' in stiffener):
                plate= topPlate
                location= 'top_stiffener'
            elif('bottom_' in stiffener):
                plate= bottomPlate
                location= 'bottom_stiffener'
            stiffenerPlate= stiffeners.Stiffener(width= None, length= None, thickness= plate.thickness, steelType= plate.steelType)
            positiveSide= (stiffener[-1]=='+')
            if(positiveSide):
                stiffenerPlate.connectedTo= 'column_web+'
            else:
                stiffenerPlate.connectedTo= 'column_web-'
            stiffenerPlate.location= location
            platePlane= plate.refSys.getXYPlane()
            plateOrigin= plate.refSys.getOrg()
            columnWebContour= geom.Polygon3d(self.column.getWebMidPlaneContourPoints())
            columnWebMidPlane= columnWebContour.getPlane()
            columnWebLine= columnWebContour.getIntersection(platePlane)
            stiffenerHalfSpace= geom.HalfSpace3d(columnWebMidPlane, plateOrigin).getSwapped() # stiffener at the opposite side of the plate.
            columnTopFlangeContour= geom.Polygon3d(self.column.getTopFlangeMidPlaneContourPoints())
            columnTopFlangeLine= columnTopFlangeContour.getIntersection(platePlane)
            columnBottomFlangeContour= geom.Polygon3d(self.column.getBottomFlangeMidPlaneContourPoints())
            columnBottomFlangeLine= columnBottomFlangeContour.getIntersection(platePlane)
            halfTopFlange= stiffenerHalfSpace.clip(columnTopFlangeLine)
            halfBottomFlange= stiffenerHalfSpace.clip(columnBottomFlangeLine)
            # Weld definition.
            ## Compute weld size.
            weldLegSize= (plate.getFilletWeldLegSize(otherThickness= self.column.shape.getWebThickness())+2*plate.getFilletWeldLegSize(otherThickness= self.column.shape.getFlangeThickness()))/3.0
            ## Weld lines.
            weldDict= dict() # Weld line container.
            weldDict['columnBottomFlangeWeld']= halfBottomFlange
            weldDict['columnWebWeld']= columnWebLine
            weldDict['columnTopFlangeWeld']= halfTopFlange
            stiffenerPlate.setWeldLines(weldDict)
            stiffenerPlate.weldLegSize= weldLegSize
            # Update the plates connected to the column
            if(not hasattr(self.column,'connectedPlates')):
                self.column.connectedPlates= dict()
            key= stiffenerPlate.connectedTo +','+stiffenerPlate.location
            self.column.connectedPlates[key]= stiffenerPlate
            retval.append(stiffenerPlate)
        return retval
    
    def getStiffenerBlocks(self, blockProperties= None):
        ''' Return the blocks corresponding to the faces of the beams.

        :param blockProperties: labels and attributes to assign to the newly 
                                created blocks.
        '''
        retval= bte.BlockData()
        stiffeners= self.getStiffeners()
        for stiffener in stiffeners:
            retval.extend(stiffener.getBlocks(blockProperties))
        return retval
    
    def getBlocks(self, blockProperties= None):
        ''' Creates the block data for later meshing.

        :param blockProperties: labels and attributes to assign to the newly 
                                created blocks.
        '''
        originNodeTag= str(self.originNode.tag)
        properties= bte.BlockProperties.copyFrom(blockProperties)
        properties.appendAttribute('jointId', originNodeTag)
        retval= bte.BlockData()
        # Column blocks.
        columnShapeBlocks= self.getColumnShapeBlocks(blockProperties= properties)
        retval.extend(columnShapeBlocks)
        # Beam blocks.
        beamShapeBlocks= self.getBeamShapeBlocks(blockProperties= properties)
        retval.extend(beamShapeBlocks)
        # Stiffeners.
        stiffenerBlocks= self.getStiffenerBlocks(blockProperties= properties)
        retval.extend(stiffenerBlocks)            
        # Diagonal blocks.
        for e in self.diagonals:
            retval.extend(self.getGussetBlocksForDiagonal(e, blockProperties= properties))
        if(hasattr(self,'basePlate')):
            retval.extend(self.getBasePlateBlocks(columnShapeBlocks, blockProperties= properties))
        else:
            lmsg.warning('base plate not found.')
        return retval

    def getFlangeLegMinSize(self):
        ''' Return the minimum weld leg size of the gusset plate
            with the flange.
        '''
        flangeThickness= self.getColumnShape().get('tf')
        return self.boltedPlateTemplate.getFilletMinimumLeg(flangeThickness)
    
    def getFlangeLegMaxSize(self):
        ''' Return the maximum weld leg size of the gusset plate
            with the flange.
        '''
        flangeThickness= self.getColumnShape().get('tf')
        return self.boltedPlateTemplate.getFilletMaximumLeg(flangeThickness)
    
    def getFlangeWeldLegSize(self, factor= 0.75):
        ''' Return the weld leg size of the gusset plate
            with the flange.

        :param factor: factor to interpolate between minimum and maximum values.
        '''
        minFlangeLeg= self.getFlangeLegMinSize()
        maxFlangeLeg= self.getFlangeLegMaxSize()
        return minFlangeLeg+factor*(maxFlangeLeg-minFlangeLeg)

    def getWebLegMinSize(self):
        ''' Return the minimum weld leg size of the gusset plate
            with the web.
        '''
        webThickness= self.getColumnShape().get('tw')
        return self.boltedPlateTemplate.getFilletMinimumLeg(webThickness)
    
    def getWebLegMaxSize(self):
        ''' Return the maximum weld leg size of the gusset plate
            with the web.
        '''
        webThickness= self.getColumnShape().get('tw')
        return self.boltedPlateTemplate.getFilletMaximumLeg(webThickness)
    
    def getWebLegSize(self, factor= 0.6):
        ''' Return the weld leg size of the gusset plate
            with the web.
        '''
        minWebLeg= self.getWebLegMinSize()
        maxWebLeg= self.getWebLegMaxSize()
        return minWebLeg+factor*(maxWebLeg-minWebLeg)
                    
    def report(self, outputFile):
        ''' Reports connection design values.

        :param outputFile: output file.
        '''
        outputFile.write('Connection ID: '+str(self.originNode.tag)+'\n')
        # report column shape.
        columnShape= self.getColumnShape()
        outputFile.write('  column steel shape: '+columnShape.getMetricName()+' (US: '+columnShape.name+')\n')
        # report beam shapes.
        if(len(self.beams)):
            outputFile.write('  beam shapes: ')
            for b in self.beams:
                outputFile.write(str(b.eTag)+' '+b.shape.getMetricName()+' (US: '+b.shape.name+') ')
        if(len(self.diagonals)):
            outputFile.write('  diagonal shapes: ')
            for d in self.diagonals:
                outputFile.write(str(d.eTag)+' '+d.shape.getMetricName()+' (US: '+d.shape.name+') ')
        outputFile.write('\n')
        # report connection plates.
        for key in self.column.connectedPlates:
            plateKeys= key.split(',')
            plate= self.column.connectedPlates[key]
            outputFile.write('    plate: ('+plateKeys[1]+' -> '+plateKeys[0]+')')
            plate.report(outputFile)
                    
        # self.boltedPlateTemplate.report(outputFile)
        # outputFile.write('    gusset plate - column welds:\n')
        # outputFile.write('      with the flange(s): 2 x '+str(math.floor(self.getFlangeWeldLegSize()*1000))+' mm (fillet weld leg size)\n')
        # outputFile.write('      with the web: 2 x '+str(math.floor(self.getWebLegSize()*1000))+' mm (fillet weld leg size)\n')
        # outputFile.write('      with the plate 2 x '+str(math.floor(self.getHorizontalWeldLegSize()*1000))+' mm (fillet weld leg size)\n')

class DiagonalConnection(Connection):
    ''' Connection that has one or more diagonals.'''
    
    def __init__(self, connectionMetaData, columnLengthFactor, beamLengthFactor, gussetLengthFactor, beamsShearEfficiency, boltedPlateTemplate, intermediateJoint):
        ''' Constructor.

        :param connectionMetaData: connection origin node and members 
                                   connected to it.
        :param columnLengthFactor: factor that multiplies the column unary
                                   direction vector to obtain the length
                                   of the column.
        :param beamLengthFactor: factor that multiplies the beam unary
                                 direction vector to obtain the length
                                 of the beam to modelize.
        :param gussetLengthFactor: factor that multiplies the bolted plate
                                   length to obtain the lenght of the gusset
                                   plate.
        :param beamsShearEfficiency: ratio between the design shear and 
                                    the shear strength for the beams in
                                    this connection.
        :param boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                    arrangement.
        :param intermediateJoint: true if the joint doesn't correspond
                                  to an extremity of the column (i.e.
                                  a base plate or the top end of the column).
                                  Defaults to false for backward compatibility
                                  reasons.
        '''
        super(DiagonalConnection,self).__init__(connectionMetaData, columnLengthFactor, beamLengthFactor, gussetLengthFactor, beamsShearEfficiency, boltedPlateTemplate, intermediateJoint)
        
    def getHorizontalWeldLegSize(self):
        ''' Return the size of the weld that connects the 
            gusset plate with horizontal plate.
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+': not implemented yet.')
        return 0.0

class BasePlateConnection(Connection):
    ''' Base plate connection.'''
    
    def __init__(self, connectionMetaData, columnLengthFactor, beamLengthFactor, gussetLengthFactor, boltedPlateTemplate):
        ''' Constructor.

        :param connectionMetaData: connection origin node and members 
                                   connected to it.
        :param columnLengthFactor: vector that multiplies the column unary
                                   direction vector to obtain the length
                                   of the column.
        :param beamLengthFactor: factor that multiplies the beam unary
                                 direction vector to obtain the length
                                 of the beam to modelize.
        :param gussetLengthFactor: factor that multiplies the bolted plate
                                   length to obtain the lenght of the gusset
                                   plate.
        :param boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                    arrangement.
        '''
        super(BasePlateConnection,self).__init__(connectionMetaData, columnLengthFactor, beamLengthFactor, gussetLengthFactor, beamsShearEfficiency= 1.0, boltedPlateTemplate= boltedPlateTemplate)

    def setBasePlate(self, basePlate):
        ''' Set the base plate for this connection.

        :param basePlate: base plate.
        '''
        self.basePlate= basePlate
        # Update the plates connected to the column
        if(not hasattr(self.column,'connectedPlates')):
            self.column.connectedPlates= dict()
        self.column.connectedPlates['base_plate']= basePlate
        
    def getBasePlateIntersectionPoint(self, sg):
        ''' Get the intersection of the segment with the baseplate.

        :param sg: segment to intersect.
        '''
        basePlatePlane= self.basePlate.getMidPlane()
        retval= basePlatePlane.getIntersection(sg)
        if(math.isnan(retval.x) or math.isnan(retval.y) or math.isnan(retval.z)):
            lmsg.warning('No intersection with base plate.')
        return retval
    
    def centerAnchors(self):
        ''' Center anchors with respect to the column steel shape.'''
        columnShape= self.getColumnShape()
        flangeThickness= columnShape.get('tf')
        interiorDist= columnShape.get('h')-2*flangeThickness
        self.basePlate.centerAnchors(columnShape)
        
    def getBasePlateWeldLegMinSize(self):
        ''' Return the minimum leg size of the weld that connects the 
            gusset plate with the base plate.
        '''
        plateThickness= self.basePlate.t
        return self.boltedPlateTemplate.getFilletMinimumLeg(plateThickness)
    
    def getBasePlateWeldLegMaxSize(self):
        ''' Return the maximum leg size of the weld that connects the 
            gusset plate with the base plate.
        '''
        plateThickness= self.basePlate.t
        return self.boltedPlateTemplate.getFilletMaximumLeg(plateThickness)
    
    def getPlateWeldLegSize(self, factor= 0.6):
        ''' Return the leg size of the weld that connects the 
            gusset plate with the base plate.
        '''
        minPlateThickness= self.getBasePlateWeldLegMinSize()
        maxPlateThickness= self.getBasePlateWeldLegMaxSize()
        return minPlateThickness+factor*(maxPlateThickness-minPlateThickness)

    def getHorizontalWeldLegSize(self):
        ''' Return the size of the weld that connects the 
            gusset plate with the base plate.
        '''
        return self.getPlateWeldLegSize()

    def getColumnBaseplateWeldBlocks(self, flangeWeldLegSize, webWeldLegSize, blockProperties= None):
        ''' Return the lines roughly corresponding to weld beads.

        :param flangeWeldLegSize: leg size for the weld to the flange.
        :param webWeldLegSize: leg size for the weld to the web.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        basePlateProperties= bte.BlockProperties.copyFrom(blockProperties)
        basePlateProperties.appendAttribute('objType', 'baseplate')
        return self.column.getFrontalWeldBlocks(flangeWeldLegSize, webWeldLegSize, blockProperties= basePlateProperties)
    
    def getBasePlateBlocks(self, columnShapeBlocks, blockProperties= None):
        ''' Create the blocks corresponding to the baseplate.

        :param columnShapeBlocks: blocks of the column welded to the baseplate.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        properties= bte.BlockProperties.copyFrom(blockProperties)
        retval= bte.BlockData()
        retval.extend(self.basePlate.getBlocks(blockProperties= properties))
        flangeWeldLegSize= math.floor(self.basePlate.getFlangeWeldLegSize(0.3)*1e3)/1e3 # Arbitrary factor: temporary solution. LP 29/09/2020
        webWeldLegSize= math.floor(self.basePlate.getWebWeldLegSize()*1e3)/1e3 # Default factor: temporary solution. LP 29/09/2020
        columnBasePlateWeldBlocks= self.getColumnBaseplateWeldBlocks(flangeWeldLegSize, webWeldLegSize, properties)
        for face, weld in zip(columnShapeBlocks.faceBlocks, columnBasePlateWeldBlocks.weldBlocks):
            weldProperties= bte.BlockProperties.copyFrom(properties)
            weldProperties.appendAttribute('ownerId', 'f'+str(face.id)) # owner identifier.
            weld.blockProperties+=(weldProperties)
        retval.extend(columnBasePlateWeldBlocks)
        return retval
    
    def report(self, outputFile):
        ''' Reports connection design values.

        :param outputFile: Python file descriptor for output.
        '''
        super(BasePlateConnection,self).report(outputFile)
        self.basePlate.report(outputFile)
    
class ConnectionGroup(object):
    ''' Group of similar connections.

    :ivar name: name for the group of connections.
    :ivar connectionData: connection origin node and elements connected 
                           to it classified as column or diagonals.
    :ivar gussetLengthFactor: factor that multiplies the boltedPlate
                               length to obtain the lenght of the gusset
                               plate.
    :ivar boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                arrangement.
    :ivar connections: list of connections of the group
    '''
    def __init__(self, name, connectionData, columnLengthFactor, beamLengthFactor, gussetLengthFactor, beamsShearEfficiency, boltedPlateTemplate, intermediateJoints, ConnectionType):
        ''' Constructor.

        :param name: name for the group of connections.
        :param connectionData: connection origin node and members connected 
                                 to it classified as columns, beams or
                                 diagonals.
        :param columnLengthFactor: factor that multiplies the column unary
                                   direction vector to obtain the length
                                   of the column to modelize.
        :param beamLengthFactor: factor that multiplies the beam unary
                                 direction vector to obtain the length
                                 of the beam to modelize.
        :param gussetLengthFactor: factor that multiplies the boltedPlate
                                   length to obtain the lenght of the gusset
                                   plate.
        :param beamsShearEfficiency: ratio between the design shear and 
                                     the shear strength.
        :param boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                    arrangement.
        :param intermediateJoints: true if the joints doesn't correspond
                                   to an extremity of the column (i.e.
                                   a base plate or the top end of the column).
        :param ConnectionType: class representing the connection type.
        '''
        self.name= name
        self.connectionData= connectionData
        self.gussetLengthFactor= gussetLengthFactor
        self.boltedPlateTemplate= boltedPlateTemplate
        self.connections= list()
        for nTag in self.connectionData:
            cData= self.connectionData[nTag]
            connect= ConnectionType(cData, columnLengthFactor, beamLengthFactor, self.gussetLengthFactor, beamsShearEfficiency, self.boltedPlateTemplate, intermediateJoints)
            self.connections.append(connect)

    def joinBasePlates(self, basePlateGroup, tol= 1e-2):
        ''' Add base plates to each connection using the origin
            coordinates as key.

        :param basePlateGroup: base plates to add.
        '''
        basePlates= basePlateGroup.basePlates
        for key in basePlates:
            bp= basePlates[key]
            bpOrg= bp.origin
            for c in self.connections:
                cOrg= c.getOrigin()
                dist= cOrg.dist(bpOrg)
                if(dist<tol):
                    c.setBasePlate(bp)

    def setFlangeGussetLegsSlope(self, flangeGussetLegsSlope):
        ''' Set the slope for the gusset flange legs.'''
        for c in self.connections:
            c.flangeGussetLegsSlope= flangeGussetLegsSlope

    def setWebGussetBottomLegSlope(self, webGussetBottomLegSlope):
        ''' Set the slope for the gusset bottom legs.'''
        for c in self.connections:
            c.webGussetBottomLegSlope= webGussetBottomLegSlope

    def centerAnchors(self):
        ''' Center anchors with respect to the column steel shape.'''
        for c in self.connections:
            c.centerAnchors()
            
    def getBlocks(self):
        ''' Creates the block data for meshing.'''
        retval= bte.BlockData()
        retval.name= self.name+'_blocks'
        for c in self.connections:
            retval.extend(c.getBlocks())
        return retval

    def getLoadData(self,inputFileName):
        ''' Extracts the internal forces for each connection member
            from the loads contained in the argument file.

        :param inputFileName: data file containing the internal forces.
        '''
        retval= dict()
        with open(inputFileName) as inputFile:
            data= json.load(inputFile)
            for c in self.connections:
                retval.update(c.getLoadData(data))
        return retval

    def writeLoadData(self, inputFileName, outputFileName):
        ''' Extracts the internal forces for each connection member
            from the loads contained in the argument file and writes
            them to a file.

        :param inputFileName: data file containing the internal forces.
        :param outputFileName: name for the output file.
        '''
        loadData= self.getLoadData(inputFileName)
        with open(outputFileName, 'w') as outfile:
            json.dump(loadData, outfile)


    def report(self, outputFile):
        ''' Reports connection design values.'''
        numberOfPlates= len(self.connections)
        outputFile.write(str(numberOfPlates)+' x ')
        connect= self.connections[0]
        connect.report(outputFile)
        # for c in self.connections:
        #     c.report(outputFile)

    def output(self, outputPath= './'):
        ''' Write output: report + dxf file. '''
        blocks= self.getBlocks()

        outputFileNameBase= self.name
        
        xcImportExportData= nmd.XCImportExportData()
        xcImportExportData.problemName= outputFileNameBase+'_FEPrb'
        xcImportExportData.outputFileName= outputPath+outputFileNameBase+'_blocks'
        xcImportExportData.xcFileName= outputFileNameBase+'.py'
        xcImportExportData.blockData= blocks
        # Write XC file.
        xcImportExportData.writeToXCFile()
        # Write DXF file.
        dxfFileName= outputPath+outputFileNameBase+'.dxf'
        xcImportExportData.writeDxfFile(dxfFileName)
        # Write TXT file.
        txtFileName= outputPath+outputFileNameBase+'.txt'
        outputFile= open(txtFileName, 'w')
        self.report(outputFile)
        outputFile.close()
        return blocks

class DiagonalConnectionGroup(ConnectionGroup):
    ''' Connection group with one or more diagonals. '''
    
    def __init__(self, name, columnLengthFactor, beamLengthFactor, gussetLengthFactor, beamsShearEfficiency, xcSet, diagonalBoltedPlate, intermediateJoints= False, ConnectionType= DiagonalConnection):
        ''' Constructor.

        :param name: name for the group of connections.
        :param columnLengthFactor: factor that multiplies the column unary
                                   direction vector to obtain the length
                                   of the column to modelize.
        :param beamLengthFactor: factor that multiplies the beam unary
                                 direction vector to obtain the length
                                 of the beam to modelize.
        :param gussetLengthFactor: factor that multiplies the boltedPlate
                                   length to obtain the lenght of the gusset
                                   plate.
        :param beamsShearEfficiency: ratio between the design shear and 
                                     the shear strength.
        :param xcSet: set containing the joint nodes.
        :param diagonalBoltedPlate: bolted plate attaching diagonal.
        :param intermediateJoints: true if the joints doesn't correspond
                                  to an extremity of the column (i.e.
                                  a base plate or the top end of the column).
                                  Defaults to false for backward compatibility
                                  reasons.
        :param ConnectionType: type of the connection (defaults to DiagonalConnection).
        '''
        
        # Get members connected to the joint
        # from the model of the whole structure.
        materialModule= getmodule(diagonalBoltedPlate.__class__)
        jointMembers= connected_members.getConnectedMembers(xcSet, ConnectedMemberType= materialModule.ConnectedMember)

        super(DiagonalConnectionGroup, self).__init__(name= name, connectionData= jointMembers, columnLengthFactor= columnLengthFactor, beamLengthFactor= beamLengthFactor, gussetLengthFactor= gussetLengthFactor, beamsShearEfficiency= beamsShearEfficiency, boltedPlateTemplate= diagonalBoltedPlate, intermediateJoints= intermediateJoints, ConnectionType= ConnectionType)
        
class BasePlateConnectionGroup(DiagonalConnectionGroup):
    ''' Base plate connection group. '''
    
    def __init__(self, name, columnLengthFactor, beamLengthFactor, gussetLengthFactor, xcSet, diagonalBoltedPlate, basePlateGroup):
        ''' Constructor.

        :param name: name for the group of connections.
        :param columnLengthFactor: factor that multiplies the column unary
                                   direction vector to obtain the length
                                   of the column to modelize.
        :param beamLengthFactor: factor that multiplies the beam unary
                                 direction vector to obtain the length
                                 of the beam to modelize.
        :param gussetLengthFactor: factor that multiplies the boltedPlate
                                   length to obtain the lenght of the gusset
                                   plate.
        :param xcSet: set containing the base plate nodes.
        :param diagonalBoltedPlate: bolted plate attaching diagonal.
        :param basePlateGroup: group of base plates.
        '''
        super(BasePlateConnectionGroup, self).__init__(name= name, columnLengthFactor= columnLengthFactor, beamLengthFactor= beamLengthFactor, gussetLengthFactor= gussetLengthFactor, xcSet= xcSet, diagonalBoltedPlate= diagonalBoltedPlate, ConnectionType= BasePlateConnection)

        self.joinBasePlates(basePlateGroup)

        

class WorstCase(object):
    ''' Connection design worst case.

    :ivar CF: efficiency.
    :ivar eTag: element tag.
    :ivar combName: combination name.
    :ivar forces: forces corresponding to the combination.
    '''
    def __init__(self, CF= 0.0, eTag= -1, forces= None):
        ''' Constructor.

        :param CF: efficiency.
        :param eTag: element tag.
        :param forces: forces corresponding to the combination.
        '''
        self.CF= CF
        self.eTag= eTag
        self.forces= forces

    def update(self, CF, eTag, forces):
        ''' Constructor.

        :param CF: efficiency.
        :param eTag: element tag.
        :param forces: forces corresponding to the combination.
        '''
        if(self.CF<CF):
            self.CF= CF
            self.eTag= eTag
            self.forces= forces

    def __str__(self):
        return 'CF= '+str(self.CF)+' element tag: '+str(self.eTag)+ ' load combination: ' + str(self.forces.idComb)+ ' forces: ' + str(self.forces)

class BoltedPlateController(lsc.LimitStateControllerBase):
    ''' bolted plate limit state checking.

    :ivar boltedPlate: bolted plate to check.
    '''
    def __init__(self, boltedPlate):
        ''' Constructor.

        :param boltedPlate: bolted plate to check.
        '''
        super(BoltedPlateController,self).__init__(limitStateLabel= limit_state_data.normalStressesResistance.label)
        self.boltedPlate= boltedPlate
        
    # def initControlVars(self,setCalc):
    #     '''Initialize control variables over elements.

    #     :param setCalc: set of elements to which define control variables
    #     '''
    #     for e in setCalc.elements:
    #         e.setProp(self.limitStateLabel+'Sect1',cv.AISCBiaxialBendingControlVars(idSection= 'Sect1'))
    #         e.setProp(self.limitStateLabel+'Sect2',cv.AISCBiaxialBendingControlVars(idSection= 'Sect2'))
            
    def predimConnection(self, intForcCombFileName, setCalc=None):
        '''Launch predim connection.

        :param intForcCombFileName: name of the file to read the internal 
               force results
        :param setCalc: set of elements to check
        '''
        intForcItems= limit_state_data.readIntForcesFile(intForcCombFileName,setCalc)
        internalForcesValues= intForcItems[2]
        worstCase= WorstCase()
        for e in setCalc.elements:
            sh= e.getProp('crossSection')
            elIntForc= internalForcesValues[e.tag]
            if(len(elIntForc)==0):
                lmsg.warning('No internal forces for element: '+str(e.tag)+' of type: '+e.type())
            for lf in elIntForc:
                CFtmp= self.boltedPlate.getEfficiency(lf)
                worstCase.update(CFtmp, e.tag, lf)
        return worstCase
        
