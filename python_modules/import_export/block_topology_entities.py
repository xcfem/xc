# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega.ort@gmail.com"

import ezdxf
import sys
import logging
import geom
from import_export import basic_entities as be
from import_export import mesh_entities as me
# from import_export import dxf_reader
from misc_utils import log_messages as lmsg

class BlockProperties(object):
    ''' Labels and attributes of the block.

    :ivar labels: string list that helps to identify the role of the block in the model.
    :ivar attributes: block attributes stored in a dictionary.
    ''' 
    def __init__(self, labels= None, attributes= None):
        '''
        Constructor.

        :param labels: string list that helps to identify the role of the block in the model.
        :param attributes: block attributes stored in a dictionary.
        '''
        self.labels= list()
        if(labels):
            self.labels.extend(labels)
        self.attributes= dict()
        if(attributes):
            self.attributes.update(attributes)

    @staticmethod
    def copyFrom(blockProperties):
        ''' If the argument is not None creates a new BlockProperties
            object with the arguments contents, otherwise return an
            empty BlockProperties object.

        :param blockProperties: object to copy contents from.
        '''
        retval= None
        if(blockProperties):
            retval= BlockProperties(labels= blockProperties.labels, attributes= blockProperties.attributes)
        else:
            retval= BlockProperties()
        return retval
    
    def appendLabel(self, label):
        ''' Append the label argument to the container.'''
        if(label not in self.labels):
            self.labels.append(label)
    
    def extendLabels(self, labels):
        ''' Append the labels argument to the container.'''
        for lbl in labels:
            self.appendLabel(lbl)
        
    def appendAttribute(self, key, value):
        ''' Append the label argument to the container.'''
        self.attributes[key]= value
    
    def extendAttributes(self, attributes):
        ''' Append the labels argument to the container.'''
        self.attributes.update(attributes)

    def extend(self, blockProperties):
        ''' Append the argument labels and attributes to
            the container.'''
        self.extendLabels(blockProperties.labels)
        self.extendAttributes(blockProperties.attributes)

    def hasLabel(self, label):
        ''' Return true if the label argument is the label
            container.'''
        return (label in self.labels)

    def hasAttribute(self, key):
        ''' Return true if the key argument is the attribute
            dictionary.'''
        return (key in self.attributes)
    
    def getAttribute(self, key):
        ''' Return the attribute corresponding to the 
            key argument in the dictionary.'''
        retval= None
        if(self.hasAttribute(key)):
            retval= self.attributes[key]
        return retval
    
    def __add__(self, other):
        retval= BlockProperties(labels= self.labels, attributes= self.attributes)
        retval.extend(other)
        return retval
        
    def __iadd__(self, other):
        self.extend(other)
        return self

    def getXCCommandString(self, strId):
        ''' Return the XC command that stores the properties.
 
        :param strId: identifier of the object.
        '''
        retval= strId+'.setProp("labels",'+str(self.labels)+')'
        if(len(retval)>0):
            retval+= '; '
        retval+= strId+'.setProp("attributes",'+str(self.attributes)+')'
        return retval

    def dumpToXC(self, preprocessor, entity):
        ''' Dump the entities of this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        :param entity: entity which receives the properties.
        '''
        entity.setProp('labels', self.labels)
        entity.setProp('attributes', self.attributes)        

    def __str__(self):
        return 'labels: '+str(self.labels)+' attributes: '+str(self.attributes)

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'labels':self.labels, 'attributes':self.attributes}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        if('labels' in dct):
            self.labels= dct['labels']
        if('attributes' in dct):
            self.attributes= dct['attributes']
    
