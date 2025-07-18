# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O" 
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega@ciccp.es "

import sys
from misc_utils import data_struct_utils as su
from model import predefined_spaces
## Graphics.
from postprocess.xcVtk import vtk_graphic_base
from postprocess import output_handler
from misc_utils import log_messages as lmsg

class OuputUnits(object):
    '''Unit for the generation of graphic files report files.

    :ivar unitsScaleLoads: factor to apply to loads if we want to change
                     the units (defaults to 1).
    :ivar unitsLoads: text to especify the units in which loads are 
                     represented (defaults to 'units:[m,kN]')
    :ivar vectorScaleLoads: factor to apply to the vectors length in the 
                     representation of element loads (defaults to 1).
    :ivar vectorScalePointLoads: factor to apply to the vectors length in the 
                     representation of nodal loads (defaults to 1).
    :ivar unitsScaleDispl: factor to apply to displacements if we want to change
                     the units (defaults to 1).
    :ivar unitsDispl: text to especify the units in which displacements are 
                     represented (defaults to '[m])'
    :ivar scaleDispBeamIntForc: tuple (escN,escQ,escM) correponding to the scales
                     to apply to displays of, respectively, axial forces (N),
                     shear forces (Q) or bending moments (M) in beam elements
                     (defaults to (1.0,1.0,1.0))
    :ivar scaleDispReactions: tuple (escN,escM) correponding to the scales
                     to apply to displays of, respectively, forces (F)
                     or moments (M) in nodal reactions
                     (defaults to (1.0,1.0))
    :ivar scaleDispEigenvectors: tuple (escN,escM) correponding to the scales
                     to apply to displays of eigenvectors
                     (defaults to (1.0,1.0))
    :ivar unitsScaleForc: factor to apply to internal forces if we want to change
                     the units (defaults to 1).
    :ivar unitsForc: text to especify the units in which forces are 
                     represented (defaults to '[kN/m]')
    :ivar unitsScaleMom: factor to apply to internal moments if we want to change
                     the units (defaults to 1).
    :ivar unitsMom:  text to especify the units in which bending moments are 
                     represented (defaults to '[kN.m/m]')
    '''

    def __init__(self):
        self.unitsScaleLoads=1.0
        self.unitsLoads='units:[m,kN]'
        self.vectorScaleLoads=1.0
        self.vectorScalePointLoads=1.0
        self.unitsScaleDispl=1.0
        self.unitsDispl='[m]'
        self.scaleDispBeamIntForc=(1.0,1.0,1.0)
        self.scaleDispReactions=(1.0,1.0)
        self.scaleDispEigenvectors=(1.0,1.0)
        self.unitsScaleForc=1.0
        self.unitsForc='[kN/m]'
        self.unitsScaleMom=1.0
        self.unitsMom='[kN.m/m]'
    
