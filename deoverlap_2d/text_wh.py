#!/usr/bin/env python3

import numpy as _numpy
import matplotlib as _matplotlib
import matplotlib.pyplot as _pyplot


def get_textobj_dimensions(
		axes: _matplotlib.axes.Axes,
		texts: list,
	) -> _numpy.ndarray:
	"""
	get the dimensions (width, height) of a list of text objects
	"""
	if not isinstance(axes, _matplotlib.axes.Axes):
		raise TypeError("bad type of 'axes'")
	# get info from axes
	inv_trans = axes.transData.inverted()
	# text width/height must be calculated after rendering
	renderer = axes.get_figure().canvas.get_renderer()
	whs = _numpy.empty((len(texts), 2), dtype = float)
	for wh, t_obj in zip(whs, texts):
		wh[:] = _render_textobj_wh(t_obj, renderer, inv_trans)
	return whs


def _render_textobj_wh(
		text_obj: _matplotlib.text.Text,
		_renderer: _matplotlib.backend_bases.RendererBase,
		_transform: _matplotlib.transforms.CompositeGenericTransform,
	) -> tuple:
	"""
	get the dimensions (width, height) of a text objects by rendering
	"""
	if not isinstance(text_obj, _matplotlib.text.Text):
		raise TypeError("bad type of 'text_obj'")
	bb = text_obj.get_window_extent(renderer = _renderer).transformed(_transform)
	return (bb.width, bb.height)


def get_text_dimensions(
		axes: _matplotlib.axes.Axes,
		strs: list,
		*,
		x: float = 0,
		y: float = 0,
		**kw,
	) -> _numpy.ndarray:
	"""
	get the rendering dimensions (width, height) of a list of str's

	**kw: optional arguments passed to matplotlib.text.Text constructor
	"""
	_dummy_texts = []
	for s in strs:
		t = axes.text(x = x, y = y, s = s, **kw)
		_dummy_texts.append(t)
	# text dimesnions must be get when texts are visible (to be drawable)
	# i'll hide these dummy texts afterward
	text_dims = get_textobj_dimensions(axes, _dummy_texts)
	for t in _dummy_texts:
		t.set_visible(False)
	return text_dims;
