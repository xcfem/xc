# -*- coding: utf-8 -*-


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc_base
import geom
import xc

class LinSetToMesh(object):
    '''Define the parameters to mesh a set of lines. The method generateMesh
    meshes those lines and adds the nodes and elements created to the set .
    
    :ivar linSet: set of lines.
    :ivar matSect: instance of the class BeamMaterialData that defines the 
          material-section to be applied to the set of lines.
    :ivar elemSize: mean size of the elements
    :ivar vDirLAxZ: direction vector for the element local axis Z 
          defined as xc.Vector([x,y,z]). This is the direction in which
          the Z local axis of the reinforced concrete sections will be
          oriented (i.e. in the case of rectangular sections this Z 
          local axis of the RC section is parallel to the dimension
          defined as width of the rectangle)
    :ivar elemType: type of element for the mesh (defaults to 'elastic_beam_3d')
    '''
    def __init__(self,linSet,matSect,elemSize,vDirLAxZ,elemType='elastic_beam_3d'):
        self.linSet= linSet
        self.matSect= matSect
        self.elemSize= elemSize
        self.vDirLAxZ= vDirLAxZ
        self.elemType= elemType

    def generateMesh(self, preprocessor):
        trfs= preprocessor.getTransfCooLoader
        trYGlobal=trfs.newPDeltaCrdTransf3d('trYGlobal')
        trYGlobal.xzVector=self.vDirLAxZ
        seedElemLoader= preprocessor.getElementLoader.seedElemLoader
        seedElemLoader.defaultMaterial= self.matSect.name
        seedElemLoader.defaultTransformation= 'trYGlobal'
        elem= seedElemLoader.newElement(self.elemType,xc.ID([0,0]))
        for l in self.linSet.getLines:
            l.setElemSize(self.elemSize)
            l.genMesh(xc.meshDir.I)
        self.linSet.fillDownwards()

   
class SurfSetToMesh(object):
    '''Define the parameters to mesh a set of surfaces. The method generateMesh
    meshes those surfaces and adds the nodes and elements created to the set .
    
    :ivar surfSet: set of surfaces.
    :ivar matSect: instance of the class DeckMaterialData that defines the 
          material-section to be applied to the set of surfaces.
    :ivar elemSize: mean size of the elements
    :ivar elemType: type of element for the mesh (defaults to 'shell_mitc4')
    '''
    def __init__(self,surfSet,matSect,elemSize,elemType='shell_mitc4'):
        self.surfSet= surfSet
        self.matSect= matSect
        self.elemSize= elemSize
        self.elemType= elemType

    def generateMesh(self, preprocessor):
        for s in self.surfSet.getSurfaces:
            s.setElemSizeIJ(self.elemSize,self.elemSize)
        preprocessor.getCad.getSurfaces.conciliaNDivs()
        seedElemLoader= preprocessor.getElementLoader.seedElemLoader
#        seedElemLoader.defaultTag= firstElementTag
        seedElemLoader.defaultMaterial= self.matSect.name
        elem= seedElemLoader.newElement(self.elemType,xc.ID([0,0,0,0]))
        for s in self.surfSet.getSurfaces:
            s.genMesh(xc.meshDir.I)
        self.surfSet.fillDownwards()


def multi_mesh(preprocessor,lstMeshSets):
    '''Mesh all the mesh-sets included in lstMeshSets

    :param preprocessor: preprocessor
    :param lstMeshSets: list of instances of classes LinSetToMesh or 
           SurfSetToMesh to be meshed
    '''
    for ms in lstMeshSets:
        ms.generateMesh(preprocessor)
