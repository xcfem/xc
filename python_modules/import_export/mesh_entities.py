# -*- coding: utf-8 -*-
from collections import defaultdict
from itertools import count
from import_export import basic_entities as be
from misc_utils import log_messages as lmsg

class MaterialRecord(object):
    def __init__(self,name,typo,thermalExp,rho,E,nu,G,logDec,specHeat,thermalCond):
        self.name= name
        self.typo= typo
        self.thermalExp= thermalExp
        self.rho= rho
        self.E= E
        self.nu= nu
        self.G= G
        self.logDec= logDec
        self.specHeat= specHeat
        self.thermalCond= thermalCond

class MaterialDict(dict):
    def append(self,mat):
        self[mat.name]= mat

class NodeRecord(object):
    '''Node of a finite element mesh'''
    _ids= count(0) # counter
    def __init__(self,id, coords):
        '''
        Node constructor.

        :param id: identifier for the node.
        :param coords: coordinates of the node.
        '''    
        intId= next(self._ids)
        if(id!=-1):
           intId= int(id)        
        self.id= intId
        self.coords= [coords[0],coords[1],coords[2]]
    def __str__(self):
        return str(self.id)+' '+str(self.coords)
    def getX(self):
        return self.coords[0]
    def getY(self):
        return self.coords[1]
    def getZ(self):
        return self.coords[2]
    def getStrXCCommand(self,nodeHandlerName):
        strId= str(self.id)
        strCommand= '.newNodeIDXYZ(' + strId + ',' + self.coords[0] + ',' + self.coords[1] +','+ self.coords[2]+')'
        return 'n' + strId + '= ' + nodeHandlerName + strCommand
    def writeDxf(self,drawing,layerName):
        msp= drawing.modelspace()
        msp.add_point((self.coords[0], self.coords[1], self.coords[2]), dxfattribs={'layer': layerName})

class NodeDict(dict):
    ''' Node container.'''
    def append(self,id,x,y,z):
        self[id]= NodeRecord(int(id),[x,y,z])
    def getName(self):
        return 'nodes'
    def readFromDATFile(self,lines,begin,end):
        self.clear()
        for i in range(begin,end):
            line= lines[i]
            id, x, y ,z = line.split()
            self.append(id,x,y,z)
    def readFromUMesh(self,umesh):
        nodes= umesh.getCoordinatesAndOwner()
        self.clear()
        id= 0
        for n in nodes:
            pos= n.getInitialPos3d
            self.append(id,pos.x,pos.y,pos.z)
            id+= 1
    def readFromXCSet(self,xcSet):
        nodeSet= xcSet.nodes
        for n in nodeSet:
          pos= n.getInitialPos3d
          self.append(n.tag, pos.x, pos.y, pos.z)
    def writeDxf(self,drawing):
        '''Write the node positions in dxf file.'''
        layerName= self.getName()
        drawing.add_layer(layerName)
        for key in self:
            self[key].writeDxf(drawing,layerName)
    def writeToXCFile(self,f,xcImportExportData):
        ''' Write the XC commands that define nodes.'''
        for key in self:
            strCommand= self[key].getStrXCCommand(xcImportExportData.nodeHandlerName)
            f.write(strCommand+'\n')
    def getTags(self):
        retval= list()
        for key in self:
            retval.append(self[key].id)
        return retval
    def __str__(self):
        retval= ''
        for key in self:
            retval+= str(self[key]) + '\n'

