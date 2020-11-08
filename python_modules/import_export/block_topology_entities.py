# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega.ort@gmail.com"

import ezdxf
import copy
import xc_base
import geom
from import_export import basic_entities as be
from import_export import mesh_entities as me
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
        if(labels):
            self.labels= labels
        else:
            self.labels= list()
        if(attributes):
            self.attributes= attributes
        else:
            self.attributes= dict()

    @staticmethod
    def copyFrom(blockProperties):
        ''' If the argument is not None creates a new BlockProperties
            object with the arguments contents, otherwise return an
            empty BlockProperties object.

        :param blockProperties: object to copy contents from.
        '''
        retval= None
        if(blockProperties):
            retval= BlockProperties(blockProperties.labels, copy.deepcopy(blockProperties.attributes))
        else:
            retval= BlockProperties()
        return retval
    
    def appendLabel(self, label):
        ''' Append the label argument to the container.'''
        if(label not in self.labels):
            self.labels.append(label)
    
    def extendLabels(self, labels):
        ''' Append the labels argument to the container.'''
        self.labels.extend(labels)
        
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

    def getStrXCCommand(self, strId):
        ''' Return the XC command that stores the properties.
 
        :param strId: identifier of the object.
        '''
        retval= ''
        if(self.labels):
            retval+= strId+'.setProp("labels",'+str(self.labels)+')'
        if(self.attributes):
            if(len(retval)>0):
                retval+= '; '
            retval+= strId+'.setProp("attributes",'+str(self.attributes)+')'
        return retval 

    
class PointRecord(me.NodeRecord):
    '''kPoint type entity

    '''
    def __init__(self,id, coords, blockProperties= None):
        '''
        Key point constructor.

        :param id: identifier for the point.
        :param coords: coordinates of the point.
        :param blockProperties: labels and attributes of the point.
        '''    
        super(PointRecord,self).__init__(id,coords)
        if(blockProperties):
            self.blockProperties= blockProperties
        else:
            self.blockProperties= BlockProperties()
            
    def getStrXCCommand(self,pointHandlerName):
        ''' Return the XC command that creates the point.''' 
        strId= str(self.id)
        strCommand= '.newPntIDPos3d(' + strId + ',geom.Pos3d(' + str(self.coords[0]) + ',' + str(self.coords[1]) +','+ str(self.coords[2])+'))'
        propCommand= self.blockProperties.getStrXCCommand('pt'+strId)
        if(len(propCommand)>0):
          strCommand+= '; '+propCommand
        return 'pt' + strId + '= ' + pointHandlerName + strCommand
    
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

class PointDict(me.NodeDict):
    ''' Point container.'''
    def append(self,id,x,y,z):
        pr= PointRecord(int(id),[x,y,z])
        self[pr.id]= pr
        return pr.id
        
    def getName(self):
        return 'points'
      
    def readFromXCSet(self,xcSet):
        pointSet= xcSet.getPoints
        for p in pointSet:
            pos= p.getPos
            self.append(p.tag, pos.x, pos.y, pos.z)
            
    def writeToXCFile(self,f,xcImportExportData):
        ''' Write the XC commands that define nodes.'''
        strDict= ''
        for key in self:
            strCommand= self[key].getStrXCCommand(xcImportExportData.pointHandlerName)
            f.write(strCommand+'\n')
            pointName= strCommand.split('= ')[0]
            pointId= pointName[2:]
            strDict+= pointId+':'+pointName+','
        # Write a dictionary to access those points from its id.
        f.write('\n')
        f.write('xcPointsDict= {'+strDict[:-1]+'}\n\n')
            

