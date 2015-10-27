# -*- coding: utf-8 -*-
# Representación de un modelo de elementos finitos.

import sys
import vtk
import xc_base
from vtkUtils import utilsVtk
from xcVtk import vtk_grafico_base
from xcVtk.malla_ef import vtk_define_malla_nodos
from xcVtk.malla_ef import vtk_define_malla_elementos
from xcVtk.malla_ef import vtk_define_malla_cells_ef
from xcVtk import vtk_lut_field

class ScalarField(vtk_lut_field.LUTField):
  name= "nil" #Field name.
  attrName= None #Attribute name to evaluate at nodes
  attrComponent= -1 #Index of the componenet to evaluate at nodes
  arr= None #Array of values.

  def __init__(self,name,vExpr,component,fUnitConv):
    super(ScalarField,self).__init__(fUnitConv)
    self.name= name
    self.attrName= vExpr
    self.attrComponent= component
    self.arr= None

  def fillArray(self, nodeSet):
    # Scalar values.
    self.arr= vtk.vtkDoubleArray()
    self.arr.SetName(self.name)
    self.arr.SetNumberOfTuples(len(nodeSet))
    self.arr.SetNumberOfComponents(1)
    for n in nodeSet:
      attr= getattr(n,self.attrName)
      tmp= None
      if hasattr(attr,"__getitem__"):
        tmp= attr[self.attrComponent]
      elif callable(attr):
        if(attr.__name__!='getProp'):
          tmp= attr(self.name)
        elif(n.hasProp(self.name)):
          tmp= attr(self.name)
        else:
          tmp= 0.0
      else:
        tmp= attr
      tmp*= self.fConvUnidades
      self.updateMinMax(tmp)
      self.arr.SetTuple1(n.getIdx,tmp)
    return self.arr