class CellRecord(object):
    ''' Cell object

    :ivar id: identifier for the cell.
    :ivar typ: cell type.
    :ivar nodes: nodes that define block geometry and topology.
    :ivar thk: cell thickness.
    '''
    _ids= count(0) # counter
    def __init__(self,id, typ, nodes,thk= 0.0):
        '''
        Cell record constructor.

        :param id: identifier for the cell.
        :param typ: cell type.
        :param nodes: nodes that define block geometry and topology.
        :param thk: cell thickness.
        '''
        intId= next(self._ids)
        if(id!=-1):
           intId= int(id)        
        self.id= intId
        self.cellType= typ
        self.nodeIds= nodes
        self.thickness= thk
        
    def __str__(self):
        return str(self.id)+' '+str(self.cellType)+' '+str(self.nodeIds)
    
    def getStrXCNodes(self):
        return "xc.ID("+str(self.nodeIds)+')'
    
    def getStrThicknessCommand(self, strId):
        ''' Return a string defining the thickness
            property of the cell.

        :param strId: object identifier.
        '''
        retval= '; '+strId+'.setProp("thickness",'+str(self.thickness)+')'
        return retval
        
    def getStrXCCommand(self,xcImportExportData):
        strId= str(self.id)
        type= xcImportExportData.convertCellType(self.cellType)
        strType= "'"+type+"'"
        strCommand= xcImportExportData.cellHandlerName + ".defaultTag= "+ strId +'; '
        strCommand+= 'e' + strId + '= ' + xcImportExportData.cellHandlerName + '.newElement(' + strType + ',' + self.getStrXCNodes() +')'
        return strCommand
    
    def writeDxf(self,nodeDict,drawing,layerName):
        numNodes= len(self.nodeIds)
        msp= drawing.modelspace()
        if(numNodes==2):
            coordsA= nodeDict[self.nodeIds[0]].coords
            coordsB= nodeDict[self.nodeIds[1]].coords
            ptA= (coordsA[0], coordsA[1], coordsA[2])
            ptB= (coordsB[0], coordsB[1], coordsB[2])
            msp.add_line(ptA, ptB, dxfattribs={'layer': layerName})
        elif(numNodes==3):
            coordsA= nodeDict[self.nodeIds[0]].coords
            coordsB= nodeDict[self.nodeIds[1]].coords
            coordsC= nodeDict[self.nodeIds[2]].coords
            ptA= (coordsA[0], coordsA[1], coordsA[2])
            ptB= (coordsB[0], coordsB[1], coordsB[2])
            ptC= (coordsC[0], coordsC[1], coordsC[2])
            msp.add_3dface([ptA, ptB, ptC], dxfattribs={'layer': layerName})
        elif(numNodes==4):
            coordsA= nodeDict[self.nodeIds[0]].coords
            coordsB= nodeDict[self.nodeIds[1]].coords
            coordsC= nodeDict[self.nodeIds[2]].coords
            coordsD= nodeDict[self.nodeIds[3]].coords
            ptA= (coordsA[0], coordsA[1], coordsA[2])
            ptB= (coordsB[0], coordsB[1], coordsB[2])
            ptC= (coordsC[0], coordsC[1], coordsC[2])
            ptD= (coordsD[0], coordsD[1], coordsD[2])
            msp.add_3dface([ptA, ptB, ptC, ptD], dxfattribs={'layer': layerName})
        else:
            pntList= list()
            for i in range(0,numNodes):
                coords= nodeDict[self.nodeIds[i]].coords
                pntList.append((coords[0], coords[1], coords[2]))
            # Close polyline:
            coords= nodeDict[self.nodeIds[0]].coords
            pntList.append((coords[0], coords[1], coords[2]))
            msp.add_polyline3d(pntList, dxfattribs={'layer': layerName})

class CellDict(dict):
    '''Cell container.'''
    def append(self,cell):
        self[cell.id]= cell
    def readFromDATFile(self,lines,begin,end):
        self.clear()
        for i in range(begin,end):
          line= lines[i]
          lst= line.split()
          sz= len(lst)
          id= int(lst[0])
          type= int(lst[1])
          nodeIds= list()
          for j in range(2,sz):
            nodeIds.append(int(lst[j]))
          self[id]= CellRecord(id, type, nodeIds)
    def readFromUMesh(self,umesh):
        for i in range(0,umesh.getNumberOfCells()):
          self.append(CellRecord(umesh.getTypeOfCell(i), umesh.getNodeIdsOfCell(i)))
    def readFromXCSet(self,xcSet):
        elemSet= xcSet.elements
        for e in elemSet:
          nodes= e.getNodes.getExternalNodes
          numNodes= len(nodes)
          if(numNodes==4):
            tagNodes= [nodes[0],nodes[1],nodes[2],nodes[3]]
            thickness= e.getPhysicalProperties.getVectorMaterials[0].h
            cell= CellRecord(e.tag,str(e.tag),tagNodes,thickness)
          elif(numNodes==2):
            tagNodes= [nodes[0],nodes[1]]
            cell= CellRecord(e.tag,str(e.tag),tagNodes,0.0)
          self.append(cell)        
    def writeDxf(self,nodeDict,drawing):
        '''Write the cells in dxf file.'''
        layerName= 'cells'
        drawing.add_layer(layerName)
        for key in self:
          self[key].writeDxf(nodeDict,drawing,layerName)
    def writeToXCFile(self,f,xcImportExportData):
        '''Write the XC commands that define the cells (elements).'''
        for key in self:
            cell= self[key]
            type= xcImportExportData.convertCellType(cell.cellType)
            if(type!=None):
                strCommand= self[key].getStrXCCommand(xcImportExportData)
                f.write(strCommand+'\n')
    def getTags(self):
        retval= list()
        for key in self:
          retval.append(self[key].id)
        return retval
    def __str__(self):
        retval= ''
        for key in self:
          retval+= str(self[key]) + '\n'

