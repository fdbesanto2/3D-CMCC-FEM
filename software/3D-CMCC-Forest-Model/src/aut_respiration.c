/*
 * maintenance_respiration.c
 *
 *  Created on: 25/set/2013
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


//FOLLOWING BIOME-BGC

void Maintenance_respiration (SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int height)
{

	int i;
	//maintenance respiration routine
	//Uses reference values at 20 deg C and an empirical relationship between
	//tissue N content and respiration rate given in:

	//Ryan, M.G., 1991. Effects of climate change on plant respiration.
	//Ecological Applications, 1(2):157-167.

	//Uses the same value of Q_10 (2.0) for all compartments, leaf, stem,
	//coarse and fine roots.

	//From Ryan's figures and regressions equations, the maintenance respiration
	//in kgC/day per kg of tissue N is:
	//mrpern = 0.218 (kgC/kgN/d)

	//Leaf maintenance respiration is calculated separately for day and
	//night, since the PSN routine needs the daylight value.

	//Leaf and fine root respiration are dependent on phenology.

	double t1;
	double q10 = 2.0;
	double mrpern = 0.1584; //0.218; //linear N relationship with MR being kgC/kgN/day, 0.218 from Ryan 1991, 0.1584 Campili et al., 2013 and from Dufrene et al 2005
	double exponent_tday;
	double exponent_tnight;
	double exponent_tavg;
	double exponent_tsoil;
	//double n_area_sun, n_area_shade, dlmr_area_sun, dlmr_area_shade;

	//double n_area/*, dlmr_area*/;


	Log("\n**MAINTENANCE_RESPIRATION**\n");

	// leaf day and night maintenance respiration when leaves on
	if (s->counter[VEG_UNVEG] == 1)
	{
		/* foliage */
		exponent_tday = (met[month].d[day].tday - 20.0) / 10.0;

		t1 = pow(q10, exponent_tday);

		s->value[DAILY_LEAF_MAINT_RESP] = (s->value[LEAF_NITROGEN] * mrpern * t1 * (met[month].d[day].daylength/24.0));
		Log("daily leaf maintenance respiration = %f gC/m2/day\n", s->value[DAILY_LEAF_MAINT_RESP]);

		exponent_tnight =  (met[month].d[day].tnight - 20.0) / 10.0;

		t1 = pow(q10, exponent_tnight);

		s->value[NIGHTLY_LEAF_MAINT_RESP]= (s->value[LEAF_NITROGEN] * mrpern * t1 * (1.0 - (met[month].d[day].daylength/24.0)));
		Log("nightly leaf maintenance respiration = %f gC/m2/day\n", s->value[NIGHTLY_LEAF_MAINT_RESP]);

		s->value[TOT_DAY_LEAF_MAINT_RESP]= s->value[DAILY_LEAF_MAINT_RESP] + s->value[NIGHTLY_LEAF_MAINT_RESP];
		Log("BIOME Total daily leaf maintenance respiration = %f gC/m2/day\n", s->value[TOT_DAY_LEAF_MAINT_RESP]);

		/* fine roots */
		exponent_tsoil = (met[month].d[day].tsoil - 20.0) / 10.0;
		t1 = pow(q10, exponent_tsoil);
		s->value[FINE_ROOT_MAINT_RESP] = (s->value[FINE_ROOT_NITROGEN] * mrpern * t1);
		Log("BIOME Fine root maintenance respiration = %f gC/m2/day\n", s->value[FINE_ROOT_MAINT_RESP]);
	}

	// live stem maintenance respiration
	exponent_tavg = (met[month].d[day].tavg - 20.0) / 10.0;
	t1 = pow(q10, exponent_tavg);
	s->value[STEM_MAINT_RESP] = (s->value[STEM_NITROGEN] * mrpern * t1);
	Log("BIOME Stem maintenance respiration = %f gC/m2/day\n", s->value[STEM_MAINT_RESP]);

	//live branch maintenance respiration
	s->value[BRANCH_MAINT_RESP] = (s->value[BRANCH_NITROGEN] * mrpern * t1);
	Log("BIOME Branch maintenance respiration = %f gC/m2/day\n", s->value[BRANCH_MAINT_RESP]);

	//live coarse root maintenance respiration
	exponent_tsoil = (met[month].d[day].tsoil - 20.0) / 10.0;
	t1 = pow(q10, exponent_tsoil);
	s->value[COARSE_ROOT_MAINT_RESP] = (s->value[COARSE_ROOT_NITROGEN] * mrpern * t1);
	Log("BIOME Coarse root maintenance respiration = %f gC/m2/day\n", s->value[COARSE_ROOT_MAINT_RESP]);

	//COMPUTE TOTAL MAINTENANCE RESPIRATION
	s->value[TOTAL_MAINT_RESP]= s->value[TOT_DAY_LEAF_MAINT_RESP]+
			s->value[FINE_ROOT_MAINT_RESP]+
			s->value[STEM_MAINT_RESP]+
			s->value[COARSE_ROOT_MAINT_RESP]+
			s->value[BRANCH_MAINT_RESP];
	Log("TOTAL maintenance respiration = %f gC/m2/day\n", s->value[TOTAL_MAINT_RESP]);

	c->daily_leaf_maint_resp += s->value[TOT_DAY_LEAF_MAINT_RESP];
	c->daily_stem_maint_resp += s->value[STEM_MAINT_RESP];
	c->daily_fine_root_maint_resp += s->value[FINE_ROOT_MAINT_RESP];
	c->daily_branch_maint_resp += s->value[BRANCH_MAINT_RESP];
	c->daily_coarse_root_maint_resp += s->value[COARSE_ROOT_MAINT_RESP];


	i = c->heights[height].z;
	c->layer_daily_maint_resp[i] += s->value[TOTAL_MAINT_RESP];
	c->layer_monthly_maint_resp[i] += s->value[TOTAL_MAINT_RESP];
	c->layer_annual_maint_resp[i] += s->value[TOTAL_MAINT_RESP];

	c->daily_maint_resp += s->value[TOTAL_MAINT_RESP];
	c->monthly_maint_resp += s->value[TOTAL_MAINT_RESP];
	c->annual_maint_resp += s->value[TOTAL_MAINT_RESP];

	CHECK_CONDITION(s->value[TOTAL_MAINT_RESP], < 0)
}

