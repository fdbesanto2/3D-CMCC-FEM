/*mortality.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void Layer_cover_mortality(cell_t *const c, const int layer, const int height, const int age, const int species, const double layer_cover, const int tree_number)
{
	//int i;
	//int oldNtree;
	int deadtree = 0;
	int oldNstump;
	int deadstump = 0;

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	//the model makes die trees of the lower height class for that layer because
	//it passes through the function sort_by_height_desc the height classes starting from the lowest

	logger(g_log, "\n\n*****LAYER_COVER_MORTALITY (CROWDING COMPETITION FUNCTION)*****\n");


	logger(g_log, "MORTALITY BASED ON HIGH CANOPY COVER layer %d, species %s height %f dbh %f !!!\n", s->name,
			s->value[AVDBH]);

	//Layer coverage mortality for timber
	//mortality occurs directly for timber
	if (s->management == T)
	{

		logger(g_log, "Layer coverage mortality for timber \n");

		//compute average biomass
		s->value[AV_LEAF_MASS_KgC] = (s->value[LEAF_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_STEM_MASS_KgC] = (s->value[STEM_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_ROOT_MASS_KgC] = (s->value[TOT_ROOT_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_FINE_ROOT_MASS_KgC] = (s->value[FINE_ROOT_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_COARSE_ROOT_MASS_KgC] =(s->value[COARSE_ROOT_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_RESERVE_MASS_KgC] = (s->value[RESERVE_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_BRANCH_MASS_KgC] = (s->value[BRANCH_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_LIVE_STEM_MASS_KgC] = (s->value[STEM_LIVE_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_DEAD_STEM_MASS_KgC] = (s->value[STEM_DEAD_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_LIVE_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_LIVE_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_DEAD_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_DEAD_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_LIVE_BRANCH_MASS_KgC] = (s->value[BRANCH_LIVE_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
		s->value[AV_DEAD_BRANCH_MASS_KgC] = (s->value[BRANCH_DEAD_WOOD_C] / (double)s->counter[N_TREE])/1000.0;


		//check if cc passed to function is correct
		//ALESSIOC
		//if (c->annual_layer_number == 1 && c->layer_cover_dominant >= g_settings->max_layer_cover)
		//{
		//	logger(g_log, "Layer cover in layer 0 passed to while = %f %% \n", c->layer_cover_dominant * 100);
		//}
		//if (c->annual_layer_number == 2 && (c->layer_cover_dominant >= g_settings->max_layer_cover || c->layer_cover_dominated >= g_settings->max_layer_cover))
		//{
		//	if(c->layer_cover_dominant >= g_settings->max_layer_cover)
		//		logger(g_log, "Layer cover in layer 1 passed to while= %f %%\n", c->layer_cover_dominant * 100);
		//	if(c->layer_cover_dominated >= g_settings->max_layer_cover)
		//		logger(g_log, "Layer cover in layer 0 passed to while= %f %% \n", c->layer_cover_dominated * 100);
		//}
		//if (c->annual_layer_number > 2 && (c->layer_cover_dominant >= g_settings->max_layer_cover || c->layer_cover_dominated >= g_settings->max_layer_cover || c->layer_cover_subdominated >= g_settings->max_layer_cover))
		//{
		//	if(c->layer_cover_dominant >= g_settings->max_layer_cover)
		//		logger(g_log, "Layer cover in layer 2 passed to while= %f %%\n", c->layer_cover_dominant * 100);
		//	if(c->layer_cover_dominated >= g_settings->max_layer_cover)
		//		logger(g_log, "Layer cover in layer 1 passed to while= %f %% \n", c->layer_cover_dominated * 100);
		//	if(c->layer_cover_subdominated >= g_settings->max_layer_cover)
		//		logger(g_log, "Layer cover in layer 0 passed to while= %f %% \n", c->layer_cover_subdominated * 100);
		//}

		////test 13 MAY 2016 test check to reduce the DBHDC up to the minimum value before kill trees
		////see structure.c

		//switch (c->annual_layer_number)
		//{
		//case 1:
		//	while ((c->layer_cover_dominant >= g_settings->max_layer_cover) && (s->counter[N_TREE] > 0))
		//	{
		//		s->counter[N_TREE] -= 1;
		//		deadtree += 1;

		//		c->layer_cover_dominant -= ((s->value[CROWN_AREA_DBHDC_FUNC] * 100.0) / g_settings->sizeCell)/100.0;
		//		s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC]																				   * s->counter[N_TREE]) / g_settings->sizeCell;
		//	}
		//	break;
		//case 2:
		//	if (c->layer_cover_dominant >= g_settings->max_layer_cover)
		//	{
		//		while ((c->layer_cover_dominant >= g_settings->max_layer_cover) && (s->counter[N_TREE] > 0))
		//		{
		//			s->counter[N_TREE] -= 1;
		//			deadtree += 1;

		//			c->layer_cover_dominant -= ((s->value[CROWN_AREA_DBHDC_FUNC] * 100.0) / g_settings->sizeCell)/100.0;
		//			s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC]																					   * s->counter[N_TREE]) / g_settings->sizeCell;
		//		}
		//	}
		//	else
		//	{
		//		while (c->layer_cover_dominated >= g_settings->max_layer_cover && s->counter[N_TREE] > 0)
		//		{
		//			s->counter[N_TREE] -= 1;
		//			deadtree += 1;

		//			c->layer_cover_dominated -= ((s->value[CROWN_AREA_DBHDC_FUNC] * 100.0) / g_settings->sizeCell)/100.0;
		//			s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC]																					   * s->counter[N_TREE]) / g_settings->sizeCell;
		//		}
		//	}
		//	break;
		//case 3:
		//	if (c->layer_cover_dominant >= g_settings->max_layer_cover )
		//	{
		//		while (c->layer_cover_dominant >= g_settings->max_layer_cover && s->counter[N_TREE] > 0)
		//		{
		//			s->counter[N_TREE] -= 1;
		//			deadtree += 1;

		//			c->layer_cover_dominant -= ((s->value[CROWN_AREA_DBHDC_FUNC] * 100.0) / g_settings->sizeCell)/100.0;
		//			s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC]																					   * s->counter[N_TREE]) / g_settings->sizeCell;
		//		}
		//	}
		//	else if (c->layer_cover_dominated >= g_settings->max_layer_cover)
		//	{
		//		while (c->layer_cover_dominated >= g_settings->max_layer_cover && s->counter[N_TREE] > 0)
		//		{
		//			s->counter[N_TREE] -= 1;
		//			deadtree += 1;

		//			c->layer_cover_dominated -= ((s->value[CROWN_AREA_DBHDC_FUNC] * 100.0) / g_settings->sizeCell)/100.0;
		//			s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC]																					   * s->counter[N_TREE]) / g_settings->sizeCell;
		//		}
		//	}
		//	else
		//	{
		//		while (c->layer_cover_subdominated >= g_settings->max_layer_cover && s->counter[N_TREE] > 0)
		//		{
		//			s->counter[N_TREE] -= 1;
		//			deadtree += 1;

		//			c->layer_cover_subdominated -= ((s->value[CROWN_AREA_DBHDC_FUNC] * 100.0) / g_settings->sizeCell)/100.0;
		//			s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC]
		//			                                                                                                                               * s->counter[N_TREE]) / g_settings->sizeCell;
		//		}
		//	}
		//	break;
		//}

		//if(s->counter[N_TREE] == 0)
		//{
		//	//delete classes due to mortality
		//	logger(g_log, "All trees of species %s dbh %g height %g age %d are died!!!!\n",
		//			s->name, s->value[AVDBH], c->heights[height].value, c->heights[height].ages[age].value);

		//	logger(g_log, "reducing counter for died classes...........!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1\n");
		//	logger(g_log, "heights_count = %d, ages_count = %d, species_count = %d \n", c->heights_count, c->heights[height].ages_count, c->heights[height].ages[age].species_count);
		//	//free (s->name);

		//	//todo it!!!
		//	//fixme in this case to BE CANCELLED is the last PROCESSED class BUT NOT NECESSARILY THE CLASS REALLY DIED
		//	if (c->heights[height].ages[age].species_count > 1)
		//		logger(g_log, "Reducing species_count..\n");
		//		c->heights[height].ages[age].species_count --;
		//	//free (c->heights[height].ages[age].species);
		//	if(c->heights[height].ages_count > 1)
		//		logger(g_log, "Reducing ages_count..\n");
		//		c->heights[height].ages_count --;
		//	//free (c->heights[height].ages);
		//	if (c->heights_count > 1)
		//		logger(g_log, "Reducing heights_count..\n");
		//		c->heights_count --;


		//}


		//i = c->t_layers[layer].heights[height].z;

		//ALESSIOC
		//c->layer_daily_dead_tree[i] += deadtree;
		//c->layer_monthly_dead_tree[i] += deadtree;
		//c->layer_annual_dead_tree[i] += deadtree;

		c->n_tree -= deadtree;

		c->daily_dead_tree += deadtree;
		c->monthly_dead_tree += deadtree;
		c->annual_dead_tree += deadtree;


		s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE]) / g_settings->sizeCell;
		logger(g_log, "Tree Removed for Crowding Competition from height class %f species %s dbh %f = %d trees\n", c->heights[height].value, s->name,
				s->value[AVDBH], deadtree);

		s->value[LEAF_C] -= (s->value[AV_LEAF_MASS_KgC]*1000.0*deadtree);
		s->value[FINE_ROOT_C] -= (s->value[AV_FINE_ROOT_MASS_KgC]*1000.0*deadtree);
		s->value[COARSE_ROOT_C] -= (s->value[AV_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
		s->value[STEM_C] -= (s->value[AV_STEM_MASS_KgC]*1000.0*deadtree);
		s->value[RESERVE_C] -= (s->value[AV_RESERVE_MASS_KgC]*1000.0*deadtree);
		s->value[BRANCH_C] -= (s->value[AV_BRANCH_MASS_KgC]*1000.0*deadtree);
		s->value[STEM_LIVE_WOOD_C] -= (s->value[AV_LIVE_STEM_MASS_KgC]*1000.0*deadtree);
		s->value[STEM_DEAD_WOOD_C] -= (s->value[AV_DEAD_STEM_MASS_KgC]*1000.0*deadtree);
		s->value[COARSE_ROOT_LIVE_WOOD_C] -= (s->value[AV_LIVE_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
		s->value[COARSE_ROOT_DEAD_WOOD_C] -= (s->value[AV_DEAD_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
		s->value[BRANCH_LIVE_WOOD_C] -= (s->value[AV_LIVE_BRANCH_MASS_KgC]*1000.0*deadtree);
		s->value[BRANCH_DEAD_WOOD_C] -= (s->value[AV_DEAD_BRANCH_MASS_KgC]*1000.0*deadtree);

		s->value[LITTER_C] += (s->value[AV_LEAF_MASS_KgC]*1000.0*deadtree) +
				(s->value[AV_FINE_ROOT_MASS_KgC]*1000.0*deadtree) +
				(s->value[AV_COARSE_ROOT_MASS_KgC]*1000.0*deadtree) +
				(s->value[AV_STEM_MASS_KgC]*1000.0*deadtree) +
				(s->value[AV_RESERVE_MASS_KgC]*1000.0*deadtree) +
				(s->value[AV_BRANCH_MASS_KgC]*1000.0*deadtree);


		logger(g_log, "Number of Trees = %d trees \n", s->counter[N_TREE]);
		logger(g_log, "LEAF_C removed =%f tC\n",(s->value[AV_LEAF_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "FINE_ROOT_C removed =%f tC\n",(s->value[AV_FINE_ROOT_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "COARSE_ROOT_C removed =%f tC\n",(s->value[AV_COARSE_ROOT_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "STEM_C removed =%f tC\n",(s->value[AV_STEM_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "RESERVE_C removed =%f tC\n",(s->value[AV_RESERVE_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "BRANCH_C removed =%f tC\n",(s->value[AV_BRANCH_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "STEM_LIVE_WOOD_C removed =%f tC\n",(s->value[AV_LIVE_STEM_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "STEM_DEAD_WOOD_C removed =%f tC\n",(s->value[AV_DEAD_STEM_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "COARSE_ROOT_LIVE_WOOD_C removed =%f tC\n",(s->value[AV_LIVE_COARSE_ROOT_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "COARSE_ROOT_DEAD_WOOD_C removed =%f tC\n",(s->value[AV_DEAD_COARSE_ROOT_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "BRANCH_LIVE_WOOD_C removed =%f tC\n",(s->value[AV_LIVE_BRANCH_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "BRANCH_DEAD_WOOD_C removed =%f tC\n",(s->value[AV_DEAD_BRANCH_MASS_KgC]*1000.0*deadtree));


		//reset dead tree
		deadtree = 0;
	}
	//Layer coverage mortality for coppice
	//mortality occurs only for stools
	else
	{
		logger(g_log, "Layer coverage mortality for coppice \n");

		//compute average biomass
		s->value[AV_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_tDM] / (double)s->counter[N_STUMP];

		oldNstump = s->counter[N_STUMP];

		while (layer_cover >= g_settings->max_layer_cover )
		{
			s->counter[N_STUMP] -= 1;
			deadstump += 1;
			//layer_cover = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_STUMP] / g_settings->sizeCell;
			s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE]) / g_settings->sizeCell;
		}
		oldNstump -= s->counter[N_STUMP];
		//s->value[BIOMASS_FOLIAGE] = s->value[WF] - s->value[MF] * s->counter[DEL_STEMS] * (s->value[WF] / s->counter[N_TREE]);
		logger(g_log, "Tot Root Biomass before reduction = %f tDM/tree\n", s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] );
		logger(g_log, "Stump Biomass before reduction = %f tDM/tree\n", s->value[BIOMASS_STEM_tDM]);
		s->value[BIOMASS_FINE_ROOT_tDM] -= (s->value[AV_FINE_ROOT_MASS_KgDM] * 1000.0* deadstump);
		s->value[BIOMASS_COARSE_ROOT_tDM] -= (s->value[AV_COARSE_ROOT_MASS_KgDM] * deadstump);
		s->value[BIOMASS_STEM_tDM] -= (s->value[AV_STEM_MASS_KgDM] * deadstump);
		logger(g_log, "Tot Root Biomass before reduction = %f tDM/tree\n", s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] );
		logger(g_log, "Stump Biomass before reduction = %f tDM/tree\n", s->value[BIOMASS_STEM_tDM]);
		logger(g_log, "Number of Trees = %d trees \n", s->counter[N_STUMP]);
		logger(g_log, "Tree Removed for Crowding Competition = %d trees\n", deadstump );
		logger(g_log, "Canopy Cover in while = %f \n", s->value[CANOPY_COVER_DBHDC]);

	}

}


/*Age mortality function from LPJ*/
//todo add to log results this function
void Age_Mortality (species_t *const s, age_t *const a)
{

	static int Dead_trees;




	//Age probability function
	s->value[AGEMORT] = (-(3 * log (0.001)) / (s->value[MAXAGE])) * pow (((double)a->value /s->value[MAXAGE]), 2);


	if ((s->counter[N_TREE] * s->value[AGEMORT]) > 1)
	{
		logger(g_log, "**MORTALITY based on Tree Age (LPJ)**\n");
		logger(g_log, "Age = %d years\n", a->value);
		logger(g_log, "Age Mortality based on Tree Age (LPJ) = %f\n", s->value[AGEMORT]);
		Dead_trees = (int)(s->counter[N_TREE] * s->value[AGEMORT]);
		logger(g_log, "DEAD TREES = %d\n", Dead_trees);
		s->value[BIOMASS_FOLIAGE_tDM] = s->value[BIOMASS_FOLIAGE_tDM] - s->value[MF] * Dead_trees * (s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE]);
		s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_ROOTS_TOT_tDM] - s->value[MR] * Dead_trees * (s->value[BIOMASS_ROOTS_TOT_tDM] / s->counter[N_TREE]);
		s->value[BIOMASS_STEM_tDM] = s->value[BIOMASS_STEM_tDM] - s->value[MS] * Dead_trees * (s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE]);
		//logger(g_log, "Wf after dead = %f tDM/ha\n", s->value[BIOMASS_FOLIAGE]);
		//logger(g_log, "Wr after dead = %f tDM/ha\n", s->value[BIOMASS_ROOTS_TOT]);
		//logger(g_log, "Ws after dead = %f tDM/ha\n", s->value[BIOMASS_STEM] );

		//----------------Number of trees after mortality---------------------

		s->counter[N_TREE] = s->counter[N_TREE] - Dead_trees;
		logger(g_log, "Number of Trees  after age mortality = %d trees\n", s->counter[N_TREE]);

		if ( !s->counter[DEAD_STEMS])
		{
			s->counter[DEAD_STEMS] = Dead_trees;
		}
		else
		{
			s->counter[DEAD_STEMS] += Dead_trees;
		}
	}
	else
	{
		logger(g_log, "**NO-MORTALITY based on Tree Age (LPJ)**\n");
	}
	logger(g_log, "**********************************\n");
}


