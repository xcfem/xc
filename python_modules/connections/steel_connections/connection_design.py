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

import math
from import_export import block_topology_entities as bte
import xc_base
import geom
from connections.steel_connections import gusset_plate as gp
from connections.steel_connections import connected_members


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
    :ivar boltedPlateTemplate: bolted plate object: dimensions of the bolted 
                          plate and bolt type and arrangement.
    :ivar flangeGussetLegsSlope: tangent of the angle of the flange gusset legs
                                 with its member axis.
    :ivar webGussetBottomLegSlope:
    '''
    def __init__(self, connectionMetaData, columnLengthFactor, beamLengthFactor, gussetLengthFactor, boltedPlateTemplate, boltSteel):
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
        :param boltSteel: steel for the connection bolts.
        '''
        super(Connection,self).__init__(connectionMetaData.originNode, connectionMetaData.column, connectionMetaData.beams, connectionMetaData.diagonals)
        self.boltSteel= boltSteel
        self.columnLengthFactor= columnLengthFactor
        self.beamLengthFactor= beamLengthFactor
        self.gussetLengthFactor= gussetLengthFactor
        self.boltedPlateTemplate= boltedPlateTemplate
        self.flangeGussetLegsSlope= math.tan(math.radians(30))
        self.webGussetBottomLegSlope= 'vertical'
                    
    def getBasePlateIntersectionPoint(self, sg):
        ''' Get the intersection of the segment with the baseplate.

        :param sg: segment to intersect.
        '''
        basePlatePlane= self.basePlate.getMidPlane()
        retval= basePlatePlane.getIntersection(sg)
        if(math.isnan(retval.x) or math.isnan(retval.y) or math.isnan(retval.z)):
            lmsg.warning('No intersection with base plate.')
        return retval
    
    def getFlangeGussetPlate(self, baseVectors, diagSegment, gussetLength, halfChamfer, slope):
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
        p0= self.getColumnIntersectionPoint(diagSegment) # intersection with the column
        gussetTip= p0-gussetLength*diagSegment.getVDir().normalized()
        retval= gp.GussetPlate(self.boltedPlateTemplate, gussetTip, halfChamfer, ijkVectors= baseVectors)
        # Top leg
        p1, p2= retval.getSlopedTopLeg(slope, gussetLength)
        topLegSegment= geom.Segment3d(p1,p2)
        p2= self.getColumnIntersectionPoint(topLegSegment) # intersection with the column
        # Bottom leg.
        p3, p4, p5= retval.getToColumnBottomLeg(p0, cutKnifePoint= 0.6)
        if(p5): # knife point not cutted
            retval.setContour([p1, p2, p0, p5, p4, p3])
        else:
            retval.setContour([p1, p2, p0, p4, p3])
        return retval;

    def getWebGussetPlate(self, baseVectors, diagSegment, gussetLength, halfChamfer, bottomLegSlope):
        ''' Return the GussetPlate object for the gusset attached to the
            flange.

        :param baseVectors: unary vectors of the gusset plate reference system.
        :param diagSegment: 3D segment on the diagonal axis.
        :param gussetLength: distance from the connection origin to the
                             gusset chamfer.
        :param halfChamfer: 3D vector from the gusset chamfer mid-point
                            to the end of the chamfer.
        :param bottomLegSlope: if not 'vertical' tangent of the angle of the 
                               gusset bottom leg with its  member axis.
        '''
        origin= diagSegment.getFromPoint()
        gussetTip= origin+gussetLength*diagSegment.getVDir().normalized()
        retval= gp.GussetPlate(self.boltedPlateTemplate, gussetTip, halfChamfer, ijkVectors= baseVectors)
        # Top leg.
        p1, p2= retval.getHorizontalTopLeg(origin)
        # Bottom leg.
        p3= None; p4= None
        if(bottomLegSlope=='vertical'):
            p3, p4= retval.getVerticalBottomLeg(origin)
        else:
            p3, p4= retval.getSlopedBottomLeg(bottomLegSlope, gussetLength)
        bottomLegSegment= geom.Segment3d(p3,p4)
        p4= self.getBasePlateIntersectionPoint(bottomLegSegment) # intersection with the base plate
        retval.setContour([p1, p2, origin, p4, p3])
        return retval
        
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
        
        retval= bte.BlockData()
        origin= self.getOrigin()
        baseVectors= diagonal.getDirection(origin)
        extrusionLength= self.columnLengthFactor
        p1= origin+extrusionLength*baseVectors[0]
        webPlane= self.getColumnWebMidPlane()
        angleWithWeb= webPlane.getAngleWithVector3d(baseVectors[0])
        dgSegment= geom.Segment3d(origin,p1)
        gussetLength= self.gussetLengthFactor*self.boltedPlateTemplate.length
        halfChamferVector= getHalfChamferVector(diagonal)
        halfChamfer= self.boltedPlateTemplate.width/2.0*halfChamferVector
        verticalWeldLegSize= 0.0 # leg size for the vertical welds.
        horizontalWeldLegSize= math.floor(self.getHorizontalWeldLegSize()*1e3)/1e3 # leg size for the horizontal welds.      
        if(abs(angleWithWeb)<1e-3): # diagonal parallel to web => flange gusset.
            verticalWeldLegSize= math.floor(self.getFlangeLegSize()*1e3)/1e3
            objType= 'flange_gusset'
            gussetPlate= self.getFlangeGussetPlate(baseVectors= baseVectors, diagSegment= dgSegment, gussetLength= gussetLength, halfChamfer= halfChamfer, slope= self.flangeGussetLegsSlope)
        else: # diagonal normal to web  => web gusset
            verticalWeldLegSize= math.floor(self.getWebLegSize()*1e3)/1e3
            objType= 'web_gusset'
            gussetPlate= self.getWebGussetPlate(baseVectors= baseVectors, diagSegment= dgSegment, gussetLength= gussetLength, halfChamfer= halfChamfer, bottomLegSlope= self.webGussetBottomLegSlope)
        # Attached plate.
        boltedPlate= self.getBoltedPlateTemplate()
        gussetPlateProperties= bte.BlockProperties.copyFrom(blockProperties)
        gussetPlateProperties.appendAttribute('objType', objType)
        
        gussetPlateBlocks= gussetPlate.getBlocks(verticalWeldLegSize, horizontalWeldLegSize, boltedPlate, diagonal, self.getOrigin(), blockProperties= gussetPlateProperties)
        retval.extend(gussetPlateBlocks)
        return retval
    
    def getBlocks(self, blockProperties= None):
        ''' Creates the block data for later meshing.

        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        originNodeTag= str(self.originNode.tag)
        properties= bte.BlockProperties.copyFrom(blockProperties)
        properties.appendAttribute('jointId', originNodeTag)
        retval= bte.BlockData()
        # Column blocks.
        columnShapeBlocks= self.getColumnShapeBlocks(self.columnLengthFactor, blockProperties= properties)
        retval.extend(columnShapeBlocks)
        # Beam blocks.
        beamShapeBlocks= self.getBeamShapeBlocks(self.beamLengthFactor, blockProperties= properties)
        retval.extend(beamShapeBlocks)
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
    
    def getFlangeLegSize(self, factor= 0.75):
        ''' Return the weld leg size of the gusset plate
            with the flange.
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
        columnShape= self.getColumnShape()
        outputFile.write('  column steel shape: '+columnShape.getMetricName()+' (US: '+columnShape.name+')\n')
        diagonalShapes= self.getDiagonalShapes()
        outputFile.write('  diagonal shapes: ')
        for ds in diagonalShapes:
            outputFile.write(ds.getMetricName()+' (US: '+ds.name+') ')
        outputFile.write('\n')
        self.boltedPlateTemplate.report(outputFile)
        outputFile.write('    gusset plate - column welds:\n')
        outputFile.write('      with the flange(s): 2 x '+str(math.floor(self.getFlangeLegSize()*1000))+' mm (fillet weld leg size)\n')
        outputFile.write('      with the web: 2 x '+str(math.floor(self.getWebLegSize()*1000))+' mm (fillet weld leg size)\n')
        outputFile.write('      with the plate 2 x '+str(math.floor(self.getHorizontalWeldLegSize()*1000))+' mm (fillet weld leg size)\n')
