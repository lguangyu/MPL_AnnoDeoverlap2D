#!/usr/bin/env python3

import sys
if sys.version_info < (3, 3, 0):
	raise SystemError("this package is not compatible with Python %s" %\
		sys.version)


from .text_wh import get_textobj_dimensions, get_text_dimensions
from .deoverlap import ode_deoverlap
