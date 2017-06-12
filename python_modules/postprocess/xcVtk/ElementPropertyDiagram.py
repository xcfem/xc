 # -*- coding: utf-8 -*-

''' Diagram display a property defined over linear elements. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


from miscUtils import LogMessages as lmsg
import vtk
from postprocess.xcVtk import ColoredDiagram as cd
from postprocess import extrapolate_elem_attr




class ElementPropertyDiagram(cd.ColoredDiagram):
  '''Diagram of element properties'''
  envelopes= set(['N+','N-','My+','My-','Mz+','Mz-','Vy+','Vy-','Vz+','Vz-','T+','T-'])

  def __init__(self,scaleFactor,fUnitConv,sets,propertyName):
    '''Diagram that represents a property value over several sets of elements.

       :param scaleFactor: scale factor (size of the diagram).
       :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
       :param propertyName: name of the element's property to represent.
       :param sets: represent the field over those element sets.
    '''
    super(ElementPropertyDiagram,self).__init__(scaleFactor,fUnitConv)
    self.conjuntos= sets
    self.propertyName= propertyName

  def appendDataToDiagram(self, eSet,indxDiagrama,defFScale=0.0):
    ''' Append property values to diagram .
       :param  eSet: Element set.
       :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
    '''
    elems= eSet.getElements
    if(self.propertyName=='FCTNCP'):
      for e in elems:
        self.vDir= e.getJVector3d(True) #initialGeometry= True
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='FCVCP'):
      for e in elems:
        self.vDir= e.getJVector3d(True) #initialGeometry= True
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='N+'):
      for e in elems:
        self.vDir= e.getJVector3d(True) #initialGeometry= True
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='N-'):
      for e in elems:
        self.vDir= e.getJVector3d(True) #initialGeometry= True
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='My+'):
      for e in elems:
        self.vDir= e.elem.getKVector3d(True) # initialGeometry= True  
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='My-'):
      for e in elems:
        self.vDir= e.elem.getKVector3d(True) # initialGeometry= True  
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='Mz+'):
      for e in elems:
        self.vDir= e.elem.getJVector3d(True) # initialGeometry= True  
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='Mz-'):
      for e in elems:
        self.vDir= e.elem.getJVector3d(True) # initialGeometry= True  
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='Vy+'):
      for e in elems:
        self.vDir= e.elem.getJVector3d(True) # initialGeometry= True  
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='Vy-'):
      for e in elems:
        self.vDir= e.elem.getJVector3d(True) # initialGeometry= True  
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='Vz+'):
      for e in elems:
        self.vDir= e.elem.getKVector3d(True) # initialGeometry= True  
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    elif(self.propertyName=='Vz-'):
      for e in elems:
        self.vDir= e.elem.getKVector3d(True) # initialGeometry= True  
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],defFScale)      
    else:
      extrapolate_elem_attr.extrapolate_elem_function_attr(elems,self.propertyName,"getProp", self.propertyName)
      for e in elems:
        self.vDir= e.getJVector3d(True) #initialGeometry= True
        v0= e.getNodes[0].getProp(self.propertyName)
        v1= e.getNodes[1].getProp(self.propertyName)
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,v0,v1,defFScale)

  def addDiagram(self):
    self.creaEstrucDatosDiagrama()
    self.creaLookUpTable()
    self.creaActorDiagrama()

    indxDiagrama= 0
    indiceSet= 0
    numSetsDiagrama= len(self.conjuntos)
    for s in self.conjuntos:
      self.appendDataToDiagram(s,indxDiagrama)

    self.updateLookUpTable()
    self.updateActorDiagrama()

