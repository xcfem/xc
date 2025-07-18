 # -*- coding: utf-8 -*-

''' Colored diagram of a function over a linear domain (set of 1D elments for example). '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from postprocess.xcVtk.fields import vtk_lut_field
from operator import itemgetter
from vtk.vtkCommonCore import (
    vtkDoubleArray,
    vtkPoints
    )
from vtk.vtkCommonDataModel import (
    vtkPolyData,
    vtkCellArray
  )
from vtk.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper
)

class ColoredDiagram(vtk_lut_field.LUTField):
    ''' Colored diagram of a function over a linear domain (set of 1D 
        elements for example).

      :ivar scaleFactor: scale factor.
      :ivar rgMinMax: range (vmin,vmax) with the maximum and minimum values of 
                the field to be represented. All the values less than vmin are 
                displayed in blue and those greater than vmax iqn red
                (defaults to None)
    '''
    def __init__(self, scaleFactor, fUnitConv, rgMinMax= None):
        ''' Constructor.

        :param scaleFactor: scaleFactor.
        :param fUnitConv: unit conversion factor.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(ColoredDiagram,self).__init__(fUnitConv)
        self.scaleFactor= scaleFactor
        self.diagram= None
        self.points= None
        self.scalars= None
        self.cells= None
        if(rgMinMax):
            self.rgMinMax= [1.0,1.0]
            # Convert the range units.
            self.rgMinMax[0]= rgMinMax[0]*fUnitConv
            self.rgMinMax[1]= rgMinMax[1]*fUnitConv
        else:
            self.rgMinMax= None

    def createDiagramDataStructure(self):
        # Crea las estructuras de datos necesarias para crear el diagrama.
        self.initializeMinMax()
        self.points= vtkPoints()
        self.scalars= vtkDoubleArray()
        self.cells= vtkCellArray()

    def resetEstrucDatosDiagrama(self):
        #«Resetea» las estructuras de datos necesarias para crear el diagrama.
        self.initializeMinMax()
        self.points.reset()
        self.scalars.reset()
        self.cells.reset()

    def createConstantSignDiagramInterval(self, offset, org, valOrg, dest, valDest, dirVector):
        ''' Creates a diagram interval where the function to draw 
            doesn't changes its sign.

        :param offset: Offset for the index of the values to insert.
        :param org: Back end of the 1D element.
        :param dest: Front end of the 1D element.
        :param valOrg: Value of the field at the back end.
        :param valDest: Value of the field at the front end.
        :param dirVector: direction vector of the diagram.
        '''

        org2= org+(valOrg*self.scaleFactor)*dirVector
        dest2= dest+(valDest*self.scaleFactor)*dirVector
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
        org_dest= dest-org # org->dest vector.
        s0= 0.0
        if(abs(valOrg-valDest)>0):
            s0= valOrg/(valOrg-valDest)
        return (org+s0*org_dest)


    def createChangedSignDiagramInterval(self, offset, org, valOrg, dest, valDest, dirVector):
        ''' Creates a diagram interval where the function to draw 
            changes its sign.

        :param offset: Offset for the index of the values to insert.
        :param org: Back end of the 1D element.
        :param dest: Front end of the 1D element.
        :param valOrg: Value of the field at the back end.
        :param valDest: Value of the field at the front end.
        :param dirVector: direction vector of the diagram.
        '''
        org2= org+(valOrg*self.scaleFactor)*dirVector
        dest2= dest+(valDest*self.scaleFactor)*dirVector
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

    def createDiagramInterval(self, offset,org, valOrg, dest, valDest, dirVector):
        ''' Crea un tramo de diagrama.

        :param offset: index-counter for the values to insert.
        :param org: Extremo dorsal del elemento lineal.
        :param dest: Extremo frontal del elemento lineal.
        :param valOrg: Valor del campo escalar en el extremo dorsal.
        :param valDest: Valor del campo escalar en el extremo frontal.
        :param dirVector: direction vector of the diagram.
        '''
        vOrg= valOrg*self.fUnitConv
        vDest= valDest*self.fUnitConv
        if(self.rgMinMax):
            self.updateMinMaxWithinRange(value= vOrg, rg= self.rgMinMax)
            self.updateMinMaxWithinRange(value= vDest, rg= self.rgMinMax)
        else:
            self.updateMinMax(value= vOrg)
            self.updateMinMax(value= vDest)
        if(vOrg*vDest>0.0):
            return self.createConstantSignDiagramInterval(offset, org, vOrg, dest, vDest, dirVector= dirVector)
        else:
            return self.createChangedSignDiagramInterval(offset, org, vOrg, dest, vDest, dirVector= dirVector)

    def createDiagramActor(self):
        # Crea el actor para el diagrama.
        self.diagram= vtkPolyData()
        self.diagram.SetPoints(self.points)
        point_data= self.diagram.GetPointData()
        point_data.SetScalars(self.scalars)
        self.diagram.SetPolys(self.cells)

        self.mapper= vtkPolyDataMapper()
        self.mapper.SetInputData(self.diagram)
        self.mapper.SetScalarRange(self.valMin, self.valMax)
        self.mapper.SetLookupTable(self.lookUpTable)
        self.actor= vtkActor() 
        self.actor.SetMapper(self.mapper)

    def addDiagramToScene(self, recordDisplay, orientation= 1, title= None):
        ''' Adds the diagram to de scene

        :param recordDisplay: DisplaySettings object.
        :param orientation: orientation of the scalar bar (defaults to 1-horiz)
        :param title: title for the scalar bar (defaults to None)
        '''
        recordDisplay.renderer.AddActor(self.actor)
        self.creaColorScaleBar(orientation, title)
        recordDisplay.renderer.AddActor2D(self.scalarBar)

    def appendDataToDiagram(self, elements, diagramIndex, valueCouples, directions, defFScale=0.0):
        ''' Appends to the diagram the values being passed as parameter.

                       ___----* value2
           value1  *---       |
                   |          |
                   |          |
                 1 +----------+ 2

           :param elements: elements over which diagram is represented.
           :param diagramIndex: index-counter for the values to insert.
           :param valueCouples: (vStart, vEnd) couple of values at the start node
                                and the end node of the element.
           :param directions: direction of the diagram on each element.
           :param defFScale: deformation scale factor. Factor to apply to the
                             current displacement of the nodes so that the 
                             displayed position of each node equals to
                             the initial position plus its displacement 
                             multiplied by this factor. (Defaults to 0.0, i.e. 
                             display the initial/undeformed shape).
        '''
        for elem, (v0, v1), direction in zip(elements, valueCouples, directions):
            posNode0= elem.getNodes[0].getCurrentPos3d(defFScale)
            posNode1= elem.getNodes[1].getCurrentPos3d(defFScale)
            diagramIndex= self.createDiagramInterval(offset= diagramIndex, org= posNode0, valOrg= v0, dest= posNode1, valDest= v1, dirVector= direction)
        if(self.rgMinMax):
            lmsg.warning('Displayed values have been clipped whitin the range: ('+str(self.rgMinMax[0])+', '+str(self.rgMinMax[1])+") so they don't correspond to the computed ones.")
        return diagramIndex

    def filterValueCouples(self, valueCouples):
        ''' Removes the values outside the rgMinMax interval if any.

                       ___----* value2
           value1  *---       |
                   |          |
                   |          |
                 1 +----------+ 2

        :param valueCouples: couple of values corresponding to each element.
        '''
        retval= list()
        if(self.rgMinMax): # filter values.
            retval= list()
            # vMax= max(max(valueCouples, key= itemgetter(0))[0], max(valueCouples, key= itemgetter(1))[1])
            # vMin= min(min(valueCouples, key= itemgetter(0))[0], min(valueCouples, key= itemgetter(1))[1])
            vMin= self.rgMinMax[0]
            vMax= self.rgMinMax[1]
            for (v0, v1) in valueCouples:
                if(v0>vMax):
                    v0= vMax
                elif(v0<vMin):
                    v0= vMin
                if(v1>vMax):
                    v1= vMax
                elif(v1<vMin):
                    v1= vMin
                retval.append((v0, v1))
        else:
            retval= valueCouples
        return retval

    def appendDataFromElementEnds(self, dirVectors, elements, diagramIndex, valueCouples, defFScale=0.0):
        '''Append to the diagram the data from the element ends as follows:

                       ___----* value2
           value1  *---       |
                   |          |
                   |          |
                 1 +----------+ 2

           :param dirVectors: direction vector for each element diagram.
           :param elements: element over which diagram is represented.
           :param diagramIndex: index-counter for the values to insert.
           :param value1: value at the start node.
           :param value2: value at the end node. 
           :param defFScale: deformation scale factor. Factor to apply to the
                             current displacement of the nodes so that the 
                             displayed position of each node equals to
                             the initial position plus its displacement 
                             multiplied by this factor. (Defaults to 0.0, i.e. 
                             display the initial/undeformed shape).
        '''
        for elem, (v0, v1), dirVector in zip(elements, valueCouples, dirVectors):
            posNode0= elem.getNodes[0].getCurrentPos3d(defFScale)
            posNode1= elem.getNodes[1].getCurrentPos3d(defFScale)
            diagramIndex= self.createDiagramInterval(offset= diagramIndex, org= posNode0, valOrg= v0, dest= posNode1, valDest= v1, dirVector= dirVector)
        return diagramIndex
