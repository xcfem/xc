# -*- coding: utf-8 -*-
''' Data structures to describe a finite element mesh.'''

import os
import json
from import_export import mesh_entities as me
from import_export import block_topology_entities as bte

class GroupRecord(object):
    ''' Object defining a grop of entities (nodes, cells, points, or lines).

    :ivar name: group name.
    :ivar nodeIds: identifiers of the nodes.
    :ivar cellIds: identifiers of the cells.
    :ivar pointIds: identifiers of the points.
    :ivar lineIds: identifiers of the lines.
    '''

    def readFromDATFile(self,fName):
        self.name=  os.path.splitext(fName)[0]
        meshDesc= me.MeshData()
        meshDesc.readFromDATFile(fName)
        self.nodeIds= meshDesc.nodes.getTags()
        self.cellIds= meshDesc.cells.getTags()
        self.pointIds= []
        self.lineIds= []

    def setUp(self,name,points,lines):
        self.name= name
        self.nodeIds= []
        self.cellIds= []
        self.pointIds= points
        self.lineIds= lines

    def empty(self):
        return not(self.nodeIds or self.cellIds or self.pointIds or self.lineIds)

    def writeDxfFile(self, dummy):
        '''groups have not representation in dxf files.

        :param dummy: dummy argument (not used).
        '''
        return

    def writeToXCFile(self, xcImportExportData):
        ''' writes the XC commands to define the group in a file.

        :param xcImportExportData:Data used when importing and/or exporting XC models.
        '''
        if(not self.empty()):
            f= xcImportExportData.outputFile
            strCommand= self.name + '= ' + xcImportExportData.setHandlerName + '.defSet("' + self.name +'")'
            f.write(strCommand+'\n')
            for n in self.nodeIds:
                strCommand= self.name + '.getNodes.append(n' + str(n) + ')'
                f.write(strCommand+'\n')
            for e in self.cellIds:
                strCommand= self.name + '.elements.append(e' + str(e) + ')'
                f.write(strCommand+'\n')
            for p in self.pointIds:
                strCommand= self.name + '.getMultiBlockTopology.getPoints.append(' + str(p) + ')'
                f.write(strCommand+'\n')
            for l in self.lineIds:
                strCommand= self.name + '.getMultiBlockTopology.getLines.append(' + str(l) + ')'
                f.write(strCommand+'\n')

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''        
        return {'name':self.name, 'nodeIds':self.nodeIds, 'cellIds':self.cellIds, 'pointIds':self.pointIds, 'lineIds':self.lineIds}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.name= dct['name']
        self.nodeIds= dct['nodeIds']
        self.cellIds= dct['cellIds']
        self.pointIds= dct['pointIds']
        self.lineIds= dct['lineIds']