class NodeSupportRecord(be.SupportRecord):
    ''' Constraints for node displacements.'''
    def __init__(self, id, nodeId, xComp= be.ComponentSupportRecord(), yComp= be.ComponentSupportRecord(), zComp= be.ComponentSupportRecord(), rxComp= be.ComponentSupportRecord('Free'), ryComp= be.ComponentSupportRecord('Free'), rzComp= be.ComponentSupportRecord('Free')):
        super(NodeSupportRecord,self).__init__(id,xComp,yComp,zComp,rxComp,ryComp,rzComp)
        self.nodeId= nodeId
    def __str__(self):
        return str(self.id) + ' nodeId: ' + str(self.nodeId) + ' ' + self.getStrConstraints()

def getConstraintsByNode(domain):
    retval= defaultdict(list)
    spIter= domain.getConstraints.getSPs
    sp= spIter.next()
    while sp:
        retval[sp.nodeTag].append(sp)
        sp= spIter.next()
    return retval

class NodeSupportDict(dict):
  def append(self, ns):
      if (ns.nodeId in self):
          lmsg.warning('support for node: '+ns.nodeId+' redefined.')
      self[ns.nodeId]= ns
  def readFromXCDomain(self,domain):
      '''Read SP constraints from an XC domain.'''
      spConstraintsByNode= getConstraintsByNode(domain)
      supportId= 1
      for tagNode in spConstraintsByNode:
          constraintsNode= spConstraintsByNode[tagNode]
          gdlLabels= ['Free','Free','Free','Free','Free','Free']
          for sp in constraintsNode:
              gdl= sp.getDOFNumber
              dispValue= sp.getValue
              if(dispValue == 0.0):
                  gdlLabels[gdl]= 'Rigid'
              else:
                  lmsg.error('Error; imposed displacement constraints not implemented.')
          nsr= NodeSupportRecord(supportId,tagNode)
          nsr.setupFromComponentLabels(gdlLabels)
          self.append(nsr)
          supportId+= 1
        
  def getNodeTags(self):
      '''Return dictionary keys in a list.'''
      retval= list()
      for key in self:
        nodeTag= self[key].nodeId
        retval.append(nodeTag)
      return retval

class MeshData(object):

    def __init__(self):
        self.name= None
        self.numberOfCells= None
        self.numberOfNodes= None
        self.materials= MaterialDict()
        self.nodes= NodeDict()
        self.cells= CellDict()
        self.nodeSupports= NodeSupportDict()
        self.groups= []

    def readFromDATFile(self,fName):
        # Read the source mesh
        f= open(fName,"r")

        lines= f.readlines()
        line= lines[0]
        nn,nc= line.split()
        self.numberOfNodes= int(nn)
        self.numberOfCells= int(nc)

        beginNodes= 1
        endNodes= self.numberOfNodes+1

        self.nodes.readFromDATFile(lines,beginNodes,endNodes)

        beginCells= endNodes
        endCells= beginCells+self.numberOfCells
        self.cells.readFromDATFile(lines,beginCells,endCells)

    def readFromXCSet(self,xcSet):
        '''Read nodes and elements from an XC set.'''
        self.nodes.readFromXCSet(xcSet)
        self.cells.readFromXCSet(xcSet)
        self.nodeSupports.readFromXCDomain(xcSet.getPreprocessor.getDomain)
        
    def writeDxf(self,drawing):
        '''Write mesh in a DXF file.'''
        self.nodes.writeDxf(drawing)
        self.cells.writeDxf(self.nodes,drawing)
        #groups have no representation in DXF files.
        
    def writeDxfFile(self,fileName, silent= True):
        '''Write mesh in a DXF file.

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
            logger.setLevel(level= oldLoggingLevel)
        
    def writeToXCFile(self,xcImportExportData):
        '''Write the XC commands that define the mesh.'''
        f= xcImportExportData.outputFile
        self.nodes.writeToXCFile(f,xcImportExportData)
        self.cells.writeToXCFile(f,xcImportExportData)
        for g in self.groups:
          g.writeToXCFile(xcImportExportData)
    def __str__(self):
        retval= "numberOfNodes= " +' '+str(self.numberOfNodes) + '\n'
        retval+= "numberOfCells= " +' '+str(self.numberOfCells) + '\n'
        retval+= str(self.nodes)
        retval+= str(self.cells)
        return retval