class PointRecord(me.NodeRecord):
    '''kPoint type entity

    :ivar blockProperties: labels and attributes of the point.
    '''
    def __init__(self, ident, coords, blockProperties= None):
        '''
        Key point constructor.

        :param ident: identifier for the point.
        :param coords: coordinates of the point.
        :param blockProperties: labels and attributes of the point.
        '''    
        super(PointRecord,self).__init__(ident,coords)
        if(blockProperties):
            self.blockProperties= blockProperties
        else:
            self.blockProperties= BlockProperties()
            
    def getXCCommandString(self, pointHandlerName: str):
        ''' Return the XC command that creates the point.
  
        :param pointHandlerName: name of the point handler
        '''
        strId= str(self.ident)
        strCommand= '.newPoint(' + strId + ',geom.Pos3d(' + str(self.coords[0]) + ',' + str(self.coords[1]) +','+ str(self.coords[2])+'))'
        propCommand= self.blockProperties.getXCCommandString('pt'+strId)
        if(len(propCommand)>0):
          strCommand+= '; '+propCommand
        return 'pt' + strId + '= ' + pointHandlerName + strCommand

    def dumpToXC(self, preprocessor):
        ''' Dump the entities of this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        # Create point.
        pointHandler= preprocessor.getMultiBlockTopology.getPoints
        newPoint= pointHandler.newPoint(self.ident, geom.Pos3d(self.coords[0],self.coords[1],self.coords[2]))
        # Assing properties.
        self.blockProperties.dumpToXC(preprocessor, newPoint)
        return newPoint
    
    def hasLabel(self, label):
        ''' Return true if the label argument is the label
            container.'''
        return self.blockProperties.hasLabel(label)

    def hasAttribute(self, key):
        ''' Return true if the key argument is the attribute
            dictionary.'''
        return self.blockProperties.hasAttribute(key)
    
    def getAttribute(self, key):
        ''' Return the attribute corresponding to the 
            key argument in the dictionary.'''
        return self.blockProperties.getAttribute(key)

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= super().getDict()
        blockProperties= dict()
        if(self.blockProperties):
            blockProperties= self.blockProperties.getDict()
        retval.update({'blockProperties':blockProperties})
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        blockProperties= dct['blockProperties']
        if(blockProperties):
            self.blockProperties= BlockProperties()
            self.blockProperties.setFromDict(dct)
    
class PointDict(me.NodeDict):
    ''' Point container.'''
    
    def append(self, ident , x, y, z, blockProperties= None):
        ''' Append a new point to the container.

        :param ident: identifier for the point.
        :param x: x-coordinate of the point.
        :param y: y-coordinate of the point.
        :param z: z-coordinate of the point.
        :param blockProperties: labels and attributes of the point.
        '''
        pr= PointRecord(ident= int(ident), coords= [x,y,z], blockProperties= blockProperties)
        self[pr.ident]= pr
        return pr.ident
        
    def getName(self):
        return 'points'
      
    def readFromXCSet(self, xcSet):
        ''' Read the points from the XC set and append them to this container.

        :param xcSet: xc set.
        '''
        pointSet= xcSet.getPoints
        for p in pointSet:
            pos= p.getPos
            self.append(p.tag, pos.x, pos.y, pos.z)
        return len(self)

    def getXCCommandString(self,xcImportExportData):
        ''' Return a string with the XC commands that define the nodes.'''
        strDict= ''
        retval= ''
        for key in self:
            strCommand= self[key].getXCCommandString(xcImportExportData.pointHandlerName)
            retval+= (strCommand+'\n')
            pointName= strCommand.split('= ')[0]
            pointId= pointName[2:]
            strDict+= pointId+':'+pointName+','
        # Write a dictionary to access those points from its id.
        retval+= '\nxcPointsDict= {'+strDict[:-1]+'}\n\n'
        return retval;

    def dumpToXC(self, preprocessor):
        ''' Dump the points of this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        retval= list()
        for key in self:
            newPoint= self[key].dumpToXC(preprocessor)
            retval.append(newPoint)
            
    def writeToXCFile(self,f, xcImportExportData):
        ''' Write the XC commands that define nodes.'''
        xcCommandString= self.getXCCommandString(xcImportExportData)
        f.write(xcCommandString)
                        
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        for key in self.keys():
            point= self[key]
            retval[key]= point.getDict()
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        for key in dct.keys():
            values= dct[key]
            ident= values['ident']
            coords= values['coords']
            [x, y, z]= values['coords']
            blocProperties= None
            tmp= values['blockProperties']
            if(tmp):
                blockProperties= BlockProperties()
                blockProperties.setFromDict(tmp)
            self.append(ident= ident, x= x, y= y, z= z, blockProperties= blockProperties)