class RecordDisp(OuputUnits):
    '''Generation of graphic displays

    :ivar setsToDispLoads: ordered list of sets of elements to display loads
    :ivar setsToDispBeamLoads: ordered list of sets of beam elements to display 
                     loads (defaults to [])
    :ivar compElLoad: component of load on beam elements to be represented
                     available components: 'axialComponent', 'transComponent', 
                     'transYComponent','transZComponent' (defaults to 'transComponent')
    :ivar multByElemSizeLoads: boolean value that must be True if we want to 
                               represent the total load on each element 
                               (=load multiplied by element size -length, 
                               area or volume-) and False if we are going 
                               to depict the value of the uniform load 
                               per unit area (defaults to False)
    :ivar listDspRot: ordered list of displacement or rotations to be displayed
                     available components: 'uX', 'uY', 'uZ', 'rotX', rotY', 'rotZ'
                     (defaults to ['uX', 'uY', 'uZ'])
    :ivar setsToDispDspRot: ordered list of sets of elements to display displacements 
                     or rotations
    :ivar listIntForc: ordered list of internal forces to be displayed
                     as scalar field over «shell» elements
                     available components: 'N1', 'N2', 'M1', 'M2', 'Q1', 'Q2'
                     (defaults to ['N1', 'N2', 'M1', 'M2', 'Q1', 'Q2'])
    :ivar setsToDispIntForc: ordered list of sets of elements  (of type «shell»)
                     to display internal forces
    :ivar listBeamIntForc: ordered list of internal forces to be displayed 
                   as diagrams on lines for «beam» elements
                   available components: 'N', 'My', 'Mz', 'Qy', 'Qz','T'
                   (defaults to ['N', 'My', 'Mz', 'Qy', 'Qz','T'])
    :ivar setsToDispBeamIntForc: ordered list of sets of elements (of type «beam»)
                      to display internal forces (defaults to [])
    :ivar setsToDispReactions: ordered list of sets of nodes
                      to display reactions (defaults to [])
    :ivar cameraParameters: parameters that define the camera position,
                            zoom, etc.
    :ivar cameraParametersForBeams: parameters that define the camera position,
                            zoom, etc for displaying beam elements.
    '''

    def __init__(self,setsToDispLoads= None, setsToDispDspRot= None ,setsToDispIntForc= None):
        super(RecordDisp,self).__init__()
        if(setsToDispLoads):
            self.setsToDispLoads= setsToDispLoads
        else:
            self.setsToDispLoads= list()
        if(setsToDispDspRot):
            self.setsToDispDspRot= setsToDispDspRot
        else:
            self.setsToDispDspRot= list()
        if(setsToDispIntForc):
            self.setsToDispIntForc= setsToDispIntForc
        else:
            self.setsToDispIntForc= list()
        self.setsToDispBeamLoads= list()
        self.compElLoad='transComponent'
        self.multByElemSizeLoads=False
        self.listDspRot=['uX', 'uY', 'uZ']
        self.setsToDispDspRot=setsToDispDspRot
        self.listIntForc=['N1', 'N2', 'M1', 'M2', 'Q1', 'Q2']
        self.setsToDispIntForc= setsToDispIntForc
        self.listBeamIntForc=['N', 'My', 'Mz', 'Qy', 'Qz','T']
        self.setsToDispBeamIntForc=[]
        self.setsToDispReactions=[]
        self.setsToDispEigenvectors=[]
        self.cameraParameters= vtk_graphic_base.CameraParameters('XYZPos')
        self.cameraParametersBeams= vtk_graphic_base.CameraParameters('XYZPos')

    def getDescription(self):
        return ''

    def getOutputHandler(self, setToDisplay):
        ''' Return an output handler from the set argument.
            NOT a very elegant solution-> to refactor.'''
        modelSpace= predefined_spaces.getModelSpaceFromPreprocessor(setToDisplay.getPreprocessor)
        outputHandler= output_handler.OutputHandler(modelSpace)
        outputHandler.outputStyle.cameraParameters= self.cameraParameters
        return outputHandler
        
    def displayDispRot(self, fileName= None):
        '''Displays the component of the displacement or rotations
           on the set argument.

        :param setToDisplay: set of elements to be displayed (defaults to total set)
        :param fileName: full name of the graphic file to generate. Defaults to 
               ` None`, in this case it returns a console output graphic.,
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        # Not a very elegant solution. Needs to be enhanced.
        outputHandler= self.getOutputHandler(self.setsToDispDspRot)
        for st in self.setsToDispDspRot:
            for arg in self.listDspRot:
                outputHandler.displayDispRot(itemToDisp=arg,setToDisplay=st,fileName= fileName)

    def displayIntForcDiag(self, itemToDisp, fileName=None, defFScale=0.0, orientScbar=1, overrideScaleFactor= None, titleScbar=None):
        '''displays the component of internal forces as a 
         diagram over lines (i.e. appropriated for beam elements).

        :param itemToDisp: component of the internal forces 
          ('N', 'Qy' (or 'Vy'), 'Qz' (or 'Vz'), 'My', 'Mz', 'T') to be depicted 
        :param unitDescription: string like '[kN/m] or [kN m/m]'
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        :param overrideScaleFactor: if not none, override the scale factor 
                                    in outputStyle.
        :param orientScbar: orientation of the scalar bar (defaults to 1-horiz)
        :param titleScbar: title for the scalar bar (defaults to None)
        '''
        outputHandler= self.getOutputHandler(self.setsToDispDspRot)
        for st in self.setsToDispIntForc:
            outputHandler.displayIntForcDiag(itemToDisp= itemToDisp,setToDisplay= st,fileName= fileName, defFScale= defFScale, overrideScaleFactor= overrideScaleFactor, orientScbar=orientScbar,titleScbar=titleScbar)

    def dispLoadCaseBeamEl(self, setToDisplay,caption= None,fileName=None,defFScale=0.0):
        '''Display the loads applied on beam elements and nodes for a given load case

        :param setToDisplay:    set of beam elements to be represented
        :param caption:   caption for the graphic
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                          in that case an screen display is generated
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        outputHandler= self.getOutputHandler(self.setsToDispDspRot)
        if(not caption):
          caption= 'load case: ' + self.getDescription() + ', set: ' + setToDisplay.name + ', '  + self.unitsLoads
        outputHandler.dispLoads(setToDisplay=setToDisplay,elLoadComp=self.compElLoad,caption= caption, fileName= fileName, defFScale= defFScale)

    def displayLoadOnSets(self, caption= None, fileName= None, defFScale= 0.0):
        '''Displays load vectors for each of the sets in self.setsToDispLoads

        :param caption: text to display in the graphic. Defaults to 
               ` None` in this case the text is the load case description
               and the units of the loads.
        :param fileName: full name of the graphic file to generate. Defaults to 
               ` None`, in this case it returns a console output graphic.,
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        outputHandler= self.getOutputHandler(self.setsToDispDspRot)
        for st in self.setsToDispLoads:
            outputHandler.displayLoads(setToDisplay= st,caption= caption,fileName= fileName,defFScale= defFScale)
        for st in self.setsToDispBeamLoads:
            outputHandler.displayLoads(setToDisplay=st, fileName= fileName, defFScale= defFScale)

    def displayReactionsOnSets(self, fileName=None, defFScale=0.0, inclInertia= False, reactionCheckTolerance= 1e-7):
        '''displays the reactions as vector on affected nodes

        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        :param inclInertia: include inertia effects (defaults to false).
        :param reactionCheckTolerance: relative tolerance when checking 
                                       reaction values.
        '''
        outputHandler= self.getOutputHandler(self.setsToDispDspRot)
        outputHandler.displayReactionsOnSets(setsToDisplayReactions= self.setsToDispReactions, fileName= fileName, defFScale= defFScale, inclInertia= inclInertia, reactionCheckTolerance= reactionCheckTolerance)
            
    def displayEigenvectorsOnSets(self, eigenMode, fileName=None,defFScale=0.0):
        '''displays the reactions as vector on affected nodes

        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        outputHandler= self.getOutputHandler(self.setsToDispEigenvectors[0])
        outputHandler.displayEigenvectorsOnSets(eigenMode,self.setsToDispEigenvectors, fileName,defFScale)

