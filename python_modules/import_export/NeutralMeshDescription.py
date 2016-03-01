#!/usr/bin/env python
import os
from collections import defaultdict
from dxfwrite import DXFEngine
import DxfReader

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
  def __init__(self,id, coords):
    self.id= id
    self.coords= [coords[0],coords[1],coords[2]]
  def __str__(self):
    return str(self.id)+' '+str(self.coords)
  def getX(self):
     return self.coords[0]
  def getY(self):
     return self.coords[1]
  def getZ(self):
     return self.coords[2]
  def getStrXCCommand(self,nodeLoaderName):
    strId= str(self.id)
    strCommand= '.newNodeIDXYZ(' + strId + ',' + self.coords[0] + ',' + self.coords[1] +','+ self.coords[2]+')'
    return 'n' + strId + '= ' + nodeLoaderName + strCommand
  def writeDxf(self,drawing,layerName):
    drawing.add(dxfEngine.point((self.coords[0], self.coords[1], self.coords[2]), color=0, layer=layerName))


class CellRecord(object):
  def __init__(self,id, type, nodes,thk= 0.0):
    self.id= id
    self.cellType= type
    self.nodeIds= nodes
    self.thickness= thk
  def __str__(self):
    return str(self.id)+' '+str(self.cellType)+' '+str(self.nodeIds)
  def getStrXCNodes(self):
    return "xc.ID("+str(self.nodeIds)+')' 
  def getStrXCCommand(self,xcImportExportData):
    strId= str(self.id)
    type= xcImportExportData.convertCellType(self.cellType)
    strType= "'"+type+"'"
    strCommand= xcImportExportData.cellLoaderName + ".defaultTag= "+ strId +'; '
    strCommand+= 'e' + strId + '= ' + xcImportExportData.cellLoaderName + '.newElement(' + strType + ',' + self.getStrXCNodes() +')'
    return strCommand
  def writeDxf(self,nodeDict,drawing,layerName):
    numNodes= len(self.nodeIds)
    for i in range(0,numNodes-1):
      coordsA= nodeDict[self.nodeIds[i]].coords
      coordsB= nodeDict[self.nodeIds[i+1]].coords
      drawing.add(dxfEngine.line((coordsA[0], coordsA[1], coordsA[2]),(coordsB[0], coordsB[1], coordsB[2]), color=0, layer=layerName))

class ComponentSupportRecord:
  '''Constraints for x,y,z,rx,ry,rz displacements of a node.'''
  def __init__(self,typ= 'Rigid', k= 0.0):
    self.typ= typ #Free","Rigid", "Flexible"
    self.k= k
  def getTypeCode(self):
    if(self.typ=='Free'):
      return '0'
    elif(self.typ=='Rigid'):
      return '1'
    elif(self.typ=='Flexible'):
      return '2'
    else:
      print 'ComponentSupportRecord::getTypeCode; error: unknown type: ', self.typ 
      return '0'
  def __str__(self):
    retval= self.typ
    if(self.typ == 'Flexible'):
      retval+= '('+str(self.k)+')'
    return retval


class NodeSupportRecord(object):
  ''' Constraints for node displacements'''
  def __init__(self, id, nodeId, xComp= ComponentSupportRecord(), yComp= ComponentSupportRecord(), zComp= ComponentSupportRecord(), rxComp= ComponentSupportRecord('Free'), ryComp= ComponentSupportRecord('Free'), rzComp= ComponentSupportRecord('Free')):
    self.id= id
    self.nodeId= nodeId
    self.typ= 'Standard'
    self.xComp= xComp
    self.yComp= yComp
    self.zComp= zComp
    self.rxComp= rxComp
    self.ryComp= ryComp
    self.rzComp= rzComp
  def setupFromComponentLabels(self,componentLabels):
    self.xComp= ComponentSupportRecord(componentLabels[0])
    self.yComp= ComponentSupportRecord(componentLabels[1])
    self.zComp= ComponentSupportRecord(componentLabels[2])
    self.rxComp= ComponentSupportRecord(componentLabels[3])
    self.ryComp= ComponentSupportRecord(componentLabels[4])
    self.rzComp= ComponentSupportRecord(componentLabels[5])
  def __str__(self):
    return str(self.id) + ' nodeId: ' + str(self.nodeId) + ' type: ' + self.typ + ' x: ' + str(self.xComp)+ ' y: ' + str(self.yComp) + ' z: ' + str(self.zComp)+ ' rx: ' + str(self.rxComp)+ ' ry: ' + str(self.ryComp) + ' rz: ' + str(self.rzComp)
 


