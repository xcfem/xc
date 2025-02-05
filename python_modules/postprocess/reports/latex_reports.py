# -*- coding: utf-8 -*-
''' Headers and tails for reports in LaTeX format.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
from pathlib import Path
import shutil
import subprocess
from misc_utils import log_messages as lmsg

latex_aux_extensions= ['.aux', '.log', '.out', 
                       '.bbl', '.blg', '.bcf', # Leftovers from bibtex
                       '.glg', '.gls', '.glo', # Leftovers from glossaries  
                       '.lof', '.lot', '.toc', # Remove leftovers from lists
                       '.mtc*', '.maf', # Remove leftovers from minitoc
                       '.run.xml', # Remove other stuff
                       '.acn', '.acr', '.alg', 
                       '.ist', '.synctex*', '.alg']

def get_latex_aux_posix_paths(texFileName):
    ''' Get LaTeX auxiliary file names from the given name.

    :param texFileName: name of the TeX file.
    '''
    pth= Path(texFileName)
    retval= list()
    for ext in latex_aux_extensions:
        retval.append(pth.with_suffix(ext))
    return retval
  
def latex_string_to_pdf(texString:str, outputFileName, showPDF= False, targetDir= './', tmpDir= '/tmp'):
    ''' Converts a LaTeX string into a PDF file.

    :param outputFileName: name of the PDF output file.
    :param showPdf: if true display the PDF file on the screen.
    '''
    output_directory_path= tmpDir
    outputFilePath= Path(outputFileName)
    outputBaseName= outputFilePath.stem
    outputFileName= output_directory_path+'/'+outputFileName

    # Write LaTeX file.
    with open(outputFileName, "w") as tex_file:
        tex_file.write(texString)

    # Run pdflatex
    output_directory_opt= '--output-directory='+output_directory_path#+' -interaction=nonstopmode'
    tmp= subprocess.run(["pdflatex", output_directory_opt, outputFileName], stdout= subprocess.DEVNULL,  stderr=subprocess.STDOUT)
    if(tmp.returncode!=0):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; LaTeX compilation crashed.')
        exit(1)
    tmp= subprocess.run(["pdflatex", output_directory_opt, outputFileName], stdout= subprocess.DEVNULL,  stderr=subprocess.STDOUT)
    if(tmp.returncode!=0):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; LaTeX compilation crashed.')
        exit(1)
    tmp= subprocess.run(["pdflatex", output_directory_opt, outputFileName], stdout= subprocess.DEVNULL,  stderr=subprocess.STDOUT)
    if(tmp.returncode!=0):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; LaTeX compilation crashed.')
        exit(1)
    # Remove auxiliary files.
    latex_aux_posix_paths= get_latex_aux_posix_paths(outputFileName)
    for pth in latex_aux_posix_paths:
        pth.unlink(missing_ok=True)
    # Move PDF file to target directory.
    pdf_pth= Path(output_directory_path+'/'+outputBaseName+'.pdf')
    target_pth= Path(targetDir+'/'+outputBaseName+'.pdf')
    shutil.move(pdf_pth, target_pth)
    # pdf_pth.rename(target_pth)
    filepath= str(target_pth)
    if(showPDF):
        subprocess.run(('xdg-open', filepath))
    return filepath


concrete02_report_latex_header= '''\\documentclass{article} \n
\\usepackage{graphicx} %%\\postscript includes\n
\\usepackage{multirow} %%\\multirow command\n
\\usepackage{wasysym} %%\\permil command\n
\\usepackage{gensymb} %%\\degree command\n
\\usepackage{gensymb} \n
\\begin{document} \n
\\begin{table} \n
\\begin{center} \n
\\begin{tabular}{ll} \n
\\begin{minipage}{95mm} \n
\\vspace{2mm} \n
\\begin{center} \n
\\includegraphics[width=90mm]{'+grFileNameNoPath+'} \n
\\end{center} \n
\\vspace{1pt} \n
\\end{minipage} & \n
\\begin{tabular}{lr} \n
''' 
concrete02_report_latex_tail= '''\\end{tabular} \n
\\end{tabular} \n
\\end{center} \n
\\end{table} \n
\\end{document} \n
'''

rc_section_report_latex_header= '''\\documentclass{article} \n
\\usepackage{graphicx} %%\\postscript includes\n
\\usepackage{multirow} %%\\multirow command\n
\\usepackage{wasysym} %%\\permil command\n
\\usepackage{gensymb} %%\\degree command\n
\\begin{document} \n
''' 

rc_section_report_latex_tail= '''\\end{document} \n'''