class BlockRecord(me.CellRecord):
    '''Block type entities: line, face, body,...

    :ivar blockProperties: labels and attributes of the block.
    :ivar matId: material identifier.
    '''    
    def __init__(self, blkId, typ, kPoints, blockProperties= None, thk= 0.0, matId= None):
        '''
        Block record constructor.

        :param blkId: identifier for the block.
        :param typ: block type.
        :param kPoints: key points that define block geometry and topology.
        :param blockProperties: labels and attributes of the block.
        :param thk: block thickness.
        :param matId: material identifier.
        '''
        if(blockProperties):
            self.blockProperties= blockProperties
        else:
            self.blockProperties= BlockProperties()
        if('Material' in self.blockProperties.attributes):
            matId= self.blockProperties.attributes['Material']
        self.blockProperties.appendAttribute('matId', matId)
        if('Thickness' in self.blockProperties.attributes):
            thk= self.blockProperties.attributes['Thickness']
        super(BlockRecord,self).__init__(ident= blkId, typ= typ, nodes= kPoints, thk= thk)

    def getKPointIds(self):
        ''' Return the key points identifiers of the block.'''
        return self.nodeIds
    
    def getType(self):
        '''Return the type of the block.'''
        return self.cellType

    def isHole(self):
        ''' Return true if the block defines a hole'''
        objType= self.getAttribute('objType')
        retval= (objType=='hole')
        return retval

    def hasHoles(self):
        ''' Return true if the block has holes in it.'''
        retval= hasattr(self,'holes')
        if(not retval):
            retval= self.hasAttribute('holeNames')
        return retval

    def getHoles(self):
        ''' Return the blocks defining the holes of the
            block.'''
        retval= None
        if(hasattr(self,'holes')):
            retval= self.holes
        return retval

    def getVertices(self, pointDict):
        ''' Return the positions of the k-points defining
            this block.'''
        retval= list()
        pointIds= self.getKPointIds()
        for pId in pointIds:
            p= pointDict[pId]
            retval.append(geom.Pos3d(p.getX(), p.getY(), p.getZ()))
        return retval

    def getGeomObject(self, pointDict):
        ''' Return the geometric object that represents this
            line, surface or volume.'''
        retval= None
        vertices= self.getVertices(pointDict)
        if(self.cellType=='line'):
            retval= geom.Segment3d(vertices[0], vertices[1])
        elif(self.cellType=='face'):
            retval= geom.Polygon3d(vertices)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for blocks of type: '+ self.cellType)
        return retval
      
    def getStrKeyPointsIds(self):
        tmp= str(self.nodeIds)
        return tmp[tmp.index("[") + 1:tmp.rindex("]")]

    def getXCCommandString(self,xcImportExportData):
        ''' Return the XC Python string defining the object.'''
        strId= str(self.ident)
        handlerName= xcImportExportData.getBlockHandlerName(self.getType())
        strCommand= None
        pointIds= self.getStrKeyPointsIds()
        if(self.cellType=='line'):
            strId= 'l'+strId
            strCommand= strId + '= ' + handlerName + '.newLine(' + pointIds +')'
        elif(self.cellType=='face'):
            strId= 'f'+strId
            if(len(self.nodeIds)==4 and (not self.hasHoles()) and (not self.isHole())): # quad surface.
                strCommand= strId + '= ' + handlerName + '.newQuadSurfacePts(' + pointIds  +')'
            else:
                strCommand= strId + '= ' + handlerName + '.newPolygonalFacePts([' + pointIds  +'])'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for blocks of type: '+ self.cellType)
        propCommand= self.blockProperties.getXCCommandString(strId)
        if(len(propCommand)>0):
            strCommand+= '; '+propCommand
        strCommand+= self.getStrThicknessCommand(strId)
        return strCommand

    def dumpToXC(self, preprocessor):
        ''' Defines the corresponding (line, surface or volume) entity in XC.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        # Create line or surface.
        newBlock= None
        pointIds= self.nodeIds
        if(self.cellType=='line'): # Create line.
            lines= preprocessor.getMultiBlockTopology.getLines
            newBlock= lines.newLine(pointIds[0], pointIds[1])
        elif(self.cellType=='face'): # Create surface.
            surfaces= preprocessor.getMultiBlockTopology.getSurfaces
            if(len(pointIds)==4 and (not self.hasHoles()) and (not self.isHole())): # quad surface.
                newBlock= surfaces.newQuadSurfacePts(pointIds[0], pointIds[1], pointIds[2], pointIds[3])
            else: # polygonal surface.
                newBlock= surfaces.newPolygonalFacePts(pointIds)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for blocks of type: '+ self.cellType)
        # Assign properties.
        self.blockProperties.dumpToXC(preprocessor, newBlock)
        newBlock.setProp("thickness",self.thickness)
        return newBlock        
    
    def hasLabel(self, label):
        ''' Return true if the label argument is the label
            container.'''
        return self.blockProperties.hasLabel(label)

    def hasAttribute(self, key):
        ''' Return true if the key argument is the attribute
            dictionary.'''
        return self.blockProperties.hasAttribute(key)
    
    def getAttribute(self, key):
        ''' Return the attribute corresponding to the 
            key argument in the dictionary.'''
        return self.blockProperties.getAttribute(key)
    
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= super().getDict()
        blockProperties= dict()
        if(self.blockProperties):
            blockProperties= self.blockProperties.getDict()
        retval.update({'blockProperties':blockProperties})
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        blockProperties= dct['blockProperties']
        if(blockProperties):
            self.blockProperties= BlockProperties()
            self.blockProperties.setFromDict(blockProperties)        

class BlockDict(dict):
    '''Block container.'''    
    def append(self,cell):
        self[cell.ident]= cell
        
    def readFromXCSet(self, xcSet):
        ''' Read blocks from XC set.'''
        surfaces= xcSet.getSurfaces
        for s in surfaces:
            points= s.getKPoints()
            numPoints= len(points)
            if(numPoints==4):
                tagPoints= [points[0],points[1],points[2],points[3]]
                thickness= 0.0
                if(len(s.elements)>0):
                    thickness= s.getElement(1,1,1).getPhysicalProperties.getVectorMaterials[0].h
                block= BlockRecord(blkId= s.tag, typ= 'surface',kPoints= tagPoints,thk= thickness)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; surface with '+str(numPoints)+' points.')
            self.append(block)
        lines= xcSet.getLines
        for l in lines:
            points= l.getKPoints()
            numPoints= len(points)
            if(numPoints==2):
                tagPoints= [points[0],points[1]]
                block= BlockRecord(blkId= l.tag, typ= 'line',kPoints= tagPoints, thk= 0.0)
            self.append(block)
        return len(self)
        
    def writeDxf(self, name, pointDict,drawing):
        '''Write the cells in dxf file.'''
        layerName= name+'_blocks'
        drawing.layers.new(name= layerName)
        for key in self:
            self[key].writeDxf(pointDict,drawing,layerName)
            
    def getXCCommandString(self,xcImportExportData):
        ''' Return a string with the XC commands that define the cells.'''
        retval= ''
        strDict= ''
        holeDict= dict() # Blocks which are holes.
        blocksWithHoles= dict() # blocks which have holes.
        for key in self:
            block= self[key]
            strCommand= block.getXCCommandString(xcImportExportData)
            retval+= (strCommand+'\n')
            blockName= strCommand.split('= ')[0]
            blockId= blockName[1:]
            strDict+= blockId+':'+blockName+','
            # Check for holes.
            holeNames= block.getAttribute('holeNames')
            if(holeNames!=None): # Block has holes.
                blocksWithHoles[block.getAttribute('name')]= blockId
            if(block.isHole()): # Block is a hole.
                holeDict[blockId]= block.getAttribute('ownerName')            
            
        # Write a dictionary to access those blocks from its id.
        retval+= ('\nxcBlocksDict= {'+strDict[:-1]+'}\n\n')

        # Loop on holes.
        for holeId in holeDict:
            ownerName= holeDict[holeId] # name of the block with the hole.
            ownerId= blocksWithHoles[ownerName] # id of the block with the hole.
            retval+= ('f'+str(ownerId)+'.addHole(f'+holeId+')\n')
        return retval

    def dumpToXC(self, preprocessor):
        ''' Dump the entities of this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        retval= list()
        holeDict= dict() # Blocks which are holes.
        blocksWithHoles= dict() # blocks which have holes.
        for key in self:
            block= self[key]
            xcBlock= block.dumpToXC(preprocessor)
            retval.append(xcBlock)
            # Check for holes.
            holeNames= block.getAttribute('holeNames')
            if(holeNames!=None): # Block has holes.
                blocksWithHoles[block.getAttribute('name')]= block
            if(block.isHole()): # Block is a hole.
                holeDict[block.ident]= (xcBlock, block.getAttribute('ownerName'))           
        # Loop on holes.
        for holeId in holeDict:
            (hole, ownerName)= holeDict[holeId] # hole block and name of the block with the hole.
            owner= blocksWithHoles[ownerName] # id of the block with the hole.
            owner.addHole(hole.name)
        return retval

    def writeToXCFile(self,f,xcImportExportData):
        '''Write the XC commands that define the cells (elements).'''
        xcCommandString= self.getXCCommandString(xcImportExportData)
        f.write(xcCommandString)
          
    def getTags(self):
        ''' Return the identifiers of the objects.'''
        retval= list()
        for key in self:
          retval.append(self[key].ident)
        return retval
    
    def __str__(self):
        ''' Return a string representing the object.'''
        retval= ''
        for key in self:
            retval+= str(self[key]) + '\n'
          
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        for key in self.keys():
            block= self[key]
            retval[key]= block.getDict()
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        for key in dct.keys():
            values= dct[key]            
            newBlock= BlockRecord(blkId= values['ident'], typ= values['cellType'], kPoints= values['nodeIds'])
            newBlock.setFromDict(values)
            self.append(newBlock)

