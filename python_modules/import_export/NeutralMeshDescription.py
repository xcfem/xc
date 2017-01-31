# -*- coding: utf-8 -*-

import os
from dxfwrite import DXFEngine
import DxfReader
import MeshEntities as me
import BlockTopologyEntities as bte


class GroupRecord(object):

  def readFromDATFile(self,fName):
    self.name= None
    meshDesc= me.MeshData()
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
    self.lineLoaderName= "lines"
    self.surfaceLoaderName= "surfaces"
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
    strCommand= self.lineLoaderName + '= preprocessor.getCad.getLines'
    self.outputFile.write(strCommand+'\n')
    strCommand= self.surfaceLoaderName + '= preprocessor.getCad.getSurfaces'
    self.outputFile.write(strCommand+'\n')
    #strCommand= self.lineLoaderName + '= preprocessor.getCad.getLines'
    #self.outputFile.write(strCommand+'\n')
    strCommand= self.setLoaderName + '= preprocessor.getSets'
    self.outputFile.write(strCommand+'\n')
    if(self.blockData):
      self.blockData.writeToXCFile(self)
    if(self.meshDesc):
      self.meshDesc.writeToXCFile(self)


class MEDMeshData(me.MeshData):
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

