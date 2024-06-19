# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import ezdxf
from postprocess.reports import graph_material 
from misc_utils import log_messages as lmsg

class RCSectionReportGenerator(object):
    ''' Reinforced concrete section report generator.

    :ivar graphicOutputRelPath: relative path of the graphic files.
    :ivar graphicOutputPath: output path for the graphic files.
    :ivar reportFileName: name of the report output file.
    '''
    def __init__(self, cfg):
        ''' Constructor.

        :param cfg: environment configuration.
        '''
        self.graphicOutputPath= cfg.projectDirTree.getReportSectionsGrPath()
        self.graphicOutputRelPath= cfg.projectDirTree.getRltvReportSectionsGrPath()
        self.reportFileName= cfg.projectDirTree.getReportSectionsFile()

    def dxfExport(self, rcSection):
        ''' Create a DXF drawing of the section.

        :param rcSection: reinforced concrete section to draw.
        '''
        dxfFileName= rcSection.name+'.dxf'
        lmsg.log(dxfFileName)
        doc = ezdxf.new("R2000")
        msp = doc.modelspace()
        rcSection.writeDXF(modelSpace= msp)
        doc.saveas(self.graphicOutputPath+'/'+dxfFileName)

    @staticmethod
    def plotInteractionDiag(diag, title, xAxLab, yAxLab, grFileNm, reportFile, grRltvFileNm):
        ''' Plot 2D interaction diagram of the section.

        :param diag: interaction diagram.
        :param xAxLab: label for the x axis.
        :param yAxLab: label for the y axis.
        :param grFileNm: name of the file containing the interaction diagram graphic.
        :param reportFile: name of the LaTeX file where the figure will be included.
        :param grRltvFileNm: relative path of the graphic file.
        '''
        diagGraphic= graph_material.InteractionDiagramGraphic(title)
        diagGraphic.decorations.xLabel= xAxLab
        diagGraphic.decorations.yLabel= yAxLab
        diagGraphic.setupGraphic(diag)
        # diagGraphic.savefig(grFileNm+'.eps')
        diagGraphic.savefig(grFileNm+'.jpeg')
        reportFile.write('\\begin{center}\n')
        reportFile.write('\includegraphics[width=80mm]{'+grRltvFileNm+'}\n')
        reportFile.write('\end{center}\n')

    def plotMaterialDiagram(self, preprocessor, materialType, reportFile):
        ''' Plot the stress strain diagram of the given material type.

        :param preprocessor: pre-processor of the FE problem.
        :param materialType: type of the material to plot the diagram for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        '''
        materialDiag= materialType.plotDesignStressStrainDiagram(preprocessor= preprocessor, path= self.graphicOutputPath)
        materialGrphFileName= materialType.materialName+'_design_stress_strain_diagram'
        reportFile.write('\\begin{center}\n')
        reportFile.write('\includegraphics[width=120mm]{'+self.graphicOutputRelPath+materialGrphFileName+'}\n')
        reportFile.write('\end{center}\n')

    def plotSteelDiagram(self, preprocessor, steelType, reportFile):
        ''' Plot the stress strain diagram of the reinforcing steel.

        :param preprocessor: pre-processor of the FE problem.
        :param steelType: type of the steel to plot the diagram for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        '''
        self.plotMaterialDiagram(preprocessor= preprocessor, materialType= steelType, reportFile= reportFile)


    def plotConcreteDiagram(self, preprocessor, concreteType, reportFile):
        ''' Plot the stress strain diagram of the concrete.

        :param preprocessor: pre-processor of the FE problem.
        :param concreteType: type of the concrete to plot the diagram for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        :param outputRelPath: relative path of the graphic file.
        '''
        self.plotMaterialDiagram(preprocessor= preprocessor, materialType= concreteType, reportFile= reportFile)

    def plotMaterialsDiagramsIfNew(self, preprocessor, newRCSection, oldConcreteType, oldSteelType, reportFile):
        ''' Plot the stress strain diagram of the concrete.

        :param preprocessor: pre-processor of the FE problem.
        :param newRCSection: new reinforced concrete section to report.
        :param oldConcreteType: type of the last plotted concrete diagram.
        :param oldSteelType: type of the last plotted steel diagram.
        :param reportFile: name of the LaTeX file where the figure will be included.
        '''
        newSteelType= newRCSection.fiberSectionParameters.reinfSteelType
        newConcreteType= newRCSection.fiberSectionParameters.concrType
        if newSteelType != oldSteelType or newConcreteType != oldConcreteType:
            self.plotSteelDiagram(preprocessor= preprocessor, steelType= newSteelType, reportFile= reportFile)
            self.plotConcreteDiagram(preprocessor= preprocessor, concreteType= newConcreteType, reportFile= reportFile)
            return newConcreteType, newSteelType
        else:
            return oldConcreteType, oldSteelType
        reportFile.write('\\newpage\n\n')

    def rcSectionLatexReport(self, preprocessor, rcSection, reportFile):
        ''' Write the RC section report in LaTeX format.

        :param preprocessor: pre-processor of the FE problem.
        :param rcSection: reinforced concrete section to write the report for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        '''
        # plotting of section geometric and mechanical properties
        rcSection.latexReport(os= reportFile, outputPath= self.graphicOutputPath, includeGraphicsPath= self.graphicOutputRelPath)
        # plotting of interaction diagrams
        diagNMy= rcSection.defInteractionDiagramNMy(preprocessor)
        grFileName= self.graphicOutputPath+rcSection.name+'NMy'
        grRltvFileName= self.graphicOutputRelPath+rcSection.name+'NMy'
        self.plotInteractionDiag(diag= diagNMy, title= rcSection.name+ ' N-My interaction diagram',xAxLab='My [kNm]',yAxLab='N [kN]', grFileNm=grFileName, reportFile= reportFile, grRltvFileNm= grRltvFileName)
        diagNMz= rcSection.defInteractionDiagramNMz(preprocessor)
        grFileName= self.graphicOutputPath+rcSection.name+'NMz'
        grRltvFileName= self.graphicOutputRelPath+rcSection.name+'NMz'
        self.plotInteractionDiag(diag=diagNMz,title=rcSection.name+ ' N-Mz interaction diagram',xAxLab='Mz [kNm]',yAxLab='N [kN]',grFileNm=grFileName,reportFile= reportFile,grRltvFileNm=grRltvFileName)

    def rcSectionsReport(self, preprocessor, rcSectionsToReport, reportFile, writeSection2= True):
        ''' Write the RC section report in LaTeX format.

        :param preprocessor: pre-processor of the FE problem.
        :param rcSection: reinforced concrete section to write the report for.
        :param reportFile: name of the LaTeX file where the figure will be included.
        :param writeSection2: if true, also report the 2nd section.
        '''
        lastSteel= None
        lastConcrete= None
        #for sect in sections.sections:
        for sect in rcSectionsToReport:
            sect.createSections()
            sect1=sect.lstRCSects[0]
            sect1.defRCSection(preprocessor,'d')
            self.dxfExport(sect1)
            if writeSection2:
                sect2= sect.lstRCSects[1]
                sect2.defRCSection(preprocessor,'d')
                self.dxfExport(sect2)
            # plotting of steel stress-strain diagram (only if not equal to precedent steel)
            lastConcrete, lastSteel= self.plotMaterialsDiagramsIfNew(preprocessor= preprocessor, newRCSection= sect1, oldConcreteType= lastConcrete, oldSteelType= lastSteel, reportFile= reportFile)
            # Section 1
            # plotting of section geometric and mechanical properties
            self.rcSectionLatexReport(preprocessor= preprocessor, rcSection= sect1, reportFile= reportFile)
            if writeSection2:
                # Section 2
                # plotting of section geometric and mechanical properties
                self.rcSectionLatexReport(preprocessor= preprocessor, rcSection= sect2, reportFile= reportFile)
