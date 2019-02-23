#include "ode_deoverlap.h"


void ode_deoverlap(
	std::vector< double >& t_xys,
	const std::vector< double >& t_whs,
	const std::vector< double >& a_xys,
	const ClipBox& clip_box,
	double t_repel,
	double a_repel,
	double a_attract)
{
	/* using error stepper */
	OdeDeoverlap::controlled_stepper_t stepper;
	OdeDeoverlap deoverlap_func(t_whs, a_xys, clip_box, t_repel, a_repel, a_attract);
	boost::numeric::odeint::integrate_adaptive(stepper, deoverlap_func,
		t_xys, 0.0, 10.0, 0.01);
	return;
}
