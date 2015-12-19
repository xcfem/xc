 # -*- coding: utf-8 -*-

''' Representación de diagramas de esfuerzos (o de otras magnitudes)
   sobre elementos lineales. '''

import geom
import vtk
from xcVtk import vtk_lut_field

''' Define las variables que se emplean para definir
   un diagrama de esfuerzos. '''
class RecordDefDiagramaEsf(vtk_lut_field.LUTField):
  diagram= None
  points= None
  escalares= None
  cells= None
  vDir= [0,0,1] #Vector dirección para el diagrama.
  escala= 0.01 #Escala que se emplea para el diagrama.
  conjuntos= None # Nombres de los conjuntos de elementos cuyos esfuerzos se van a representar.
  nmbRutinaDiagrama= None #Rutina que se emplea para generar el diagrama.

  def __init__(self,scale,fUnitConv,sets,component):
    super(RecordDefDiagramaEsf,self).__init__(fUnitConv)
    self.vDir= [0,0,1]
    self.escala= scale
    self.conjuntos= sets
    if(component == "N"):
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaAxiles
    elif(component == "Qy"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaVY
    elif(component == "Qz"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaVZ
    elif(component == "T"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaT
    elif(component == "My"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaMomEjeY
    elif(component == "Mz"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaMomEjeZ
    else:
      print "component :'", component, "' unknown."

    self.diagram= None
    self.points= None
    self.escalares= None
    self.cells= None

  def getVDirAsVector3D(self):
    retval= geom.Vector3d(0.0,0.0,1.0)
    if(len(self.vDir)>2):
      retval= geom.Vector3d(self.vDir[0],self.vDir[1],self.vDir[2])
    else:
      retval= geom.Vector3d(self.vDir[0],self.vDir[1],0.0)
    return retval

  def creaEstrucDatosDiagrama(self):
    # Crea las estructuras de datos necesarias para crear el diagrama.

    self.points= vtk.vtkPoints()
    self.escalares= vtk.vtkDoubleArray()
    self.cells= vtk.vtkCellArray()

  def resetEstrucDatosDiagrama(self):
    #«Resetea» las estructuras de datos necesarias para crear el diagrama.
    self.points.reset()
    self.escalares.reset()
    self.cells.reset()


  def creaTramoDiagramaSignoCte(self, offset,org, valOrg, dest, valDest):
    ''' Crea un tramo de diagrama en el que la función a representar
       no cambia de signo.
       offset: Offset para el índice de los valores a insertar.
       org: Extremo dorsal del elemento lineal.
       dest: Extremo frontal del elemento lineal.
       valOrg: Valor del campo escalar en el extremo dorsal.
       valDest: Valor del campo escalar en el extremo frontal.'''

    self.updateMinMax(valOrg)
    self.updateMinMax(valDest)
    v3d= self.getVDirAsVector3D()
    org2= org+(valOrg*self.escala)*v3d
    dest2= dest+(valDest*self.escala)*v3d
    self.points.InsertPoint(offset,org.x,org.y,org.z)
    self.points.InsertPoint(offset+1,org2.x,org2.y,org2.z)
    self.points.InsertPoint(offset+2,dest2.x,dest2.y,dest2.z)
    self.points.InsertPoint(offset+3,dest.x,dest.y,dest.z)
    self.escalares.InsertValue(offset,valOrg)
    self.escalares.InsertValue(offset+1,valOrg)
    self.escalares.InsertValue(offset+2,valDest)
    self.escalares.InsertValue(offset+3,valDest)
    #self.cells.InsertNextCell([offset,offset+1,offset+2,offset+3])
    self.cells.InsertNextCell(4)
    self.cells.InsertCellPoint(offset)
    self.cells.InsertCellPoint(offset+1)
    self.cells.InsertCellPoint(offset+2)    
    self.cells.InsertCellPoint(offset+3)    
    return offset+4

  def getRaizTramoDiagrama(self, org, valOrg, dest, valDest):
    ''' Calcula la posición de la raíz del diagrama que se define
       mediante los siguientes parámetros:
      org: Extremo dorsal del elemento lineal.
     dest: Extremo frontal del elemento lineal.
     valOrg: Valor del campo escalar en el extremo dorsal.
     valDest: Valor del campo escalar en el extremo frontal. '''

    assert(valOrg*valDest<=0.0)
    org_dest= dest-org
    longTramo= (org_dest).getModulo
    s0= 0.0
    if(abs(valOrg-valDest)>0):
      s0= valOrg/(valOrg-valDest)
    return(org+s0*org_dest)



  def creaTramoDiagramaCambioSigno(self, offset, org, valOrg, dest, valDest):
    ''' Crea un tramo de diagrama en el que la función a representar
     cambia de signo.
     offset: Offset para el índice de los valores a insertar.
     org: Extremo dorsal del elemento lineal.
     dest: Extremo frontal del elemento lineal.
     valOrg: Valor del campo escalar en el extremo dorsal.
     valDest: Valor del campo escalar en el extremo frontal.'''

    self.updateMinMax(valOrg)
    self.updateMinMax(valDest)
    v3d= self.getVDirAsVector3D()
    org2= org+(valOrg*self.escala)*v3d
    dest2= dest+(valDest*self.escala)*v3d
    ptoRaiz= self.getRaizTramoDiagrama(org,valOrg,dest,valDest)
        
    self.points.InsertPoint(offset,org.x,org.y,org.z)
    self.points.InsertPoint(offset+1,org2.x,org2.y,org2.z)
    self.points.InsertPoint(offset+2,ptoRaiz.x,ptoRaiz.y,ptoRaiz.z)
    self.points.InsertPoint(offset+3,dest.x,dest.y,dest.z)
    self.points.InsertPoint(offset+4,dest2.x,dest2.y,dest2.z)

    self.escalares.InsertValue(offset,valOrg)
    self.escalares.InsertValue(offset+1,valOrg)
    self.escalares.InsertValue(offset+2,0)
    self.escalares.InsertValue(offset+3,valDest)
    self.escalares.InsertValue(offset+4,valDest)

    #self.cells.InsertNextCell([offset,offset+2,offset+1])
    self.cells.InsertNextCell(3)
    self.cells.InsertCellPoint(offset)
    self.cells.InsertCellPoint(offset+2)
    self.cells.InsertCellPoint(offset+1)
    #self.cells.InsertNextCell([offset+2,offset+3,offset+4])
    self.cells.InsertNextCell(3)
    self.cells.InsertCellPoint(offset+2)
    self.cells.InsertCellPoint(offset+3)
    self.cells.InsertCellPoint(offset+4)

    return offset+5


  def creaTramoDiagrama(self, offset,org, valOrg, dest, valDest):
    ''' Crea un tramo de diagrama.
       offset: Offset para el índice de los valores a insertar.
       org: Extremo dorsal del elemento lineal.
       dest: Extremo frontal del elemento lineal.
       valOrg: Valor del campo escalar en el extremo dorsal.
       valDest: Valor del campo escalar en el extremo frontal.'''
    vOrg= valOrg*self.fConvUnidades
    vDest= valDest*self.fConvUnidades

    if(vOrg*vDest>0.0):
      return self.creaTramoDiagramaSignoCte(offset,org,vOrg,dest,vDest)
    else:
      return self.creaTramoDiagramaCambioSigno(offset,org,vOrg,dest,vDest)



  def creaActorDiagrama(self):
    # Crea el actor para el diagrama.

    self.diagram= vtk.vtkPolyData()
    self.diagram.SetPoints(self.points)
    point_data= self.diagram.GetPointData()
    point_data.SetScalars(self.escalares)
    self.diagram.SetPolys(self.cells)

    self.mapper= vtk.vtkPolyDataMapper()
    self.mapper.SetInput(self.diagram)
    self.mapper.SetScalarRange(self.valMin,self.valMax)
    self.mapper.SetLookupTable(self.lookUpTable)

    self.actor= vtk.vtkActor() 
    self.actor.SetMapper(self.mapper)


  def agregaDiagramaAEscena(self, recordDisplay):
    # Agrega el diagrama al display.
    recordDisplay.renderer.AddActor(self.actor)
    self.creaEscalaColores()
    recordDisplay.renderer.AddActor2D(self.scalarBar)

  def agregaDatosADiagrama(self, elem,indxDiagrama,v0,v1):
    # Agrega a el diagrama los valores para el elemento que se pasa como parámetro.
      posNodo0= elem.getNodes[0].getInitialPos3d
      posNodo1= elem.getNodes[1].getInitialPos3d
      return self.creaTramoDiagrama(indxDiagrama,posNodo0,v0,posNodo1,v1)

  def agregaDatosADiagramaAxiles(self, eSet,indxDiagrama):
    # Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getVDirEjeDebilGlobales
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getN1,e.getN2)

  def agregaDatosADiagramaMomEjeY(self, eSet,indxDiagrama):
    #Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getCoordTransf.getKVector 
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getMy1,-e.getMy2)

  def agregaDatosADiagramaMomEjeZ(self, eSet,indxDiagrama):
    # Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getCoordTransf.getJVector 
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,-e.getMz1,e.getMz2)

  def agregaDatosADiagramaVY(self, eSet,indxDiagrama):
    # Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getCoordTransf.getJVector 
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getVy1,-e.getVy2)

  def agregaDatosADiagramaVZ(self, eSet,indxDiagrama):
    #Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getCoordTransf.getKVector 
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getVz1,-e.getVz2)


  def agregaDatosADiagramaT(self, eSet,indxDiagrama):
    #Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getVDirEjeDebilGlobales
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getT1,-e.getT2)

  def agregaDiagrama(self):
    self.creaEstrucDatosDiagrama()
    self.creaLookUpTable()
    self.creaActorDiagrama()

    indxDiagrama= 0
    indiceSet= 0
    numSetsDiagrama= len(self.conjuntos)
    for s in self.conjuntos:
      self.nmbRutinaDiagrama(s,indxDiagrama)

    self.updateLookUpTable()
    self.updateActorDiagrama()