//from LPJ Growth efficiency mortality
void Greff_Mortality (species_t *const s)
{
	static double greff;
	static double kmort1 = 0.02; //modified from original version
	static double kmort2 = 0.3;
	static double mortgreff;
	static int GreffDeadTrees;

	logger(g_log, "**MORTALITY based on Growth Efficiency (LPJ)**\n");

	greff = (s->value[DEL_TOTAL_W] / (s->value[BIOMASS_FOLIAGE_tDM] * s->value[SLA_AVG]));
	//logger(g_log, "greff from LPJ = %f\n", greff);
	//logger(g_log, "DEL_TOTAL_W = %f\n", s->value[DEL_TOTAL_W]);
	//logger(g_log, "WF= %f\n", s->value[BIOMASS_FOLIAGE]);

	mortgreff = kmort1 / (1 + kmort2 * greff);
	//logger(g_log, "rate mort for greff from LPJ = %f\n", mortgreff);

	GreffDeadTrees = (int)(mortgreff * s->counter[N_TREE]);

	logger(g_log, "Dead trees for greff = %d\n", GreffDeadTrees);

	logger(g_log, "Number of trees before greff mortality = %d trees/ha\n", s->counter[N_TREE]);

	s->counter[N_TREE] -= GreffDeadTrees;
	logger(g_log, "Number of trees less greff mortality = %d trees/ha\n", s->counter[N_TREE]);

}