class RecordDefDisplayEF(vtk_grafico_base.RecordDefDisplay):
  ''' Define las variables que se emplean para definir
     el dispositivo de salida. '''
  nodos= None
  gridMapper= None

  def VtkDefineActorElementos(self,uGrid, tipoRepr,field):
    # Creamos el actor para las superficies.
    if(field):
      uGrid.GetPointData().SetScalars(field.arr)
      uGrid.GetPointData().SetActiveScalars(field.name)
      uGrid.Modified()
    self.gridMapper= vtk.vtkDataSetMapper()
    self.gridMapper.SetInput(uGrid)
    if(field):
      self.gridMapper.SetScalarRange(field.valMin,field.valMax)
      self.gridMapper.SetLookupTable(field.lookUpTable)
      self.gridMapper.SetScalarModeToUsePointData()
      self.gridMapper.ScalarVisibilityOn()
    elemActor= vtk.vtkActor()
    elemActor.SetMapper(self.gridMapper)
    elemActor.GetProperty().SetColor(1,1,0)

    if(tipoRepr=="points"):
      elemActor.GetProperty().SetRepresentationToPoints()
    elif(tipoRepr=="wireframe"):
      elemActor.GetProperty().SetRepresentationToWireFrame()
    elif(tipoRepr=="surface"):
      elemActor.GetProperty().SetRepresentationToSurface()
    else:
      print "Representation type: '", tipoRepr, "' unknown."
    self.renderer.AddActor(elemActor)
    if(field):
      field.creaEscalaColores()
      self.renderer.AddActor2D(field.scalarBar)

  # Define el actor a emplear para dibujar nodos.
  def defineActorNode(self,uGrid, radio):
    sphereSource= vtk.vtkSphereSource()
    sphereSource.SetRadius(radio)
    sphereSource.SetThetaResolution(5)
    sphereSource.SetPhiResolution(5)
    
    markNodos= vtk.vtkGlyph3D()
    markNodos.SetInput(uGrid)
    markNodos.SetSource(sphereSource.GetOutput())
    markNodos.ScalingOff()
    markNodos.OrientOff()
    
    mappNodos= vtk.vtkPolyDataMapper()
    mappNodos.SetInput(markNodos.GetOutput())
    visNodos= vtk.vtkActor()
    visNodos.SetMapper(mappNodos)
    visNodos.GetProperty().SetColor(.7, .5, .5)
    self.renderer.AddActor(visNodos)

  def VtkCargaMallaElem(self,preprocessor,recordGrid,field):
    # Definimos grid
    self.nodos= vtk.vtkPoints()
    recordGrid.uGrid= vtk.vtkUnstructuredGrid()
    recordGrid.uGrid.SetPoints(self.nodos)
    eSet= preprocessor.getSets.getSet(recordGrid.nmbSet)
    eSet.numerate()
    # Scalar values.
    nodeSet= eSet.getNodes
    if(field):
      arr= field.fillArray(nodeSet)
      field.creaLookUpTable()      
    # Cargamos los nodos en vtk
    setNodos= eSet.getNodes
    if(recordGrid.dispScale==0.0):
      for n in setNodos:
        pos= n.getInitialPos3d
        self.nodos.InsertPoint(n.getIdx,pos.x,pos.y,pos.z)
    else:
      posNodo= xc.Vector([0,0,0])
      for n in setNodos:
        posNodo= n.get3dCoo+recordGrid.dispScale*n.getDispXYZ
        self.nodos.insertPoint(n.getIdx,posNodo[0],posNodo[1],posNodo[2])
    # Cargamos los elementos en vtk
    setElems= eSet.getElements
    for e in setElems:
      vertices= xc_base.vector_int_to_py_list(e.getIdxNodes)
      vtx= vtk.vtkIdList()
      for vIndex in vertices:
        vtx.InsertNextId(vIndex)
      if(e.getVtkCellType!= vtk.VTK_VERTEX):
        recordGrid.uGrid.InsertNextCell(e.getVtkCellType,vtx)
 
    #Cargamos las condiciones de contorno en vtk XXX FALLA
    setConstraints= eSet.getConstraints
    for c in setConstraints:
      vtx= vtk.vtkIdList()
      vtx.InsertNextId(c.getNodeIdx)
      if(c.getVtkCellType!="line2"):
         recordGrid.uGrid.InsertNextCell(c.getVtkCellType,vtx)

  def defineEscenaMalla(self, preprocessor,recordGrid,field):
    # Define la escena de la malla en el dispositivo de salida.
    self.VtkCargaMallaElem(preprocessor,recordGrid,field)
    self.renderer= vtk.vtkRenderer()
    self.renderer.SetBackground(self.bgRComp,self.bgGComp,self.bgBComp)
    #self.defineActorNode(recordGrid.uGrid,0.02)
    self.VtkDefineActorElementos(recordGrid.uGrid,"surface",field)
    self.renderer.ResetCamera()

    #Implementar dibujo de etiquetas.
    # if(recordGrid.entToLabel=="elementos"):
    #   VtkDibujaIdsElementos(recordGrid,self.renderer)
    # elif(recordGrid.entToLabel=="nodos"):
    #   vtk_define_malla_nodos.VtkDibujaIdsNodos(recordGrid,self.renderer)
    # else:
    #   print "Entity: ", recordGrid.entToLabel, " unknown."
  def grafico_mef(self,preprocessor,nmbSet):
    defGrid= vtk_grafico_base.RecordDefGrid()
    defGrid.nmbSet= nmbSet
    self.muestraMalla(preprocessor,defGrid)

  def displayScalarField(self, preprocessor, nmbSet, field):
    defGrid= vtk_grafico_base.RecordDefGrid()
    defGrid.nmbSet= nmbSet
    self.defineEscenaMalla(preprocessor,defGrid,field)
    self.muestraEscena()

  def plotScalarField(self, preprocessor, nmbSet, field, fName):
    defGrid= vtk_grafico_base.RecordDefGrid()
    defGrid.nmbSet= nmbSet
    self.defineEscenaMalla(preprocessor,defGrid,field)
    self.plotScene(fName)

  def displayNodalLoad(self, nod, color, carga, momento, fEscala):
    #actorName= baseName+"%04d".format(nod.tag) # Tag nodo.

    pos= nod.getCurrentPos3d
    absCarga= carga.Norm()
    if(absCarga>1e-6):
      utilsVtk.dibujaFlecha(self.renderer,color,pos,carga,fEscala*absCarga)

    absMomento= momento.Norm()
    if(absMomento>1e-6):
      utilsVtk.dibujaFlechaDoble(self.renderer,color,pos,momento,fEscala*absMomento)

  def displayNodalLoads(self, preprocessor, loadPattern, color, fEscala):
    loadPattern.addToDomain()
    loadPatternName= loadPattern.getProp("dispName")
    lIter= loadPattern.getNodalLoadIter
    load= lIter.next()
    while not(load is None):
      actorName= "flecha"+loadPatternName+"%04d".format(load.tag) # Tag carga.
      nodeTag= load.getNodeTag
      node= preprocessor.getNodeLoader.getNode(nodeTag)
      carga= load.getForce
      momento= load.getMoment
      self.displayNodalLoad(node, color,carga,momento,fEscala)    
      load= lIter.next()
    loadPattern.removeFromDomain()


  def displayElementPunctualLoad(self, preprocessor, pLoad,loadPattern, renderer, color, carga, fEscala):
    xCarga= pLoad.getElems()
    eleTags= pLoad.elementTags
    loadPatternName= loadPattern.getProp("dispName")
    actorName= "flechaP"+loadPatternName+"%04d".format(tag) # Tag carga.
    for tag in eleTags:
      ele= preprocessor.getElementLoader.getElement(tag)
      actorName+= "%04d".format(tag) # Tag elemento.
      pos= ele.punto(xCarga)
      utilsVtk.dibujaFlecha(self.renderer,color,pos,carga,fEscala)()

  def displayElementUniformLoad(self, preprocessor, unifLoad,loadPattern, color, carga, fEscala):
    loadPatternName= loadPattern.getProp("dispName")
    actorName= "flechaU"+loadPatternName+"%04d".format(unifLoad.tag) # Tag carga.
    eleTags= unifLoad.elementTags
    for tag in eleTags:
      ele= preprocessor.getElementLoader.getElement(tag)
      actorName+= "%04d".format(tag) # Tag elemento.
      print "displayElementUniformLoad not implemented."
      # puntos= ele.getPoints(3,1,1,True)
      # i= 0
      # for capa in puntos:
      #   for pos in capa: 
      #     print pos
      #     utilsVtk.dibujaFlecha(self.renderer,color,pos,carga,fEscala*carga.Norm())
      #     i+= 1

  def displayElementalLoads(self, preprocessor,loadPattern, color, fEscala):
    loadPattern.addToDomain()
    eleLoadIter= loadPattern.getElementalLoadIter
    eleLoad= eleLoadIter.next()
    print "displayElementalLoads not implemented."
    # while not(eleLoad is None):
    #   carga= eleLoad.getGlobalForces()
    #   categoria= eleLoad.category
    #   if(categoria=="uniforme"):
    #     self.displayElementUniformLoad(preprocessor, eleLoad,loadPattern,color,carga,fEscala)
    #   else:
    #     self.displayElementPunctualLoad(preprocessor, eleLoad,loadPattern,color,carga,fEscala)
    # loadPattern.removeFromDomain()

  def displayLoads(self, preprocessor, loadPattern):
    clrVectores= loadPattern.getProp("color")
    fEscalaVectores= loadPattern.getProp("scale")
    self.displayElementalLoads(preprocessor, loadPattern,clrVectores,fEscalaVectores)
    self.displayNodalLoads(preprocessor, loadPattern,clrVectores,fEscalaVectores)