void Growth_respiration (SPECIES *s, CELL *const c, int height, int day, int month, int years)
{

	int i;
	Log("\n**GROWTH_RESPIRATION**\n");

	//toremove first part later
	if (s->value[DEL_FOLIAGE] > 0.0
			|| s->value[DEL_ROOTS_FINE] > 0.0
			|| s->value[DEL_STEMS] > 0.0
			|| s->value[DEL_ROOTS_COARSE] > 0.0
			|| s->value[DEL_BB] > 0.0)
		/*
	If (s->value[C_TO_LEAF] > 0.0
	|| s->value[C_TO_FINEROOT] > 0.0
	|| s->value[C_TO_COARSEROOT] > 0.0
	|| s->value[C_TO_STEM] > 0.0
	|| s->value[C_TO_BRANCH] > 0.0)
		 */

	{
		//converting to gC
		//fixme see if use CANOPY_COVER_DBHDC or just sizecell
		s->value[LEAF_GROWTH_RESP] = (s->value[C_TO_LEAF]/settings->sizeCell) * GRPERC;
		Log("daily leaf growth respiration = %f gC/m2/day\n", s->value[LEAF_GROWTH_RESP]);

		s->value[FINE_ROOT_GROWTH_RESP] = (s->value[C_TO_FINEROOT]/(settings->sizeCell))* GRPERC;
		Log("daily fine root growth respiration = %f gC/m2/day\n", s->value[FINE_ROOT_GROWTH_RESP]);

		s->value[STEM_GROWTH_RESP] = (s->value[C_TO_STEM]/(settings->sizeCell))* GRPERC;
		Log("daily stem growth respiration = %f gC/m2/day\n", s->value[FINE_ROOT_GROWTH_RESP]);

		s->value[COARSE_ROOT_GROWTH_RESP] = (s->value[C_TO_COARSEROOT]/(settings->sizeCell))* GRPERC;
		Log("daily coarse root growth respiration = %f gC/m2/day\n", s->value[COARSE_ROOT_GROWTH_RESP]);

		s->value[BRANCH_GROWTH_RESP] = (s->value[C_TO_BRANCH]/(settings->sizeCell))* GRPERC;
		Log("daily branch growth respiration = %f gC/m2/day\n", s->value[BRANCH_GROWTH_RESP]);

		s->value[TOTAL_GROWTH_RESP] = s->value[LEAF_GROWTH_RESP] +
				s->value[FINE_ROOT_GROWTH_RESP] +
				s->value[STEM_GROWTH_RESP] +
				s->value[COARSE_ROOT_GROWTH_RESP] +
				s->value[BRANCH_GROWTH_RESP];
	}
	Log("daily total growth respiration = %f gC/m2/day\n", s->value[TOTAL_GROWTH_RESP]);

	c->daily_leaf_growth_resp += s->value[LEAF_GROWTH_RESP];
	c->daily_stem_growth_resp += s->value[STEM_GROWTH_RESP];
	c->daily_fine_root_growth_resp += s->value[FINE_ROOT_GROWTH_RESP];
	c->daily_branch_growth_resp += s->value[BRANCH_GROWTH_RESP];
	c->daily_coarse_root_growth_resp += s->value[COARSE_ROOT_GROWTH_RESP];

	i = c->heights[height].z;

	c->layer_daily_growth_resp[i] += s->value[TOTAL_GROWTH_RESP];
	c->layer_monthly_gowth_resp[i] += s->value[TOTAL_GROWTH_RESP];
	c->layer_annual_growth_resp[i] += s->value[TOTAL_GROWTH_RESP];

	c->daily_growth_resp += s->value[TOTAL_GROWTH_RESP];
	c->monthly_growth_resp += s->value[TOTAL_GROWTH_RESP];
	c->annual_growth_resp += s->value[TOTAL_GROWTH_RESP];
	CHECK_CONDITION(s->value[TOTAL_GROWTH_RESP], < 0);
}

