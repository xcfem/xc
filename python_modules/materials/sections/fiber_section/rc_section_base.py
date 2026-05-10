# -*- coding: utf-8 -*-
''' RCSectionBase class.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import math
import geom
import matplotlib.pyplot as plt
from pathlib import Path
from misc.latex import latex_utils
from misc_utils import log_messages as lmsg
from materials.sections.fiber_section import plot_fiber_section as pfs
from materials.sections.fiber_section.rc_fiber_section_parameters import RCFiberSectionParameters
from materials.sections import section_properties
from postprocess.reports import common_formats as cf
from postprocess.reports import latex_reports as ltxr
import io # strstream Python equivalent

class RCSectionBase(object):
    '''
    Base class for reinforced concrete sections.

    :ivar sectionDescr: section description.
    :ivar fiberSectionParameters: Parameters needed to create a reinforced 
                                  concrete fiber section.
    :ivar fiberSectionRepr: fiber model of the section.
    '''
    def __init__(self, sectionDescr= None, concrType=None, reinfSteelType=None, nDivIJ= 10, nDivJK= 10):
        ''' Constructor.

        :param sectionDescr: section description.
        :param concrType: type of concrete (e.g. EHE_materials.HA25).     
        :param reinfSteelType: type of reinforcement steel.
        :param nDivIJ: number of cells in IJ (width or radial) direction.
        :param nDivJK: number of cells in JK (height or tangential) direction.
        '''
        if(sectionDescr is None):
            self.sectionDescr= 'Text describing the role/position of the section in the structure.'
        else:
            self.sectionDescr= sectionDescr
        self.fiberSectionParameters= RCFiberSectionParameters(concrType= concrType, reinfSteelType= reinfSteelType, nDivIJ= nDivIJ, nDivJK= nDivJK)
        self.fiberSectionRepr= None
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= dict()
        retval['section_description']= self.sectionDescr
        retval['fiber_section_parameters']= self.fiberSectionParameters
        retval['fiber_section_representation']= self.fiberSectionRepr
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        self.sectionDescr= dct['section_description']
        self.fiberSectionParameters= dct['fiber_section_parameters']
        self.fiberSectionRepr= dct['fiber_section_representation']

    def clear(self):
        ''' Clears all the members of this object.'''
        self.clearShearResponse()
        self.clearDiagrams()
        self.clearRCSection()
        self.sectionDescr= None
        self.fiberSectionParameters.clear()
        self.fiberSectionParameters= None
        if(self.fiberSectionRepr):
            self.fiberSectionRepr.clear()
            self.fiberSectionRepr= None
        
    @classmethod
    def newFromDict(cls, dct= None):
        ''' Builds a new object from the data in the given dictionary.

        :param cls: class of the object itself.
        :param dct: dictionary contaning the data.
        '''
        newObject = cls.__new__(cls) # just object.__new__
        if(dct):
            newObject.setFromDict(dct)
        return newObject
    
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.sectionDescr == other.sectionDescr)
                if(retval):
                    retval= (self.fiberSectionParameters == other.fiberSectionParameters)
                if(retval):
                    retval= (self.fiberSectionRepr == other.fiberSectionRepr)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def getCopy(self):
        ''' Returns a copy of the object.'''
        retval= RCSectionBase(sectionDescr= self.sectionDescr, concrType= self.getConcreteType(), reinfSteelType= self.getReinfSteelType(), nDivIJ= self.getNDivIJ(), nDivJK= self.getNDivJK())
        return retval

    def gmSectionName(self):
        ''' returns the name of the geometric section'''
        return "geom"+self.name

    def getMaterialHandler(self):
        ''' Return the material handler used to define the XC materials
            corresponding to this section object.'''
        return self.fiberSectionRepr.getMaterialHandler
        
    def getSectionGeometry(self):
        ''' Return the geometry defined for this section in XC.'''
        geomSectionName= self.gmSectionName()
        materialHandler= self.getMaterialHandler()
        retval= materialHandler.getSectionGeometry(geomSectionName)
        return retval
            
    def getConcreteType(self):
        ''' returns the concrete type of this sections.'''
        return self.fiberSectionParameters.concrType
    
    def getReinfSteelType(self):
        ''' returns the type of the reinforcing steel in this sections.'''
        return self.fiberSectionParameters.reinfSteelType

    def getElasticMaterialData(self, overrideRho= None):
        ''' Return an elastic material constitutive model.

        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return self.fiberSectionParameters.concrType.getElasticMaterialData(overrideRho= overrideRho)

    def getEcm(self):
        ''' Return the elastic modulus of the concrete.'''
        return self.fiberSectionParameters.concrType.Ecm()

    def getEs(self):
        ''' Return the elastic modulus of the concrete.'''
        return self.fiberSectionParameters.reinfSteelType.Es
    
    def getHomogenizationCoefficient(self):
        '''Return the homogenization coefficient of the section.'''
        return self.getEs()/self.getEcm()
    
    def izHomogenizedSection(self):
        '''Return the radius of gyration of the section around
           the axis parallel to the section width that passes 
           through section centroid.
        '''
        return math.sqrt(self.getIzHomogenizedSection()/self.getAreaHomogenizedSection())
    
    def getNonDimensionalAxialForce(self, Nd):
        ''' Return the valud of the non-dimensional axial force corresponding 
            to the given axial force according to clause 43.1.2 of EHE-08.

        :param Nd: design value of the axial force.
        '''
        fcd= self.fiberSectionParameters.concrType.fcd()
        Ac= self.getAc()
        return Nd/Ac/fcd
    
    def getNDivIJ(self):
        ''' Return the number of cells in IJ (width or radial) direction.'''
        return self.fiberSectionParameters.nDivIJ

    def getNDivJK(self):
        ''' Return the number of cells in JK (height or tangential) direction.'''
        return self.fiberSectionParameters.nDivJK
    
    def getConcreteDiagram(self, preprocessor):
        ''' Return the concrete stress-strain diagram.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return self.fiberSectionParameters.getConcreteDiagram(preprocessor)
      
    def getSteelDiagram(self, preprocessor):
        ''' Return the reinforcing steel stress-strain diagram.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return self.fiberSectionParameters.getSteelDiagram(preprocessor)
      
    def getSteelEquivalenceCoefficient(self, preprocessor):
        ''' Return the steel equivalence coefficient: Es/Ec.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return self.fiberSectionParameters.getSteelEquivalenceCoefficien(preprocessor)

    def defDiagrams(self, preprocessor, matDiagType):
        '''Stress-strain diagrams definition.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
        '''
        return self.fiberSectionParameters.defDiagrams(preprocessor, matDiagType)
    def clearDiagrams(self):
        '''Clear previously defined diagrams.'''
        return self.fiberSectionParameters.clearDiagrams()
    
    def defShearResponse2d(self, preprocessor):
        ''' Define the shear response of the 2D section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        self.respVy= self.getRespVy(preprocessor)
        
    def defShearResponse(self, preprocessor):
        ''' Define the shear/torsional response of the section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        self.respT= self.getRespT(preprocessor) # Torsional response of the section.
        self.respVy= self.getRespVy(preprocessor)
        self.respVz= self.getRespVz(preprocessor)
        
    def clearShearResponse(self):
        ''' Clear the shear/torsional response of the section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        self.respT= None # Torsional response of the section.
        self.respVy= None
        self.respVz= None

    def getFiberSectionName(self):
        ''' Return the name to assign to the fiber section created from the 
            data of this object.

        The suffix is added to the name of the fiber section materials
        to avoid collision with other materials (elastic sections, etc.)
        created from the same section definition object.
        '''
        return self.name+section_properties.fiber_section_name_suffix

    def defFiberSection2d(self, preprocessor):
        '''Define 2D fiber section from geometry data.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        fiberSectionName= self.getFiberSectionName()
        self.fiberSection= preprocessor.getMaterialHandler.newMaterial("fiberSectionShear2d", fiberSectionName)
        self.fiberSectionRepr= self.fiberSection.getFiberSectionRepr()
        self.fiberSectionRepr.setGeomNamed(self.gmSectionName())
        self.fiberSection.setupFibers()
        self.fiberSection.setRespVyByName(self.respVyName())
        self.fiberSection.setProp('sectionData',self)
        return fiberSectionName
        
    def defRCSection2d(self, preprocessor, matDiagType):
        ''' Definition of a 2D reinforced concrete section.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
         '''
        self.defShearResponse2d(preprocessor)
        self.defSectionGeometry(preprocessor,matDiagType, twoDimensional= True)
        return self.defFiberSection2d(preprocessor)

    def defFiberSection(self, preprocessor):
        '''Define fiber section from geometry data.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        fiberSectionName= self.getFiberSectionName()
        self.fiberSection= preprocessor.getMaterialHandler.newMaterial("fiberSectionShear3d", fiberSectionName)
        self.fiberSectionRepr= self.fiberSection.getFiberSectionRepr()
        self.fiberSectionRepr.setGeomNamed(self.gmSectionName())
        self.fiberSection.setupFibers()
        self.fiberSection.setRespVyByName(self.respVyName())
        self.fiberSection.setRespVzByName(self.respVzName())
        self.fiberSection.setRespTByName(self.respTName())
        self.fiberSection.setProp('sectionData',self)
        return fiberSectionName
        
    def clearFiberSection(self):
        '''Clear the previously defined fiber section.'''
        if(self.fiberSectionRepr):
            self.fiberSectionRepr.clear()
            self.fiberSectionRepr= None
            self.fiberSection.clear()
            self.fiberSection= None
        
    def defRCSection(self, preprocessor, matDiagType):
        ''' Definition of an XC reinforced concrete section.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
        '''
        self.defShearResponse(preprocessor= preprocessor)
        self.defSectionGeometry(preprocessor= preprocessor, matDiagType= matDiagType, twoDimensional= False)
        return self.defFiberSection(preprocessor= preprocessor)
        
    def clearRCSection(self):
        ''' Clear a previously defined XC reinforced concrete section (possibly
            with a different preprocessor, which can lead to errors).
        '''
        self.clearShearResponse()
        self.clearSectionGeometry()
        self.clearFiberSection()

    def isCircular(self):
        ''' Return true if it's a circular section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return False
        
    def getMainReinforcementArea(self):
        ''' Return the total area of the main reinforcement.'''
        return self.geomSection.getReinfLayers.getAreaGrossSection()
    
    def defInteractionDiagramParameters(self, preprocessor):
        ''' parameters for interaction diagrams.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return self.fiberSectionParameters.defInteractionDiagramParameters(preprocessor)

    def defInteractionDiagram(self, preprocessor, matDiagType= None):
        '''Defines 3D interaction diagram.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: if not None call defRCSection to create the fiber
                            model of this section.
        '''
        if(matDiagType):
            self.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)
        if(not self.fiberSectionRepr):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "Fiber section representation for section: "+ self.name + ";  not defined yet; use defRCSection (or defRCSection2d) method."
            lmsg.error(className+'.'+methodName+errMsg)
            exit(1)
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagram(self.getFiberSectionName(),self.fiberSectionParameters.idParams)

    def defInteractionDiagramNMy(self, preprocessor, matDiagType= None):
        '''Defines N-My interaction diagram.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: if not None call defRCSection to create the fiber
                            model of this section.
        '''
        if(matDiagType):
            self.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)
        if(not self.fiberSectionRepr):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "Fiber section representation for section: "+ self.name + ";  not defined yet; use defRCSection (or defRCSection2d) method."
            lmsg.error(className+'.'+methodName+errMsg)
            exit(1)
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagramNMy(self.getFiberSectionName(),self.fiberSectionParameters.idParams)

    def defInteractionDiagramNMz(self, preprocessor, matDiagType= None):
        '''Defines N-Mz interaction diagram.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: if not None call defRCSection to create the fiber
                            model of this section.
        '''
        if(matDiagType):
            self.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)
        if(not self.fiberSectionRepr):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "Fiber section representation for section: "+ self.name + ";  not defined yet; use defRCSection (or defRCSection2d) method."
            lmsg.error(className+'.'+methodName+errMsg)
            exit(1)
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagramNMz(self.getFiberSectionName(), self.fiberSectionParameters.idParams)

    def subplot(self, ax, preprocessor, matDiagType= 'k'):
        ''' Put the section drawing in the subplot argument.

        :param ax: matplotlib subplot.
        :param preprocessor: pre-processor of the finite element problem.
        :param matDiagType: type of stress-strain diagram
                     ("k" for characteristic diagram, "d" for design diagram)
        '''
        ax.axis('equal')
        ax.set_title('Section: '+self.name)
        ax.grid(visible= True, linestyle='dotted')
        # Plot contour.
        contour= self.getContour()
        x= list(); y= list()
        for p in contour:
            x.append(p.x)
            y.append(p.y)
        ax.fill(x,y,'tab:gray')
        #ax.plot(x,y,'tab:blue')
        # Plot reinforcement.
        if(not hasattr(self, 'geomSection')):
            self.defSectionGeometry(preprocessor, matDiagType)
        reinforcement= self.geomSection.getReinfLayers
        pfs.mplot_section_reinforcement(ax, reinforcement)

    def plot(self, preprocessor, matDiagType= 'k'):
        ''' Get a drawing of the section using matplotlib.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                            ("k" for characteristic diagram, 
                             "d" for design diagram)
        '''
        fig = plt.figure()
        ax = fig.add_subplot(111)
        self.subplot(ax, preprocessor= preprocessor, matDiagType= matDiagType)
        plt.show()
   
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.

        :param os: output stream.
        '''
        os.write(indentation+'Section description: '+str(self.sectionDescr)+'\n')
        indentation+= '  '
        os.write(indentation+'Fiber section parameters:\n')
        self.fiberSectionParameters.report(os, indentation+'  ')
        if(self.fiberSectionRepr):
            self.fiberSectionRepr.report(os, indentation+'  ')
            
    def latexReportMainReinforcementLayer(self, reinfLayer, concreteArea, os= sys.stdout):
        ''' Write a report of the reinforcement layer argument 
            in LaTeX format.

        :param os: output stream.
        '''
        name= reinfLayer.name
        nRebars= reinfLayer.numReinfBars # number of rebars
        rebarsDiam= reinfLayer.barDiameter # Rebars diameter
        areaRebar= reinfLayer.barArea # total area of reinforcement in the layer
        minEffCover= reinfLayer.getCover() # Minimum value of effective cover
        barsCOG= reinfLayer.getCenterOfMass() # center of gravity of the bars
        os.write(name+' & '+str(nRebars))
        os.write(' & '+str(round(rebarsDiam*1e3)))
        os.write(' & '+cf.fmt5_2f.format(areaRebar*1e4))
        os.write(' & '+cf.fmt4_2f.format(areaRebar/concreteArea*1e3))
        os.write(' & '+cf.fmt4_1f.format(minEffCover*1e2))
        os.write(' & '+cf.fmt5_3f.format(barsCOG[0]) +' & '+cf.fmt5_3f.format(barsCOG[1]) +"\\\\\n")

    def latexReportMainReinforcement(self, os= sys.stdout):
        ''' Write a report of the main reinforcement in LaTeX format.

        :param os: output stream.
        '''
        os.write("\\begin{tabular}{ll}\n")
        mainReinfFamilies= self.geomSection.getReinfLayers
        areaMainReinforcement= mainReinfFamilies.getAreaGrossSection()
        concreteArea= self.geomSection.getAreaGrossSection() # Area
        os.write("Total area $A_s="+cf.fmt5_2f.format(areaMainReinforcement*1e4) +"\\ cm^2$ & Geometric quantity $\\rho= "+cf.fmt4_2f.format(areaMainReinforcement/concreteArea*1e3) +"\\permil$\\\\\n")
        os.write("\\end{tabular} \\\\\n")
        os.write("\\hline\n")
        os.write("Layers of main reinforcement:\\\\\n")
        os.write("\\hline\n")
        os.write("\\begin{tabular}{cccccccc}\n")
        os.write("Id & N$^o$ bars & $\\phi$ & area & c. geom. & eff. cover & $y_{COG}$ & $z_{COG}$\\\\\n")
        os.write(" &  & $(mm)$ & $(cm^2)$ & $(\\permil)$ & $(cm)$ & $(m)$ & $(m)$\\\\\n")
        for f in mainReinfFamilies:
            os.write("\\hline\n")
            self.latexReportMainReinforcementLayer(f, concreteArea, os)
        os.write("\\end{tabular} \\\\\n")

    def getCrossSectionFigureFileName(self, outputPath= None):
        ''' Return the file name to use for the cross-section graphics.

        :param outputPath: directory to write the section plot into.
        '''
        retval= ''.join(x for x in self.name if x.isalnum())
        if(outputPath):
            retval= outputPath+'/'+retval
        return retval

    def latexReport(self, os= sys.stdout, graphicWidth='70mm', outputPath= None, includeGraphicsPath= None, preprocessor= None, matDiagType= 'k'):
        ''' Write a report of the object in LaTeX format.

        :param os: output stream.
        :param graphicWidth: width for the cross-section graphic.
        :param outputPath: directory to write the section plot into.
        :param includeGraphicsPath: directory to use in the latex includegraphics command.
        :param preprocessor: pre-processor of the FE problem.
        :param matDiagType: diagram type; if "k" use the diagram 
                            corresponding to characteristic values of the 
                            material, if "d" use the design values one.
        '''
        temporaryFiles= list()
        clearRCSection= False
        # Retrieve section geometry definition.
        if(not self.fiberSectionRepr):
            if(preprocessor):
                self.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)
                clearRCSection= True
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errMsg= "; no section representation for section: '"+self.name+"'. and undefined preprocessor. Can't call defRCSection."
                lmsg.error(className+'.'+methodName+errMsg)
        if(self.fiberSectionRepr):
            geomSection= self.getSectionGeometry()
            # Plot cross-section
            crossSectionFigureFName= self.getCrossSectionFigureFileName(outputPath= outputPath)
            if(self.geomSection):
                epsFileName= crossSectionFigureFName+'.eps'
                pfs.plot_section_geometry(geomSection,epsFileName)
                temporaryFiles.append(Path(epsFileName))
                # Convert the image to PNG
                pfs.eps2png(inputFileName= epsFileName, outputFileName= None)
                temporaryFiles.append(Path(epsFileName).with_suffix('.png'))
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; geometry of section: '+self.name+' not defined\n')

            # Some convenience lines.
            os.write('%% ****** Packages needed for LaTeX document: ****** \n')
            os.write('%%\\usepackage{graphicx} %%\\postscript includes\n')
            os.write('%%\\usepackage{multirow} %%\\multirow command\n')
            os.write('%%\\usepackage{wasysym} %%\\permil command\n')
            os.write('%%\\usepackage{gensymb} %%\\degree command\n')
            # Write table header.
            os.write('\\begin{table}\n')
            os.write('\\begin{center}\n')
            os.write('\\begin{tabular}{|c|}\n')
            os.write('\\hline\n')
            os.write('\\begin{large} '+latex_utils.toLaTeX(self.name)+' \\end{large}\\\\\n')
            os.write('\\hline\n')
            os.write(self.sectionDescr+'\\\\\n')
            os.write('\\hline\n')
            # Section geometry
            os.write('\\begin{tabular}{c|l}\n')
            ## Include figure in table
            os.write('\\begin{minipage}{85mm}\n')
            os.write('\\vspace{2mm}\n')
            os.write('\\begin{center}\n')

            #  name without extension to allow pdfLatex choose the file
            nameWOExt= Path(crossSectionFigureFName).stem
            if(includeGraphicsPath):
                nameWOExt= includeGraphicsPath+'/'+nameWOExt
            os.write('\\includegraphics[width='+graphicWidth+']{'+nameWOExt+'}\n')
            os.write('\\end{center}\n')
            os.write('\\vspace{1pt}\n')
            os.write('\\end{minipage} & \n')
            self.latexReportGeometry(os)
            os.write('\\end{tabular} \\\\\n')
            # Write materials.
            os.write('\\hline\n')
            os.write('\\textbf{Materials - mechanical properties}:\\\\\n')
            os.write('\\hline\n')
            os.write('\\begin{tabular}{ll}\n')
            concrete= self.fiberSectionParameters.concrType
            os.write('Concrete: '+concrete.materialName+' & Modulus of elasticity: $E_c= '+'{0:.2f}'.format(concrete.Ecm()/1e9)+'\\ GPa$\\\\\n')
            os.write('\\hline\n')
            steel= self.fiberSectionParameters.reinfSteelType
            os.write('Steel: '+steel.materialName+' & Modulus of elasticity: $E_s= '+'{0:.2f}'.format(steel.Es/1e9)+'\\ GPa$\\\\\n')
            os.write('\\end{tabular} \\\\\n')
            os.write('\\hline\n')
            # Write section mechanical properties.
            os.write('\\textbf{Sections - geometric and mechanical characteristics}:\\\\\n')
            os.write('\\hline\n')

            os.write('Gross section:\\\\\n')
            GB= self.geomSection.getCenterOfMassGrossSection() # Center of mass.
            AB= self.geomSection.getAreaGrossSection() # Area
            IyB= self.geomSection.getIyGrossSection() # Inertia
            IzB= self.geomSection.getIzGrossSection()
            PyzB= self.geomSection.getPyzGrossSection()
            JTorsion= self.getJTorsion()
            os.write('\\hline\n')
            os.write('\\begin{tabular}{ll}\n')
            os.write('$A_{gross}='+cf.fmt6_3f.format(AB) +'\\ m^2$ & \\multirow{3}{*}{Inertia tensor ($cm^4$): $ \\left( \\begin{array}{ccc}'+ cf.fmt5_2f.format(JTorsion*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ cf.fmt5_2f.format(IyB*1e4) +' & '+cf.fmt5_2f.format(PyzB) +' \\\\ 0.00 & '+cf.fmt5_2f.format(PyzB) +' & '+cf.fmt5_2f.format(IzB*1e4) +' \\end{array} \\right)$} \\\\\n')
            os.write('& \\\\\n')
            os.write('C.O.G.: $('+cf.fmt5_3f.format(GB[0])+','+cf.fmt5_3f.format(GB[1])+')\\ m$  & \\\\\n')
            os.write('\\end{tabular} \\\\\n')
            os.write('\\hline\n')
            os.write('Homogenized section:\\\\\n')
            preprocessor= self.getMaterialHandler().getPreprocessor
            tangConcr= self.getConcreteDiagram(preprocessor).getTangent()
            GH= self.geomSection.getCenterOfMassHomogenizedSection(tangConcr) # Center of gravity of the homogenized section
            AH= self.geomSection.getAreaHomogenizedSection(tangConcr) # Area of the homogenized section
            IyH= self.geomSection.getIyHomogenizedSection(tangConcr) # Inertia tensor of homogenized section.
            IzH=  self.geomSection.getIzHomogenizedSection(tangConcr)
            PyzH= self.geomSection.getPyzHomogenizedSection(tangConcr)

            os.write('\\hline\n')
            os.write('\\begin{tabular}{ll}\n')
            os.write('$A_{homog.}='+cf.fmt6_3f.format(AH) +'\\ m^2$ & \\multirow{3}{*}{Inertia tensor ($cm^4$): $ \\left( \\begin{array}{ccc}'+ cf.fmt5_2f.format(JTorsion*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ cf.fmt5_2f.format(IyH*1e4) +' & '+cf.fmt5_2f.format(PyzH) +' \\\\ 0.00 & '+cf.fmt5_2f.format(PyzH) +' & '+cf.fmt5_2f.format(IzH*1e4)+' \\end{array} \\right)$} \\\\\n')
            os.write('& \\\\\n')
            os.write('C.O.G.: $('+cf.fmt5_3f.format(GH[0])+','+cf.fmt5_3f.format(GH[1])+')\\ m$  & \\\\\n')
            os.write('\\end{tabular} \\\\\n')
            os.write('\\hline\n')
            # Passive reinforcement.
            os.write('\\textbf{Passive reinforcement}:\\\\\n')
            os.write('\\hline\n')
            ## Main reinforcement.
            self.latexReportMainReinforcement(os)
            ## Shear reinforcement.
            os.write('\\hline\n')
            os.write('Layers of shear reinforcement:\\\\\n')
            os.write('\\hline\n')
            os.write('\\begin{tabular}{cccccccc}\n')
            os.write('Id & N$^o$ branch & $\\phi$ & area & spac. & area/m & $\\alpha$ & $\\beta$\\\\\n')
            os.write(' &  & $(mm)$ & $(cm^2)$ & $(cm)$ & $(cm^2/m^2)$ & $( \\degree)$ & $( \\degree)$\\\\\n')
            self.latexReportShearReinforcement(os)
            os.write('\\end{tabular} \\\\\n')
            os.write('\\hline\n')
            os.write('\\end{tabular}\n')
            os.write('\\end{center}\n')
            os.write('\\caption{'+self.sectionDescr+' ('+ latex_utils.toLaTeX(self.name) +').'+'} \\label{tb_'+self.name.replace(' ','_')+'}\n')
            os.write('\\end{table}\n')
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "; no section representation for section: '"+self.name+"'. Can't create report. Have you called defRCSection (or defRCSection2d) method?"
            lmsg.error(className+'.'+methodName+errMsg)
        if(clearRCSection):
            self.clearRCSection()
        return temporaryFiles

    def pdfReport(self, outputFileName:str= None, graphicWidth='70mm', showPDF= False, keepPDF= True, preprocessor= None, matDiagType= 'k'):
        ''' Write a report of the object in LaTeX format.

        :param outputFileName: name of the output file.
        :param graphicWidth: width for the cross-section graphic.
        :param showPDF: if true display the PDF output on the screen.
        :param keepPDF: if true don't remove the PDF output.
        :param preprocessor: pre-processor of the FE problem.
        :param matDiagType: diagram type; if "k" use the diagram 
                            corresponding to characteristic values of the 
                            material, if "d" use the design values one.
        '''
        retval= None
        if(showPDF or keepPDF):
            if(not outputFileName):
                outputFileName= self.gmSectionName()+'.tex'
            outputPath= '/tmp/'
            ltxIOString= io.StringIO()
            temporaryFiles= self.latexReport(os= ltxIOString, graphicWidth= graphicWidth, outputPath= outputPath, includeGraphicsPath= outputPath, preprocessor= preprocessor, matDiagType= matDiagType)
            # Compile LaTeX document.
            if(temporaryFiles):
                ltxIOString.seek(0)
                ltxString= ltxr.rc_section_report_latex_header+str(ltxIOString.read())+ltxr.rc_section_report_latex_tail
                ltxIOString.close()
                pdfFile= ltxr.latex_string_to_pdf(texString= str(ltxString), outputFileName= outputFileName, showPDF= showPDF)
                # Remove temporary files
                ## cross-section graphics.
                for f in temporaryFiles:
                    f.unlink()
                ## LaTeX source file
                Path(outputPath+outputFileName).unlink()
                if(showPDF):
                    input("Press Enter to continue...")
                if(keepPDF): # return the PDF file name for later use.
                    retval= pdfFile
                else: # remove PDF file.
                    if os.path.exists(pdfFile):
                        os.remove(pdfFile)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; latexReport returned nothing.')                     
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; both showPDF and keepPDF are false; nothing to do.')
        return retval
        
    def writeDXF(self, modelSpace, concreteLayerName= 'concrete', reinforcementLayerName= 'reinforcement'):
        ''' Writes the shape contour in the model
            space argument.

        :param modelSpace: ezdxf model space to write into.
        :param concretLayerName: DXF layer name for concrete material.
        :param reinforcementLayerName: DXF layer name for steel material.
        '''
        # Draw contour
        # Retrieve section geometry definition.
        if(self.fiberSectionRepr):
            geomSection= self.getSectionGeometry()
            write_dxf(geomSection= geomSection, modelSpace= modelSpace, concreteLayerName= concreteLayerName, reinforcementLayerName= reinforcementLayerName)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; no fiber section representation. Have you called defRCSection (or defRCSection2d) method?')
            
def write_dxf(geomSection, modelSpace, concreteLayerName= 'concrete', reinforcementLayerName= 'reinforcement'):
    ''' Writes the shape contour in the given DXF model space.

    :param modelSpace: ezdxf model space to write into.
    :param concretLayerName: DXF layer name for concrete material.
    :param reinforcementLayerName: DXF layer name for steel material.
    '''
    regions= geomSection.getRegions
    for r in regions:
        vertices= r.getPolygon().getVertexList()
        points= list()
        for v in vertices:
            points.append((v.x,v.y,0.0))
        points.append(points[0]) # close region.
        modelSpace.add_lwpolyline(points, dxfattribs={"layer": concreteLayerName})
    # Draw reinforcement.
    reinforcement= geomSection.getReinfLayers
    for reinfLayer in reinforcement:
        rebars= reinfLayer.getReinfBars
        for b in rebars:
            ptPlot= b.getPos2d
            rPlot= b.diameter/2.0
            #labelPlot= str(int(round(b.diameter*1e3)))
            modelSpace.add_circle(center= (ptPlot.x, ptPlot.y), radius= rPlot, dxfattribs={"layer": reinforcementLayerName})