class GroupRecord(object):

  def readFromDATFile(self,fName):
    self.name= None
    meshDesc= MeshData()
    meshDesc.readFromDATFile(fName)
    self.nodeIds= meshDesc.nodes.getTags()
    self.cellIds= meshDesc.cells.getTags()

  def setUp(self,name,points,lines):
    self.name= name
    self.nodeIds= []
    self.cellIds= []
    self.pointIds= points
    self.lineIds= lines

  def empty(self):
    return not(self.nodeIds or self.cellIds or self.pointIds or self.lineIds)

  def writeDxfFile(self,xcImportExportData):
    '''groups have not representation in dxf files.'''
    return

  def writeToXCFile(self,xcImportExportData):
    ''' writes the XC commands to define the group in a file.'''
    if(not self.empty()):
      f= xcImportExportData.outputFile
      strCommand= self.name + '= ' + xcImportExportData.setLoaderName + '.defSet("' + self.name +'")'
      f.write(strCommand+'\n')
      for n in self.nodeIds:
        strCommand= self.name + '.getNodes.append(n' + str(n) + ')'
        f.write(strCommand+'\n')
      for e in self.cellIds:
        strCommand= self.name + '.getElements.append(e' + str(e) + ')'
        f.write(strCommand+'\n')
      for p in self.pointIds:
        strCommand= self.name + '.getCad.getPoints.append(' + str(p) + ')'
        f.write(strCommand+'\n')
      for l in self.lineIds:
        strCommand= self.name + '.getCad.getLines.append(' + str(l) + ')'
        f.write(strCommand+'\n')


class XCImportExportData(object):

  def __init__(self):
    self.mainDATFile= ""
    self.groupDATFiles= [] 
    self.dxfLayers= []
    self.outputFileName= 'xc_mesh' 
    self.problemName= None
    self.nodeLoaderName= "nodes"
    self.cellLoaderName= "elements"
    self.setLoaderName= "groups"
    self.pointLoaderName= "points"
    self.cellConversion= {}
    self.outputFile= None
    self.meshDesc= None
    self.blockData= None

  def getXCFileName(self):
    return self.outputFileName+'.py'

  def getDxfFileName(self):
    return self.outputFileName+'.dxf'

  def getBlockLoaderName(self,blockType):
    if(blockType=='line'):
      return 'lines'
    elif(blockType=='face'):
      return 'surfaces'
    elif(blockType=='solid'):
      return 'solids'

  def convertCellType(self,tp):
    if tp in self.cellConversion:
      return self.cellConversion[tp]
    else:
      return None
  def readDATFiles(self):
    self.meshDesc= MeshData()
    self.meshDesc.readFromDATFile(self.mainDATFile)
    for fName in self.groupDATFiles:
      grp= GroupRecord()
      grp.name= os.path.splitext(fName)[0]
      grp.readFromDATFile(fName)
      self.meshDesc.groups.append(grp)
  def readDxfFile(self,fName,preprocessor):
    self.blockData= BlockData()
    self.blockData.name= fName
    self.blockData.readFromDxfFile(fName,preprocessor,self.dxfLayers)
    for l in self.dxfLayers:
      grp= GroupRecord()
      grp.setUp(l,self.blockData.pointsInLayer[l],self.blockData.blocksInLayer[l])
      self.blockData.groups.append(grp)
      
  def writeDxfFile(self,fileName):
    drawing= dxfEngine.drawing(fileName)
    if(self.blockData):
      self.blockData.writeDxf(self)
    if(self.meshDesc):
      self.meshDesc.writeDxf(self)
    drawing.save()
      
  def writeToXCFile(self):
    self.outputFile= open(self.getXCFileName(),"w")
    strCommand= 'preprocessor= ' + self.problemName + '.getPreprocessor'
    self.outputFile.write(strCommand+'\n')
    strCommand= self.nodeLoaderName + '= preprocessor.getNodeLoader'
    self.outputFile.write(strCommand+'\n')
    strCommand= self.cellLoaderName + '= preprocessor.getElementLoader'
    self.outputFile.write(strCommand+'\n')
    strCommand= self.pointLoaderName + '= preprocessor.getCad.getPoints'
    self.outputFile.write(strCommand+'\n')
    #strCommand= self.lineLoaderName + '= preprocessor.getCad.getLines'
    #self.outputFile.write(strCommand+'\n')
    strCommand= self.setLoaderName + '= preprocessor.getSets'
    self.outputFile.write(strCommand+'\n')
    if(self.blockData):
      self.blockData.writeToXCFile(self)
    if(self.meshDesc):
      self.meshDesc.writeToXCFile(self)

