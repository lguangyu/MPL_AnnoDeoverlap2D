#!/usr/bin/env python3

import numpy as _numpy
from . import _deoverlap_croutine


def ode_deoverlap(
		text_xys: _numpy.ndarray,
		text_dims: _numpy.ndarray,
		*,
		anno_xys: _numpy.ndarray = None,
		clip_box: _numpy.ndarray = None,
		text_repel_force: float = 1.0,
		anno_repel_force: float = 1.0,
		anno_attract_force: float = 1.0,
		init_noise: float = 0.0
	) -> _numpy.ndarray:
	"""
	deoverlap using ode

	text_xys: initial positions 
	text_dims: w/h of texts
	anno_xys: annotation coords of each text (default: same as text_xys)
	clip_box: clip boundary box as [x_min, x_max, y_min, y_max]; (default: unlimited)
	text_repel_force: repelling force between texts (default: 1.0)
	anno_repel_force = repelling force from an annotation point to a text (default: 1.0);
		each text only repelled by annotation points other than the one it annotates
	anno_attract_force: attracting force between the text and the point it annotates;
		(default: 1.0)
	init_noise: add noise to text_xys before deoverlapping (default: 0.0)
	"""
	# below calls should autometically check the data types
	# note the shape of each array is checked in C routine
	text_xys = _numpy.asarray(text_xys, dtype = float)
	text_dims = _numpy.asarray(text_dims, dtype = float)
	# if anno_xys is not specified, using text_xys, but make a copy
	# this should be done before adding random noise (to ret_xys)
	if anno_xys is None:
		anno_xys = text_xys.copy()
	else:
		anno_xys = _numpy.asarray(anno_xys, dtype = float)
	if clip_box is None:
		# using -inf - inf i.e. unlimited
		clip_box = _numpy.array([-_numpy.inf, _numpy.inf] * 2, dtype = float)
	else:
		clip_box = _numpy.asarray(clip_box, dtype = float)
	text_repel_force = float(text_repel_force)
	anno_repel_force = float(anno_repel_force)
	anno_attract_force = float(anno_attract_force)
	# force init_noise to be non-negative
	init_noise = _numpy.abs(float(init_noise))
	# ret_xys is a copy of original text_xys
	# ret_xys is updated in-place in C routine
	ret_xys = text_xys.copy()
	# add random noise if needed
	if init_noise != 0.0:
		ret_xys += _numpy.random.uniform(-init_noise, init_noise, ret_xys.shape)
	# call C routine
	_deoverlap_croutine.ode_deoverlap(ret_xys, text_dims, anno_xys, clip_box,
		text_repel_force, anno_repel_force, anno_attract_force)
	return ret_xys
