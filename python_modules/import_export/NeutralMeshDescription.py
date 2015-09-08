#!/usr/bin/env python
import os

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

class GroupRecord(object):

  def readFromDATFile(self,fName):
    self.name= None
    meshDesc= MeshData()
    meshDesc.readFromDATFile(fName)
    self.nodeIds= meshDesc.getNodeTags()
    self.cellIds= meshDesc.getCellTags()

  def writeToXCFile(self,xcImportExportData):
    f= xcImportExportData.outputFile
    strCommand= self.name + '= ' + xcImportExportData.setLoaderName + '.defSet("' + self.name +'")'
    f.write(strCommand+'\n')
    for n in self.nodeIds:
      strCommand= self.name + '.getNodes.append(n' + str(n) + ')'
      f.write(strCommand+'\n')
    for e in self.cellIds:
      strCommand= self.name + '.getElements.append(e' + str(e) + ')'
      f.write(strCommand+'\n')


class XCImportExportData(object):
  mainDATFile= ""
  groupDATFiles= [] 
  xcFileName= "xc_mesh.py"
  problemName= None
  nodeLoaderName= "nodes"
  cellLoaderName= "elements"
  setLoaderName= "groups"
  cellConversion= {}
  meshDesc= None
  outputFile= None

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
  def writeToXCFile(self):
    self.outputFile= open(self.xcFileName,"w")
    strCommand= 'mdlr= ' + self.problemName + '.getModelador'
    self.outputFile.write(strCommand+'\n')
    strCommand= self.nodeLoaderName + '= mdlr.getNodeLoader'
    self.outputFile.write(strCommand+'\n')
    strCommand= self.cellLoaderName + '= mdlr.getElementLoader'
    self.outputFile.write(strCommand+'\n')
    strCommand= self.setLoaderName + '= mdlr.getSets'
    self.outputFile.write(strCommand+'\n')
    self.meshDesc.writeToXCFile(self)

class MeshData(object):

  def __init__(self):
    self.name= None
    self.numberOfCells= None
    self.numberOfNodes= None
    self.nodes= {}
    self.cells= {}
    self.nodeSupports= {}
    self.groups= []

  def appendNode(self,id,x,y,z):
    self.nodes[id]= NodeRecord(int(id),[x,y,z])
  def appendCell(self,cell):
    self.cells[cell.id]= cell
  def appendNodeSupport(self, ns):
    self.nodeSupports[ns.id]= ns

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

    self.nodes= {}
    for i in range(beginNodes,endNodes):
      line= lines[i]
      id, x, y ,z = line.split()
      appendNode(id,x,y,z)

    self.cells= {}
    beginCells= endNodes
    endCells= beginCells+self.numberOfCells
    for i in range(beginCells,endCells):
      line= lines[i]
      lst= line.split()
      sz= len(lst)
      id= int(lst[0])
      type= int(lst[1])
      nodeIds= []
      for j in range(2,sz):
        nodeIds.append(int(lst[j]))
      self.cells[id]= CellRecord(id, type, nodeIds)

  def writeToXCFile(self,xcImportExportData):
    f= xcImportExportData.outputFile
    for key in self.nodes:
      strCommand= self.nodes[key].getStrXCCommand(xcImportExportData.nodeLoaderName)
      f.write(strCommand+'\n')
    for key in self.cells:
      cell= self.cells[key]
      type= xcImportExportData.convertCellType(cell.cellType)
      if(type!=None):
        strCommand= self.cells[key].getStrXCCommand(xcImportExportData)
        f.write(strCommand+'\n')
    for g in self.groups:
      g.writeToXCFile(xcImportExportData)

  def getNodeTags(self):
    tags= []
    for key in self.nodes:
      tags.append(self.nodes[key].id)
    return tags;

  def getCellTags(self):
    tags= []
    for key in self.cells:
      tags.append(self.cells[key].id)
    return tags;


  def __str__(self):
    retval= "numberOfNodes= " +' '+str(self.numberOfNodes) + '\n'
    retval+= "numberOfCells= " +' '+str(self.numberOfCells) + '\n'
    for key in self.nodes:
      retval+= str(self.nodes[key]) + '\n'
    for key in self.cells:
      retval+= str(self.cells[key]) + '\n'
    return retval


class MEDMeshData(MeshData):
  meshDimension= None
  spaceDimension= None

  def __init__(self,umesh):
    self.meshDimension= umesh.getMeshDimension()
    self.spaceDimension= umesh.getSpaceDimension()
    self.numberOfCells= umesh.getNumberOfCells()
    self.numberOfNodes= umesh.getNumberOfNodes()
      
    nodes= umesh.getCoordinatesAndOwner()
    self.nodes= {}
    id= 0
    for n in nodes:
      self.nodes[id]= NodeRecord(id,n)
      id+= 1

    for i in range(0,self.numberOfCells):
      self.cells.append(CellRecord(umesh.getTypeOfCell(i), umesh.getNodeIdsOfCell(i)))

  def __str__(self):
    retval= "meshDimension= " + str(self.meshDimension) + '\n'
    retval+= "spaceDimension= " +' '+str(self.spaceDimension) + '\n'
    retval+= "numberOfNodes= " +' '+str(self.numberOfNodes) + '\n'
    retval+= "numberOfCells= " +' '+str(self.numberOfCells) + '\n'
    for key in self.nodes:
      retval+= str(self.nodes[key]) + '\n'
    for e in self.cells:
      retval+= str(e) + '\n'
    return retval


import pickle
def dumpMeshes(meshes,fName):
  with open(fName + '.pkl', 'wb') as f:
    pickle.dump(meshes, f, pickle.HIGHEST_PROTOCOL)

def loadMeshes(fName):
  with open(fName + '.pkl', 'r') as f:
    return pickle.load(f)