class PointSupportRecord(me.NodeSupportRecord):
    ''' Constraints for node displacements.'''
    
    def __init__(self, ident, pointId, nsr):
        ''' Constructor.

        :param ident: object identifier.
        :param pointId: constrained point identifier.
        :param nsr: NodeSupportRecord object containing the values of the constraints.
        '''
        # Constraint values.
        if(nsr):
            xComp= nsr.xComp
            yComp= nsr.yComp
            zComp= nsr.zComp
            rxComp= nsr.rxComp
            ryComp= nsr.ryComp
            rzComp= nsr.rzComp
        else:
            xComp= be.ComponentSupportRecord()
            yComp= be.ComponentSupportRecord()
            zComp= be.ComponentSupportRecord()
            rxComp= be.ComponentSupportRecord('Free')
            ryComp= be.ComponentSupportRecord('Free')
            rzComp= be.ComponentSupportRecord('Free')
        super(PointSupportRecord,self).__init__(ident= ident, nodeId= pointId, xComp= xComp, yComp= yComp, zComp= zComp, rxComp= rxComp, ryComp= ryComp, rzComp= rzComp)

class PointSupportDict(dict):
    '''Point support container.'''
    
    def append(self, ps):
        if(ps.nodeId in self):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; support for node: '+ps.nodeId+' redefined.')
        self[ps.nodeId]= ps
        return ps.nodeId

    def readFromXCSet(self,xcSet,points):
        '''Read supports from an XC set.'''
        nodeSupports= me.NodeSupportDict()
        preprocessor= xcSet.getPreprocessor
        nodeSupports.readFromXCDomain(preprocessor.getDomain)
        nodeSupportsTags= nodeSupports.getNodeTags()
        if(len(nodeSupportsTags)>0):
            supportId= 0
            for k in points:
              p= preprocessor.getMultiBlockTopology.getPoints.get(k)
              pointTag= p.tag
              node= p.getNode()
              if(node):
                nodeTag= node.tag
                if(nodeTag in nodeSupportsTags):
                  psr= PointSupportRecord(supportId,pointTag,nodeSupports[nodeTag])
                  self.append(psr)
                  supportId+= 1
        return len(self)
          
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        for key in self.keys():
            support= self[key]
            retval[key]= support.getDict()
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        for key in dct.keys():
            values= dct[key]
            newPointSupport= PointSupportRecord(ident= values['ident'], pointId= values['pointId'])
            newPointSupport.setFromDict(values)
            self.append(newPointSupport)

