# -*- coding: utf-8 -*-
''' Read block topology data from FreeCAD files.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2020, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega.ort@gmail.com"

import sys
from import_export import reader_base
from scipy.spatial.distance import cdist
from misc_utils import log_messages as lmsg
from import_export import block_topology_entities as bte

# path to FreeCAD.so
FREECADPATH = '/usr/lib/freecad-daily-python2/lib/' 
if(sys.version_info.major == 3):
    FREECADPATH = '/usr/lib/freecad-daily-python3/lib/'
sys.path.append(FREECADPATH)

# import FreeCAD
try:
    import FreeCAD
except ModuleNotFoundError as err:
    # Error handling
    lmsg.log(err)
    quit()

ifcStringAttributes= ['IfcType', 'PredefinedType', 'IfcProperties', 'SteelGrade']

ifcLengthAttributes= ['Thickness', 'LongitudinalBarNominalDiameter', 'LongitudinalBarSpacing', 'TransverseBarNominalDiameter', 'TransverseBarSpacing']

ifcAreaAttributes= ['LongitudinalBarCrossSectionArea', 'TransverseBarCrossSectionArea']

    
def get_ifc_attributes(obj):
    ''' Return the ifc attributes of the argument.

    :param obj: object to get the IFC attributes from.
    '''
    retval= dict()
    # Read "regular" attributes
    for attrName in ifcStringAttributes:
        if hasattr(obj,attrName):
            retval[attrName]= getattr(obj,attrName)
    # Read attributes that correspond to a lenght measurement.
    for attrName in ifcLengthAttributes:
        if hasattr(obj,attrName):
            retval[attrName]= getattr(obj,attrName).getValueAs('m') # meter
    # Read attributes that correspond to an area measurement.
    for attrName in ifcAreaAttributes:
        if hasattr(obj,attrName):
            retval[attrName]= getattr(obj,attrName) # square meter
    # Read attributes that have a somewhat "special" treatment in XC.
    if hasattr(obj,'Material') and obj.Material:
        retval['Material']= obj.Material.Label
    if hasattr(obj,'Description'):
        retval['IfcDescription']= obj.Description
    return retval

class FreeCADImport(reader_base.ReaderBase):
    '''Import FreeCAD geometric entities.

     :ivar groupsToImport: list of regular expressions to be tested.
    '''
    def __init__(self,fileName, groupsToImport, getRelativeCoo, threshold= 0.01,importLines= True, importSurfaces= True):
        ''' Constructor.

           :param fileName: file name to import.
           :param grouptsToImport: list of regular expressions to be tested.
           :param getRelativeCoo: coordinate transformation to be applied to the
                                  points.
           :param importLines: if true import lines.
           :param importSurfaces: if true import surfaces.
        '''
        super(FreeCADImport, self).__init__(fileName, getRelativeCoo, threshold, importLines, importSurfaces)
        self.document= FreeCAD.openDocument(self.fileName)
        self.groupsToImport= self.getObjectsToImport(groupsToImport)
        if(len(self.groupsToImport)):
            self.kPointsNames= self.selectKPoints()
            self.importPoints()
            if(self.impLines):
                self.importLines()
            if(self.impSurfaces):
                self.importFaces()
        else:
            self.kPoints= None
        
    def getObjectsToImport(self, namesToImport):
        '''Return the object names that will be imported according to the
           regular expressions contained in the second argument.

           :param namesToImport: list of regular expressions to be tested.
        '''
        retval= []
        for obj in self.document.Objects:
            if(reader_base.nameToImport(obj.Label, namesToImport)):
                retval.append(obj.Label)
        if(len(retval)==0):
            lmsg.warning(f'No groups to import (names to import: {namesToImport})')
        return retval
    
    def extractPoints(self):
        '''Extract the points from the entities argument.'''
        retval_pos= dict()
        retval_properties= dict()
        def append_point(pt, groupName, pointName, objProperties):
            '''Append the point to the lists.'''
            retval_pos[pointName]= self.getRelativeCoo(pt)
            # group name as label.
            objProperties.extendLabels([groupName])
            retval_properties[pointName]= objProperties
        def append_points(vertexes, objName, groupName, objProperties):
            '''Append the points to the list.'''
            if(len(vertexes)>1):
                for ptCount, v in enumerate(vertexes):
                    pointName= f'{objName}.{ptCount}'
                    append_point([v.X, v.Y, v.Z], grpName, pointName, objProperties)
            else:
                v= vertexes[0]
                append_point([v.X, v.Y, v.Z], grpName, objName, objProperties)
                
        for grpName in self.groupsToImport:
            grp= self.document.getObjectsByLabel(grpName)[0]
            if(hasattr(grp,'Shape')): # Object has shape.
                objName= grp.Name
                shape= grp.Shape
                shapeType= shape.ShapeType
                objProperties= bte.BlockProperties(labels= [grp.Label])
                if(shapeType=='Shell'):
                    for fCount, f in enumerate(shape.SubShapes):
                        thisFaceName= f'{objName}.{fCount}'
                        append_points(f.OuterWire.OrderedVertexes, thisFaceName, grpName, objProperties)
                else:
                    append_points(grp.Shape.Vertexes, objName, grpName, objProperties)
            elif(len(grp.OutList)>0): # Object is a group
                for obj in grp.OutList: 
                    if(hasattr(obj,'Shape')): # Object has shape.
                        shapeType= obj.Shape.ShapeType
                        objName= obj.Name
                        objProperties= bte.BlockProperties(labels= [obj.Label])
                        if(shapeType=='Face'):
                            append_points(obj.Shape.Vertexes, objName, grpName, objProperties)
        return retval_pos, retval_properties
    
    def importPoints(self):
        ''' Import points from FreeCAD.'''
        self.points= dict()
        for obj in self.document.Objects:
            if(hasattr(obj,'Shape')):
                shape= obj.Shape
                shapeType= shape.ShapeType
                pointName= obj.Name
                labelName= obj.Label
                if((shapeType=='Vertex') and (labelName in self.groupsToImport)):
                    vertices= [-1]
                    p= self.getRelativeCoo([float(shape.X), float(shape.Y), float(shape.Z)])
                    vertices[0]= self.getIndexNearestPoint(p)
                    self.points[pointName]= vertices
                    properties= bte.BlockProperties(labels= [labelName], attributes= get_ifc_attributes(obj))
                    self.propertyDict[pointName]= properties
                    
    def importLines(self):
        ''' Import lines from FreeCAD file.'''
        self.lines= {}
        for obj in self.document.Objects:
            if(hasattr(obj,'Shape')):
                shapeType= obj.Shape.ShapeType
                labelName= obj.Label
                if((shapeType=='Wire') and (labelName in self.groupsToImport)):
                    for i, e in enumerate(obj.Shape.Edges):
                        lineName= f"{obj.Name}{i}"
                        vertices= [-1,-1]
                        v0= e.Vertexes[0]
                        v1= e.Vertexes[1]
                        p1= self.getRelativeCoo([float(v0.X), float(v0.Y), float(v0.Z)])
                        p2= self.getRelativeCoo([float(v1.X), float(v1.Y), float(v1.Z)])
                        length= cdist([p1],[p2])[0][0]
                        # Try to have all lines with the
                        # same orientation.
                        idx0, idx1= self.getOrientation(p1, p2, length/1e4)
                        # end orientation.
                        vertices[0]= idx0
                        vertices[1]= idx1
                        if(vertices[0]==vertices[1]):
                            lmsg.error(f'Error in line {lineName} vertices are equal: {vertices}')
                        if(length>self.threshold):
                            self.lines[lineName]= vertices
                            objLabels= [labelName]
                            # # groups
                            # if(lineName in self.entitiesGroups):
                            #     objLabels.extend(self.entitiesGroups[lineName])
                            self.propertyDict[lineName]= bte.BlockProperties(labels= objLabels, attributes= get_ifc_attributes(obj))
                        else:
                            lmsg.error(f'line too short: {p1},{p2}, {length}')

                        
    def importFaces(self):
        ''' Import faces from FreeCAD file.'''
        self.facesTree= {}
        for name in self.groupsToImport:
            self.facesTree[name]= dict()

        def import_face(faceShape, faceName, labelName):
            ''' Add the face argument to the dictionary.'''
            vertices= list()
            objPoints= list()
            for v in faceShape.OuterWire.OrderedVertexes:                    
                objPoints.append([float(v.X), float(v.Y), float(v.Z)])
            for pt in objPoints:
                p= self.getRelativeCoo(pt)
                idx= self.getIndexNearestPoint(p)
                vertices.append(idx)
            facesDict[faceName]= vertices
            properties= bte.BlockProperties(labels= [labelName], attributes= get_ifc_attributes(obj))
            self.propertyDict[faceName]= properties

        def import_shell(shapeContainer, faceName, labelName):
            ''' Import shell objects from the container argument.'''
            for fCount, f in enumerate(shapeContainer):
                thisFaceName= f'{faceName}.{fCount}'
                import_face(f, thisFaceName, labelName)

        def import_shape(shape, objName, labelName):
            ''' Import simple shape.'''
            shapeType= shape.ShapeType
            if(shapeType=='Face'):
                import_face(shape, objName, labelName)
            elif(shapeType=='Shell'):
                for s in shape.SubShapes:
                    import_shape(s, objName, labelName)
            elif(shapeType=='Compound'):
                for cCount, ss in enumerate(shape.SubShapes):
                    ssType= ss.ShapeType
                    ssName= f'{objName}.{cCount}'
                    import_shape(ss, ssName, labelName)
            elif(shapeType=='Vertex'):
                count=0 # Nothing to do with those here.
            elif(shapeType in ['Wire']):
                count= 0 # Nothing to do with those.
            else:
                lmsg.log('Entity with shape of type: '+shapeType+' ignored.')      
            

        for obj in self.document.Objects:
            if(hasattr(obj,'Shape')):
                shapeType= obj.Shape.ShapeType
                objName= obj.Name
                labelName= obj.Label
                if(labelName in self.groupsToImport):
                    facesDict= self.facesTree[labelName]
                    import_shape(obj.Shape, objName, labelName)

              
    def getNamesToImport(self):
        ''' Return the names of the objects to import.'''
        return self.groupsToImport
