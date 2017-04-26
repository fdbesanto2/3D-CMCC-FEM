/* biomass.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "biomass.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern logger_t* g_debug_log;


void live_total_wood_age(const age_t *const a, species_t *const s)
{
	/* this function update based on current tree age the amount of live:total wood ratio
	 * based on the assumption that the live wood decrease linearly increasing age */
	/* e.g. for Fagus sylvatica  base on simulation for Hesse site (age 30) and Collelongo site (age 160)*/
	// fixme values should be included in species.txt
	double max_live_total_ratio;
	double min_live_total_ratio;
	double t1;
	double t2;

	int min_age;
	int max_age;

	logger(g_debug_log, "\n*live:total wood ratio based on tree age*\n");

	max_age = (int)s->value[MAXAGE];

	//fixme values should be included in species.txt
	/* age at which live:total wood ratio is maximum */
	min_age = 25;

	max_live_total_ratio = s->value[LIVE_TOTAL_WOOD];

	//fixme values should be included in species.txt
	min_live_total_ratio = 0.03;

	t1 = max_live_total_ratio - min_live_total_ratio;
	t2 = max_age - min_age;

	s->value[EFF_LIVE_TOTAL_WOOD_FRAC] = (t1/t2)*(max_age - a->value) + min_live_total_ratio;
	//logger(g_debug_log, "Effective live:total wood fraction based on stand age = %g\n", s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
}


void annual_tree_increment(cell_t *const c, const int height, const int dbh, const int age, const int species, const int year)
{
	double prev_vol;
	double tree_prev_vol;
	double max_diff = 0.05;           /* maximum allowed differences in m3/tree between initialized volume and computed at the first year */

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	h = &c->heights[height];
	d = &c->heights[height].dbhs[dbh];
	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* in m^3/cell/yr */
	/* assumption: CAI = Volume t1 - Volume t0 */
	/* assumption: MAI = Volume t1 / Age t1 */

	/* CURRENT ANNUAL INCREMENT-CAI */

	logger(g_debug_log, "***VOLUME & CAI & MAI***\n");

	/* STAND VOLUME-(STEM VOLUME) */
	/* assign previous volume to temporary variables */
	prev_vol              = s->value[VOLUME];
	logger(g_debug_log, "Previous stand volume        = %f m^3/cell\n", prev_vol );

	tree_prev_vol  = s->value[TREE_VOLUME];
	logger(g_debug_log, "Previous single tree volume  = %f m^3DM/tree\n", tree_prev_vol );

	/* compute single tree volume */
	s->value[TREE_VOLUME] = (Pi * s->value[FORM_FACTOR] * pow((d->value / 100.) , 2.) * h->value) / 4.;
	logger(g_debug_log, "-Single tree volume = %g m3/tree\n", s->value[TREE_VOLUME]);

	/* compute class volume */
	s->value[VOLUME] = s->value[TREE_VOLUME] * s->counter[N_TREE];
	logger(g_debug_log, "-Class volume = %g m3/sizeCell\n", s->value[VOLUME]);

	/* CAI-Current Annual Increment */
	s->value[CAI]         = s->value[VOLUME]      - prev_vol;
	logger(g_debug_log, "CAI-Current Annual Increment = %f m^3DM/cell/yr\n", s->value[CAI]);

	s->value[TREE_CAI]    = s->value[TREE_VOLUME] - tree_prev_vol;
	logger(g_debug_log, "CAI-Current Annual Increment = %f m^3DM/tree/yr\n", s->value[TREE_CAI]);

	/* MAI-Mean Annual Increment */
	s->value[MAI]         = s->value[VOLUME]      / (double)a->value;
	logger(g_debug_log, "MAI-Mean Annual Increment    = %f m^3DM/cell/yr \n", s->value[MAI]);

	s->value[TREE_MAI]    = s->value[TREE_VOLUME] / (double)a->value;
	logger(g_debug_log, "MAI-Mean Annual Increment    = %f m^3DM/tree/yr \n", s->value[TREE_MAI]);

	c->volume     += s->value[VOLUME];
	c->cum_volume += s->value[VOLUME];

	/* check every year after the first */
	if ( ! year ) { CHECK_CONDITION( fabs(s->value[TREE_VOLUME] - tree_prev_vol), > , max_diff ); }
	else { CHECK_CONDITION( s->value[TREE_VOLUME], < , tree_prev_vol - eps ); }


}

