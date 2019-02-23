from matplotlib import pyplot
from deoverlap_2d import get_text_dimensions, ode_deoverlap

# texts
texts = ["some label", "another long label", "multi-line\nlabel"]
# initial text positions
init_xys = [(-0.12345, 0), (-0.3, 0.1), (0, 0)]
# annotation points of each text
anno_xys = [(-1, 0), (0.5, 1.0), (0, 0)]

figure, axes = pyplot.subplots(nrows = 1, ncols = 2, figsize = (8, 4),
	sharex = True, sharey = True)
axes[0].set_xlim(-3, 2)
axes[0].set_ylim(-1, 2)
text_dims = get_text_dimensions(axes[0], texts, fontsize = 12)

# adjust labels
deoverlap_xys = ode_deoverlap(init_xys, text_dims,
	anno_xys = anno_xys, clip_box = [-3.0, 2.0, -1.0, 2.0],
	text_repel_force = 1.0, anno_repel_force = 1.0, anno_attract_force = 0.2)

# show on matplotlib
for s, a_xy, before_xy, after_xy in zip(texts, anno_xys, init_xys, deoverlap_xys):
	axes[0].annotate(s, a_xy, before_xy, fontsize = 12, color = "#FF8000",
		arrowprops = {"arrowstyle": "->", "edgecolor": "#FF8000"})
	axes[1].annotate(s, a_xy, after_xy, fontsize = 12, color = "#4040FF",
		arrowprops = {"arrowstyle": "->", "edgecolor": "#4040FF"})
axes[0].set_title("Before")
axes[1].set_title("After")
figure.suptitle("Adjust overlapping labels (ode_deoverlap)")
pyplot.show()
pyplot.close()
