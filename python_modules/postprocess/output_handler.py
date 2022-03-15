# -*- coding: utf-8 -*-

from __future__ import print_function
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

from misc_utils import log_messages as lmsg
from postprocess.xcVtk import vtk_graphic_base
from postprocess.xcVtk.CAD_model import vtk_CAD_graphic
from postprocess.xcVtk.FE_model import vtk_FE_graphic
from postprocess.xcVtk.fields import fields
from postprocess.xcVtk.fields import vector_field as vf
from postprocess.xcVtk.fields import load_vector_field as lvf
from postprocess.xcVtk.diagrams import control_var_diagram as cvd
from postprocess.xcVtk.diagrams import linear_load_diagram as lld
from postprocess.xcVtk.diagrams import node_property_diagram as npd
from postprocess.xcVtk.diagrams import element_property_diagram as epd
import vtk
from postprocess import output_styles
from misc_utils import log_messages as lmsg

class OutputHandler(object):
    ''' Object that handles the ouput (graphics, etc.)

       :ivar modelSpace: FE model data.
       :ivar outputStyle: style of the output.
    '''
    def __init__(self, modelSpace= None, outputStyle= output_styles.defaultOutputStyle):
        '''Defines the dimension of the space and the number 
         of DOFs for each node.

         :param modelSpace: FE model data
         :param outputStyle: style of the output.
        '''
        self.modelSpace= modelSpace
        self.outputStyle= outputStyle

    def getCaptionText(self, itemToDisp, unitDescription, setToDisplay):
        ''' Return the text to use in the image caption.'''
        loadCaseName= self.modelSpace.preprocessor.getDomain.currentCombinationName
        return loadCaseName+' '+itemToDisp+' '+unitDescription+' '+setToDisplay.description

    def getOutputLengthUnitSym(self):
        return self.outputStyle.outputUnits.dynamicUnits.lengthUnit.symbol

    def getOutputForceUnitSym(self):
        return self.outputStyle.outputUnits.dynamicUnits.forceUnit.symbol

    def getDefaultCameraParameters(self):
        '''Return the default camera parameters.'''
        if(self.modelSpace.getSpaceDimension()==3): # 3D problem
            return vtk_graphic_base.CameraParameters('XYZPos')
        else: # 2D or 1D problem
            retval= vtk_graphic_base.CameraParameters('Custom')
            retval.viewUpVc= [0,1,0]#[0,0,1]
            retval.posCVc= [0,0,100] #[0,-100,0]
            return retval
        
    def getCameraParameters(self):
        ''' Return a suitable set of camera parameters if not
            already defined.'''
        if(not self.outputStyle.cameraParameters):
            self.outputStyle.cameraParameters= self.getDefaultCameraParameters()
        return self.outputStyle.cameraParameters

    def setCameraParameters(self, cameraParameters):
        ''' Set the camera parameters from the arguments.

        :param cameraParameters: CameraParameters object (see vtk_graphic_base)
                                 parameters that define the camera.               
        '''
        self.outputStyle.cameraParameters= cameraParameters
        
    def displayBlocks(self, setToDisplay= None, caption= None, fileName=None):
        '''Display the blocks (points, lines, surfaces and volumes)
           of the set.

           :param setToDisplay: set to display.
           :param caption: title of the graphic.
           :param fileName: name of the file to plot the graphic. Defaults to 
                       None, in that case an screen display is generated
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        if(caption==None):
            caption= setToDisplay.name+' set; blocks'
        displaySettings= vtk_CAD_graphic.DisplaySettingsBlockTopo()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.displayBlocks(setToDisplay,caption= caption, fileName= fileName)
        
    def displayFEMesh(self, setsToDisplay= None, caption= None, fileName= None, defFScale= 0.0):
        '''Display the mesh (nodes, elements and constraints)
           of the set.

           :param setsToDisplay: list of sets to display (defaults to TotalSet).
           :param caption: title of the graphic.
           :param fileName: name of the file to plot the graphic. Defaults to 
                       None, in that case an screen display is generated
           :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        '''
        if(setsToDisplay==None):
            setsToDisplay= [self.modelSpace.getTotalSet()]
        if(caption==None):
            caption= 'mesh'
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.displayMesh(xcSets=setsToDisplay,caption= caption, scaleConstr= self.outputStyle.constraintsScaleFactor, fileName= fileName, defFScale= defFScale)

    def displayLocalAxes(self, setToDisplay= None, caption= None, fileName=None, defFScale= 0.0):
        '''Display the local axes of the elements contained in the set.

           :param setToDisplay: set to display.
           :param caption: title of the graphic.
           :param fileName: name of the file to plot the graphic. Defaults to 
                       None, in that case an screen display is generated
           :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        if(caption==None):
            caption= setToDisplay.name+' set; local axes'
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.displayLocalAxes(setToDisplay,caption= caption, vectorScale= self.outputStyle.localAxesVectorsScaleFactor, fileName= fileName, defFScale= defFScale)

    def displayStrongWeakAxis(self, setToDisplay= None, caption= None, fileName=None, defFScale= 0.0):
        '''Display the local axes of the elements contained in the set.

           :param setToDisplay: set to display.
           :param caption: title of the graphic.
           :param fileName: name of the file to plot the graphic. Defaults to 
                       None, in that case an screen display is generated
           :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
         '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        if(caption==None):
            caption= setToDisplay.name+' set; strong [red] and weak [blue] axes'
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
#        displaySettings.displayStrongWeakAxis(setToDisplay,caption= caption, vectorScale= self.outputStyle.localAxesVectorsScaleFactor, fileName= fileName, defFScale= defFScale)
        displaySettings.displayStrongWeakAxis(setToDisplay,caption= caption, vectorScale= self.outputStyle.localAxesVectorsScaleFactor)

    def displayScalarPropertyAtNodes(self,propToDisp, fUnitConv, unitDescription, captionText, setToDisplay, fileName=None, defFScale=0.0, rgMinMax=None):
        '''displays the scalar property defined at the nodes of the set.

        :param propeToDisp: scalar property defined at nodes. 
        :param fUnitConv: conversion factor for units
        :param unitDescription: unit(s) symbol(s)
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
              of the field to be represented (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)

        '''
        field= fields.ScalarField(name=propToDisp,functionName="getProp",component=None,fUnitConv= fUnitConv,rgMinMax=rgMinMax)
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.displayMesh(xcSets=setToDisplay, field= field, diagrams= None, caption= captionText, fileName=fileName, defFScale=defFScale)
        
    def displayDispRot(self,itemToDisp, setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None):
        '''displays the component of the displacement or rotations in the 
        set of entities.

        :param itemToDisp: component of the displacement ('uX', 'uY' or 'uZ') 
                    or the rotation ('rotX', rotY', 'rotZ') to be depicted 
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)

        '''
        # Define the property at nodes.
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= 'disp'+itemToDisp
        vCompDisp= self.modelSpace.getDispComponentFromName(itemToDisp)
        nodSet= setToDisplay.nodes
        for n in nodSet:
            n.setProp(propertyName,n.getDisp[vCompDisp])
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)

        captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, fUnitConv= unitConversionFactor, unitDescription= unitDescription, captionText= captionText, setToDisplay= setToDisplay, fileName= fileName, defFScale= defFScale, rgMinMax= rgMinMax)

    def displayStresses(self,itemToDisp, setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None):
        '''display the stresses on the elements.

        :param itemToDisp: component of the stress ('sigma_11', 'sigma_22'...)
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)

        '''
        # Define the property at nodes.
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= self.modelSpace.setNodePropertyFromElements(compName= itemToDisp, xcSet= setToDisplay, function= self.modelSpace.getStressComponentFromName, propToDefine= 'stress')
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)

        captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, unitConversionFactor, unitDescription, captionText, setToDisplay, fileName, defFScale, rgMinMax)

    def displayStrains(self,itemToDisp, setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None):
        '''displays the strains on the elements.

        :param itemToDisp: component of the stress ('eps_11', 'eps_22'...)
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)

        '''
        # Define the property at nodes.
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= self.modelSpace.setNodePropertyFromElements(compName= itemToDisp, xcSet= setToDisplay, function= self.modelSpace.getStrainComponentFromName, propToDefine= 'strain')
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)

        captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, unitConversionFactor, unitDescription, captionText, setToDisplay, fileName, defFScale, rgMinMax)
        
    def displayVonMisesStresses(self, vMisesCode= 'von_mises_stress', setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None):
        '''display the stresses on the elements.

        :param vMisesCode: string that will be passed to the element
                             getValues method to retrieve the Von Mises
                             stress. This may vary depending on the
                             element type.
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)

        '''
        # Define the property at nodes.
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= self.modelSpace.setNodePropertyFromElements(compName= None, xcSet= setToDisplay, function= self.modelSpace.getStressComponentFromName, propToDefine= vMisesCode)
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters('stress')

        captionText= self.getCaptionText(vMisesCode, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, unitConversionFactor, unitDescription, captionText, setToDisplay, fileName, defFScale, rgMinMax)
        
    def displayReactions(self, setToDisplay=None, fileName=None, defFScale=0.0, inclInertia= False, reactionCheckTolerance= 1e-7):
        ''' Display reactions.

        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param inclInertia: include inertia effects (defaults to false).
        :param reactionCheckTolerance: relative tolerance when checking reaction values.
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        self.modelSpace.preprocessor.getNodeHandler.calculateNodalReactions(inclInertia, reactionCheckTolerance)
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
        maxAbs=0.0
        forcePairs= list()
        momentPairs= list()
        threshold= LrefModSize/1000.0
        for n in setToDisplay.nodes:
            f3d= n.getReactionForce3d
            m3d= n.getReactionMoment3d
            modF3d= f3d.getModulus()
            if(modF3d>threshold):
                p=n.getCurrentPos3d(defFScale)
                forcePairs.append(([p.x,p.y,p.z],[f3d.x,f3d.y,f3d.z]))
            modM3d= m3d.getModulus()
            if(modM3d>threshold):
                p=n.getCurrentPos3d(defFScale)
                momentPairs.append(([p.x,p.y,p.z],[m3d.x,m3d.y,m3d.z]))
            modR= max(modF3d,modF3d)
            if(modR>maxAbs):
                maxAbs=modR
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        unitDescription= self.outputStyle.getForceUnitsDescription()
        
        scaleFactor= self.outputStyle.reactionVectorsScaleFactor
        if(maxAbs>0):
            scaleFactor*=0.15*LrefModSize/(maxAbs*unitConversionFactor)

        captionText= self.getCaptionText('Reactions', unitDescription, setToDisplay)
        vFieldF= vf.VectorField(name='Freact',fUnitConv=unitConversionFactor,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()) # Force
        vFieldM= vf.VectorField(name='Mreact',fUnitConv=unitConversionFactor,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()) # Moment
        vFieldF.populateFromPairList(forcePairs)
        vFieldM.populateFromPairList(momentPairs)

        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.setupGrid(setToDisplay)
        displaySettings.defineMeshScene(None,defFScale,color= setToDisplay.color)
        scOrient=1
        if(len(forcePairs)>0):
            vFieldF.addToDisplay(displaySettings,orientation=scOrient,
                                 title='Forces ('+ self.getOutputForceUnitSym() +')')
            scOrient+=1
        if(len(momentPairs)>0):
            vFieldM.addToDisplay(displaySettings,orientation=scOrient,
                                 title='Moments (' + self.getOutputForceUnitSym() + self.getOutputLengthUnitSym() +')')
        displaySettings.displayScene(captionText,fileName)
        
    def displayReactionsOnSets(self, setsToDisplayReactions, fileName=None, defFScale=0.0, inclInertia= False, reactionCheckTolerance= 1e-7):
        '''displays the reactions as vector on affected nodes

        :param setsToDisplayReactions: ordered list of sets of nodes to display 
                              reactions on them.
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param inclInertia: include inertia effects (defaults to false).
        :param reactionCheckTolerance: relative tolerance when checking reaction values.
        '''
        for st in setsToDisplayReactions:
            self.displayReactions(setToDisplay= st, fileName= fileName, defFScale= defFScale, inclInertia= inclInertia, reactionCheckTolerance= reactionCheckTolerance)
            
    def displayDiagram(attributeName,component, setToDispRes,setDisp,caption,scaleFactor= 1.0, fileName= None, defFScale= 0.0,orientScbar=1,titleScbar=None):
        '''Auxiliary function to display results on linear elements.

        :param attributeName: attribute name(e.g. 'ULS_normalStressesResistance')
        :param component:    result item to display (e.g. 'N', 'My', ...)
        :param setToDispRes: set of linear elements to which display results
        :param setToDisplay:      set of elements (any type) to be depicted
        :param scaleFactor:  factor of scale to apply to the auto-scaled display
                             (defaults to 1)
        :param caption:      caption to display
        :param fileName:     file to dump the display
        :param defFScale:    factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        :param orientScbar: orientation of the scalar bar (defaults to 1-horiz)
        :param titleScbar: title for the scalar bar (defaults to None)
        '''
        diagram= cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= unitConversionFactor,sets=[setToDispRes],attributeName= attributeName,component= component)
        diagram.addDiagram()
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters=cameraParameters
        displaySettings.setupGrid(setToDisplay)
        displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        displaySettings.appendDiagram(diagram,orientScbar,titleScbar) #Append diagram to the scene.
        displaySettings.displayScene(caption=caption,fileName=fileName)

    def displayIntForcDiag(self, itemToDisp, setToDisplay=None,fileName=None,defFScale=0.0,orientScbar=1,titleScbar=None):
        '''displays the component of internal forces in the set of entities as a 
         diagram over lines (i.e. appropiated for beam elements).

        :param itemToDisp: component of the internal forces 
          ('N', 'Qy' (or 'Vy'), 'Qz' (or 'Vz'), 'My', 'Mz', 'T') to be depicted 
        :param setToDisplay: set of entities (elements of type beam) to be 
          represented
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param orientScbar: orientation of the scalar bar (defaults to 1-horiz)
        :param titleScbar: title for the scalar bar (defaults to None)
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        #auto-scale parameters
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
        scaleFactor= self.outputStyle.internalForcesDiagramScaleFactor
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        unitDescription= self.outputStyle.getForceUnitsDescription()
        diagAux= cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= unitConversionFactor,sets=[setToDisplay],attributeName= "intForce",component= itemToDisp)
        maxAbs= diagAux.getMaxAbsComp()
        if maxAbs > 0:
            scaleFactor*=0.15*LrefModSize/(maxAbs*unitConversionFactor)
        captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        diagram= cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= unitConversionFactor,sets=[setToDisplay],attributeName= "intForce",component= itemToDisp)
        diagram.addDiagram()
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.setupGrid(setToDisplay)
        displaySettings.defineMeshScene(None,defFScale,color= setToDisplay.color)
        displaySettings.appendDiagram(diagram,orientScbar,titleScbar) #Append diagram to the scene.
        displaySettings.displayScene(caption= captionText,fileName= fileName)
        
    def displayIntForc(self,itemToDisp, setToDisplay=None,fileName=None,defFScale=0.0, rgMinMax=None):
        '''displays the component of internal forces in the 
        set of entities as a scalar field (i.e. appropiated for 2D elements; 
        shells...).

        :param itemToDisp:   component of the internal forces ('N1', 'N2', 'N12', 
             'M1', 'M2', 'M12', 'Q1', 'Q2') to be depicted 
        :param setToDisplay: set of entities to be represented (default to all 
             entities)
        :param fileName: name of the file to plot the graphic. Defaults to None,
             in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        vCompDisp= self.modelSpace.getIntForceComponentFromName(itemToDisp)
        elSet= setToDisplay.elements.pickElemsOfDimension(2)
        if(len(elSet)>0):
            propName= 'propToDisp_'+str(itemToDisp)
            for e in elSet:
                if(e.getDimension==2):
                    e.getResistingForce()
                    physProp= e.getPhysicalProperties
                    e.setProp(propName,physProp.getMeanGeneralizedStressByName(vCompDisp))
                else:
                    lmsg.warning('OutputHandler::displayIntForc; not a 2D element; ignored.')
            unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
            unitDescription= self.outputStyle.getForceUnitsDescription()
            field= fields.ExtrapolatedProperty(propName,"getProp",setToDisplay,fUnitConv= unitConversionFactor,rgMinMax=rgMinMax)
            displaySettings= vtk_FE_graphic.DisplaySettingsFE()
            displaySettings.cameraParameters= self.getCameraParameters()
            captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
            field.display(displaySettings=displaySettings,caption= captionText,fileName=fileName, defFScale=defFScale)
            
    def displayLoadVectors(self, setToDisplay= None, caption= None, fileName= None, defFScale= 0.0):
        '''Displays load vectors on the set argument.

        :param setToDisplay: set of elements to be displayed (defaults to total set)
        :param caption: text to display in the graphic. Defaults to 
               ` None` in this case the text is the load case description
               and the units of the loads.
        :param fileName: full name of the graphic file to generate. Defaults to 
                     ` None`, in this case it returns a console output graphic.,
        :param defFScale: factor to apply to current displacement of nodes 
                      so that the display position of each node equals to
                      the initial position plus its displacement multiplied
                      by this factor. (Defaults to 0.0, i.e. display of 
                      initial/undeformed shape)
        '''
        lmsg.warning('displayLoadVectors:: deprecated; Use displayLoads')
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        preprocessor= self.modelSpace.preprocessor
        loadCaseName= self.modelSpace.preprocessor.getDomain.currentCombinationName
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        unitDescription= self.outputStyle.getForceUnitsDescription()
        if(not caption):
            caption= 'load case: ' + loadCaseName + ', set: ' + setToDisplay.name + ', '  + unitDescription
        LrefModSize=setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to auto-scale)
        vectorScale= self.outputStyle.loadVectorsScaleFactor*LrefModSize/10.
        vField= lvf.LoadVectorField(loadCaseName,setToDisplay,unitConversionFactor,vectorScale)
        vField.multiplyByElementArea= self.outputStyle.multLoadsByElemArea
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.setupGrid(setToDisplay)
        vField.dumpVectors(preprocessor,defFScale,showElementalLoads= True, showNodalLoads= True)
        displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color) 
        vField.addToDisplay(displaySettings)
        displaySettings.displayScene(caption,fileName)
        return displaySettings
        
    def displayLoads(self,  setToDisplay=None,elLoadComp='xyzComponents',fUnitConv=1,caption= None,fileName=None,defFScale=0.0, scaleConstr= 0.2):
        '''Display the loads applied on beam elements and nodes for a given load case

        :param setToDisplay: set of beam elements to be represented (defaults to TotalSet)
        :param elLoadComp:component of the linear loads on elements to be 
               depicted [available components: 'xyzComponents' (default),
               'axialComponent', 'transComponent', 'transYComponent', 
               'transZComponent']
        :param fUnitConv:  factor of conversion to be applied to the results
                        (defaults to 1)
        :param caption:   caption for the graphic
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                          in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
        :param scaleConstr: scale of SPConstraints symbols (defaults to 0.2)
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        preprocessor= self.modelSpace.preprocessor
        loadCaseName= self.modelSpace.preprocessor.getDomain.currentCombinationName
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        unitDescription= self.outputStyle.getForceUnitsDescription()
        preprocessor= setToDisplay.getPreprocessor
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        grid= displaySettings.setupGrid(setToDisplay)
        displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        scOrient=1 #scalar bar orientation (1 horiz., 2 left-vert, 3 right-vert)
        # auto-scaling parameters
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to auto-scale)
        elLoadScaleF= self.outputStyle.loadDiagramsScaleFactor
        diagram= lld.LinearLoadDiagram(setToDisp=setToDisplay, scale=elLoadScaleF, fUnitConv= unitConversionFactor, component=elLoadComp)
        maxAbs= diagram.getMaxAbsComp(preprocessor)
        if(maxAbs>0.0):
            elLoadScaleF*= LrefModSize/maxAbs*100.0
            diagram.scaleFactor= elLoadScaleF
            #Linear loads
            diagram.addDiagram(preprocessor)
            if(diagram.isValid()):
                displaySettings.appendDiagram(diagram, orientScbar=scOrient,
                                              titleScbar='Linear loads ('+self.getOutputForceUnitSym()+'/'+
                                              self.getOutputLengthUnitSym()+')' )
                scOrient+=1
        
        vectorScale= self.outputStyle.loadVectorsScaleFactor*LrefModSize/10.
        # concentrated loads (on elements).
        
        # surface loads
        vFieldEl= lvf.LoadVectorField(loadPatternName= loadCaseName, setToDisp=setToDisplay,
                                      fUnitConv= unitConversionFactor, scaleFactor= vectorScale,
                                      showPushing= self.outputStyle.showLoadsPushing,
                                      multiplyByElementArea= self.outputStyle.multLoadsByElemArea)
        count= vFieldEl.dumpElementalLoads(preprocessor, defFScale=defFScale)
        if(count >0):
            vFieldEl.addToDisplay(displaySettings,orientation= scOrient,
                                  title='Surface loads ('+self.getOutputForceUnitSym()+'/'+
                                  self.getOutputLengthUnitSym()+'2)' )
            scOrient+=1
        # nodal loads
        ## forces on nodes.
        vFieldFNod= lvf.LoadVectorField(loadPatternName= loadCaseName,
                                       setToDisp=setToDisplay, fUnitConv= unitConversionFactor,
                                       scaleFactor= vectorScale, showPushing= self.outputStyle.showLoadsPushing)
        count= vFieldFNod.dumpNodalLoads(preprocessor, defFScale=defFScale)
        if(count >0):
            vFieldFNod.addToDisplay(displaySettings,orientation= scOrient,
                                   title='Nodal loads ('+self.getOutputForceUnitSym()+')')
            scOrient+=1
        ## moments on nodes.
        vFieldMNod= lvf.LoadVectorField(loadPatternName= loadCaseName,
                                       setToDisp=setToDisplay, fUnitConv= unitConversionFactor,
                                       scaleFactor= vectorScale, showPushing= self.outputStyle.showLoadsPushing, components= [3,4,5])
        count= vFieldMNod.dumpNodalLoads(preprocessor, defFScale=defFScale)
        if(count >0):
            vFieldMNod.addToDisplay(displaySettings,orientation= scOrient,
                                   title='Nodal moments ('+self.getOutputForceUnitSym()+')')
            scOrient+=1
        if(not caption):
          caption= 'load case: ' + loadCaseName +' '+elLoadComp + ', set: ' + setToDisplay.name + ', '  + unitDescription
        displaySettings.displaySPconstraints(setToDisplay= setToDisplay, scale= scaleConstr)
        displaySettings.displayScene(caption=caption,fileName=fileName)

    def displayNodeValueDiagram(self, itemToDisp, setToDisplay=None,caption= None,fileName=None,defFScale=0.0):
        '''displays the a displacement (uX,uY,...) or a property defined in nodes 
        as a diagram over lines.

        :param itemToDisp: item to display (uX,uY,...).
        :param setToDisplay: set of entities (elements of type beam) to be 
               represented
        :param fileName: name of the file to plot the graphic. Defaults to None,
                         in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
         '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
        lmsg.warning("Auto scale not implemented yet.")
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
        scaleFactor= LrefModSize/unitConversionFactor 
        diagram= npd.NodePropertyDiagram(scaleFactor= scaleFactor,fUnitConv= unitConversionFactor,sets=[setToDisplay], attributeName= itemToDisp)
        diagram.addDiagram()
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        grid= displaySettings.setupGrid(setToDisplay)
        displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        displaySettings.appendDiagram(diagram) #Append diagram to the scene.

        loadCaseName= self.modelSpace.preprocessor.getDomain.currentCombinationName
        if(not caption):
            caption= loadCaseName+' '+itemToDisp+' '+unitDescription +' '+setToDisplay.description
        displaySettings.displayScene(caption=caption,fileName=fileName)

    def displayElementValueDiagram(self, itemToDisp, setToDisplay=None,caption= None,fileName=None,defFScale=0.0):
        '''displays the a displacement (uX,uY,...) or a property defined in nodes 
        as a diagram over lines.

        :param itemToDisp: item to display (uX,uY,...).
        :param setToDisplay: set of entities (elements of type beam) to be 
               represented
        :param fileName: name of the file to plot the graphic. Defaults to None,
                         in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
         '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
        scaleFactor= 1.0
        diagram= epd.ElementPropertyDiagram(scaleFactor= scaleFactor,fUnitConv= unitConversionFactor,sets=[setToDisplay], propertyName= itemToDisp)
        diagram.addDiagram()
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        grid= displaySettings.setupGrid(setToDisplay)
        displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        displaySettings.appendDiagram(diagram) #Append diagram to the scene.

        loadCaseName= self.modelSpace.preprocessor.getDomain.currentCombinationName
        if(not caption):
            caption= loadCaseName+' '+itemToDisp+' '+unitDescription +' '+setToDisplay.description
        displaySettings.displayScene(caption=caption,fileName=fileName)

    def displayEigenvectors(self, mode= 1, setToDisplay=None, caption= None, fileName=None,defFScale=0.0):
        '''Displays the computed eigenvectors on the set argument.

        :param setToDisplay: set of elements to be displayed (defaults to total set)
        :param mode: mode to which the eigenvectors belong.
        :param caption: text to display in the graphic. Defaults to 
               ` None` in this case the text is the load case description
               and the units of the loads.
        :param fileName: full name of the graphic file to generate. Defaults to 
                     ` None`, in this case it returns a console output graphic.,
        :param defFScale: factor to apply to current displacement of nodes 
                      so that the display position of each node equals to
                      the initial position plus its displacement multiplied
                      by this factor. (Defaults to 0.0, i.e. display of 
                      initial/undeformed shape)
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        preprocessor= self.modelSpace.preprocessor
        domain= preprocessor.getDomain
        numModes= domain.numModes # number of computed modes.
        if(mode<=numModes):
            norm= preprocessor.getDomain.getMesh.normalizeEigenvectors(mode)
            #auto-scale
            LrefModSize=setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
            maxAbs= 0.0
            dispPairs= list()
            rotPairs= list()
            threshold= LrefModSize/1000.0
            for n in setToDisplay.nodes:
                disp3d= n.getEigenvectorDisp3dComponents(mode)
                rot3d= n.getEigenvectorRot3dComponents(mode)
                modDisp3d= disp3d.getModulus()
                if(modDisp3d>threshold):
                    p=n.getCurrentPos3d(defFScale)
                    dispPairs.append(([p.x,p.y,p.z],[disp3d.x,disp3d.y,disp3d.z]))
                modRot3d= rot3d.getModulus()
                if(modRot3d>threshold):
                    p=n.getCurrentPos3d(defFScale)
                    rotPairs.append(([p.x,p.y,p.z],[rot3d.x,rot3d.y,rot3d.z]))
                modR= max(modDisp3d,modRot3d)
                if(modR>maxAbs):
                    maxAbs=modR
            scaleFactor= self.outputStyle.eigenvectorsScaleFactor
            if(maxAbs > 0):
                scaleFactor*=0.15*LrefModSize/(maxAbs)
            #
            if(not caption):
                caption= 'Mode '+ str(mode) + ' eigenvectors' + ' '+setToDisplay.description
            vFieldD= vf.VectorField(name='Deigenvectors',fUnitConv=1.0,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()) #Force
            vFieldR= vf.VectorField(name='Reigenvectors',fUnitConv=1.0,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource())
            vFieldD.populateFromPairList(dispPairs)
            vFieldR.populateFromPairList(rotPairs)

            displaySettings= vtk_FE_graphic.DisplaySettingsFE()
            displaySettings.cameraParameters= self.getCameraParameters()
            displaySettings.setupGrid(setToDisplay)
            displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
            scOrient= 1
            if(len(dispPairs)>0):
                vFieldD.addToDisplay(displaySettings,orientation=scOrient,title='Displacement')
                scOrient+=1
            if(len(rotPairs)>0):
                vFieldR.addToDisplay(displaySettings,orientation=scOrient,title='Rotation')
            displaySettings.displayScene(caption,fileName)
        else:
            lmsg.error('mode: '+str(mode)+' out of range (1,'+str(numModes)+')')
        
    def displayEigenResult(self,eigenMode, setToDisplay=None,  accelMode=None, caption= '',fileName=None, defFScale= 0.0):
        '''Display the deformed shape and/or the equivalent static forces 
        associated with the eigenvibration mode passed as parameter.

        :param eigenMode: eigenvibration mode to be displayed
        :param setToDisplay: set of elements to be displayed (defaults to total set)
        :param accelMode: acceleration associated with the eigen mode depicted, 
                only used if the equivalent static loads are to be displayed.
        :param caption:   text to display in the graphic 
        :param fileName:  full name of the graphic file to generate. 
                 Defaults to ` None`, in this case it returns a console output 
                 graphic.
        :param defFScale: factor to apply to deformed shape so that the 
               displayed position of each node equals to the initial position 
               plus its eigenVector multiplied by this factor. (Defaults to 0.0 
               i.e. display of initial/undeformed shape)
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        equLoadVctScale= self.outputStyle.equivalentLoadVectorsScaleFactor
        if((equLoadVctScale!=0.0) and accelMode==None):
            lmsg.warning("Can't display equivalent static loads. Parameter accelMode should not be null ")
            equLoadVctScale=None
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.setupGrid(setToDisplay)
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.defineMeshScene(None,defFScale,eigenMode,color=setToDisplay.color)
        unitsScale= 1.0
        if equLoadVctScale not in [None,0]:
            vField=vf.VectorField(name='mode'+str(eigenMode),fUnitConv=unitsScale,scaleFactor=equLoadVctScale,showPushing= True)
            setNodes= setToDisplay.nodes
            for n in setNodes:
                pos= n.getEigenPos3d(defFScale,eigenMode)
                vEqLoad= n.getEquivalentStaticLoad(eigenMode,accelMode)
                vField.data.insertNextPair(pos.x,pos.y,pos.z,vEqLoad[0],vEqLoad[1],vEqLoad[2],unitsScale,pushing= True)
            vField.addToDisplay(displaySettings)
        if(not caption):
            caption= 'Mode '+ str(eigenMode) + ' shape' + ' '+setToDisplay.description            
        displaySettings.displayScene(caption,fileName)
        return displaySettings

    def displayBeamResult(self,attributeName,itemToDisp,beamSetDispRes,setToDisplay=None,caption=None,fileName=None,defFScale=0.0):
        '''display results for beam elements from a limit state verification file.

        :param attributeName:attribute name(e.g. 'ULS_normalStressesResistance')
        :param itemToDisp:   result item to display (e.g. 'N', 'My', ...)
        :param beamSetDispRes:set of linear elements to which display results 
        :param setToDisplay: set of elements (any type) to be depicted
               (defaults to None, in that case only elements in beamSetDispRes
               are displayed)
        :param caption:      caption to display
               (defaults to 'attributeName + itemToDisp')
        :param fileName:     file to dump the display (defaults to screen display)
        :param defFScale:    factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)

       '''
        #auto-scale parameters
        if(len(beamSetDispRes.elements)):            
            LrefModSize=setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
            lstArgVal=[e.getProp(attributeName+'Sect1')(itemToDisp) for e in beamSetDispRes.elements]
            unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
            scaleFactor= 1.0
            maxAbs=max(abs(max(lstArgVal)),abs(min(lstArgVal)))
            if(maxAbs>0):
                scaleFactor*=0.15*LrefModSize/(maxAbs*unitConversionFactor)
            if not setToDisplay:
                setToDisplay= beamSetDispRes
            if not caption:
                if hasattr(beamSetDispRes,'description'):
                    descrSet= beamSetDispRes.description.capitalize()
                if(len(descrSet)==0): # No description provided.
                    descrSet= beamSetDispRes.name
                caption= attributeName + ', ' + itemToDisp +' '+unitDescription+ '. '+ descrSet
            diagram= cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= unitConversionFactor,sets=[beamSetDispRes],attributeName= attributeName,component= itemToDisp)
            diagram.addDiagram()
            displaySettings= vtk_FE_graphic.DisplaySettingsFE()
            displaySettings.cameraParameters= self.getCameraParameters()
            displaySettings.setupGrid(setToDisplay)
            displaySettings.defineMeshScene(None,defFScale,color= setToDisplay.color)
            displaySettings.appendDiagram(diagram) #Append diagram to the scene.
            displaySettings.displayScene(caption= caption,fileName= fileName)
        else:
            lmsg.warning('Element set: \''+beamSetDispRes.name+'\' is empty. There is nothing to display.')
                    
        
    def displayEigenvectorsOnSets(self, eigenMode, setsToDisplay, fileName=None,defFScale=0.0):
        '''displays the reactions as vector on affected nodes

        :param eigenMode: mode to which the eigenvectors belong.
        :param setsToDisplay: sets to display on batch mode.
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        '''
        for st in setsToDisplay:
            self.displayEigenvectors(mode= eigenMode, setToDisplay= st,fileName= fileName, defFScale= defFScale)

    def displayFieldDirs1and2(self,limitStateLabel, argument, component, setToDisplay, fileName, defFScale=0.0, rgMinMax=None):
        '''Display a field defined over bi-dimensional elements in its two 
           directions.

        :param limitStateLabel: label that identifies the limit state.
        :param argument: name of the control var to represent.
        :param component: component of the control var to represent.
        :param setToDisplay: represent the field over those elements.
        :param fileName: file name to store the image. If none -> window on screen.
        :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                    of the scalar field (if any) to be represented  (in units 
                    of calculation, not units of display). All the values 
                    less than vmin are displayed in blue and those greater than vmax 
                    in red (defaults to None)
        '''
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        self.displayField(limitStateLabel, 1,argument, component, setToDisplay, fileName, defFScale, rgMinMax)
        self.displayField(limitStateLabel, 2,argument, component, setToDisplay, fileName, defFScale, rgMinMax)
        
    def displayField(self,limitStateLabel, section,argument, component, setToDisplay, fileName, defFScale=0.0, rgMinMax=None):
        '''Display a field defined over bi-dimensional elements in its two 
           directions.

        :param limitStateLabel: label that identifies the limit state.
        :param section: section to display (1 or 2 or None if the value is not section dependent).
        :param argument: name of the control var to represent.
        :param component: component of the control var to represent.
        :param setToDisplay: represent the field over those elements.
        :param fileName: file name to store the image. If none -> window on screen.
        :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                    of the scalar field (if any) to be represented  (in units 
                    of calculation, not units of display). All the values 
                    less than vmin are displayed in blue and those greater than vmax 
                    in red (defaults to None)
        '''
        sectRef= ''
        sectDescr= ''
        if(section):
            if section not in [1,2]:
                lmsg.warning('section', section, "doesn't exist, section 1 is displayed instead")
                section=1
            sectRef='Sect'+str(section)
            sectDescr= self.outputStyle.directionDescription[section-1]
  
        if(setToDisplay==None):
            setToDisplay= self.modelSpace.getTotalSet()
        
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        attributeName= limitStateLabel + sectRef
        fUnitConv, unitDescription= self.outputStyle.getUnitParameters(argument)

        field= fields.getScalarFieldFromControlVar(attributeName,argument,setToDisplay,component,fUnitConv,rgMinMax)
        captionTexts= self.outputStyle.getCaptionTextsDict()
        captionBaseText= captionTexts[limitStateLabel] + ', ' + captionTexts[argument] + unitDescription + '. '+ setToDisplay.description.capitalize()
        field.display(displaySettings,caption=  captionBaseText + ', ' + sectDescr, fileName= fileName, defFScale= defFScale)

def insertGrInTex(texFile,grFileNm,grWdt,capText,labl=''):
    '''Include a graphic in a LaTeX file.

    :param texFile:    laTex file where to include the graphics 
                       (e.g.\:'text/report_loads.tex')
    :param grFileNm:   name of the graphic file with path and without extension
    :param grWdt:      width to be applied to graphics
    :param capText:    text for the caption
    :param labl:       label
    '''
    texFile.write('\\begin{figure}\n')
    texFile.write('\\begin{center}\n')
    texFile.write('\\includegraphics[width='+grWdt+']{'+grFileNm+'}\n')
    texFile.write('\\caption{'+capText+'}\n')
    if(labl!=''):
        texFile.write('\\label{'+labl+'}\n')
    texFile.write('\\end{center}\n')
    texFile.write('\\end{figure}\n')
    return