void abg_bgb_biomass(cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	static double old_agb;       //fixme this should be moved to matrix since classes can change
	static double old_bgb;       //fixme this should be moved to matrix since classes can change
	static double tree_old_agb;  //fixme this should be moved to matrix since classes can change
	static double tree_old_bgb;  //fixme this should be moved to matrix since classes can change

	species_t *s;

	//ALESSIOR
	/*
	if ( ! c->heights ) return;
	if ( ! c->heights[height].dbhs ) return;
	if ( ! c->heights[height].dbhs[dbh].ages ) return;
	if ( ! c->heights[height].dbhs[dbh].ages[age].species ) return;
	 */
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "**AGB & BGB**\n");

	old_agb = s->value[AGB];
	old_bgb = s->value[BGB];
	tree_old_agb = s->value[TREE_AGB];
	tree_old_bgb = s->value[TREE_BGB];

	s->value[AGB]            = s->value[LEAF_C]  + s->value[STEM_C] + s->value[BRANCH_C] +s->value[FRUIT_C];
	logger(g_debug_log, "AGB              = %f tC/cell\n", s->value[AGB]);

	s->value[BGB]            = s->value[FROOT_C] + s->value[CROOT_C];
	logger(g_debug_log, "BGB              = %f tC/cell\n", s->value[BGB]);

	s->value[DELTA_AGB]      = s->value[AGB] - old_agb;
	logger(g_debug_log, "DELTA_AGB        = %f tC/cell/year\n", s->value[DELTA_AGB]);

	s->value[DELTA_BGB]      = s->value[BGB] - old_bgb;
	logger(g_debug_log, "DELTA_BGB        = %f tC/cell/year\n", s->value[DELTA_BGB]);

	s->value[TREE_AGB]       = s->value[AGB] / (double)s->counter[N_TREE];
	logger(g_debug_log, "Yearly Class AGB = %f tC/tree\n", s->value[TREE_AGB]);

	s->value[TREE_BGB]       = s->value[BGB] / (double)s->counter[N_TREE];
	logger(g_debug_log, "Yearly Class BGB = %f tC/tree\n", s->value[TREE_BGB]);

	s->value[DELTA_TREE_AGB] = s->value[TREE_AGB] - tree_old_agb;
	logger(g_debug_log, "DELTA_TREE_AGB   = %f tC/tree/year\n", s->value[DELTA_TREE_AGB]);

	s->value[DELTA_TREE_BGB] = s->value[TREE_BGB] - tree_old_bgb;
	logger(g_debug_log, "DELTA_TREE_BGB   = %f tC/tree/year\n", s->value[DELTA_TREE_BGB]);
}