class BlockRecord(me.CellRecord):
    '''Block type entities: line, face, body,...

    :ivar blockProperties: labels and attributes of the block.
    '''    
    def __init__(self,id, typ, kPoints, blockProperties= None, thk= 0.0, matId= None):
        '''
        Block record constructor.

        :param id: identifier for the block.
        :param typ: block type.
        :param kPoints: key points that define block geometry and topology.
        :param blockProperties: labels and attributes of the block.
        :param thk: block thickness.
        :param matId: material identifier.
        '''
        super(BlockRecord,self).__init__(id,typ,kPoints,thk)
        if(blockProperties):
            self.blockProperties= blockProperties
        else:
            self.blockProperties= BlockProperties()
        self.blockProperties.appendAttribute('matId', matId)

    def getKPointIds(self):
        ''' Return the key points identifiers of the block.'''
        return self.nodeIds
    
    def getType(self):
        '''Return the type of the block.'''
        return self.cellType

    def hasHoles(self):
        ''' Return true if the block has holes in it.'''
        return hasattr(self,'holes')

    def getHoles(self):
        ''' Return the blocks defining the holes of the
            block.'''
        retval= None
        if(self.hasHoles()):
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
            p1= vertices[0]
            retval= geom.Segment3d(vertices[0], vertices[1])
        elif(self.cellType=='face'):
            retval= geom.Polygon3d(vertices)
        else:
            lmsg.error('BlockRecord::getGeomObject not implemented for blocks of type: '+ self.cellType)
        return retval
      
    def getStrKeyPointsIds(self):
        tmp= str(self.nodeIds)
        return tmp[tmp.index("[") + 1:tmp.rindex("]")]

    def getStrXCCommand(self,xcImportExportData):
        ''' Return the XC Python string defining the object.'''
        strId= str(self.id)
        handlerName= xcImportExportData.getBlockHandlerName(self.getType())
        strCommand= None
        pointIds= self.getStrKeyPointsIds()
        if(self.cellType=='line'):
            strId= 'l'+strId
            strCommand= strId + '= ' + handlerName + '.newLine(' + pointIds +')'
        elif(self.cellType=='face'):
            strId= 'f'+strId
            if(len(self.nodeIds)==4 and (not self.hasHoles())): # quad surface.
                strCommand= strId + '= ' + handlerName + '.newQuadSurfacePts(' + pointIds  +')'
            else:
                strCommand= strId + '= ' + handlerName + '.newPolygonalFacePts([' + pointIds  +'])'
        else:
            lmsg.error('BlockRecord::getStrXCCommand not implemented for blocks of type: '+ self.cellType)
        propCommand= self.blockProperties.getStrXCCommand(strId)
        if(len(propCommand)>0):
          strCommand+= '; '+propCommand
        strCommand+= self.getStrThicknessCommand(strId)
        return strCommand
    
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

class BlockDict(dict):
    '''Block container.'''    
    def append(self,cell):
        self[cell.id]= cell
        
    def readFromXCSet(self,xcSet):
        ''' Read blocks from XC set.'''
        surfaces= xcSet.getSurfaces
        for s in surfaces:
            points= s.getKPoints()
            numPoints= len(points)
            if(numPoints==4):
                tagPoints= [points[0],points[1],points[2],points[3]]
                thickness= s.getElement(1,1,1).getPhysicalProperties.getVectorMaterials[0].h
                block= BlockRecord(id= s.tag, typ= 'surface',kPoints= tagPoints,thk= thickness)
            else:
                lmsg.error('surface with ',str(numPoints), 'points.')
            self.append(block)
        lines= xcSet.getLines
        for l in lines:
            points= l.getKPoints()
            numPoints= len(points)
            if(numPoints==2):
                tagPoints= [points[0],points[1]]
                block= BlockRecord(id= l.tag, typ= 'line',kPoints= tagPoints, thk= 0.0)
            self.append(block)
        
    def writeDxf(self, name, pointDict,drawing):
        '''Write the cells in dxf file.'''
        layerName= name+'_blocks'
        drawing.layers.new(name= layerName)
        for key in self:
            self[key].writeDxf(pointDict,drawing,layerName)

    def writeToXCFile(self,f,xcImportExportData):
        '''Write the XC commands that define the cells (elements).'''
        strDict= ''
        for key in self:
            block= self[key]
            strCommand= self[key].getStrXCCommand(xcImportExportData)
            f.write(strCommand+'\n')
            blockName= strCommand.split('= ')[0]
            blockId= blockName[1:]
            strDict+= blockId+':'+blockName+','
        # Write a dictionary to access those blocks from its id.
        f.write('\n')
        f.write('xcBlocksDict= {'+strDict[:-1]+'}\n\n')
          
    def getTags(self):
        ''' Return the identifiers of the objects.'''
        retval= list()
        for key in self:
          retval.append(self[key].id)
        return retval
    
    def __str__(self):
        ''' Return a string representing the object.'''
        retval= ''
        for key in self:
            retval+= str(self[key]) + '\n'

