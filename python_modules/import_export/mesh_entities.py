# -*- coding: utf-8 -*-
from collections import defaultdict
from itertools import count
from import_export import basic_entities as be
import ezdxf
import logging
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
        
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'name':self.name, 'typo':self.typo, 'thermalExp':self.thermalExp, 'rho':self.rho, 'E':self.E, 'nu':self.nu, 'G':self.G, 'logDec':self.logDec, 'specHeat':self.specHeat, 'thermalCond':self.thermalCond} 
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.name= dct['name']
        self.typo= dct['typo']
        self.thermalExp= dct['thermalExp']
        self.rho= dct['rho']
        self.E= dct['E']
        self.nu= dct['nu']
        self.G= dct['G']
        self.logDec= dct['logDec']
        self.specHeat= dct['specHeat']
        self.thermalCond= dct['thermalCond']

class MaterialDict(dict):
    ''' Material container indexed by the material name.'''
    
    def append(self,mat):
        self[mat.name]= mat
        
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        for key in self.keys():
            mat= self[key]
            retval[mat.name]= mat.getDict()
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        for key in dct.keys():
            values= dct[key]
            newMaterial= MaterialRecord(name= values['name'], typo= values['typo'], thermalExp= values['thermalExp'], rho= values['rho'], E= values['E'], nu= values['nu'], G= values['G'], logDec= values['logDec'], specHeat= values['specHeat'], thermalCond= values['thermalCond'])
            self.append(newMaterial)


class NodeRecord(object):
    '''Node of a finite element mesh.

    ;ivar ident: identifier for the node.
    :ivar coords: coordinates of the node.
    '''
    _ids= count(0) # counter
    
    def __init__(self,ident, coords):
        '''
        Node constructor.

        :param ident: identifier for the node.
        :param coords: coordinates of the node.
        '''    
        intId= next(self._ids)
        if(ident!=-1):
           intId= int(ident)        
        self.ident= intId
        self.coords= [coords[0],coords[1],coords[2]]

    def __str__(self):
        return str(self.ident)+' '+str(self.coords)

    def getX(self):
        return self.coords[0]
    
    def getY(self):
        return self.coords[1]
    
    def getZ(self):
        return self.coords[2]
    
    def getXCCommandString(self,nodeHandlerName):
        strId= str(self.ident)
        strCommand= '.newNodeIDXYZ(' + strId + ',' + self.coords[0] + ',' + self.coords[1] +','+ self.coords[2]+')'
        return 'n' + strId + '= ' + nodeHandlerName + strCommand
    
    def dumpToXC(self, preprocessor):
        ''' Dump the node into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        # Create point.
        nodeHandler= preprocessor.getNodeHandler()
        newNode= nodeHandler.newNodeIDXYZ(self.ident, self.coords[0],self.coords[1],self.coords[2])
        # Assing properties.
        self.blockProperties.dumpToXC(preprocessor, newNode)
        return newNode

    def writeDxf(self,drawing,layerName):
        msp= drawing.modelspace()
        msp.add_point((self.coords[0], self.coords[1], self.coords[2]), dxfattribs={'layer': layerName})

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'ident':self.ident, 'coords':self.coords}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.ident= dct['ident']
        self.coords= dct['coords']

class NodeDict(dict):
    ''' Node container.'''
    
    def append(self,ident,x,y,z):
        self[ident]= NodeRecord(int(ident),[x,y,z])
        
    def getName(self):
        return 'nodes'
    
    def readFromDATFile(self,lines,begin,end):
        self.clear()
        for i in range(begin,end):
            line= lines[i]
            ident, x, y ,z = line.split()
            self.append(ident,x,y,z)
            
    def readFromUMesh(self,umesh):
        nodes= umesh.getCoordinatesAndOwner()
        self.clear()
        ident= 0
        for n in nodes:
            pos= n.getInitialPos3d
            self.append(ident,pos.x,pos.y,pos.z)
            ident+= 1
            
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

    def getXCCommandString(self,xcImportExportData):
        ''' Return a string with the XC commands that define the nodes.'''
        retval= ''
        for key in self:
            strCommand= self[key].getXCCommandString(xcImportExportData.nodeHandlerName)
            retval+= (strCommand+'\n')
        return retval
    
    def dumpToXC(self, preprocessor):
        ''' Dump the nodes of this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        retval= list()
        for key in self:
            newNode= self[key].dumpToXC(preprocessor)
            retval.append(newNode)
        
    def writeToXCFile(self,f,xcImportExportData):
        ''' Write the XC commands that define nodes.'''
        xcCommandString= self.getXCCommandString(xcImportExportData)
        f.write(xcCommandString)
            
    def getTags(self):
        retval= list()
        for key in self:
            retval.append(self[key].ident)
        return retval
    
    def __str__(self):
        retval= ''
        for key in self:
            retval+= str(self[key]) + '\n'
            
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        for key in self.keys():
            node= self[key]
            retval[key]= node.getDict()
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        for key in dct.keys():
            values= dct[key]
            ident= values['ident']
            [x, y, z]= values['coords']
            self.append(ident= ident, x= x, y= y, z= z)

