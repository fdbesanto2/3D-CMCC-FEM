/*
 * C-evergreen-partitioning-allocation.c
 *
 *  Created on: 20/nov/2013
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "lai.h"
#include "leaf_fall.h"
#include "turnover.h"
#include "dendometry.h"
#include "biomass.h"
#include "check_balance.h"
#include "C-allocation.h"
#include "mortality.h"

extern settings_t* g_settings;
extern logger_t* g_log;

extern int MonthLength [];
extern int MonthLength_Leap [];

/* Evergreen carbon allocation routine */
void daily_C_evergreen_partitioning_allocation(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species,
		const meteo_daily_t *const meteo_daily, const int day, const int month, const int year)
{
	double s0Ctem;
	double r0Ctem;
	double omegaCtem;
	double pS_CTEM;
	double pR_CTEM;
	double pF_CTEM;

	//double reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR

	double Light_trasm;

	double r0Ctem_increment;
	double old_r0Ctem;
	double s0Ctem_increment;
	double old_s0Ctem;

	/* for check */
	double npp_to_alloc;

	//height_t *h;
	age_t *a;
	species_t *s;

	//h = &c->heights[height];
	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &a->species[species];

	old_s0Ctem = s0Ctem = s->value[S0CTEM];
	old_r0Ctem = r0Ctem = s->value[R0CTEM];
	omegaCtem = s->value[OMEGA_CTEM];

	Light_trasm = exp(- s->value[K] * s->value[LAI]);

	/* Marconi: here the allocation of biomass reserve is divided in fineroot and leaves following the
	* allocation ratio parameter between them. That because
	* in evergreen we don't have bud burst phenology phase, and indeed there are two phenology phases;
	* the former in which carbon is allocated in fineroot and foliage, the latter in
	* every pool except foliage*/

	logger(g_log, "\n**C-PARTITIONING-ALLOCATION**\n");
	logger(g_log, "Carbon partitioning for evergreen\n");

	/* partitioning block using CTEM approach (Arora and Boer 2005) */
	logger(g_log, "*Partitioning ratios*\n");
	pR_CTEM = (r0Ctem + (omegaCtem * (1.0 - s->value[F_SW]))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW])));
	logger(g_log, "Roots CTEM ratio layer = %g %%\n", pR_CTEM * 100);
	pS_CTEM = (s0Ctem + (omegaCtem * (1.0 - Light_trasm))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW])));
	logger(g_log, "Stem CTEM ratio = %g %%\n", pS_CTEM * 100);
	pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
	logger(g_log, "Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);
	CHECK_CONDITION( fabs ( pR_CTEM + pS_CTEM + pF_CTEM ), > 1 + 1e-4 );

	if ( !s->management )
	{
		//logger(g_log, "Management type = TIMBER\n");
	}
	else
	{
		logger(g_log, "min r0 ctem = %g\n",s->value[MIN_R0CTEM] );
		logger(g_log, "max s0 ctem = %g\n",s->value[MAX_S0CTEM] );
		logger(g_log, "years for conversion = %g\n",s->value[YEARS_FOR_CONVERSION]);

		//considering a LINEAR increment
		//allocation ratio to roots
		r0Ctem -= s->value[MIN_R0CTEM];
		r0Ctem_increment = r0Ctem / s->value[YEARS_FOR_CONVERSION];
		r0Ctem = s->value[MIN_R0CTEM] + (r0Ctem_increment * s->value[F_AGE]);
		logger(g_log, "new r0_CTEM = %g \n", r0Ctem);

		if (r0Ctem > old_r0Ctem || r0Ctem < s->value[MIN_R0CTEM])
		{
			logger(g_log, "ERROR IN r0Ctem !!! \n");
		}

		//considering a LINEAR decrement
		//allocation ratio to stem + bb
		s0Ctem = s->value[MAX_S0CTEM] - s0Ctem;
		s0Ctem_increment = s0Ctem / s->value[YEARS_FOR_CONVERSION];
		s0Ctem = s->value[MAX_S0CTEM] - (s0Ctem_increment * s->value[F_AGE]);
		logger(g_log, "new s0_CTEM = %g \n", s0Ctem);

		if (s0Ctem > s->value[MAX_S0CTEM] || s0Ctem < old_s0Ctem)logger(g_log, "ERROR IN s0Ctem !!! \n");

	}

	logger(g_log, "\nCarbon allocation for evergreen\n");
	logger(g_log, "PHENOLOGICAL PHASE = %d\n", s->phenology_phase);
	logger(g_log, "LAI = %f \n", s->value[LAI]);
	logger(g_log, "PEAK LAI = %f \n", s->value[PEAK_LAI]);

	/* assign NPP to local variable */
	npp_to_alloc = s->value[NPP_tC];

	/* note: none carbon pool is refilled if reserve is lower than minimum
	reserves have priority before all other pools */

	switch ( s->phenology_phase )
	{
	/************************************************************/
	case 1:
		/*
		 * this phenological phase happens when:
		 * - month < 6 for north hemisphere
		 * - LAI < PEAK_LAI
		 * */

		/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
		 * and Barbaroux et al., 2002, the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
		 * the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
		 * sharing the daily remaining amount (taking into account respiration costs)of NSC */

		/*note: every day of the year leaf and fine root turnover happens, that's why every day
		 * an amount of leaf and fine root C is re-translocated to the reserve pool*/

		/* partitioning */
		if (npp_to_alloc > 0.0)
		{
			/* check if minimum reserve pool needs to be refilled */
			/* it doesn't need */
			if(s->value[RESERVE_C] >= s->value[MIN_RESERVE_C])
			{
				logger(g_log, "Allocating only into foliage and fine root pools (positive NPP)\n");
				s->value[C_TO_LEAF] = npp_to_alloc * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]);
				s->value[C_TO_FINEROOT] = npp_to_alloc - s->value[C_TO_LEAF];
				s->value[C_TO_RESERVE] = 0.0;
			}
			/* it needs */
			else if (s->value[RESERVE_C] < s->value[MIN_RESERVE_C])
			{
				logger(g_log, "Allocating only into reserve pool (low reserves, positive NPP)\n");
				s->value[C_TO_LEAF] = 0.0;
				s->value[C_TO_FINEROOT] = 0.0;
				s->value[C_TO_RESERVE] = npp_to_alloc;
			}

			s->value[C_TO_COARSEROOT] = 0.0;
			s->value[C_TO_STEM] = 0.0;
			s->value[C_TO_BRANCH] = 0.0;
			s->value[C_TO_FRUIT] = 0.0;

		}
		else
		{
			logger(g_log, "Consuming reserve pool (negative NPP)\n");
			s->value[C_TO_LEAF] = 0.0;
			s->value[C_TO_FINEROOT] = 0.0;
			s->value[C_TO_RESERVE] = npp_to_alloc;
			s->value[C_TO_COARSEROOT] = 0.0;
			s->value[C_TO_STEM] = 0.0;
			s->value[C_TO_BRANCH] = 0.0;
			s->value[C_TO_FRUIT] = 0.0;
		}
		/**********************************************************************/
		break;
	case 2:
		/*
		 * this phenological phase happens when:
		 * - month > 6 for north hemisphere
		 * - LAI > PEAK_LAI
		 */

		/* partitioning */
		if (npp_to_alloc > 0.0)
		{
			/* check if minimum reserve pool needs to be refilled */
			/* it doesn't need */
			if(s->value[RESERVE_C] >= s->value[MIN_RESERVE_C])
			{
				logger(g_log, "Allocating only into Coarse root, Reserve, Stem and Branch pools (positive NPP)\n");

				/* reproduction ONLY for needle leaf */
				if( ( s->value[PHENOLOGY] == 1.2 ) && ( a->value > s->value[SEXAGE] ) )
				{
					/* NPP for reproduction */
					s->value[C_TO_FRUIT] = npp_to_alloc * s->value[FRUIT_PERC];
					npp_to_alloc -= s->value[C_TO_FRUIT];
					logger(g_log, "including Biomass increment into cones = %g tC/area\n", s->value[C_TO_FRUIT]);
				}
				else
				{
					s->value[C_TO_FRUIT] = 0.0;
				}
				s->value[C_TO_LEAF] = 0.0;
				s->value[C_TO_COARSEROOT] = npp_to_alloc * pR_CTEM;
				s->value[C_TO_FINEROOT] = 0.0;
				s->value[C_TO_RESERVE] = (npp_to_alloc * pF_CTEM);
				s->value[C_TO_TOT_STEM] = npp_to_alloc * pS_CTEM;
				s->value[C_TO_STEM] = (npp_to_alloc* pS_CTEM) * (1.0 - s->value[FRACBB]);
				s->value[C_TO_BRANCH] = (npp_to_alloc * pS_CTEM) * s->value[FRACBB];
			}
			/* it needs */
			else
			{
				logger(g_log, "Allocating only into reserve pool (low reserves, positive NPP)\n");
				s->value[C_TO_RESERVE] = npp_to_alloc;
				s->value[C_TO_FINEROOT] = 0.0;
				s->value[C_TO_COARSEROOT] = 0.0;
				s->value[C_TO_TOT_STEM] = 0.0;
				s->value[C_TO_STEM] = 0.0;
				s->value[C_TO_BRANCH] = 0.0;
				s->value[C_TO_LEAF] = 0.0;
				s->value[C_TO_FRUIT] = 0.0;
			}
		}
		else
		{
			logger(g_log, "Consuming reserve pool (negative NPP)\n");
			s->value[C_TO_RESERVE] = npp_to_alloc;
			s->value[C_TO_FINEROOT] = 0.0;
			s->value[C_TO_COARSEROOT] = 0.0;
			s->value[C_TO_TOT_STEM] = 0.0;
			s->value[C_TO_STEM] = 0.0;
			s->value[C_TO_BRANCH] = 0.0;
			s->value[C_TO_LEAF] = 0.0;
			s->value[C_TO_FRUIT] = 0.0;
		}
		/**********************************************************************/
		break;
	}

	/* check for daily growth efficiency mortality */
	//note: since forest structure works on annual scale it must be turned off
	//daily_growth_efficiency_mortality ( c, height, dbh, age, species );


	/* sum all biomass pools increments */
	logger(g_log, "C_TO_LEAF = %g tC/cell/day\n", s->value[C_TO_LEAF]);
	logger(g_log, "C_TO_FINEROOT = %g tC/cell/day\n", s->value[C_TO_FINEROOT]);
	logger(g_log, "C_TO_COARSEROOT = %g tC/cell/day\n", s->value[C_TO_COARSEROOT]);
	logger(g_log, "C_TO_STEM = %g tC/cell/day\n", s->value[C_TO_STEM]);
	logger(g_log, "C_TO_RESERVE = %g tC/cell/day\n", s->value[C_TO_RESERVE]);
	logger(g_log, "C_TO_BRANCH = %g tC/cell/day\n", s->value[C_TO_BRANCH]);
	logger(g_log, "C_TO_FRUIT = %g tC/cell/day\n", s->value[C_TO_FRUIT]);

	/* update live_total wood fraction based on age */
	live_total_wood_age (a, species);

	/* allocate daily carbon */
	carbon_allocation ( s );

	/* compute single tree biomass pools */
	average_tree_biomass (s);

	/* to avoid "jumps" of dbh it is computed once monthly */
	//ALESSIOR is that correct?
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month] ) : (MonthLength[month] )) == c->doy )
	{
		dendrometry ( c, height, dbh, age, species );
	}

	logger(g_log, "\n-Daily increment in carbon pools-\n");
	logger(g_log, "C_TO_LEAF = %g tC/cell/day\n", s->value[C_TO_LEAF]);
	logger(g_log, "C_TO_FINEROOT = %g tC/cell/day\n", s->value[C_TO_FINEROOT]);
	logger(g_log, "C_TO_COARSEROOT = %g tC/cell/day\n", s->value[C_TO_COARSEROOT]);
	logger(g_log, "C_TO_STEM = %g tC/cell/day\n", s->value[C_TO_STEM]);
	logger(g_log, "C_TO_RESERVE = %g tC/cell/day\n", s->value[C_TO_RESERVE]);
	logger(g_log, "C_TO_BRANCH = %g tC/cell/day\n", s->value[C_TO_BRANCH]);
	logger(g_log, "C_TO_FRUIT = %g tC/cell/day\n", s->value[C_TO_FRUIT]);
	logger(g_log, "C_TO_LITTER = %g tC/cell/day\n", s->value[C_TO_LITTER]);
	logger(g_log, "C_LEAF_TO_RESERVE = %g tC/cell/day\n", s->value[C_LEAF_TO_RESERVE]);
	logger(g_log, "C_FINEROOT_TO_RESERVE = %g tC/cell/day\n", s->value[C_FINEROOT_TO_RESERVE]);
	logger(g_log, "C_STEM_LIVEWOOD_TO_DEADWOOD = %g tC/cell/day\n", s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]);
	logger(g_log, "C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD = %g tC/cell/day\n", s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD]);
	logger(g_log, "C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD = %g tC/cell/day\n", s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD]);

	/* leaf fall */
	leaf_fall_evergreen(c, height, dbh, age, species);

	/* turnover */
	turnover ( s );

	/* update Leaf Area Index */
	daily_lai ( s );

	logger(g_log, "\n-Daily increment in carbon pools (after leaf fall and turnover)-\n");
	logger(g_log, "C_TO_LEAF = %g tC/cell/day\n", s->value[C_TO_LEAF]);
	logger(g_log, "C_TO_FINEROOT = %g tC/cell/day\n", s->value[C_TO_FINEROOT]);
	logger(g_log, "C_TO_COARSEROOT = %g tC/cell/day\n", s->value[C_TO_COARSEROOT]);
	logger(g_log, "C_TO_STEM = %g tC/cell/day\n", s->value[C_TO_STEM]);
	logger(g_log, "C_TO_RESERVE = %g tC/cell/day\n", s->value[C_TO_RESERVE]);
	logger(g_log, "C_TO_BRANCH = %g tC/cell/day\n", s->value[C_TO_BRANCH]);
	logger(g_log, "C_TO_FRUIT = %g tC/cell/day\n", s->value[C_TO_FRUIT]);
	logger(g_log, "C_TO_LITTER = %g tC/cell/day\n", s->value[C_TO_LITTER]);

	/* update class level annual carbon biomass increment in tC/cell/year */
	s->value[DEL_Y_WTS] += s->value[C_TO_TOT_STEM];
	s->value[DEL_Y_WS] += s->value[C_TO_STEM];
	s->value[DEL_Y_WF] += s->value[C_TO_LEAF];
	s->value[DEL_Y_WFR] += s->value[C_TO_FINEROOT];
	s->value[DEL_Y_WCR] += s->value[C_TO_COARSEROOT];
	s->value[DEL_Y_WRES] += s->value[C_TO_RESERVE];
	s->value[DEL_Y_WR] += s->value[C_TO_ROOT];
	s->value[DEL_Y_BB] += s->value[C_TO_BRANCH];

	/* update cell level carbon biomass in gC/m2/day */
	c->daily_leaf_carbon += s->value[C_TO_LEAF] * 1000000.0 / g_settings->sizeCell ;
	c->daily_stem_carbon += s->value[C_TO_STEM] * 1000000.0 / g_settings->sizeCell ;
	c->daily_fine_root_carbon += s->value[C_TO_FINEROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_coarse_root_carbon += s->value[C_TO_COARSEROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_branch_carbon += s->value[C_TO_BRANCH] * 1000000.0 / g_settings->sizeCell ;
	c->daily_reserve_carbon += s->value[C_TO_RESERVE] * 1000000.0 / g_settings->sizeCell ;
	c->daily_root_carbon += s->value[C_TO_ROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_litter_carbon += s->value[C_TO_LITTER] * 1000000.0 / g_settings->sizeCell;
	c->daily_fruit_carbon += s->value[C_TO_FRUIT] * 1000000.0 / g_settings->sizeCell;

	/* update cell level carbon biomass in tC/cell/day */
	c->daily_leaf_carbon_tC += s->value[C_TO_LEAF];
	c->daily_stem_carbon_tC += s->value[C_TO_STEM];
	c->daily_fine_root_carbon_tC += s->value[C_TO_FINEROOT];
	c->daily_coarse_root_carbon_tC += s->value[C_TO_COARSEROOT];
	c->daily_branch_carbon_tC += s->value[C_TO_BRANCH];
	c->daily_reserve_carbon_tC += s->value[C_TO_RESERVE];
	c->daily_root_carbon_tC += s->value[C_TO_ROOT];
	c->daily_litter_carbon_tC += s->value[C_TO_LITTER];
	c->daily_fruit_carbon_tC += s->value[C_TO_FRUIT] * 1000000.0 / g_settings->sizeCell;

	logger(g_log, "******************************\n");
}

/**/

