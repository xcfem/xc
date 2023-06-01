# -*- coding: utf-8 -*-

from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom

class LoadRecord(object):
    ''' Load definition.

    :ivar loadCaseId: load case identifier.
    :ivar loadCaseName: load case name.
    :ivar loadName: name of the load.
    :ivar value: absolute value of the load.
    :ivar vDir: direction vector.
    '''
    def __init__(self, loadCase, bName= 'nil',v= 1.0):
        ''' Constructor.

        :param loadCase: load case.
        :param bName: name of the load.
        :param v: absolute value of the load.
        '''
        self.loadCaseId= loadCase.id
        self.loadCaseName= loadCase.name
        self.loadName= bName
        self.value= v
        self.vDir= [0,0,-1]
        
    def __str__(self):
        return 'id= ' + str(self.loadCaseId) + ' name= ' + self.loadCaseName + ' loadName= ' + str(self.loadName) + ' value= ' +  str(self.value) + ' vDir= ' + str(self.vDir)

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'loadCaseId':self.loadCaseId, 'loadCaseName':self.loadCaseName, 'loadName':self.loadName, 'value': self.value, 'vDir': self.vDir}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.loadCaseId= dct['loadCaseId']
        self.loadCaseName= dct['loadCaseName']
        self.loadName= dct['loadName']
        self.value= dct['value']
        self.vDir= dct['vDir']
   
class PointForceRecord(LoadRecord):
    '''Force applied in a point.

    :ivar pos: position of the point.
    '''
    def __init__(self, loadCase, bName, pos, v):
        ''' Constructor.

        :param loadCase: load case.
        :param bName: name of the load.
        :param pos: position of the point.
        :param v: absolute value of the load.
        '''
        super(PointForceRecord,self).__init__(loadCase, bName, v)
        self.pos= pos
        
    def __str__(self):
        retval= super(PointForceRecord,self).__str__()
        if(self.pos): retval+= ' ' + str(self.pos)
        return retval

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= super().getDict()
        retval.extend({'pos':self.pos})
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        self.pos= dct['pos']
        
class NodalLoadRecord(PointForceRecord):
    '''Load over a node or an element.

    :ivar tag: identifier of the node or the element.
    '''
    def __init__(self, loadCase, bName, pos, v):
        ''' Constructor.

        :param loadCase: load case.
        :param bName: name of the load.
        :param pos: position of the point.
        :param v: absolute value of the load.
        '''
        super(NodalLoadRecord,self).__init__(loadCase, bName, pos, v)
        self.tag= -1 #node tag.
      
    def __str__(self):
        retval= 'tag= ' + str(self.tag)
        retval+= ' ' + super(NodalLoadRecord,self).__str__()
        return retval
    
    def searchLoadedElement(self, elemSet):
        pos= geom.Pos3d(self.pos[0],self.pos[1],self.pos[2])
        e= elemSet.getNearestElement(pos)
        self.tag= e.tag
        
    def searchLoadedNode(self,elemSet):
        pos= geom.Pos3d(self.pos[0],self.pos[1],self.pos[2])
        n= elemSet.getNearestNode(pos)
        self.tag= n.tag

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= super().getDict()
        retval.extend({'tag':self.tag})
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        self.tag= dct['tag']

class ElementLoadRecord(LoadRecord):
    '''Load over an elemetn.

    :ivar tags: identifiers of the elements.
    '''
    def __init__(self, loadCase, bName= 'nil', v= 1.0,mode= 'nil'):
        ''' Constructor.

        :param loadCase: load case.
        :param bName: name of the load.
        :param v: absolute value of the load.
        :param mode: ??
        '''
        super(ElementLoadRecord,self).__init__(loadCase,bName,v)
        self.mode= mode
        self.tags= list() #Element tags
        
    def __str__(self):
        retval= super(ElementLoadRecord,self).__str__()
        retval+= ' ' + str(self.mode) + ' ' + str(self.tags)
        return retval

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= super().getDict()
        retval.extend({'mode': mode, 'tags':self.tags})
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        self.mode= dct['mode']
        self.tags= dct['tags']