class CellRecord(object):
    ''' Cell object

    :ivar ident: identifier for the cell.
    :ivar typ: cell type.
    :ivar nodes: nodes that define block geometry and topology.
    :ivar thickness: cell thickness.
    '''
    _ids= count(0) # counter
    def __init__(self, ident, typ, nodes, thk= 0.0):
        '''
        Cell record constructor.

        :param ident: identifier for the cell.
        :param typ: cell type.
        :param nodes: nodes that define block geometry and topology.
        :param thk: cell thickness.
        '''
        intId= next(self._ids)
        if(ident!=-1):
           intId= int(ident)        
        self.ident= intId
        self.cellType= typ
        self.nodeIds= nodes
        self.thickness= thk
        
    def __str__(self):
        return str(self.ident)+' '+str(self.cellType)+' '+str(self.nodeIds)
    
    def getStrXCNodes(self):
        return "xc.ID("+str(self.nodeIds)+')'
    
    def getStrThicknessCommand(self, strId):
        ''' Return a string defining the thickness
            property of the cell.

        :param strId: object identifier.
        '''
        retval= '; '+strId+'.setProp("thickness",'+str(self.thickness)+')'
        return retval
        
    def getXCCommandString(self,xcImportExportData):
        strId= str(self.ident)
        cellType= xcImportExportData.convertCellType(self.cellType)
        strType= "'"+cellType+"'"
        strCommand= xcImportExportData.cellHandlerName + ".defaultTag= "+ strId +'; '
        strCommand+= 'e' + strId + '= ' + xcImportExportData.cellHandlerName + '.newElement(' + strType + ',' + self.getStrXCNodes() +')'
        return strCommand

    def dumpToXC(self, preprocessor):
        ''' Defines the corresponding (line, surface or volume) entity in XC.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        # Create line or surface.
        newElement= None
        elementHandler= preprocessor.getElementHandler()
        cellType= xcImportExportData.convertCellType(self.cellType)
        strType= "'"+cellType+"'"
        elementHandler.defaultTag= self.ident
        newElement= elementHandler.newElement(cellType, xc.ID(self.nodeIds))
        # Assign properties.
        self.blockProperties.dumpToXC(preprocessor, newElement)
        return newElement
    
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

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'ident':self.ident, 'cellType':self.cellType, 'nodeIds': self.nodeIds, 'thickness': self.thickness}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.ident= dct['ident']
        self.cellType= dct['cellType']
        self.nodeIds= dct['nodeIds']
        self.thickness= dct['thickness']
        
class CellDict(dict):
    '''Cell container.'''
    def append(self,cell):
        self[cell.ident]= cell
    def readFromDATFile(self,lines,begin,end):
        self.clear()
        for i in range(begin,end):
          line= lines[i]
          lst= line.split()
          sz= len(lst)
          ident= int(lst[0])
          type= int(lst[1])
          nodeIds= list()
          for j in range(2,sz):
            nodeIds.append(int(lst[j]))
          self[ident]= CellRecord(ident= ident, typ= type, nodes= nodeIds)
          
    def readFromUMesh(self,umesh):
        for i in range(0,umesh.getNumberOfCells()):
          self.append(CellRecord(ident= -1, typ= umesh.getTypeOfCell(i), nodes= umesh.getNodeIdsOfCell(i)))
    def readFromXCSet(self,xcSet):
        elemSet= xcSet.elements
        for e in elemSet:
          nodes= e.getNodes.getExternalNodes
          numNodes= len(nodes)
          if(numNodes==4):
            tagNodes= [nodes[0],nodes[1],nodes[2],nodes[3]]
            thickness= e.getPhysicalProperties.getVectorMaterials[0].h
            cell= CellRecord(ident= e.tag, typ= str(e.tag), nodes= tagNodes, thk= thickness)
          elif(numNodes==2):
            tagNodes= [nodes[0],nodes[1]]
            cell= CellRecord(ident= e.tag, typ= str(e.tag), nodes= tagNodes, thk= 0.0)
          self.append(cell)
          
    def writeDxf(self,nodeDict,drawing):
        '''Write the cells in dxf file.'''
        layerName= 'cells'
        drawing.add_layer(layerName)
        for key in self:
          self[key].writeDxf(nodeDict,drawing,layerName)
          
    def getXCCommandString(self,xcImportExportData):
        ''' Return a string with the XC commands that define the cells.'''
        retval= ''
        for key in self:
            cell= self[key]
            cellType= xcImportExportData.convertCellType(cell.cellType)
            if(cellType!=None):
                strCommand= self[key].getXCCommandString(xcImportExportData)
                retval+= (strCommand+'\n')
        return retval

    def dumpToXC(self, preprocessor):
        ''' Dump the elementss of this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        retval= list()
        for key in self:
            element= self[key]
            xcElement= element.dumpToXC(preprocessor)
            retval.append(xcElement)
        return retval    
          
    def writeToXCFile(self,f,xcImportExportData):
        '''Write the XC commands that define the cells (elements).'''
        xcCommandString= self.getXCCommandString(xcImportExportData)
        f.write(xcCommandString)
                
    def getTags(self):
        retval= list()
        for key in self:
          retval.append(self[key].ident)
        return retval
    
    def __str__(self):
        retval= ''
        for key in self:
          retval+= str(self[key]) + '\n'
          
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        for key in self.keys():
            cell= self[key]
            retval[key]= cell.getDict()
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        for key in dct.keys():
            values= dct[key]
            newCell= CellRecord(ident= values['ident'], typ= values['cellType'], nodes= values['nodeIds'], thk= values['thickness'])
            self.append(newCell)