class BlockData(object):
    '''Block topology entities container: points, lines, faces, solids,...

    :ivar name: container name.
    :ivar materials: materials dictionary.
    :ivar points: point container.
    :ivar blocks: block (line, surface, volumen) container.
    :ivar pointSupports: constrained points.
    :ivar verbosity: verbosity level.
    '''
    def __init__(self, verbosity= 1):
        ''' Constructor.'''
        self.name= None
        self.materials= me.MaterialDict()
        self.points= PointDict()
        self.blocks= BlockDict()
        self.pointSupports= PointSupportDict()
        self.verbosity= verbosity

    def appendPoint(self,id,x,y,z, pointProperties= None):
        ''' Append a point to the block data.

        :param id: point identifier (-1 == auto).
        :param x: x coordinate of the point.
        :param y: y coordinate of the point.
        :param z: z coordinate of the point.
        :param pointProperties: labels and attributes for the point.
        '''
        pr= PointRecord(id,[x,y,z], pointProperties)
        self.points[pr.ident]= pr 
        return pr.ident
        
    def appendBlock(self, block):
        ''' Append a block (line, surface, volume) to the 
            block data.

        :param block: block to append.
        '''
        if block.ident in self.blocks:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; redefinition of block: '+str(block.ident))
        self.blocks[block.ident]= block
        return block.ident

    def blockFromPoints(self, points, blockProperties, thickness= 0.0, matId= None):
        ''' Create a block with the points and append it
            to the container.

        :param points: points to build the new block.
        :param blockProperties: labels and attributes for the new block.
        :param thickness: thickness of the new block.
        :param matId: material of the new block.
        '''
        pointIds= list()
        for p in points:
            pointIds.append(self.appendPoint(-1, p.x, p.y, p.z, pointProperties= blockProperties))
        if(len(pointIds)>2):
            block= BlockRecord(-1, 'face', pointIds, blockProperties, thk= thickness, matId= matId)
        elif(len(pointIds)>1):
            block= BlockRecord(-1, 'line', pointIds, blockProperties, thk= thickness, matId= matId)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; at least 2 points required.')
            block= None
        if(block):
            self.appendBlock(block)
        return block
    
    def extend(self, other):
        ''' Append a the data from the argument to this container.

        :param other: container to append here.
        '''
        self.materials.update(other.materials)
        self.points.update(other.points)
        self.blocks.update(other.blocks)
        self.pointSupports.update(other.pointSupports)

    def readFromXCSet(self,xcSet):
        '''Read points and surfaces from an XC set.'''
        self.name= xcSet.name
        retval= self.points.readFromXCSet(xcSet)
        if(self.verbosity>0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+'; '+str(len(self.points))+' points read.')
        retval+= self.blocks.readFromXCSet(xcSet)
        if(self.verbosity>0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+'; '+str(len(self.blocks))+ ' blocks read.')
        retval+= self.pointSupports.readFromXCSet(xcSet,self.points)
        return retval

    # def readFromDxfFile(self,fName,preprocessor,dxfLayers):
    #     ''' Read block topology from a DXF file.

    #     :param fName: name of the DXF file to write.
    #     :param preprocessor: XC preprocessor
    #     :param dxfLayers: layers to import objects from.
    #     '''
    #     dxfReader= dxf_reader.OldDxfReader()
    #     dxfReader.read(fName,preprocessor,dxfLayers)
    #     pointSet= preprocessor.getSets.getSet("total").getPoints
    #     for p in pointSet:
    #         pos= p.getPos
    #         self.appendPoint(p.tag, pos.x, pos.y, pos.z)
    #     lineSet= preprocessor.getSets.getSet("total").getLines
    #     for l in lineSet:
    #         points= l.getKPoints()
    #         tagPoints= [points[0],points[1]]
    #         block= BlockRecord(l.tag,'line',tagPoints)
    #         self.appendBlock(block)
    #     self.pointsInLayer= dxfReader.pointsInLayer
    #     self.blocksInLayer= dxfReader.blocksInLayer

    def writeDxf(self,drawing):
        '''Write the blocks (point, lines, surfaces, volumes,...) 
           in a DXF file.

        :param drawing: ezdxf drawing object.
        '''
        layerName= 'points'
        drawing.layers.new(name= layerName)
        for key in self.points:
            self.points[key].writeDxf(drawing,layerName)
        self.blocks.writeDxf(self.name, self.points,drawing)

    def writeDxfFile(self, fileName, silent= True):
        '''Write block topology in a DXF file.

        :param fileName: name of the DXF file to write.
        :param silent: if true instruct ezdxf not to write
                       regular info messages (only warning
                       or error messages).
        '''
        oldLoggingLevel= None # Previous logging level.
        if(silent): # Avoid logging info messages.
            logger= logging.getLogger('ezdxf')
            oldLoggingLevel= logger.getEffectiveLevel()
            logger.setLevel(level=logging.WARNING) 
        drawing= ezdxf.new()
        self.writeDxf(drawing)
        drawing.saveas(fileName)
        if(silent): # Restore logging level.
            logger.setLevel(level=oldLoggingLevel)
    
    def getXCCommandString(self,xcImportExportData):
        ''' Return a string with the XC commands that define the blocks.'''
        retval= ''
        if(hasattr(self,'logMessage')): # not a very elegant solution.
            retval+= (self.logMessage+'\n')
        retval+= self.points.getXCCommandString(xcImportExportData)
        retval+= self.blocks.getXCCommandString(xcImportExportData)
        return retval

    def dumpToXC(self, preprocessor):
        ''' Dump the entities of this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        points= self.points.dumpToXC(preprocessor)
        blocks= self.blocks.dumpToXC(preprocessor)
        return blocks

    def writeToXCFile(self,xcImportExportData):
        ''' Write a Python file with XC commands.'''
        f= xcImportExportData.outputFile
        xcCommandString= self.getXCCommandString(xcImportExportData)
        f.write(xcCommandString)
        f.close()

    def writeToJSON(self, xcImportExportData):
        '''Write the objects that define the mesh.

        :param xcImportExportData: import/export parameters.
        '''
        f= xcImportExportData.outputFile
        blockDict= self.getDict()
        jsonString= json.dumps(blockDict)
        f.write(jsonString)

    def getPointTags(self):
        ''' Return the identifiers of the points.'''
        tags= []
        for key in self.points:
            tags.append(self.points[key].ident)
        return tags

    def getBlockTags(self):
        ''' Return the identifiers of the blocks.'''
        tags= []
        for key in self.blocks:
            tags.append(self.blocks[key].ident)
        return tags

    def hasHoles(self):
        ''' Return true if one or more of the surfaces has
            holes in it.'''
        retval= False
        for key in self.blocks:
            block= self.blocks[key]
            if(block.hasHoles()):
                retval= True
                break
        return retval

    def getHoles(self):
        ''' Return the holes defined in this container.'''
        retval= list()
        for key in self.blocks:
            block= self.blocks[key]
            if(block.hasHoles()):
                retval.append(block.holes)
        return retval

    def getNearest(self, pos3d):
        ''' Return the block nearest to the point argument.

        :param pos3d: position of the point.
        '''
        retval= None
        dist= 6.023e23
        for key in self.blocks:
            block= self.blocks[key]
            geomObj= block.getGeomObject(self.points)
            d= geomObj.dist(pos3d)
            if(d<dist):
                dist= d
                retval= block
        return retval
            
    def __str__(self):
        ''' Return a string representing the object.'''
        retval= ''
        for key in self.points:
            retval+= str(self.points[key]) + '\n'
        for key in self.blocks:
            retval+= str(self.blocks[key]) + '\n'
        return retval
    
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'name':self.name, 'materials': self.materials.getDict(), 'points': self.points.getDict(), 'blocks': self.blocks.getDict(), 'pointSupports': self.pointSupports.getDict(), 'verbosity': self.verbosity}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.name= dct['name']
        self.materials.setFromDict(dct['materials'])
        self.points.setFromDict(dct['points'])
        self.blocks.setFromDict(dct['blocks'])
        self.pointSupports.setFromDict(dct['pointSupports'])
        self.verbosity= dct['verbosity']
