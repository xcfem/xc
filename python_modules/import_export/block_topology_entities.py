# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega.ort@gmail.com"

import datetime
from import_export import basic_entities as be
from import_export import mesh_entities as me
from misc_utils import log_messages as lmsg

class PointRecord(me.NodeRecord):
    '''kPoint type entity'''
    def __init__(self,id, coords, labels= None):
        '''
        Key point constructor.

        :param id: identifier for the point.
        :param coords: coordinates of the point.
        :param labels: string list that helps to identify the role of the point in the model.
        '''    
        super(PointRecord,self).__init__(id,coords)
        if(labels):
            self.labels= labels
        else:
            self.labels= list()
    def getStrXCCommand(self,pointHandlerName):
        ''' Return the XC command that creates the point.''' 
        strId= str(self.id)
        strCommand= '.newPntIDPos3d(' + strId + ',geom.Pos3d(' + str(self.coords[0]) + ',' + str(self.coords[1]) +','+ str(self.coords[2])+'))'
        if(self.labels):
          strCommand+= '; pt'+strId+'.setProp("labels",'+str(self.labels)+')'
        return 'pt' + strId + '= ' + pointHandlerName + strCommand

class PointDict(me.NodeDict):
    ''' Node container.'''
    def append(self,id,x,y,z):
        self[id]= PointRecord(int(id),[x,y,z])
        
    def getName(self):
        return 'points'
      
    def readFromXCSet(self,xcSet):
        pointSet= xcSet.getPoints
        for p in pointSet:
            pos= p.getPos
            self.append(p.tag, pos.x, pos.y, pos.z)
    def writeToXCFile(self,f,xcImportExportData):
        ''' Write the XC commands that define nodes.'''
        for key in self:
            strCommand= self[key].getStrXCCommand(xcImportExportData.pointHandlerName)
            f.write(strCommand+'\n')
            if(self.labels):
                pointName= strCommand.split('= ')[0]
                strCommand= pointName+'.setProp("labels",'+str(self.labels)+')'
                f.write(strCommand+'\n')

class BlockRecord(me.CellRecord):
    '''Block type entities: line, face, body,...'''    
    def __init__(self,id, typ, kPoints, labels= None,thk= 0.0):
        '''
        Block record constructor.

        :param id: identifier for the block.
        :param typ: block type.
        :param kPoints: key points that define block geometry and topology.
        :param labels: string list that helps to identify the role of the block in the model.
        '''
        super(BlockRecord,self).__init__(id,typ,kPoints,thk)
        if(labels):
            self.labels= labels
        else:
            self.labels= list()
            
    def getType(self):
        '''Return the type of the block.'''
        return self.cellType
      
    def getStrKeyPointsIds(self):
        tmp= str(self.nodeIds)
        return tmp[tmp.index("[") + 1:tmp.rindex("]")]
    
    def getStrXCCommand(self,xcImportExportData):
        strId= str(self.id)
        handlerName= xcImportExportData.getBlockHandlerName(self.getType())
        strCommand= None
        if(self.cellType=='line'):
            strId= 'l'+strId
            strCommand= strId + '= ' + handlerName + '.newLine(' + self.getStrKeyPointsIds() +')'
        elif(self.cellType=='face'):
            strId= 'f'+strId
            strCommand= strId + '= ' + handlerName + '.newQuadSurfacePts(' + self.getStrKeyPointsIds()  +')'
        else:
            lmsg.error('BlockRecord::getStrXCCommand not implemented for blocks of type: '+ self.cellType)
        if(self.labels):
            strCommand+= '; '+strId+'.setProp("labels",'+str(self.labels)+')'
        return strCommand

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
        for key in self:
            cell= self[key]
            type= xcImportExportData.convertCellType(cell.cellType)
            if(type!=None):
                strCommand= self[key].getStrXCCommand(xcImportExportData)
                f.write(strCommand+'\n')
          
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

    def appendPoint(self,id,x,y,z,labels= None):
        pr= PointRecord(id,[x,y,z],labels)
        self.points[pr.id]= pr 
        return pr.id
        
    def appendBlock(self,block):
        self.blocks[block.id]= block
        return block.id

    def readFromXCSet(self,xcSet):
        '''Read points and surfaces from an XC set.'''
        self.name= xcSet.name
        self.points.readFromXCSet(xcSet)
        print(len(self.points), ' points read.')
        self.blocks.readFromXCSet(xcSet)
        print(len(self.blocks), ' blocks read.')
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
        '''Write the blocs (point, lines, surfaces, volumes,...) 
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
        drawing= DXFEngine.drawing(fileName)
        self.writeDxf(drawing)
        drawing.save()    

    def writeToXCFile(self,xcImportExportData):
        ''' Write a Python file with XC commands.'''
        f= xcImportExportData.outputFile
        f.write('# imported from DXF file: '+self.dxfFileName+' on ')
        f.write(str(datetime.datetime.now())+'\n')
        for key in self.points:
            strCommand= self.points[key].getStrXCCommand(xcImportExportData.pointHandlerName)
            f.write(strCommand+'\n')
        for key in self.blocks:
            block= self.blocks[key]
            strCommand= block.getStrXCCommand(xcImportExportData)
            f.write(strCommand+'\n')
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

    def __str__(self):
        ''' Return a string representing the object.'''
        for key in self.points:
            retval+= str(self.points[key]) + '\n'
        for key in self.blocks:
            retval+= str(self.blocks[key]) + '\n'
        return retval