class PointSupportRecord(be.SupportRecord):
    ''' Constraints for node displacements.'''
    def __init__(self, id, pointId, nsr):
        super(PointSupportRecord,self).__init__(id,nsr.xComp, nsr.yComp, nsr.zComp, nsr.rxComp, nsr.ryComp, nsr.rzComp)
        self.nodeId= pointId
    def __str__(self):
        ''' Return a string representing the object.'''
        return str(self.id) + ' nodeId: ' + str(self.nodeId) + ' ' + self.getStrConstraints()

class PointSupportDict(dict):
    '''Point to put constraints on.'''
    def append(self, ps):
        if (ps.nodeId in self):
            lmsg.warning('support for node: '+ps.nodeId+' redefined.')
        self[ps.nodeId]= ps
        return ps.nodeId

    def readFromXCSet(self,xcSet,points):
        '''Read supports from an XC set.'''
        nodeSupports= me.NodeSupportDict()
        preprocessor= xcSet.getPreprocessor
        nodeSupports.readFromXCDomain(preprocessor.getDomain)
        nodeSupportsTags= nodeSupports.getNodeTags()
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

class BlockData(object):
    '''Block topology entities container: points, lines, faces, solids,... '''
    def __init__(self):
        ''' Constructor.'''
        self.name= None
        self.materials= me.MaterialDict()
        self.points= PointDict()
        self.blocks= BlockDict()
        self.pointSupports= PointSupportDict()

    def appendPoint(self,id,x,y,z, pointProperties= None):
        ''' Append a point to the block data.

        :param id: point identifier (-1 == auto).
        :param x: x coordinate of the point.
        :param y: y coordinate of the point.
        :param z: z coordinate of the point.
        :param pointProperties: labels and attributes for the point.
        '''
        pr= PointRecord(id,[x,y,z], pointProperties)
        self.points[pr.id]= pr 
        return pr.id
        
    def appendBlock(self,block):
        ''' Append a block (line, surface, volume) to the 
            block data.

        :param block: block to append.
        '''
        self.blocks[block.id]= block
        return block.id

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
            lmsg.error('At least 2 points required.')
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
        self.points.readFromXCSet(xcSet)
        lmsg.log(str(len(self.points))+ ' points read.')
        self.blocks.readFromXCSet(xcSet)
        lmsg.log(str(len(self.blocks))+ ' blocks read.')
        self.pointSupports.readFromXCSet(xcSet,self.points)

    def readFromDxfFile(self,fName,preprocessor,dxfLayers):
        ''' Read block topology from a DXF file.

        :param fName: name of the DXF file to write.
        :param preprocessor: XC preprocessor
        :param dxfLayers: layers to import objects from.
        '''
        dxfReader= dxf_reader.OldDxfReader()
        dxfReader.read(fName,preprocessor,dxfLayers)
        pointSet= preprocessor.getSets.getSet("total").getPoints
        for p in pointSet:
            pos= p.getPos
            self.appendPoint(p.tag, pos.x, pos.y, pos.z)
        lineSet= preprocessor.getSets.getSet("total").getLines
        for l in lineSet:
            points= l.getKPoints()
            tagPoints= [points[0],points[1]]
            block= BlockRecord(l.tag,'line',tagPoints)
            self.appendBlock(block)
        self.pointsInLayer= dxfReader.pointsInLayer
        self.blocksInLayer= dxfReader.blocksInLayer

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

    def writeDxfFile(self,fileName):
        '''Write mesh in a DXF file.

        :param fileName: name of the DXF file to write.
        '''
        drawing= ezdxf.new()
        self.writeDxf(drawing)
        drawing.saveas(fileName)    

    def writeToXCFile(self,xcImportExportData):
        ''' Write a Python file with XC commands.'''
        f= xcImportExportData.outputFile
        if(hasattr(self,'logMessage')): # not a very elegant solution.
            f.write(self.logMessage+'\n')
        self.points.writeToXCFile(f,xcImportExportData)
        self.blocks.writeToXCFile(f,xcImportExportData)
        f.close()

    def getPointTags(self):
        ''' Return the identifiers of the points.'''
        tags= []
        for key in self.points:
            tags.append(self.points[key].id)
        return tags

    def getBlockTags(self):
        ''' Return the identifiers of the blocks.'''
        tags= []
        for key in self.blocks:
            tags.append(self.blocks[key].id)
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