class SurfaceLoadRecord(ElementLoadRecord):
    ''' Load over a surface.

    :ivar plg: polygon defining the contour of the surface.
    :ivar projPlane: projection plane.
    '''
    def __init__(self, loadCase, bName= 'nil', plg= None,v= 1.0,mode= 'nil'):
        ''' Constructor.

        :param loadCase: load case.
        :param bName: name of the load.
        :param plg: 
        :param v: absolute value of the load.
        :param mode: ??
        '''
        super(SurfaceLoadRecord,self).__init__(loadCase,bName,v)
        if(plg):
            self.setPolygon(plg)
        self.projPlane= "xy"
      
    def __str__(self):
        retval= super(SurfaceLoadRecord,self).__str__()
        retval+= ' ' + str(len(self.polygon)) + ' ' + str(self.polygon)
        return retval
  
    def get2DPolygon(self):
        retval= geom.Polygon2d()
        for p in self.polygon:
          coords= [p[0],p[1]]
          if(self.projPlane=='xz'):
            coords[1]= p[2]
          elif(self.projPlane=='yz'):
            coords=  [p[1],p[2]]
          retval.appendVertex(geom.Pos2d(coords[0],coords[1]))
        return retval
  
    def searchLoadedElements(self,elemSet):
        ''' Returns elements which have his center inside the polygon'''
        plg= self.get2DPolygon()
        self.tags= list()
        for e in elemSet.elements:
          pos= e.getPosCentroid(True)
          projPos= geom.Pos2d(pos.x,pos.y)
          if(self.projPlane=='xz'):
            projPos= geom.Pos2d(pos.x,pos.z)
          elif(self.projPlane=='yz'):
            projPos= geom.Pos2d(pos.y,pos.z)
          if plg.In(projPos,0.01):
            self.tags.append(e.tag)
        return self.tags
  
    def setPolygon(self, points):
        self.polygon= list()
        for p in points:
          self.polygon.append(p)
          
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= super().getDict()
        if(self.polygon):
            plg= self.polygon.getDict()
        else:
            plg= dict()
        retval.extend({'polygon': plg, 'projPlane':self.projPlane})
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        plg= dct['polygon']
        if(plg):
            self.polygon.setFromDict(plg)
        self.projPlane= dct['projPlane']


class LoadContainer(object):
    ''' Load container.

    :ivar name: name of the container.
    :ivar punctualLoads: punctual loads container.
    :ivar surfaceLoads: surface loads container.
    '''
    def __init__(self,n):
        ''' Constructor.

        :param n: container name.
        '''
        self.name= n
        self.punctualLoads= list()
        self.surfaceLoads= list()
        #self.elementSet= 'total'

    def __str__(self):
        retval= str(self.name)
        if(len(self.punctualLoads)>0):
            retval+= '\n  punctualLoads{' + str(len(self.punctualLoads)) + ' '
            for pl in self.punctualLoads:
                retval+= '<' + str(pl) + '> '
            retval+= '}'
        if(len(self.surfaceLoads)>0):
            retval+= '\n  surfaceLoads{'  + str(len(self.surfaceLoads)) + ' '
            for sl in self.surfaceLoads:
                retval+= '<' + str(sl) + '> '
            retval+= '}'
        return retval
    
    def getNumberOfLoads(self):
        return len(self.punctualLoads)+len(self.surfaceLoads)

    def empty(self):
        return (self.getNumberOfLoads()==0)

    def searchLoadedElements(self,elementSet):
        '''Get load distribution over elements taken from the set.'''
        if(len(self.punctualLoads)>0):
            for pl in self.punctualLoads:
                pl.searchLoadedElement(elementSet)
        if(len(self.surfaceLoads)>0):
            for sl in self.surfaceLoads:
                sl.searchLoadedElements(elementSet)
          
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        retval['name']= self.name
        pLoadsList= list()
        for pLoad in self.punctualLoads:
            pLoadsList.append(pLoad.getDict())
        retval['punctualLoads']= pLoadsList
        sLoadsList= list()
        for pLoad in self.surfaceLoads:
            sLoadsList.append(pLoad.getDict())
        retval['surfaceLoads']= sLoadsList
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.name= retval['name']
        pLoadsList= retval['punctualLoads']
        for pLoad in pLoadsList:
            newPunctualLoad= PointForceRecord(loadCase= '', bName= '', pos= None, v= None)
            newPunctualLoad.setFromDict(pLoad)
            self.punctualLoads.append(newPunctualLoad)
        sLoadsList= retval['surfaceLoads']
        for sLoad in sLoadsList:
            newSurfaceLoad= SurfaceLoadRecord(loadCase= '', bName= '', pos= None, v= None)
            newSurfaceLoad.setFromDict(sLoad)
            self.surfaceLoads.append(newSurfaceLoad)
    
class LoadGroup(object):
    ''' Loads which share some property (origin,...).

    :ivar id: load group identifier.
    :ivar desc: load group description.
    :ivar permanent: true if the load is permanent.
    '''
    def __init__(self,id, desc, permanent= False):
        self.id= id
        self.desc= desc #Group description.
        self.permanent= permanent

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'id':self.id, 'desc':self.desc, 'permanent': self.permanent}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.id= dct['id']
        self.desc= dct['desc']
        self.permanent= dct['permanent']
        
