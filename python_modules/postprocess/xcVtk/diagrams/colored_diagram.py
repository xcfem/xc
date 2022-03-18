 # -*- coding: utf-8 -*-

''' Colored diagram of a function over a linear domain (set of 1D elments for example). '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import vtk
from postprocess.xcVtk.fields import vtk_lut_field

''' Define las variables que se emplean para definir
   un diagrama de esfuerzos. '''
class ColoredDiagram(vtk_lut_field.LUTField):
    ''' Colored diagram of a function over a linear domain (set of 1D 
        elements for example).

      :ivar scaleFactor: scale factor.
    '''
    def __init__(self,scaleFactor,fUnitConv):
        ''' Constructor.

        :param scaleFactor: scaleFactor.
        :param fUnitConv: unit conversion factor.
        '''
        super(ColoredDiagram,self).__init__(fUnitConv)
        self.vDir= [0,0,1]
        self.scaleFactor= scaleFactor
        self.diagram= None
        self.points= None
        self.scalars= None
        self.cells= None

    def creaEstrucDatosDiagrama(self):
      # Crea las estructuras de datos necesarias para crear el diagrama.
      self.initializeMinMax()
      self.points= vtk.vtkPoints()
      self.scalars= vtk.vtkDoubleArray()
      self.cells= vtk.vtkCellArray()

    def resetEstrucDatosDiagrama(self):
      #«Resetea» las estructuras de datos necesarias para crear el diagrama.
      self.initializeMinMax()
      self.points.reset()
      self.scalars.reset()
      self.cells.reset()


    def createConstantSignDiagramInterval(self, offset,org, valOrg, dest, valDest):
        ''' Creates a diagram interval where the function to draw 
            doesn't changes its sign.

           :param offset: Offset for the index of the values to insert.
           :param org: Back end of the 1D element.
           :param dest: Front end of the 1D element.
           :param valOrg: Value of the field at the back end.
           :param valDest: Value of the field at the front end.
        '''

        self.updateMinMax(valOrg)
        self.updateMinMax(valDest)
        org2= org+(valOrg*self.scaleFactor)*self.vDir
        dest2= dest+(valDest*self.scaleFactor)*self.vDir
        self.points.InsertPoint(offset,org.x,org.y,org.z)
        self.points.InsertPoint(offset+1,org2.x,org2.y,org2.z)
        self.points.InsertPoint(offset+2,dest2.x,dest2.y,dest2.z)
        self.points.InsertPoint(offset+3,dest.x,dest.y,dest.z)
        self.scalars.InsertValue(offset,valOrg)
        self.scalars.InsertValue(offset+1,valOrg)
        self.scalars.InsertValue(offset+2,valDest)
        self.scalars.InsertValue(offset+3,valDest)
        #self.cells.InsertNextCell([offset,offset+1,offset+2,offset+3])
        self.cells.InsertNextCell(4)
        self.cells.InsertCellPoint(offset)
        self.cells.InsertCellPoint(offset+1)
        self.cells.InsertCellPoint(offset+2)    
        self.cells.InsertCellPoint(offset+3)    
        return offset+4

    def getDiagramIntervalRoot(self, org, valOrg, dest, valDest):
        ''' Computes the position of the diagram root in the [org, dest]
            interval.

         :param org: Extremo dorsal del elemento lineal.
         :param dest: Extremo frontal del elemento lineal.
         :param valOrg: Valor del campo escalar en el extremo dorsal.
         :param valDest: Valor del campo escalar en el extremo frontal. '''

        assert(valOrg*valDest<=0.0)
        org_dest= dest-org
        longTramo= (org_dest).getModulus
        s0= 0.0
        if(abs(valOrg-valDest)>0):
            s0= valOrg/(valOrg-valDest)
        return(org+s0*org_dest)


    def createChangedSignDiagramInterval(self, offset, org, valOrg, dest, valDest):
      ''' Creates a diagram interval where the function to draw 
          changes its sign.

         :param offset: Offset for the index of the values to insert.
         :param org: Back end of the 1D element.
         :param dest: Front end of the 1D element.
         :param valOrg: Value of the field at the back end.
         :param valDest: Value of the field at the front end.
      '''
      self.updateMinMax(valOrg)
      self.updateMinMax(valDest)
      org2= org+(valOrg*self.scaleFactor)*self.vDir
      dest2= dest+(valDest*self.scaleFactor)*self.vDir
      ptoRaiz= self.getDiagramIntervalRoot(org,valOrg,dest,valDest)

      self.points.InsertPoint(offset,org.x,org.y,org.z)
      self.points.InsertPoint(offset+1,org2.x,org2.y,org2.z)
      self.points.InsertPoint(offset+2,ptoRaiz.x,ptoRaiz.y,ptoRaiz.z)
      self.points.InsertPoint(offset+3,dest.x,dest.y,dest.z)
      self.points.InsertPoint(offset+4,dest2.x,dest2.y,dest2.z)

      self.scalars.InsertValue(offset,valOrg)
      self.scalars.InsertValue(offset+1,valOrg)
      self.scalars.InsertValue(offset+2,0)
      self.scalars.InsertValue(offset+3,valDest)
      self.scalars.InsertValue(offset+4,valDest)

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


    def createDiagramInterval(self, offset,org, valOrg, dest, valDest):
      ''' Crea un tramo de diagrama.

         :param offset: index-counter for the values to insert.
         :param org: Extremo dorsal del elemento lineal.
         :param dest: Extremo frontal del elemento lineal.
         :param valOrg: Valor del campo escalar en el extremo dorsal.
         :param valDest: Valor del campo escalar en el extremo frontal.
      '''
      vOrg= valOrg*self.fUnitConv
      vDest= valDest*self.fUnitConv
      if(vOrg*vDest>0.0):
          return self.createConstantSignDiagramInterval(offset,org,vOrg,dest,vDest)
      else:
          return self.createChangedSignDiagramInterval(offset,org,vOrg,dest,vDest)

    def creaActorDiagrama(self):
      # Crea el actor para el diagrama.

      self.diagram= vtk.vtkPolyData()
      self.diagram.SetPoints(self.points)
      point_data= self.diagram.GetPointData()
      point_data.SetScalars(self.scalars)
      self.diagram.SetPolys(self.cells)

      self.mapper= vtk.vtkPolyDataMapper()
      self.mapper.SetInputData(self.diagram)
      self.mapper.SetScalarRange(self.valMin,self.valMax)
      self.mapper.SetLookupTable(self.lookUpTable)
      self.actor= vtk.vtkActor() 
      self.actor.SetMapper(self.mapper)


    def addDiagramToScene(self, recordDisplay,orientation=1,title=None):
      ''' Adds the diagram to de scene'''
      recordDisplay.renderer.AddActor(self.actor)
      self.creaColorScaleBar(orientation,title)
      recordDisplay.renderer.AddActor2D(self.scalarBar)

    def appendDataToDiagram(self, elem,indxDiagrama,v0,v1,defFScale=0.0):
      ''' Appends to the diagram the values being passed as parameter.

                     ___----* value2
         value1  *---       |
                 |          |
                 |          |
               1 +----------+ 2

         :param elem: element over which diagram is represented.
         :param indxDiagrama: index-counter for the values to insert.
         :param v0: value at the start node.
         :param v1: value at the end node.
         :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
      '''
      posNode0= elem.getNodes[0].getCurrentPos3d(defFScale)
      posNode1= elem.getNodes[1].getCurrentPos3d(defFScale)
      return self.createDiagramInterval(indxDiagrama,posNode0,v0,posNode1,v1)


    def appendDataFromElementEnds(self,vDir,elem,indxDiagram,value1,value2):
      '''Append to the diagram the data from the element ends as follows:

                     ___----* value2
         value1  *---       |
                 |          |
                 |          |
               1 +----------+ 2

         :param elem: element over which diagram is represented.
         :param indxDiagrama: index-counter for the values to insert.
         :param value1: value at the start node.
         :param value2: value at the end node. 
      '''
      self.vDir= vDir
      return self.appendDataToDiagram(elem,indxDiagram,value1,value2)