class NodeDict(dict):
  def append(self,id,x,y,z):
    self[id]= NodeRecord(int(id),[x,y,z])
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
      self[id]= NodeRecord(id,n)
      id+= 1
  def readFromXCSet(self,xcSet):
    nodeSet= xcSet.getNodes
    for n in nodeSet:
      pos= n.getInitialPos3d
      self.append(n.tag, pos.x, pos.y, pos.z)

  def writeDxf(self,drawing):
    '''Write the node positions in dxf file.'''
    layerName= 'nodes'
    drawing.add_layer(layerName)
    for key in self:
      self[key].writeDxf(drawing,layerName)

  def writeToXCFile(self,f,xcImportExportData):
    ''' Write the XC commands that define nodes.'''
    for key in self:
      strCommand= self[key].getStrXCCommand(xcImportExportData.nodeLoaderName)
      f.write(strCommand+'\n')

  def getTags(self):
    retval= list()
    for key in self:
      retval.append(self[key].id)
    return retval;
  def __str__(self):
    retval= ''
    for key in self:
      retval+= str(self[key]) + '\n'

class CellDict(dict):
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
    elemSet= xcSet.getElements
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
    return retval;
  def __str__(self):
    retval= ''
    for key in self:
      retval+= str(self[key]) + '\n'

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
    self[ns.id]= ns
  def readFromXCDomain(self,domain):
    '''Read SP constraints from an XC domain.'''
    spConstraintsByNode= getConstraintsByNode(domain)
    supportId= 1
    for tagNode in spConstraintsByNode:
      constraintsNode= spConstraintsByNode[tagNode]
      labels= ['Free','Free','Free','Free','Free','Free']
      for sp in constraintsNode:
        gdl= sp.getDOFNumber
        dispValue= sp.getValue
        if(dispValue == 0.0):
          labels[gdl]= 'Rigid'
        else:
          print "Error; imposed displacement constraints not implemented."
      nsr= NodeSupportRecord(supportId,tagNode)
      nsr.setupFromComponentLabels(labels)
      #print "nsr= ", nsr
      self.append(nsr)
      supportId+= 1


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

  def writeDxfFile(self,fileName):
    '''Write mesh in a DXF file.'''
    drawing= dxfEngine.drawing(fileName)
    self.writeDxf(drawing)
    drawing.save()

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