void average_tree_pools(species_t *const s)
{
	/* compute tree average C pools */
	s->value[AV_LEAF_MASS_C]             = (s->value[LEAF_C]             / (double)s->counter[N_TREE]);
	s->value[AV_STEM_MASS_C]             = (s->value[STEM_C]             / (double)s->counter[N_TREE]);
	s->value[AV_FROOT_MASS_C]            = (s->value[FROOT_C]            / (double)s->counter[N_TREE]);
	s->value[AV_CROOT_MASS_C]            = (s->value[CROOT_C]            / (double)s->counter[N_TREE]);
	s->value[AV_RESERVE_MASS_C]          = (s->value[RESERVE_C]          / (double)s->counter[N_TREE]);
	s->value[AV_BRANCH_MASS_C]           = (s->value[BRANCH_C]           / (double)s->counter[N_TREE]);
	s->value[AV_FRUIT_MASS_C]            = (s->value[FRUIT_C]            / (double)s->counter[N_TREE]);
	s->value[AV_LIVE_STEM_MASS_C]        = (s->value[STEM_LIVE_WOOD_C]   / (double)s->counter[N_TREE]);
	s->value[AV_DEAD_STEM_MASS_C]        = (s->value[STEM_DEAD_WOOD_C]   / (double)s->counter[N_TREE]);
	s->value[AV_LIVE_CROOT_MASS_C]       = (s->value[CROOT_LIVE_WOOD_C]  / (double)s->counter[N_TREE]);
	s->value[AV_DEAD_CROOT_MASS_C]       = (s->value[CROOT_DEAD_WOOD_C]  / (double)s->counter[N_TREE]);
	s->value[AV_LIVE_BRANCH_MASS_C]      = (s->value[BRANCH_LIVE_WOOD_C] / (double)s->counter[N_TREE]);
	s->value[AV_DEAD_BRANCH_MASS_C]      = (s->value[BRANCH_DEAD_WOOD_C] / (double)s->counter[N_TREE]);

	/* compute tree average N pools */
	s->value[AV_LEAF_MASS_N]             = (s->value[LEAF_N]             / (double)s->counter[N_TREE]);
	s->value[AV_STEM_MASS_N]             = (s->value[STEM_N]             / (double)s->counter[N_TREE]);
	s->value[AV_FROOT_MASS_N]            = (s->value[FROOT_N]            / (double)s->counter[N_TREE]);
	s->value[AV_CROOT_MASS_N]            = (s->value[CROOT_N]            / (double)s->counter[N_TREE]);
	s->value[AV_BRANCH_MASS_N]           = (s->value[BRANCH_N]           / (double)s->counter[N_TREE]);
	s->value[AV_RESERVE_MASS_N]          = (s->value[RESERVE_N]          / (double)s->counter[N_TREE]);
	s->value[AV_FRUIT_MASS_N]            = (s->value[FRUIT_N]            / (double)s->counter[N_TREE]);

}


void tree_branch_and_bark (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	age_t *a;
	species_t *s;

	a = &c->heights[height].dbhs[dbh].ages[age];
	// ALESSIOR
	//if ( ! a ) return;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* compute branch and bark fractions */

	if ( !s->value[FRACBB0] ) s->value[FRACBB0] = s->value[FRACBB1];
	else s->value[FRACBB] = s->value[FRACBB1] + ( s->value[FRACBB0]- s->value[FRACBB1] )* exp( -ln2 * ( a->value / s->value[TBB] ) );
}