void Autotrophic_respiration (SPECIES *s, CELL *const c, int height)
{
	int i;

	Log("\n**AUTOTROPHIC_RESPIRATION**\n");

	//compute autotrophic respiration for each classes
	s->value[TOTAL_AUT_RESP] = s->value[TOTAL_GROWTH_RESP] + s->value[TOTAL_MAINT_RESP];
	Log("TOTAL autotrophic respiration = %f gC/m2/day\n", s->value[TOTAL_AUT_RESP]);

	//fixme see if use CANOPY_COVER_DBHDC or just sizecell
	Log("TOTAL autotrophic respiration = %f tC/cell/day \n", (s->value[TOTAL_AUT_RESP] /1000000.0) * settings->sizeCell);
	CHECK_CONDITION(s->value[TOTAL_AUT_RESP], < 0);

	//compute autotrophic respiration for each layer

	i = c->heights[height].z;

	c->layer_daily_aut_resp[i] +=s->value[TOTAL_AUT_RESP];
	c->layer_daily_aut_resp_tC[i] += s->value[TOTAL_AUT_RESP] / 1000000* settings->sizeCell;
	c->layer_monthly_aut_resp[i] += s->value[TOTAL_AUT_RESP];
	c->layer_annual_aut_resp[i] += s->value[TOTAL_AUT_RESP];

	c->daily_aut_resp += s->value[TOTAL_AUT_RESP];
	c->daily_aut_resp_tC +=  s->value[TOTAL_AUT_RESP] / 1000000 * settings->sizeCell;
	c->monthly_aut_resp += s->value[TOTAL_AUT_RESP];
	c->monthly_aut_resp_tC +=  s->value[TOTAL_AUT_RESP] / 1000000 * settings->sizeCell;
	c->annual_aut_resp += s->value[TOTAL_AUT_RESP];
	c->annual_aut_resp_tC += s->value[TOTAL_AUT_RESP]  / 1000000 * settings->sizeCell;
}