//in LPJ se CanCover supera 0.95 vengono uccisi tanti alberi finchè la CanCover non torna al massimo a 0.95


/*Self-thinnig mortality function from 3PG*/
//----------------------------------------------------------------------------//
//                                                                            //
//                             GetMortality                                   //
//This function determines the number of stems to remove to ensure the
//self-thinning rule is satisfied. It applies the Newton-Rhapson method
//to solve for N to an accuracy of 1 stem or less. To change this,
//change the value of "accuracy".
//This was the old mortality function:
//getMortality = oldN - 1000 * (wSx1000 * oldN / oldW / 1000) ^ (1 / thinPower)
//which has been superceded by the following ..
//                                                                            //
//----------------------------------------------------------------------------//

void Mortality (species_t *const s, int years)
// TreeNumber = m->lpCell[index].InitialNTree, Ws = Ws
// TreeNumber = m->lpCell[index].NTree, Ws = Ws
{

	int i;
	double fN, dfN;
	double dN, n, x1, x2;


	//deselected algorithm for 1Km^2 spatial resolution
	/*m->cells[cell].heights[height].ages[age].species[species].value[WS_MAX] = m->cells[cell].heights[height].ages[age].species[species].value[WSX1000] *
        pow((1000 / (double)m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE]),
                m->cells[cell].heights[height].ages[age].species[species].value[THINPOWER]);
	 */

	//modifified version for 1Km^2 spatial resolution
	s->value[WS_MAX] = s->value[WSX1000];



	if ( s->value[AV_STEM_MASS_KgDM] > s->value[WS_MAX])
	{

		logger(g_log, "MORTALITY based SELF-THINNING RULE\n");
		logger(g_log, "Average Stem Mass > WSMax\n");
		logger(g_log, "WS MAX = %f kgDM/tree\n",  s->value[WS_MAX]);
		logger(g_log, "Average Stem Mass = %f kgDM stem /tree\n", s->value[AV_STEM_MASS_KgDM]);




		logger(g_log, "Tree Number before Mortality Function = %d\n", s->counter[N_TREE]);
		logger(g_log, "Tree Stem Mass before Mortality Function = %f\n", s->value[BIOMASS_STEM_tDM]);

		n = (double)s->counter[N_TREE] / 1000;
		logger(g_log, "n = %f\n", n);
		x1 = 1000 * s->value[MS] * s->value[BIOMASS_STEM_tDM] / (double)s->counter[N_TREE];
		logger(g_log, "x1 = %f\n", x1);
		i = 0;
		while ( 1 )
		{
			i = i + 1;
			logger(g_log, "i = %d\n", i);
			x2 = s->value[WSX1000] * pow (n, (1 - s->value[THINPOWER]));
			logger(g_log, "X2 = %f\n", x2);
			fN = x2 - x1 * n - (1 - s->value[MS]) * s->value[BIOMASS_STEM_tDM];
			logger(g_log, "fN = %f\n", fN);
			dfN = (1 - s->value[THINPOWER]) * x2 / n - x1;
			logger(g_log, "dfN = %f\n", dfN);
			dN = -fN / dfN;
			logger(g_log, "dN = %f\n", dN);
			n = n + dN;
			logger(g_log, "n = %f\n", n);
			if ((fabs(dN) <= eps) || (i >= 5))
				break;
		}

		s->counter[DEAD_STEMS] = (int)(s->counter[N_TREE] - 1000 * n);
		logger(g_log, "Dead Tree In Mortality Function = %d trees \n", s->counter[DEAD_STEMS]);

		//SERGIO CONTROL: if DEAD_STEMS < 0 set it to its minimum plausible value; that is 0
		if (s->counter[DEAD_STEMS] < 0)
		{
			s->counter[DEAD_STEMS]	 = 0;
		}
		//control
		if (s->counter[DEAD_STEMS] > s->counter[N_TREE])
		{
			logger(g_log, "ERROR Number of Dead Trees > N Trees\n");
			logger(g_log, "Dead Trees = %d\n", s->counter[DEAD_STEMS]);
			logger(g_log, "Live Trees = %d\n", s->counter[N_TREE]);
		}
		else
		{
			s->counter[N_TREE] = s->counter[N_TREE] - s->counter[DEAD_STEMS];
			logger(g_log, "Number of Trees  after mortality = %d trees\n", s->counter[N_TREE]);
			s->value[BIOMASS_FOLIAGE_tDM] = s->value[BIOMASS_FOLIAGE_tDM] - s->value[MF] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE]);
			s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_ROOTS_TOT_tDM] - s->value[MR] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_ROOTS_TOT_tDM] / s->counter[N_TREE]);
			s->value[BIOMASS_STEM_tDM] = s->value[BIOMASS_STEM_tDM] - s->value[MS] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE]);
			logger(g_log, "Wf after dead = %f tDM/ha\n", s->value[BIOMASS_FOLIAGE_tDM]);
			logger(g_log, "Wr after dead = %f tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
			logger(g_log, "Ws after dead = %f tDM/ha\n", s->value[BIOMASS_STEM_tDM]);
		}



		//----------------Number of trees after mortality---------------------




		//--------------------------------------------------------------------

		//deselected algorithm for 1Km^2 spatial resolution
		//s->value[WS_MAX] = s->value[WSX1000] * pow((1000 / (double)s->counter[N_TREE]), s->value[THINPOWER]);

		//modifified version for 1Km^2 spatial resolution
		s->value[WS_MAX] = s->value[WSX1000];

		logger(g_log, "wSmax = %f KgDM/tree\n", s->value[WS_MAX]);
		s->value[AV_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_tDM] * 1000.0 / (double)s->counter[N_TREE];
		logger(g_log, "AvStemMass after dead = %f Kg/tree\n", s->value[AV_STEM_MASS_KgDM]);
	}
	else
	{
		logger(g_log, "NO MORTALITY based SELF-THINNING RULE\n");
		logger(g_log, "Average Stem Mass < WSMax\n");
	}

	logger(g_log, "**********************************\n");



}


void Stool_mortality(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	//to compute stools mortality in function on age only for coppices
	logger(g_log, "***STOOLS MORTALITY*** \n");
}
