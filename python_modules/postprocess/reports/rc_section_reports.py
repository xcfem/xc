# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import os
import sys
import tempfile
import ezdxf
from postprocess.reports import graph_material 
from postprocess.reports import latex_reports as ltxr
from misc_utils import log_messages as lmsg
import io # strstream Python equivalent
from pathlib import Path

class RCSectionReportGenerator(object):
    ''' Reinforced concrete section report generator.

    :ivar cfg: environment configuration.
    '''
    def __init__(self, cfg):
        ''' Constructor.

        :param cfg: environment configuration.
        '''
        self.cfg= cfg
        self.tempDir= None

    def _get_temporary_directory(self):
        ''' Return a temporary directory.'''
        if(self.tempDir is None):
            self.tempDir= tempfile.TemporaryDirectory()
        return self.tempDir.name+'/'

    def _get_graphic_output_path(self, tmp):
        ''' Return the graphic output path.

        :param tmp: if true return a temporary graphic output path.
        '''
        if(tmp):
            return self._get_temporary_directory()
        else:
            return self.cfg.projectDirTree.getReportSectionsGrPath()
        
    def _get_graphic_output_relative_path(self, tmp):
        ''' Return the graphic output path.

        :param tmp: if true return a temporary graphic output path.
        '''
        if(tmp):
            return  self._get_temporary_directory()
        else:
            return self.cfg.projectDirTree.getRltvReportSectionsGrPath()

    def _get_report_file_name(self, tmp):
        ''' Return the report file name.

        :param tmp: if true return a temporary output path.
        '''
        retval= self.cfg.projectDirTree.getReportSectionsFile()
        if(tmp):
            retval= os.path.basename(retval)
        return retval

    def dxfExport(self, rcSection, tmp):
        ''' Create a DXF drawing of the section.

        :param rcSection: reinforced concrete section to draw.
        :param tmp: if true write the file in a temporary folder.
        '''
        temporaryFiles= list()
        dxfFileName= rcSection.name+'.dxf'
        lmsg.log(dxfFileName)
        doc = ezdxf.new("R2000")
        msp = doc.modelspace()
        rcSection.writeDXF(modelSpace= msp)
        outputFile= self._get_graphic_output_path(tmp= tmp)+dxfFileName
        doc.saveas(outputFile)
        temporaryFiles.append(Path(outputFile))
        return temporaryFiles

    def getInteractionDiagramFileName(self, sectionName, orientation, relativePath, tmp):
        ''' Return the file name for the interaction diagram graphic file.

        :param sectionName: name of the section corresponding to the diagram.
        :param orientation: 'NMy' or 'NMz'
        :param relativePath: if true return the relative path to the file.
        :param tmp: if true write the file in a temporary folder.
        '''
        if(relativePath):
            retval= self._get_graphic_output_relative_path(tmp= tmp)
        else:
            retval= self._get_graphic_output_path(tmp= tmp)
        retval+= sectionName+orientation
        return retval
    
    def plotInteractionDiag(self, diag, reportFile, sectionName, orientation, tmp):
        ''' Plot 2D interaction diagram of the section.

        :param diag: interaction diagram.
        :param reportFile: name of the LaTeX file where the figure will be included.
        :param sectionName: name of the section corresponding to the diagram.
        :param orientation: 'NMy' or 'NMz'
        :param tmp: if true write the file in a temporary folder.
        '''
        axial= orientation[0]
        bending= orientation[1:]
        title= sectionName+' '+axial+'-'+bending+' interaction diagram'
        diagGraphic= graph_material.InteractionDiagramGraphic(title)
        diagGraphic.decorations.xLabel= bending+' [kN.m]'
        diagGraphic.decorations.yLabel= axial+' [kN]'
        diagGraphic.setupGraphic(diag)
        # diagGraphic.savefig(grFileNm+'.eps')
        grFileNm= self.getInteractionDiagramFileName(sectionName= sectionName, orientation= orientation, relativePath= False, tmp= tmp)+'.jpeg'
        diagGraphic.savefig(grFileNm)
        temporaryFiles= [Path(grFileNm)]
        reportFile.write('\\begin{center}\n')
        grRltvFileNm= self.getInteractionDiagramFileName(sectionName= sectionName, orientation= orientation, relativePath= True, tmp= tmp)
        reportFile.write('\\includegraphics[width=80mm]{'+grRltvFileNm+'}\n')
        reportFile.write('\\end{center}\n')
        return temporaryFiles

    def plotMaterialDiagram(self, preprocessor, materialType, reportFile, tmp):
        ''' Plot the stress strain diagram of the given material type.

        :param preprocessor: pre-processor of the FE problem.
        :param materialType: type of the material to plot the diagram for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        :param tmp: if true write the file in a temporary folder.
        '''
        pth= self._get_graphic_output_path(tmp= tmp)
        materialDiag= materialType.plotDesignStressStrainDiagram(preprocessor= preprocessor, path= pth)
        temporaryFiles= [Path(materialType.getDesignStressStrainDiagramFileName(path= pth, withExtension= True))]
        materialGrphFileName= materialType.getDesignStressStrainDiagramFileName(path= '', withExtension= False)
        reportFile.write('\\begin{center}\n')
        reportFile.write('\\includegraphics[width=120mm]{'+self._get_graphic_output_relative_path(tmp= tmp)+materialGrphFileName+'}\n')
        reportFile.write('\\end{center}\n')
        return temporaryFiles

    def plotSteelDiagram(self, preprocessor, steelType, reportFile, tmp):
        ''' Plot the stress strain diagram of the reinforcing steel.

        :param preprocessor: pre-processor of the FE problem.
        :param steelType: type of the steel to plot the diagram for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        :param tmp: if true write the file in a temporary folder.
        '''
        return self.plotMaterialDiagram(preprocessor= preprocessor, materialType= steelType, reportFile= reportFile, tmp= tmp)


    def plotConcreteDiagram(self, preprocessor, concreteType, reportFile, tmp):
        ''' Plot the stress strain diagram of the concrete.

        :param preprocessor: pre-processor of the FE problem.
        :param concreteType: type of the concrete to plot the diagram for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        :param outputRelPath: relative path of the graphic file.
        :param tmp: if true write the file in a temporary folder.
        '''
        return self.plotMaterialDiagram(preprocessor= preprocessor, materialType= concreteType, reportFile= reportFile, tmp= tmp)

    def plotMaterialsDiagramsIfNew(self, preprocessor, newRCSection, oldConcreteType, oldSteelType, reportFile, tmp):
        ''' Plot the stress strain diagram of the concrete.

        :param preprocessor: pre-processor of the FE problem.
        :param newRCSection: new reinforced concrete section to report.
        :param oldConcreteType: type of the last plotted concrete diagram.
        :param oldSteelType: type of the last plotted steel diagram.
        :param reportFile: name of the LaTeX file where the figure will be 
                           included.
        :param tmp: if true write the file in a temporary folder.
        '''
        temporaryFiles= list()
        newSteelType= newRCSection.fiberSectionParameters.reinfSteelType
        newConcreteType= newRCSection.fiberSectionParameters.concrType
        if newSteelType != oldSteelType or newConcreteType != oldConcreteType:
            temporaryFiles.extend(self.plotSteelDiagram(preprocessor= preprocessor, steelType= newSteelType, reportFile= reportFile, tmp= tmp))
            temporaryFiles.extend(self.plotConcreteDiagram(preprocessor= preprocessor, concreteType= newConcreteType, reportFile= reportFile, tmp= tmp))
            retval= (newConcreteType, newSteelType, temporaryFiles)
        else:
            retval= (oldConcreteType, oldSteelType, temporaryFiles)
        reportFile.write('\\newpage\n\n')
        return retval[0], retval[1], retval[2]

    
    def rcSectionLatexReport(self, preprocessor, rcSection, reportFile, tmp):
        ''' Write the RC section report in LaTeX format.

        :param preprocessor: pre-processor of the FE problem.
        :param rcSection: reinforced concrete section to write the report for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        :param tmp: if true write the file in a temporary folder.
        '''
        temporaryFiles= list()
        # plotting of section geometric and mechanical properties
        graphicOutputPath= self._get_graphic_output_path(tmp= tmp)
        graphicOutputRelPath= self._get_graphic_output_relative_path(tmp= tmp)
        temporaryFiles.extend(rcSection.latexReport(os= reportFile, outputPath= graphicOutputPath, includeGraphicsPath= graphicOutputRelPath))
        # plotting of interaction diagrams
        diagNMy= rcSection.defInteractionDiagramNMy(preprocessor)
        temporaryFiles.extend(self.plotInteractionDiag(diag= diagNMy, reportFile= reportFile, sectionName= rcSection.name, orientation= 'NMy', tmp= tmp))
        diagNMz= rcSection.defInteractionDiagramNMz(preprocessor)
        temporaryFiles.extend(self.plotInteractionDiag(diag= diagNMz, reportFile= reportFile, sectionName= rcSection.name, orientation= 'NMz', tmp= tmp))
        return temporaryFiles

    def rc_sections_latex_report(self, outputFile, preprocessor, rcSectionsToReport, writeSection2= True, matDiagType= 'd', tmp= False):
        ''' Write the RC section report in LaTeX format.

        :param outputFile: output file.
        :param preprocessor: pre-processor of the FE problem.
        :param rcSection: reinforced concrete section to write the report for.
        :param writeSection2: if true, also report the 2nd section.
        :param tmp: if true write the file in a temporary folder.
        '''
        temporaryFiles= list()
        lastSteel= None
        lastConcrete= None
        #for sect in sections.sections:
        for sect in rcSectionsToReport:
            sect.createSections()
            sect1=sect.lstRCSects[0]
            sect1.defRCSection(preprocessor, matDiagType)
            temporaryFiles.extend(self.dxfExport(sect1, tmp= tmp))
            if writeSection2:
                sect2= sect.lstRCSects[1]
                sect2.defRCSection(preprocessor, matDiagType)
                temporaryFiles.extend(self.dxfExport(sect2, tmp= tmp))
            # plotting of steel stress-strain diagram (only if not equal to precedent steel)
            lastConcrete, lastSteel, tmpFiles= self.plotMaterialsDiagramsIfNew(preprocessor= preprocessor, newRCSection= sect1, oldConcreteType= lastConcrete, oldSteelType= lastSteel, reportFile= outputFile, tmp= tmp)
            temporaryFiles.extend(tmpFiles)
            # Section 1
            # plotting of section geometric and mechanical properties
            temporaryFiles.extend(self.rcSectionLatexReport(preprocessor= preprocessor, rcSection= sect1, reportFile= outputFile, tmp= tmp))
            if writeSection2:
                # Section 2
                # plotting of section geometric and mechanical properties
                temporaryFiles.extend(self.rcSectionLatexReport(preprocessor= preprocessor, rcSection= sect2, reportFile= outputFile, tmp= tmp))
        return temporaryFiles

    def latexReport(self, preprocessor, rcSectionsToReport, writeSection2= True, matDiagType= 'd'):
        ''' Write the RC section report in LaTeX format.

        :param preprocessor: pre-processor of the FE problem.
        :param rcSection: reinforced concrete section to write the report for.
        :param writeSection2: if true, also report the 2nd section.
        :param matDiagType: type of stress-strain diagram 
                            ("k" for characteristic diagram, "d" for design
                            diagram).
        '''
        with open(self._get_report_file_name(tmp= False),'w') as reportFile:
            temporaryFiles= self.rc_sections_latex_report(outputFile= reportFile, preprocessor= preprocessor, rcSectionsToReport= rcSectionsToReport, writeSection2= writeSection2, matDiagType= matDiagType)
        return temporaryFiles

    def pdfReport(self, preprocessor, rcSectionsToReport, writeSection2= True, showPDF= False, keepPDF= True, matDiagType= 'd'):
        ''' Write the RC section report in LaTeX format.

        :param preprocessor: pre-processor of the FE problem.
        :param rcSection: reinforced concrete section to write the report for.
        :param writeSection2: if true, also report the 2nd section.
        :param showPDF: if true display the PDF output on the screen.
        :param keepPDF: if true don't remove the PDF output.
        :param matDiagType: type of stress-strain diagram 
                            ("k" for characteristic diagram, "d" for design
                            diagram).
        '''
        ltxIOString= io.StringIO()
        temporaryFiles= self.rc_sections_latex_report(outputFile= ltxIOString, preprocessor= preprocessor, rcSectionsToReport= rcSectionsToReport, writeSection2= writeSection2, matDiagType= matDiagType, tmp= True)
        ltxIOString.seek(0)
        ltxString= ltxr.rc_section_report_latex_header+str(ltxIOString.read())+ltxr.rc_section_report_latex_tail
        ltxIOString.close()
        outputFileName= self._get_report_file_name(tmp= True)
        pdfFile= ltxr.latex_string_to_pdf(texString= str(ltxString), outputFileName= outputFileName, showPDF= showPDF, tmpDir= self._get_temporary_directory())
        # Remove temporary files
        ## cross-section graphics.
        for f in temporaryFiles:
            f.unlink()
        ## LaTeX source file
        Path(self._get_temporary_directory()+outputFileName).unlink()
        if(showPDF):
            input("Press Enter to continue...")
        retval= pdfFile
        if(not keepPDF): # remove PDF file.
            if os.path.exists(pdfFile):
                os.remove(pdfFile)
                retval= None
        return retval
   
    def rcSectionsReport(self, preprocessor, rcSectionsToReport, writeSection2= True, matDiagType= 'd'):
        ''' Write the RC section report in LaTeX format.

        :param preprocessor: pre-processor of the FE problem.
        :param rcSection: reinforced concrete section to write the report for.
        :param writeSection2: if true, also report the 2nd section.
        :param matDiagType: type of stress-strain diagram 
                            ("k" for characteristic diagram, "d" for design
                            diagram).
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        warningMsg= '; this method will be DEPRECATED soon use latexReport instead.'
        lmsg.warning(className+'.'+methodName+warningMsg)
        self.latexReport(preprocessor= preprocessor, rcSectionsToReport= rcSectionsToReport, writeSection2= writeSection2, matDiagType= matDiagType)