class LoadCase(object):
    ''' Load case.

    :ivar id: load case identifier.
    :ivar name: load case name.
    :ivar desc: load case description.
    :ivar loadGroupId: load group identifier.
    :ivar actionType: action type.
    :ivar ltyp: load type.
    :ivar loads: load container.
    '''
    def __init__(self,id, name, desc, lg, ltyp):
        ''' Constructor.

        :param id: load case identifier.
        :param name: load case name.
        :param desc: load case description.
        :param loadGroupId: load group identifier.
        :param actionType: action type.
        :param ltyp: load type.
        :param loads: load container.
        '''
        self.id= id
        self.name= name
        self.desc= desc #Load description.
        self.loadGroupId= lg
        self.actionType= "Permanent"
        self.ltyp= ltyp
        self.loads= LoadContainer(name)
        
    def __str__(self):
        return str(self.id) + ' name= ' + self.name + ' desc= ' + self.desc
    
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'id':self.id, 'name':self.name, 'desc':self.desc, 'loadGroupId':self.loadGroupId, 'actionType':self.actionType, 'ltyp':self.ltyp, 'loads':self.loads.getDict()}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.id= dct['id']
        self.name= dct['name']
        self.desc= dct['desc']
        self.loadGroupId= dct['loadGroupId']
        self.actionType= dct['actionType']
        self.ltyp= dct['ltyp']
        self.loads= LoadContainer(self.name)
        self.loads.setFromDict(dct['loads'])


class LoadCombComponent(object):
    ''' Component of a load combination.

    :ivar id: component identifier.
    :ivar loadCaseId: load case identifier.
    :ivar loadCaseName: load case name.
    :ivar coef: load multiplier.
    '''
    def __init__(self,id, loadCase, coef):
        ''' Constructor.

        :param id: component identifier.
        :param loadCaseId: load case identifier.
        :param loadCaseName: load case name.
        :param coef: load multiplier.
        '''
        self.id= id
        self.loadCaseId= loadCase.id
        self.loadCaseName= loadCase.name
        self.coef= coef #Multiplier for load i.
    
    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'id':self.id, 'loadCaseId':self.loadCaseId, 'loadCaseName':self.loadCaseName, 'coef': self.coef}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.id= dct['id']
        self.loadCaseId= dct['loadCaseId']
        self.loadCaseName= dct['loadCaseName']
        self.coef= dct['coef']
    
def getComponentsFromStr(descompStr, mapLoadCases):
    retval= list()
    components= descompStr.split('+')
    counter= 0
    for c in components:
        factors= c.split('*')
        coef= float(factors[0])
        loadCaseName= factors[1]
        loadCase= mapLoadCases[loadCaseName]
        #print("coef= ", coef, "loadCase= ", loadCaseName, " id= ", loadCase.id)
        retval.append(LoadCombComponent(counter,loadCase,coef))
        counter+= 1
    return retval
    

class LoadComb(object):
    ''' Load combination.

    :ivar id: combination identifier.
    :ivar name: combination name.
    :ivar desc: combination description.
    :ivar typ: combination type.
    :ivar descomp: expression of the combination 
    '''
    def __init__(self,id, name, desc, typ, descomp):
        ''' Constructor.

        :param id: combination identifier.
        :param name: combination name.
        :param desc: combination description.
        :param typ: combination type.
        :param descomp: expression of the combination 
        '''
        self.id= id
        self.name= name
        self.desc= desc #Comb description.
        self.typ= typ
        self.descomp= descomp

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        return {'id':self.id, 'name':self.name, 'desc':self.desc, 'typ': self.typ, 'descomp': self.descomp}
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.id= dct['id']
        self.name= dct['name']
        self.desc= dct['desc']
        self.typ= dct['typ']
        self.descomp= dct['descomp']

        
class LoadData(object):
    '''Container for load groups, cases and combinations.

    :ivar loadGroups: load group container.
    :ivar loadCases: load cases container.
    :ivar loadCombs: load combination container.
    '''
    def __init__(self):
        ''' Constructor.

        :param loadGroups: load group container.
        :param loadCases: load cases container.
        :param loadCombs: load combination container.
        '''
        self.loadGroups= dict()
        self.loadCases= dict()
        self.loadCombs= dict()
        
    def readLoadCombsFromXC(self,combContainer,mapLoadCases):
        self.loadCombs= combContainer.getNeutralFormat(mapLoadCases)

    def getDict(self):
        ''' Return the object members in a Python dictionary.'''
        retval= dict()
        tmp= dict()
        for key in self.loadGroups:
            lg= self.loadGroups[key]
            tmp[key]= lg.getDict()
        retval['loadGroups']= tmp()
        tmp= dict()
        for key in self.loadCases:
            lc= self.loadCases[key]
            tmp[key]= lc.getDict()
        retval['loadCases']= tmp()
        tmp= dict()
        for key in self.loadCombs:
            lc= self.loadCombs[key]
            tmp[key]= lc.getDict()
        retval['loadCombs']= tmp()
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        tmp= dct['loadGroups']
        for key in tmp:
            lg= LoadGroup(id= -1, desc= '')
            lg.setFromDict(tmp[key])
            self.loadGroups[key]= lg
        tmp= dct['loadCases']
        for key in tmp:
            lc= LoadCase(id= -1, name= '', desc= '', lg= '', ltyp= '')
            lc.setFromDict(tmp[key])
            self.loadCases[key]= lc
        tmp= dct['loadCombs']
        for key in tmp:
            lc= LoadComb(id= -1, name= '', desc= '', typ= '', descomp= '')
            lc.setFromDict(tmp[key])
            self.loadCombs[key]= lc
        

