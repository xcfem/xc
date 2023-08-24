# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2023 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import re

def get_valid_filename(name):
    ''' Return a proper file name from the string argument.

    :param name: string to obtain the file name from.
    '''
    s = str(name).strip().replace(" ", "_")
    s = re.sub(r"(?u)[^-\w.]", "", s)
    if s in {"", ".", ".."}:
        raise SuspiciousFileOperation("Could not derive file name from '%s'" % name)
    return s