void tree_biomass_remove (cell_t *const c, species_t *const s, const int tree_remove)
{
	average_tree_pools ( s );

	/* update class carbon pools */

	/* carbon to litter fluxes */
	s->value[DEAD_LEAF_C]           += (s->value[AV_LEAF_MASS_C]    * tree_remove);
	s->value[DEAD_FROOT_C]          += (s->value[AV_FROOT_MASS_C]   * tree_remove);
	/* overall litter */
	s->value[DEAD_TO_LITRC]         += s->value[DEAD_LEAF_C] +
			s->value[DEAD_FROOT_C];

	/* carbon to cwd fluxes */
	s->value[DEAD_STEM_C]           += (s->value[AV_STEM_MASS_C]    * tree_remove);
	s->value[DEAD_CROOT_C]          += (s->value[AV_CROOT_MASS_C]   * tree_remove);
	s->value[DEAD_BRANCH_C]         += (s->value[AV_BRANCH_MASS_C]  * tree_remove);
	s->value[DEAD_RESERVE_C]        += (s->value[AV_RESERVE_MASS_C] * tree_remove);
	s->value[DEAD_FRUIT_C]          += (s->value[AV_FRUIT_MASS_C]   * tree_remove);

	/* overall cwd */
	s->value[DEAD_TO_CWDC]          += (s->value[DEAD_STEM_C] +
			s->value[DEAD_CROOT_C]  +
			s->value[DEAD_BRANCH_C] +
			s->value[DEAD_RESERVE_C]+
			s->value[DEAD_FRUIT_C]) ;

	logger(g_debug_log, "Carbon biomass to remove\n");
	logger(g_debug_log, "DEAD_LEAF_C    = %f tC/cell\n", s->value[DEAD_LEAF_C]);
	logger(g_debug_log, "DEAD_FROOT_C   = %f tC/cell\n", s->value[DEAD_FROOT_C]);
	logger(g_debug_log, "DEAD_TO_LITRC  = %f tC/cell\n", s->value[DEAD_TO_LITRC]);
	logger(g_debug_log, "DEAD_STEM_C    = %f tC/cell\n", s->value[DEAD_STEM_C]);
	logger(g_debug_log, "DEAD_CROOT_C   = %f tC/cell\n", s->value[DEAD_CROOT_C]);
	logger(g_debug_log, "DEAD_BRANCH_C  = %f tC/cell\n", s->value[DEAD_BRANCH_C]);
	logger(g_debug_log, "DEAD_RESERVE_C = %f tC/cell\n", s->value[DEAD_RESERVE_C]);
	logger(g_debug_log, "DEAD_FRUIT_C   = %f tC/cell\n", s->value[DEAD_FRUIT_C]);
	logger(g_debug_log, "DEAD_TO_CWDC   = %f tC/cell\n", s->value[DEAD_TO_CWDC]);

	/******************************************************************************************/

	/* nitrogen to litter pool */
	s->value[DEAD_LEAF_N]           += (s->value[AV_LEAF_MASS_N]     * tree_remove);
	s->value[DEAD_FROOT_N]          += (s->value[AV_FROOT_MASS_N]    * tree_remove);
	/* overall litter */
	s->value[DEAD_TO_LITRN]         += s->value[DEAD_LEAF_N] +
			s->value[DEAD_FROOT_N];

	/* nitrogen to cwd fluxes */
	s->value[DEAD_STEM_N]           += (s->value[AV_STEM_MASS_N]     * tree_remove);
	s->value[DEAD_CROOT_N]          += (s->value[AV_CROOT_MASS_N]    * tree_remove);
	s->value[DEAD_BRANCH_N]         += (s->value[AV_BRANCH_MASS_N]   * tree_remove);
	s->value[DEAD_RESERVE_N]        += (s->value[AV_RESERVE_MASS_N]  * tree_remove);
	s->value[DEAD_FRUIT_N]          += (s->value[AV_FRUIT_MASS_N]    * tree_remove);

	/* overall cwd */
	s->value[DEAD_TO_CWDN]          += (s->value[DEAD_STEM_N] +
			s->value[DEAD_CROOT_N]  +
			s->value[DEAD_BRANCH_N] +
			s->value[DEAD_RESERVE_N]+
			s->value[DEAD_FRUIT_N]) ;

	logger(g_debug_log, "Nitrogen biomass to remove\n");
	logger(g_debug_log, "DEAD_LEAF_N    = %f tN/cell\n", s->value[DEAD_LEAF_N]);
	logger(g_debug_log, "DEAD_FROOT_N   = %f tN/cell\n", s->value[DEAD_FROOT_N]);
	logger(g_debug_log, "DEAD_TO_LITRN  = %f tN/cell\n", s->value[DEAD_TO_LITRN]);
	logger(g_debug_log, "DEAD_STEM_N    = %f tN/cell\n", s->value[DEAD_STEM_N]);
	logger(g_debug_log, "DEAD_NROOT_N   = %f tN/cell\n", s->value[DEAD_CROOT_N]);
	logger(g_debug_log, "DEAD_BRANCH_N  = %f tN/cell\n", s->value[DEAD_BRANCH_N]);
	logger(g_debug_log, "DEAD_RESERVE_N = %f tN/cell\n", s->value[DEAD_RESERVE_N]);
	logger(g_debug_log, "DEAD_FRUIT_N   = %f tN/cell\n", s->value[DEAD_FRUIT_N]);
	logger(g_debug_log, "DEAD_TO_CWDN   = %f tN/cell\n", s->value[DEAD_TO_CWDN]);

}