class NodeSupportRecord(be.SupportRecord):
    ''' Constraints for node displacements.

    :ivar nodeId: identifier of the supported node.
    '''
    def __init__(self, ident, nodeId, xComp= be.ComponentSupportRecord(), yComp= be.ComponentSupportRecord(), zComp= be.ComponentSupportRecord(), rxComp= be.ComponentSupportRecord('Free'), ryComp= be.ComponentSupportRecord('Free'), rzComp= be.ComponentSupportRecord('Free')):
        ''' Constructor.

        :param ident: object identifier.
        :param nodeId: identifier of the supported node.
        :param xComp: x displacement constraint definition.
        :param yComp: y displacement constraint definition.
        :param zComp: z displacement constraint definition.
        :param rxComp: x rotation constraint definition.
        :param ryComp: y rotation constraint definition.
        :param rzComp: z rotation constraint definition.
        '''
        super(NodeSupportRecord,self).__init__(ident,xComp,yComp,zComp,rxComp,ryComp,rzComp)
        self.nodeId= nodeId
        
    def __str__(self):
        return str(self.ident) + ' nodeId: ' + str(self.nodeId) + ' ' + self.getStrConstraints()
    
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= super().getDict()
        retval.update({'nodeId':self.nodeId})
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        self.nodeId= dct['nodeId']

def getConstraintsByNode(domain):
    retval= defaultdict(list)
    spIter= domain.getConstraints.getSPs
    sp= spIter.next()
    while sp:
        retval[sp.nodeTag].append(sp)
        sp= spIter.next()
    return retval

class NodeSupportDict(dict):
    ''' Container of node supports.'''
    
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

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        for key in self.keys():
            cell= self[key]
            retval[key]= cell.getDict()
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        for key in dct.keys():
            values= dct[key]
            ident= values['ident']
            nodeId= values['nodeId']
            newSupport= NodeSupportRecord(ident= ident, nodeId= nodeId)
            newSupport.setFromDict(dct= values)
            self.append(newSupport)
            
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
            
    def getXCCommandString(self,xcImportExportData):
        ''' Return a string with the XC commands that define the blocks.'''
        retval= self.nodes.getXCCommandString(xcImportExportData)
        retval+= self.cells.getXCCommandString(xcImportExportData)
        for g in self.groups:
            retval+= g.getXCCommandString(xcImportExportData)
        return retval

    def dumpToXC(self, preprocessor):
        ''' Dump the objects in this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        nodes= self.nodes.dumpToXC(preprocessor)
        elements= self.cells.dumpToXC(preprocessor)
        groups= list()
        for g in self.groups:
            groups+= g.dumpToXC(preprocessor)
        return nodes, elements, groups
                 
    def writeToXCFile(self, xcImportExportData):
        '''Write the XC commands that define the mesh.

        :param xcImportExportData: import/export parameters.
        '''
        f= xcImportExportData.outputFile
        xcCommandString= self.getXCCommandString(xcImportExportData)
        f.write(xcCommandString)

    def writeToJSON(self, xcImportExportData):
        '''Write the objects that define the mesh.

        :param xcImportExportData: import/export parameters.
        '''
        f= xcImportExportData.outputFile
        meshDict= self.getDict()
        jsonString= json.dumps(meshDict)
        f.write(jsonString)
          
    def __str__(self):
        retval= "numberOfNodes= " +' '+str(self.numberOfNodes) + '\n'
        retval+= "numberOfCells= " +' '+str(self.numberOfCells) + '\n'
        retval+= str(self.nodes)
        retval+= str(self.cells)
        return retval
    
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'name':self.name, 'numberOfCells':self.numberOfCells, 'numberOfNodes': self.numberOfNodes, 'materials': self.materials.getDict(), 'nodes': self.nodes.getDict(), 'cells': self.cells.getDict(), 'nodeSupports': self.nodeSupports.getDict(), 'groups': self.groups}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.name= dct['name']
        self.numberOfCells= dct['numberOfCells']
        self.numberOfNodes= dct['numberOfNodes']
        self.materials.setFromDict(dct['materials'])
        self.nodes.setFromDict(dct['nodes'])
        self.cells.setFromDict(dct['cells'])
        self.nodeSupports.setFromDict(['nodeSupports'])
        self.groups= dct['groups']

