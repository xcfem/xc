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
import reader_base
from scipy.spatial.distance import cdist

# path to FreeCAD.so
FREECADPATH = '/usr/lib/freecad-daily-python2/lib/' 
if(sys.version_info.major == 3):
    FREECADPATH = '/usr/lib/freecad-daily-python3/lib/'
sys.path.append(FREECADPATH)

# import FreeCAD
import FreeCAD

class FreeCADImport(reader_base.ReaderBase):
    '''Import FreeCAD geometric entities.

     :ivar groupsToImport: list of regular expressions to be tested.
    '''
    def __init__(self,fileName,groupsToImport, getRelativeCoo, threshold= 0.01,importLines= True, importSurfaces= True):
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
        #     if(self.impSurfaces):
        #         self.importFaces()
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
            lmsg.warning('No layers to import (names to import: '+str(namesToImport)+')')
        return retval
    
    def extractPoints(self):
        '''Extract the points from the entities argument.'''
        retval_pos= []
        retval_labels= []
        def append_point(pt, groupName, pointName, objLabels):
            '''Append the point to the lists.'''
            retval_pos.append(self.getRelativeCoo(pt))
            # group name as label.
            ptLabels= [groupName]
            # xdata as label.
            for l in objLabels:
                ptLabels.append(l)
            retval_labels.append((pointName, ptLabels))
        count= 0
        for grpName in self.groupsToImport:
            grp= self.document.getObjectsByLabel(grpName)[0]
            for obj in grp.OutList:
                type= obj.Shape.ShapeType
                objName= obj.Name
                objLabels= [obj.Label]
                if(type=='Face'):
                    count= 0
                    for v in obj.Shape.Vertexes:
                        pointName= objName+'.'+str(count)
                        append_point([v.X, v.Y, v.Z], grpName, pointName, objLabels)
                        count+= 1        
        return retval_pos, retval_labels
    
    def importPoints(self):
        ''' Import points from FreeCAD.'''
        self.points= dict()
        for obj in self.document.Objects:
            if(hasattr(obj,'Shape')):
                objType= obj.Shape.ShapeType
                pointName= obj.Name
                labelName= obj.Label
                if((objType=='Vertex') and (labelName in self.groupsToImport)):
                    vertices= [-1]
                    p= self.getRelativeCoo([float(obj.X), float(obj.Y), float(obj.Z)])
                    vertices[0]= self.getIndexNearestPoint(p)
                    self.points[pointName]= vertices
                    self.labelDict[pointName]= [labelName]
                    
    def importLines(self):
        ''' Import lines from DXF.'''
        self.lines= {}
        for obj in self.document.Objects:
            if(hasattr(obj,'Shape')):
                objType= obj.Shape.ShapeType
                lineName= obj.Name
                labelName= obj.Label
                if((objType=='Wire') and (labelName in self.groupsToImport)):
                    vertices= [-1,-1]
                    v0= obj.Shape.Vertexes[0]
                    v1= obj.Shape.Vertexes[1]
                    p1= self.getRelativeCoo([float(v0.X), float(v0.Y), float(v0.Z)])
                    p2= self.getRelativeCoo([float(v1.X), float(v1.Y), float(v1.Z)])
                    length= cdist([p1],[p2])[0][0]
                    print('length= ', length)
                    # Try to have all lines with the
                    # same orientation.
                    idx0, idx1= self.getOrientation(p1, p2, length/1e4)
                    # end orientation.
                    vertices[0]= idx0
                    vertices[1]= idx1
                    if(vertices[0]==vertices[1]):
                        lmsg.error('Error in line '+lineName+' vertices are equal: '+str(vertices))
                    if(length>self.threshold):
                        self.lines[lineName]= vertices
                        objLabels= [labelName]
                        # # groups
                        # if(lineName in self.entitiesGroups):
                        #     objLabels.extend(self.entitiesGroups[lineName])
                        self.labelDict[lineName]= objLabels
                    else:
                        lmsg.error('line too short: '+str(p1)+','+str(p2)+str(length))
                            
    def getNamesToImport(self):
        ''' Return the names of the objects to import.'''
        return self.groupsToImport
