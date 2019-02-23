#ifndef __ODE_DEOVERLAP_H__
#define __ODE_DEOVERLAP_H__

#include <vector>
#include <cmath>
#include <boost/numeric/odeint.hpp>
#include "clip_box.h"


class OdeDeoverlap
{
/* types */
public:
	using error_stepper_t =
		boost::numeric::odeint::runge_kutta_cash_karp54< std::vector< double > >;
	using controlled_stepper_t =
		boost::numeric::odeint::controlled_runge_kutta< error_stepper_t >;

/* variables */
private:
	const std::vector< double >& t_whs;
	const std::vector< double >& a_xys;
	const ClipBox& clip_box;
	const double t_repel;
	const double a_repel;
	const double a_attract;

/* methods */
public:
	OdeDeoverlap(
		const std::vector< double >& t_whs,
		const std::vector< double >& a_xys,
		const ClipBox& clip_box,
		double t_repel, double a_repel, double a_attract)
	: t_whs(t_whs)
	, a_xys(a_xys)
	, clip_box(clip_box)
	, t_repel(t_repel)
	, a_repel(a_repel)
	, a_attract(a_attract)
	{};

	inline void operator() (
		std::vector< double >& xs,
		std::vector< double >& dxydt,
		const double t);

private:
	template< bool squared = true >
	static inline double _l2norm(double vx, double vy);
	/* restretch the vector (vx, vy), and normlize its l2-norm to 1;
	 * restretch reverses the ratio of (abs(vx) : abs(vy))
	 * while maintaining the signs of vx and vy;
	 * this process tends to push along 4 diagnals
	 */
	static inline void _restratch_normalize(double& xr, double& yr);

	inline void _apply_t_repel_dxydt(std::vector< double >& dxydt,
		const std::vector< double >& t_xys, const int rec_i, const int push_i);
	inline void _apply_a_repel_dxydt(std::vector< double >& dxydt,
		const std::vector< double >& t_xys, const int rec_i, const int push_i);
	inline void _apply_a_attract_dxydt(std::vector< double >& dxydt,
		const std::vector< double >& t_xys, const int i);
	inline void _check_clip(std::vector< double >& dxydt,
		const std::vector< double >& t_xys);
};


template< bool _squared >
double OdeDeoverlap::_l2norm(double vx, double vy)
{
	double norm_sq = (vx * vx + vy * vy);
	return (_squared ? norm_sq : std::sqrt(norm_sq));
}


void OdeDeoverlap::_restratch_normalize(double& xr, double& yr)
{
	if ((xr == 0.0) && (yr == 0.0))
		return;
	if (xr == 0.0)
	{
		xr = 1.0;
		yr = 0.0;
		return;
	}
	if (yr == 0.0)
	{
		xr = 0.0;
		yr = 1.0;
		return;
	}
	xr = 1.0 / xr;
	yr = 1.0 / yr;
	double norm = OdeDeoverlap::_l2norm< false >(xr, yr);
	xr /= norm;
	yr /= norm;
	return;
}


void OdeDeoverlap::_apply_t_repel_dxydt(std::vector< double >& dxydt,
	const std::vector< double >& t_xys, const int rec_i, const int push_i)
{
	double x_rela = (t_xys[rec_i    ] - t_xys[push_i    ]);
	double y_rela = (t_xys[rec_i + 1] - t_xys[push_i + 1]);
	if ((x_rela == 0.0) && (y_rela == 0.0))
		return;
	const double inv_scale = OdeDeoverlap::_l2norm< true >(x_rela, y_rela);
	if (inv_scale >= 20.0)
		return;
	OdeDeoverlap::_restratch_normalize(x_rela, y_rela);
	dxydt[rec_i    ] += this->t_repel * x_rela * this->t_whs[rec_i    ] / inv_scale;
	dxydt[rec_i + 1] += this->t_repel * y_rela * this->t_whs[rec_i + 1] / inv_scale;
	return;
}


void OdeDeoverlap::_apply_a_repel_dxydt(std::vector< double >& dxydt,
	const std::vector< double >& t_xys, const int rec_i, const int push_i)
{
	double x_rela = (t_xys[rec_i    ] - this->a_xys[push_i    ]);
	double y_rela = (t_xys[rec_i + 1] - this->a_xys[push_i + 1]);
	if ((x_rela == 0.0) && (y_rela == 0.0))
		return;
	const double inv_scale = OdeDeoverlap::_l2norm< true >(x_rela, y_rela);
	if (inv_scale >= 20.0)
		return;
	OdeDeoverlap::_restratch_normalize(x_rela, y_rela);
	dxydt[rec_i    ] += this->t_repel * x_rela * this->t_whs[rec_i    ] / inv_scale;
	dxydt[rec_i + 1] += this->t_repel * y_rela * this->t_whs[rec_i + 1] / inv_scale;
	return;
}


void OdeDeoverlap::_apply_a_attract_dxydt(std::vector< double >& dxydt,
	const std::vector< double >& t_xys, const int i)
{
	const double x_rela = (this->a_xys[i    ] - t_xys[i    ]);
	const double y_rela = (this->a_xys[i + 1] - t_xys[i + 1]);
	dxydt[i    ] += this->a_attract * x_rela;
	dxydt[i + 1] += this->a_attract * y_rela;
	return;
}


inline void OdeDeoverlap::_check_clip(std::vector< double >& dxydt,
	const std::vector< double >& t_xys)
{
	const ClipBox& cbox = this->clip_box;
	const std::vector< double >& whs = this->t_whs;
	unsigned i = 0;
	while (i < whs.size())
	{
		if ((t_xys[i] <= (cbox.x_min + whs[i] / 2.0)) && (dxydt[i] < 0))
			dxydt[i] = 0;
		if ((t_xys[i] >= (cbox.x_max - whs[i] / 2.0)) && (dxydt[i] > 0))
			dxydt[i] = 0;
		i++; /* now deal (wioxth y */
		if ((t_xys[i] <= (cbox.y_min + whs[i] / 2.0)) && (dxydt[i] < 0))
			dxydt[i] = 0;
		if ((t_xys[i] >= (cbox.y_max - whs[i] / 2.0)) && (dxydt[i] > 0))
			dxydt[i] = 0;
		i++;
	}
}


void OdeDeoverlap::operator() (
	std::vector< double >& t_xys,
	std::vector< double >& dxydt,
	const double t)
{
	for (unsigned i = 0; i < t_xys.size(); i += 2)
	{
		dxydt[i    ] = 0.0;
		dxydt[i + 1] = 0.0;
		for (unsigned j = 0; j < t_xys.size(); j += 2)
		{
			if (i == j)
				OdeDeoverlap::_apply_a_attract_dxydt(dxydt, t_xys, i);
			else
			{
				/* repel from other labels */
				OdeDeoverlap::_apply_t_repel_dxydt(dxydt, t_xys, i, j);
				/* repel from annotation nodes */
				OdeDeoverlap::_apply_a_repel_dxydt(dxydt, t_xys, i, j);
			}
		}
	}
	/* check boundary clip */
	OdeDeoverlap::_check_clip(dxydt, t_xys);
	return;
}


void ode_deoverlap(
	std::vector< double >& t_xys,
	const std::vector< double >& t_whs,
	const std::vector< double >& a_xys,
	const ClipBox& clip_box,
	double t_repel,
	double a_repel,
	double a_attract);


#endif /* __ODE_DEOVERLAP_H__ */