class XCImportExportData(object):
    ''' Data used when importing and/or exporting XC
        models.

    :ivar outputFileName: name to use for the output file.
    :ivar problemName: name for the XC problem object.
    :ivar nodeHandlerName: name for the XC node handler.
    :ivar cellHandlerName: name for the XC element handler.
    :ivar setHandlerName: name for the XC group handler.
    :ivar pointHandlerName: name for the XC point handler.
    :ivar lineHandlerName: name for the XC line handler.
    :ivar surfaceHandlerName: name for the XC surface handler.
    :ivar cellConversion: dictionary for cell conversion.
    :ivar outputFile: Python file object used for output.
    :ivar meshDesc: string describing the mesh
    :ivar blockData: block data (points, lines, surfaces, bodies).
    '''

    def __init__(self):
        self.mainDATFile= ""
        self.groupDATFiles= [] 
        self.dxfLayers= []
        self.outputFileName= 'xc_mesh' 
        self.problemName= None
        self.nodeHandlerName= "nodes"
        self.cellHandlerName= "elements"
        self.setHandlerName= "groups"
        self.pointHandlerName= "points"
        self.lineHandlerName= "lines"
        self.surfaceHandlerName= "surfaces"
        self.cellConversion= {}
        self.outputFile= None
        self.meshDesc= None
        self.blockData= None

    def getXCFileName(self):
        return self.outputFileName+'.py'
    
    def getJSONFileName(self):
        return self.outputFileName+'.json'

    def getDxfFileName(self):
        return self.outputFileName+'.dxf'

    def getBlockHandlerName(self,blockType):
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
        self.meshDesc= me.MeshData()
        self.meshDesc.readFromDATFile(self.mainDATFile)
        for fName in self.groupDATFiles:
            grp= GroupRecord()
            grp.name= os.path.splitext(fName)[0]
            grp.readFromDATFile(fName)
            self.meshDesc.groups.append(grp)

    def readDxfFile(self,fName,preprocessor):
        self.blockData= bte.BlockData()
        self.blockData.name= fName
        self.blockData.readFromDxfFile(fName,preprocessor,self.dxfLayers)
        for l in self.dxfLayers:
            grp= GroupRecord()
            grp.setUp(l,self.blockData.pointsInLayer[l],self.blockData.blocksInLayer[l])
            self.blockData.groups.append(grp)

    def writeDxfFile(self,fileName):
        self.blockData.writeDxfFile(fileName)

    def getXCCommandString(self):
        ''' Return a string with the XC commands that define the model.'''
        strCommand= ''
        if(self.problemName!=None):
            strCommand+= 'problemName= \'' + self.problemName+'\'\n'
            strCommand+= self.problemName + '= xc.FEProblem()'+'\n'
            strCommand+= 'preprocessor= ' + self.problemName + '.getPreprocessor\n'
        strCommand+= self.nodeHandlerName + '= preprocessor.getNodeHandler\n'
        strCommand+= self.cellHandlerName + '= preprocessor.getElementHandler\n'
        strCommand+= self.pointHandlerName + '= preprocessor.getMultiBlockTopology.getPoints\n'
        strCommand+= self.lineHandlerName + '= preprocessor.getMultiBlockTopology.getLines\n'
        strCommand+= self.surfaceHandlerName + '= preprocessor.getMultiBlockTopology.getSurfaces\n'
        #strCommand+= self.lineHandlerName + '= preprocessor.getMultiBlockTopology.getLines\n'
        strCommand+= self.setHandlerName + '= preprocessor.getSets\n'
        if(self.blockData):
            strCommand+= self.blockData.getXCCommandString(self)
        if(self.meshDesc):
            strCommand+= self.meshDesc.getXCCommandString(self)
        return strCommand

    def dumpToXC(self, preprocessor):
        ''' Dump the objects in this container into the preprocessor argument.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        blockTopology= None
        if(self.blockData):
            blockTopology= self.blockData.dumpToXC(preprocessor)
        mesh= None
        if(self.meshDesc):
            mesh= self.meshDesc.dumpToXC(preprocessor)
        return blockTopology, mesh

    def readFromXCSet(self, xcSet):
        ''' Read the objects from the XC set and create the 
            corresponding entities here.

        :param preprocessor: XC finite element problem preprocessor.
        '''
        if(len(xcSet.points)>0):
            self.blockData= bte.BlockData()
            self.blockData.readFromXCSet(xcSet)
        if(len(xcSet.nodes)>0):
            self.meshDesc= me.MeshData()
            self.meshDesc.readFromXC(xcSet)
        
    def writeToXCFile(self):
        ''' Write the model to a XC file.'''
        self.outputFile= open(self.getXCFileName(),"w")
        xcCommandString= self.getXCCommandString()
        self.outputFile.write(xcCommandString)
        self.outputFile.close()

    def writeToJSON(self):
        '''Write the objects that define the mesh.

        :param xcImportExportData: import/export parameters.
        '''
        self.outputFile= open(self.getJSONFileName(),"w")
        blockDict= self.getDict()
        jsonString= json.dumps(blockDict)
        self.outputFile.write(jsonString)
        self.outputFile.close()

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= {'mainDATFile':self.mainDATFile, 'groupDATFiles':self.groupDATFiles, 'dxfLayers':self.dxfLayers, 'outputFileName':self.outputFileName, 'problemName':self.problemName, 'nodeHandlerName':self.nodeHandlerName, 'cellHandlerName':self.cellHandlerName, 'setHandlerName':self.setHandlerName, 'pointHandlerName':self.pointHandlerName, 'lineHandlerName':self.lineHandlerName, 'surfaceHandlerName':self.surfaceHandlerName, 'cellConversion':self.cellConversion}
        meshDict= None
        if(self.meshDesc):
            meshDict= self.meshDesc.getDict()
        blockDict= None
        if(self.blockData):
            blockDict= self.blockData.getDict()
        retval.update({'meshDesc':meshDict, 'blockData': blockDict})
        return retval

    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.mainDATFile= dct['mainDATFile']
        self.groupDATFiles= dct['groupDATFiles']
        self.dxfLayers= dct['dxfLayers']
        self.outputFileName= dct['outputFileName']
        self.problemName= dct['problemName']
        self.nodeHandlerName= dct['nodeHandlerName']
        self.cellHandlerName= dct['cellHandlerName']
        self.setHandlerName= dct['setHandlerName']
        self.pointHandlerName= dct['pointHandlerName']
        self.lineHandlerName= dct['lineHandlerName']
        self.surfaceHandlerName= dct['surfaceHandlerName']
        self.cellConversion= dct['cellConversion']
        meshDict= dct['meshDesc']
        if(meshDict):
            self.meshDesc= me.MeshData()
            self.meshDesc.setFromDict(meshDict)
        blockDict= dct['blockData']
        if(blockDict):
            self.blockData= bte.BlockData()
            self.blockData.setFromDict(blockDict)
        
class MEDMeshData(me.MeshData):
    meshDimension= None
    spaceDimension= None

    def __init__(self,umesh):
        super(MEDMeshData, self).__init__()
        self.meshDimension= umesh.getMeshDimension()
        self.spaceDimension= umesh.getSpaceDimension()
        self.numberOfCells= umesh.getNumberOfCells()
        self.numberOfNodes= umesh.getNumberOfNodes()

        self.nodes.readFromUMesh(umesh)
        self.cells.readFromUMesh(umesh)

    def __str__(self):
        retval= "meshDimension= " + str(self.meshDimension) + '\n'
        retval+= "spaceDimension= " +' '+str(self.spaceDimension) + '\n'
        retval+= super(MEDMeshData, self).__str__()
        return retval

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= super().getDict()
        retval.update({'meshDimension':self.meshDimension, 'spaceDimension':self.spaceDimension, 'numberOfCells':self.numberOfCells, 'numberOfNodes':self.numberOfNodes})
        return retval

    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        self.meshDimension= dct['meshDimension']
        self.spaceDimension= dct['spaceDimension']
        self.numberOfCells= dct['numberOfCells']
        self.numberOfNodes= dct['numberOfNodes']
        


import pickle
def dumpMeshes(meshes,fName):
    with open(fName + '.pkl', 'wb') as f:
        pickle.dump(meshes, f, pickle.HIGHEST_PROTOCOL)

def loadMeshes(fName):
    with open(fName + '.pkl', 'r') as f:
        return pickle.load(f)