class LoadCaseDispParameters(RecordDisp):
    '''Parameters for the generation of graphic files and adding to 
       report-tex files for a load case

    :ivar loadCaseName:  name of the load case to be depicted
    :ivar loadCaseDescr: description text of the load case
    :ivar loadCaseExpr:  mathematical expression to define the load case (ex:
                     '1.0*GselfWeight+1.0*DeadLoad')
    '''

    def __init__(self,loadCaseName,loadCaseDescr,loadCaseExpr, setsToDispLoads, setsToDispDspRot, setsToDispIntForc):
        ''' Constructor.

        :param loadCaseName:  name of the load case to be depicted
        :param loadCaseDescr: description text of the load case
        :param loadCaseExpr:  mathematical expression to define the load 
                              case (ex: '1.0*GselfWeight+1.0*DeadLoad')
        :param setsToDispLoads: ordered list of sets of elements to display
                                loads.
        :param setsToDispDspRot: ordered list of sets of elements to display 
                                 displacements or rotations
        :param setsToDispIntForc: ordered list of sets of elements to 
                                  display internal forces.
        '''
        super(LoadCaseDispParameters,self).__init__(setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
        self.loadCaseName=loadCaseName
        self.loadCaseDescr=loadCaseDescr
        self.loadCaseExpr=loadCaseExpr

    def getDescription(self):
        retval= self.loadCaseDescr
        if(len(retval)==0):
          retval= self.loadCaseName
        return retval

    def getCaptionText(self,setDescr, unitsDescr, captTexts= None):
        ''' Return the caption text.

        :param setDescr: set description.
        :param unitsDescr: units description.
        :param captTexts: text describing the internal force...
        '''
        capt= self.getDescription()
        if(len(setDescr)>0):
            capt+= '. '  +  setDescr.capitalize()
        if(captTexts):
            capt+= ', ' + captTexts
        capt+= ', '  + unitsDescr
        return capt

    def writeLoadReport(self, modelSpace, texFile, cfg):
        '''Creates the graphics files of loads for the load case and insert
           them in a LaTex file

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        :param texFile:    laTex file where to include the graphics 
                           (e.g.:'text/report_loads.tex')
        :param cfg: instance of EnvConfig class with config parameters
        '''
        fullPath=cfg.projectDirTree.getReportLoadsGrPath()
        cfg.makedirs(fullPath) # Create directory if needed.
        rltvPath=cfg.projectDirTree.getRltvReportLoadsGrPath()
        # description= self.getDescription()
        FEcase= modelSpace.getProblem()
        if __debug__:
            if(not FEcase):
                AssertionError('Can\'t create XC finite element problem.')
        outputHandler= output_handler.OutputHandler(modelSpace)
        modelSpace.removeAllLoadPatternsFromDomain()
        modelSpace.revertToStart()
        modelSpace.addNewLoadCaseToDomain(self.loadCaseName,self.loadCaseExpr)
        # The disctinction between beam elements and the rest of elements
        # is to deprecate. The idea is to specify the type of output for all
        # the elements.
        for st in self.setsToDispLoads:
            fullgrfname= fullPath+self.loadCaseName+st.name
            rltvgrfname= rltvPath+self.loadCaseName+st.name
            capt= self.getCaptionText(setDescr= st.description, unitsDescr= self.unitsLoads)
            labl= getLabelText(capt)
            grFileName= fullgrfname+'.png'
            outputHandler.displayLoads(setToDisplay=st,caption= capt,fileName= grFileName)  # changed 22/06/2020
            #outputHandler.displayLoadVectors(setToDisplay=st,caption= capt,fileName=jpegFileName)
            output_handler.append_graphic_to_tex_file(texFile= texFile, graphicFileName= rltvgrfname, graphicWidth= cfg.grWidth, captionText= capt, label= labl)
        for st in self.setsToDispBeamLoads:
            fullgrfname= fullPath+self.loadCaseName+st.name
            rltvgrfname= rltvPath+self.loadCaseName+st.name
            capt= self.getCaptionText(setDescr= st.description, unitsDescr= self.unitsLoads)
            labl= getLabelText(capt)
            grFileName= fullgrfname+'.png'
            outputHandler.displayLoads(setToDisplay=st,caption= capt,fileName= grFileName)  # changed 22/06/2020
            output_handler.append_graphic_to_tex_file(texFile= texFile, graphicFileName= rltvgrfname, graphicWidth= cfg.grWidth, captionText= capt, label= labl)

    def loadReports(self,FEcase,texFile,cfg):
        '''Creates the graphics files of loads for the load case and insert 
        them in a LaTex file

        :param FEcase:     finite element problem 
        :param texFile:    laTex file where to include the graphics 
                           (e.g.:'text/report_loads.tex')
        :param cfg:        instance of EnvConfig class with config parameters
        '''
        preprocessor= FEcase.getPreprocessor
        modelSpace= predefined_spaces.getModelSpaceFromPreprocessor(preprocessor)
        self.writeLoadReport(modelSpace, texFile,cfg)

    def solveLC(self, modelSpace):
        '''Solve load case
        '''
        modelSpace.removeAllLoadPatternsFromDomain()
        modelSpace.revertToStart()
        modelSpace.addNewLoadCaseToDomain(self.loadCaseName,self.loadCaseExpr)
        #solve for load case
        modelSpace.analyze()
         

    def writeSimpleLoadCaseReport(self, modelSpace, texFile, cfg):
        '''Creates the graphics files of displacements and internal forces 
         calculated for a simple load case and insert them in a LaTex file

        :param modelSpace: model space object (see predefined_spaces.py).
        :param texFile:    laTex file where to include the graphics 
                           (e.g.:'text/report_loads.tex')
        :param cfg:        instance of EnvConfig class with config parameters
        '''
        fullPath= cfg.projectDirTree.getReportSimplLCGrPath()
        cfg.makedirs(fullPath) # Create directory if needed.
        rltvPath= cfg.projectDirTree.getRltvReportSimplLCGrPath()
        outputHandler= output_handler.OutputHandler(modelSpace)
        #Displacements and rotations displays
        for st in self.setsToDispDspRot:
            for arg in self.listDspRot:
                fullgrfname= fullPath+self.loadCaseName+st.name+arg
                rltvgrfname= rltvPath+self.loadCaseName+st.name+arg
                grFileName= fullgrfname+'.png'
                outputHandler.displayDispRot(itemToDisp=arg,setToDisplay=st,fileName= grFileName)
  #              unitConversionFactor, unDesc= cfg.outputStyle.getUnitParameters(arg)
                unitConversionFactor, unDesc= cfg.getUnitParameters(arg)
                 # if 'u' in arg:
                #     unDesc=cfg.getDisplacementUnitsDescription()
                # else:
                #     unDesc=cfg.getRotationUnitsDescription()
                capt= self.getCaptionText(setDescr= st.description, captTexts= cfg.capTexts[arg], unitsDescr= unDesc)
                output_handler.append_graphic_to_tex_file(texFile=texFile, graphicFileName= rltvgrfname, graphicWidth= cfg.grWidth, captionText= capt)
                
        # The disctinction between beam elements and the rest of elements
        # is to deprecate. The idea is to specify the type of output for all
        # the elements.
        #Internal forces displays on sets of «shell» elements
        for st in self.setsToDispIntForc:
            for arg in self.listIntForc:
                fullgrfname=fullPath+self.loadCaseName+st.name+arg
                rltvgrfname=rltvPath+self.loadCaseName+st.name+arg
                grFileName= fullgrfname+'.png'
                outputHandler.displayIntForc(itemToDisp=arg,setToDisplay=st,fileName= grFileName)
                capt= self.getCaptionText(setDescr= st.description, captTexts= cfg.capTexts[arg], unitsDescr= cfg.getForceUnitsDescription())
                output_handler.append_graphic_to_tex_file(texFile=texFile, graphicFileName= rltvgrfname, graphicWidth= cfg.grWidth, captionText= capt)
        #Internal forces displays on sets of «beam» elements
        for st in self.setsToDispBeamIntForc:
            for arg in self.listBeamIntForc:
                fullgrfname=fullPath+self.loadCaseName+st.name+arg
                rltvgrfname=rltvPath+self.loadCaseName+st.name+arg
                grFileName= fullgrfname+'.png'
                outputHandler.displayIntForcDiag(itemToDisp=arg,setToDisplay=st,fileName= grFileName,orientScbar=1,titleScbar=None)
                capt= self.getCaptionText(setDescr= st.description, captTexts= cfg.capTexts[arg], unitsDescr= cfg.getForceUnitsDescription())
                output_handler.append_graphic_to_tex_file(texFile=texFile, graphicFileName= rltvgrfname, graphicWidth= cfg.grWidth, captionText= capt)
        texFile.write('\\cleardoublepage\n')
        
    def simplLCReports(self,FEproblem,texFile,cfg):
        '''Creates the graphics files of displacements and internal forces 
         calculated for a simple load case and insert them in a LaTex file

        :param FEproblem:  finite element problem
        :param texFile:    laTex file where to include the graphics 
                           (e.g.:'text/report_loads.tex')
        :param cfg:        instance of EnvConfig class with config parameters
        '''
        preprocessor= FEproblem.getPreprocessor
        modelSpace= predefined_spaces.getModelSpaceFromPreprocessor(preprocessor)
        self.solveLC(modelSpace)
        self.writeSimpleLoadCaseReport(modelSpace, texFile,cfg)


def getLabelText(caption):
    ''' Return the text to label the figures removing
        spaces and strange chars from the input.'''
    retval= caption
    retval= retval.replace('+','pos').replace('-','neg')
    retval= su.slugify(retval)
    return retval

def getLoadCaseDispParametersFromLoadPattern(loadPattern, modelSpace= None, unitsScaleLoads= 1e-3, unitsScaleDispl= 1e-3, setsToDispLoads= None, setsToDispDspRot= None, setsToDispIntForc= None):
    domain= loadPattern.getDomain # Not always set.
    if(not domain and modelSpace):
        domain= modelSpace.preprocessor.getDomain
    if(not domain):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; can\'t get the domain from action: '+str(loadPattern.name)+' (domain not set). Could you provide the modelSpace parameter?')
        exit(1)
    if(domain):
        xcTotalSet= domain.getPreprocessor.getSets.getSet("total")
        if(not setsToDispLoads):
          setsToDispLoads= [xcTotalSet]
        if(not setsToDispDspRot):
          setsToDispDspRot= [xcTotalSet]
        if(not setsToDispIntForc):
          setsToDispIntForc= [xcTotalSet]
    name= loadPattern.name
    retval= LoadCaseDispParameters(name,loadPattern.description,'1.0*'+name,setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
    return retval
