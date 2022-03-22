# -*- coding: utf-8 -*-
''' Read block topology data from FreeCAD files.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2020, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega.ort@gmail.com"

import xc_base
import sys
import re
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
# NOTE 1: Apparently the FreeCAD module redirects C standard output
# somewhere so you stop seeing cout, clog and cerr messages in the
# console. 
try:
    import FreeCAD
    xc_base.resetStandardOutput() # See NOTE 1. 
except ModuleNotFoundError as err:
    # Error handling
    lmsg.log(err)
    quit()

ifcStringAttributes= ['IfcType', 'PredefinedType', 'IfcProperties', 'SteelGrade']

ifcLengthAttributes= ['Thickness', 'LongitudinalBarNominalDiameter', 'LongitudinalBarSpacing', 'TransverseBarNominalDiameter', 'TransverseBarSpacing']

ifcAreaAttributes= ['LongitudinalBarCrossSectionArea', 'TransverseBarCrossSectionArea']

def getType(obj):
    '''getType(object): returns the Draft type of the given object'''
    import Part
    if not obj:
        return None
    if isinstance(obj,Part.Shape):
        return 'Shape'
    if 'Proxy' in obj.PropertiesList:
        if hasattr(obj.Proxy,'Type'):
            return obj.Proxy.Type
    if obj.isDerivedFrom('Sketcher::SketchObject'):
        return 'Sketch'
    if (obj.TypeId == 'Part::Line'):
        return 'Part::Line'
    if (obj.TypeId == 'Part::Offset2D'):
        return 'Offset2D'
    if obj.isDerivedFrom('Part::Feature'):
        return 'Part'
    if (obj.TypeId == 'App::Annotation'):
        return 'Annotation'
    if obj.isDerivedFrom('Mesh::Feature'):
        return 'Mesh'
    if obj.isDerivedFrom('Points::Feature'):
        return 'Points'
    if (obj.TypeId == 'App::DocumentObjectGroup'):
        return 'Group'
    if (obj.TypeId == 'App::Part'):
        return 'App::Part'
    return 'Unknown'
    
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
           :param groupsToImport: list of regular expressions to be tested.
           :param getRelativeCoo: coordinate transformation to be applied to the
                                  points.
           :param importLines: if true import lines.
           :param importSurfaces: if true import surfaces.
        '''
        super(FreeCADImport, self).__init__(fileName, getRelativeCoo, threshold, importLines, importSurfaces)
        self.document= FreeCAD.openDocument(self.fileName)
        self.groupsToImport= self.getObjectsToImport(groupsToImport)
        self.compounds= dict() # Stack for compound names.
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
        # Compile regular expressions.
        regExpList= list()
        for name in namesToImport:
            regExpList.append(re.compile(name))
        for obj in self.document.Objects:
            if(reader_base.nameToImport(obj.Label, regExpList)):
                retval.append(obj)
        if(len(retval)==0):
            lmsg.warning(f'No groups to import (names to import: {namesToImport})')
        return retval
    
    def extractPoints(self):
        '''Extract the points from the entities argument.'''
        retval_pos= dict()
        retval_properties= dict()
        def append_point(pt, groupLabel, pointName, objProperties):
            '''Append the point to the lists.'''
            retval_pos[pointName]= self.getRelativeCoo(pt)
            # group name as label.
            objProperties.extendLabels([groupLabel])
            retval_properties[pointName]= objProperties
        def append_points(vertexes, objName, groupLabel, objProperties):
            '''Append the points to the list.'''
            if(len(vertexes)>1):
                for ptCount, v in enumerate(vertexes):
                    pointName= f'{objName}.{ptCount}'
                    append_point([v.X, v.Y, v.Z], groupLabel, pointName, objProperties)
            else:
                v= vertexes[0]
                append_point([v.X, v.Y, v.Z], groupLabel, objName, objProperties)
                
        for grp in self.groupsToImport:
            groupLabel= grp.Label
            if(hasattr(grp,'Shape')): # Object has shape.
                objName= grp.Name
                shape= grp.Shape
                shapeType= shape.ShapeType
                objProperties= bte.BlockProperties(labels= [groupLabel])
                if(shapeType=='Shell'):
                    for fCount, f in enumerate(shape.SubShapes):
                        thisFaceName= f'{objName}.{fCount}'
                        append_points(f.OuterWire.OrderedVertexes, thisFaceName, groupLabel, objProperties)
                else:
                    append_points(grp.Shape.Vertexes, objName, groupLabel, objProperties)
            elif(len(grp.OutList)>0): # Object is a group
                for obj in grp.OutList: 
                    if(hasattr(obj,'Shape')): # Object has shape.
                        shapeType= obj.Shape.ShapeType
                        objName= obj.Name
                        objProperties= bte.BlockProperties(labels= [obj.Label])
                        if(shapeType=='Face'):
                            append_points(obj.Shape.Vertexes, objName, groupLabel, objProperties)
        return retval_pos, retval_properties
    
    def importPoints(self):
        ''' Import points from FreeCAD file.'''
        self.points= dict()
        for obj in self.groupsToImport:
            if(hasattr(obj,'Shape')):
                shape= obj.Shape
                shapeType= shape.ShapeType
                pointName= obj.Name
                labelName= obj.Label
                if(shapeType=='Vertex'):
                    vertices= [-1]
                    p= self.getRelativeCoo([float(shape.X), float(shape.Y), float(shape.Z)])
                    vertices[0]= self.getIndexNearestPoint(p)
                    self.points[pointName]= vertices
                    pointAttributes= get_ifc_attributes(obj)
                    properties= bte.BlockProperties(labels= [labelName], attributes= pointAttributes)
                    self.propertyDict[pointName]= properties
                    
    def importLines(self):
        ''' Import lines from FreeCAD file.'''
        self.lines= dict()

        def import_edge(e, edgeName, labelName, edgeAttributes):
            ''' Import an edge.
            
            :param e: edge to import.
            :param edgeName: name of the edge object.
            :param labelName: label of the parent object.
            :param edgeAttributes: IFC attributes of the parent object.
            '''
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
                lmsg.error(f'Error in line {edgeName} vertices are equal: {vertices}')
            if(length>self.threshold):
                self.lines[edgeName]= vertices
                objLabels= [labelName]
                # # groups
                # if(edgeName in self.entitiesGroups):
                #     objLabels.extend(self.entitiesGroups[edgeName])
                self.propertyDict[edgeName]= bte.BlockProperties(labels= objLabels, attributes= edgeAttributes)
            else:
                lmsg.error(f'line too short: {p1},{p2}, {length}')

        def import_edges(edges, names, labelName, attributes):
            ''' Import edges from a wire shape.
            
            :param edges: edge list.
            :param names: list containing the names of the edges.
            :param labelName: label of the parent object.
            :param attributes: IFC attributes for all the edges.
            '''
            for e, name in zip(edges, names):
                import_edge(e, name, labelName, attributes)
            
        def import_wire(wireObj, attributes):
            ''' Import a wire shape.
            
            :param wireObj: shape object containing the wire.
            :param wireName: name of the wire object.
            :param objLabel: label of the parent object.
            :param attributes: IFC attributes for all the edges.
            '''
            labelName= wireObj.Label
            edges= list()
            names= list()
            for i, e in enumerate(wireObj.Shape.Edges):
                edges.append(e)
                names.append(f'{wireObj.Name}{i}')
            import_edges(edges, names, labelName, attributes)

        for obj in self.groupsToImport:
            if(hasattr(obj,'Shape')):
                shapeType= obj.Shape.ShapeType
                labelName= obj.Label
                attributes= get_ifc_attributes(obj)
                if(shapeType=='Wire'):
                    import_wire(obj, attributes)
                elif(shapeType=='Compound'):
                    for cCount, ss in enumerate(obj.Shape.SubShapes):
                        ssType= ss.ShapeType
                        ssName= f'{obj.Name}.{cCount}'
                        edges= ss.Edges
                        names= list()
                        for i in range(0,len(edges)):
                            names.append(ssName+'.'+str(i))
                        import_edges(edges, names, labelName+'.'+ssName, attributes)
                elif(shapeType=='Edge'):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.warning(className+'.'+methodName+'; entity with shape of type: '+shapeType+' not imported yet (promote it into "Line" if you want it to be imported).')

                        
    def importFaces(self):
        ''' Import faces from FreeCAD file.'''
        self.facesTree= dict()
        for obj in self.groupsToImport:
            self.facesTree[obj.Label]= dict()

        def import_face(faceShape, faceName, objLabel):
            ''' Add the face argument to the dictionary.'''
            objPoints= list()
            for wire in faceShape.Wires:
                wirePoints= list()
                for v in wire.OrderedVertexes:                    
                    wirePoints.append([float(v.X), float(v.Y), float(v.Z)])
                objPoints.append(wirePoints)
            vertices= list()
            for wirePoints in objPoints:
                wireVertices= list()
                for pt in wirePoints:
                    p= self.getRelativeCoo(pt)
                    idx= self.getIndexNearestPoint(p)
                    wireVertices.append(idx)
                vertices.append(wireVertices)
            # Outer wire.
            facesDict[faceName]= vertices[0]
            faceAttributes= get_ifc_attributes(obj)
            faceAttributes.update({'name':faceName})
            # Inner wire(s).
            holes= list()
            for idx, wire in enumerate(vertices[1:]):
                holeName= faceName+'_hole'+str(idx)
                facesDict[holeName]= wire
                holeAttributes= get_ifc_attributes(obj)
                holeAttributes.update({'objType':'hole', 'ownerName':faceName})
                holeProperties= bte.BlockProperties(labels= [objLabel], attributes= holeAttributes)
                self.propertyDict[holeName]= holeProperties
                holes.append(holeName)
            if(len(holes)>0):
                faceAttributes.update({'holeNames':holes})
            properties= bte.BlockProperties(labels= [objLabel], attributes= faceAttributes)
            self.propertyDict[faceName]= properties
            

        def import_shell(shapeContainer, faceName, objLabel):
            ''' Import shell objects from the container argument.'''
            for fCount, f in enumerate(shapeContainer):
                thisFaceName= f'{faceName}.{fCount}'
                import_face(f, thisFaceName, objLabel)

        def import_shape(shape, objName, objLabel):
            ''' Import simple shape.'''
            shapeType= shape.ShapeType
            if(shapeType=='Face'):
                import_face(shape, objName, objLabel)
            elif(shapeType=='Shell'):
                for s in shape.SubShapes:
                    import_shape(s, objName, objLabel)
            elif(shapeType=='Compound'):
                for cCount, ss in enumerate(shape.SubShapes):
                    ssType= ss.ShapeType
                    ssName= f'{objName}.{cCount}'
                    import_shape(ss, ssName, objLabel)
            elif(shapeType=='Vertex'):
                count=0 # Nothing to do with those here.
            elif(shapeType in ['Wire']):
                count= 0 # Nothing to do with those.
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; entity with shape of type: '+shapeType+' ignored.')

        for obj in self.groupsToImport:
            if(hasattr(obj,'Shape')):
                shapeType= obj.Shape.ShapeType
                objName= obj.Name
                objLabel= obj.Label
                if(objLabel in self.facesTree):
                    facesDict= self.facesTree[objLabel]
                    import_shape(obj.Shape, objName, objLabel)
                    # Store compound components.
                    if(shapeType=='Compound'):
                        objTypeId= obj.TypeId
                        compoundContainer= None
                        if(objTypeId=='App::DocumentObjectGroup'): # Object group.
                            compoundContainer= obj.Group
                            compoundType= 'Group'
                        #draftType= getType(obj)
                        elif(objTypeId!='Part::FeaturePython'):
                            compoundContainer= obj.Links
                            compoundType= 'Compound'
                        if(compoundContainer):
                            for tag, lnk in enumerate(compoundContainer):
                                componentLabel= obj.Label+'.'+compoundType+'.'+str(tag)
                                if(componentLabel in self.compounds):
                                    self.compounds[componentLabel].add({objLabel})
                                else:
                                    self.compounds[componentLabel]= {objLabel}
        # Define belongsTo attribute for compounds components. 
        for key in self.propertyDict:
            pDict= self.propertyDict[key]
            for label in pDict.labels:
                if label in self.compounds:
                    compound= list(self.compounds[label])
                    if('belongsTo' in pDict.attributes):
                        pDict.attributes['belongsTo'].extend(compound)
                    else:
                        pDict.attributes['belongsTo']= compound
              
    def getNamesToImport(self):
        ''' Return the names of the objects to import.'''
        retval= list()
        for obj in self.groupsToImport:
            retval.append(obj.Label)
        return retval