class MEDMeshData(MeshData):
  meshDimension= None
  spaceDimension= None

  def __init__(self,umesh):
    super(MEDMeshData, self).__init__()
    self.meshDimension= umesh.getMeshDimension()
    self.spaceDimension= umesh.getSpaceDimension()
    self.numberOfCells= umesh.getNumberOfCells()
    self.numberOfNodes= umesh.getNumberOfNodes()
      
    nodes.readFromUMesh(umesh)
    cells.readFromUMesh(umesh)

  def __str__(self):
    retval= "meshDimension= " + str(self.meshDimension) + '\n'
    retval+= "spaceDimension= " +' '+str(self.spaceDimension) + '\n'
    retval+= super(MEDMeshData, self).__str__()
    return retval


import pickle
def dumpMeshes(meshes,fName):
  with open(fName + '.pkl', 'wb') as f:
    pickle.dump(meshes, f, pickle.HIGHEST_PROTOCOL)

def loadMeshes(fName):
  with open(fName + '.pkl', 'r') as f:
    return pickle.load(f)

class PointRecord(NodeRecord):
  ''' Preprocessor K-Point entities'''
  def getStrXCCommand(self,pointLoaderName):
    strId= str(self.id)
    strCommand= '.newPntIDPos3d(' + strId + ',geom.Pos3d(' + str(self.coords[0]) + ',' + str(self.coords[1]) +','+ str(self.coords[2])+'))'
    return 'pt' + strId + '= ' + pointLoaderName + strCommand

class BlockRecord(CellRecord):
  ''' Preprocesor block type entities: line, face, body,...'''
  def getType(self):
    return self.cellType
  def getStrXCCommand(self,xcImportExportData):
    strId= str(self.id)
    loaderName= xcImportExportData.getBlockLoaderName(self.getType())
    strCommand= None
    if(self.cellType=='line'):
      strCommand= 'l' + strId + '= ' + loaderName + '.newLine(' + str(self.nodeIds[0]) + ',' + str(self.nodeIds[1]) +')'
    else:
      print 'BlockRecord::getStrXCCommand not implemented for blocks of type: ', self.cellType
    return strCommand

class BlockData(object):
  ''' Preprocessor entities: points, lines, faces, solids,... '''
  def __init__(self):
    self.name= None
    self.points= {}
    self.blocks= {}
    self.groups= []

  def appendPoint(self,id,x,y,z):
    self.points[id]= PointRecord(int(id),[x,y,z])
  def appendBlock(self,block):
    self.blocks[block.id]= block

  def readFromDxfFile(self,fName,preprocessor,dxfLayers):
    dxfReader= DxfReader.DxfReader()
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
    '''Write the blocs (point, lines, surfaces, volumes,...) in a DXF file'''
    layerName= 'points'
    drawing.add_layer(layerName)
    for key in self.points:
      self.points[key].writeDxf(drawing,layername)

    drawing.add_layer('lines')
    drawing.add_layer('surfaces')
    drawing.add_layer('volumes')
    for key in self.blocks:
      block= self.blocks[key]
      block.writeDxf(drawing)
    #groups have no DXF representation.
    

  def writeToXCFile(self,xcImportExportData):
    f= xcImportExportData.outputFile
    for key in self.points:
      strCommand= self.points[key].getStrXCCommand(xcImportExportData.pointLoaderName)
      f.write(strCommand+'\n')
    for key in self.blocks:
      block= self.blocks[key]
      strCommand= block.getStrXCCommand(xcImportExportData)
      f.write(strCommand+'\n')
    for g in self.groups:
      g.writeToXCFile(xcImportExportData)

  def getPointTags(self):
    tags= []
    for key in self.points:
      tags.append(self.points[key].id)
    return tags;

  def getBlockTags(self):
    tags= []
    for key in self.blocks:
      tags.append(self.blocks[key].id)
    return tags;


  def __str__(self):
    for key in self.points:
      retval+= str(self.points[key]) + '\n'
    for key in self.blocks:
      retval+= str(self.blocks[key]) + '\n'
    return retval
