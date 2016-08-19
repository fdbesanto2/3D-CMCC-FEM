/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "common.h"

//extern settings_t* g_settings;
//extern logger_t* g_log;


//void Get_turnover_Marconi (cell_t *c, const int layer, const int height, const int age, const int species, const int DaysInMonth)
//{
//	static double litterfall_rate;
//	double dStemBiomass, dCoarseBiomass, dBranchBiomass;
//
//	species_t *s;
//	s = &c->heights[height].ages[age].species[species];
//
//		logger(g_log, "****DAILY TURNOVER following Marconi flowChart****\n");
//
//	//The first cycle of turnover for the simulation the module follows a modification of Biome's routine. That is, being the first year,
//	//the model can't calculate the dBiomass for the previous year, and uses the current increments to define the quantity of tissues
//	//death (expected a slight underestimation though).
//	// from the second cycle on, the model takes the quantity of tissue developed the antecedent cycle and expects it to be dying. That
//	// assuming the average period of turnover for the specific tissue of the specific species and eventually of age a.
//	// If that's so, the model assumes from the second cycle on, that the quantity of turnover is the dot[cycle-1].dCarbonPool
//	//Marconi 17/06: at the very begining of growing season these variables are set to the value of Biomass so that dC is reliable (solves the first yos_t Litter jump)
//	//fixsergio: what if a flag years == 0 would be added too?
//
//	/*following biome turnover occurs only during growing season*/
//	if (s->counter[VEG_UNVEG] == 1)
//	{
//		//fixme see if add foliage turnover!!!!!
//
//		/*daily leaf turnover for EVERGREEN*/
//		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
//		{
//			if(c->dos  < s->turnover->FINERTOVER)
//			{
//				logger(g_log, "****leaf turnover for evergreen****\n");
//				litterfall_rate = s->value[LEAF_FINEROOT_TURNOVER]/365.0;
//				logger(g_log, "Daily litter fall rate = %f\n", litterfall_rate);
//				s->value[BIOMASS_FOLIAGE_tDM] -= (s->value[OLD_BIOMASS_LEAVES] * litterfall_rate);
//				/*recompute LAI after turnover*/
//				/*for dominant layer with sunlit foliage*/
//				logger(g_log, "Foliage Biomass after turnover = %f\n", s->value[BIOMASS_FOLIAGE_tDM]);
//				c->leafLittering += (s->value[OLD_BIOMASS_LEAVES] * litterfall_rate) / GC_GDM * 1000 / g_settings->sizeCell;
//				c->leaflitN += (s->value[OLD_BIOMASS_LEAVES] * litterfall_rate) / GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_LEAVES];
//				c->leafBiomass += s->value[BIOMASS_FOLIAGE_tDM];
//
//				/*recompute LAI after turnover*/
//				/*for dominant layer with sunlit foliage*/
//				//ALESSIOC
//				if (c->top_layer == c->heights[height].z)
//				{
//					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_tDM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) * (s->value[SLA_AVG] * GC_GDM);
//				}
//				/*for dominated shaded foliage*/
//				else
//				{
//					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_tDM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) * ((s->value[SLA_AVG] * s->value[SLA_RATIO]) * GC_GDM);
//				}
//				logger(g_log, "++Lai from foliage or reserve = %f\n", s->value[LAI]);
//				//marconi: 21/6 --> removed the 365 factor; using the dBiomass it the turnover factors may not be divided by 365 days of the year
//				//			s->value[DAILY_FINEROOT_BIOMASS_TO_REMOVE] = s->value[DEL_ROOTS_FINE_CTEM] * (s->value[LEAF_FINEROOT_TURNOVER]);
//
//				//logger(g_soil_log, "fine root senescence time: from %d for %d days\n", c->doy, s->counter[FROM_SEN_TO_EOY] );
//				if (s->value[BIOMASS_FINE_ROOT_tDM] > 0.0)
//				{
//					//fine root lost for turnover are considered lost at all?
//					/*daily fine root turnover*/
//					logger(g_log, "****Fine root turnover****\n");
//					//turnover of fine root
//					s->value[BIOMASS_FINE_ROOT_tDM] -= MAX(0, s->value[OLD_BIOMASS_FINE_ROOT_tDM] * (s->value[LEAF_FINEROOT_TURNOVER])/365.0);
//					s->value[BIOMASS_FINE_ROOT_tDM] = 1.2;
//					c->fineRootLittering += MAX(0, s->value[OLD_BIOMASS_FINE_ROOT_tDM] * (s->value[LEAF_FINEROOT_TURNOVER])/365.0) /
//							GC_GDM * 1000 / g_settings->sizeCell;
//					c->fineRootlitN += MAX(0,s->value[OLD_BIOMASS_FINE_ROOT_tDM] * (s->value[LEAF_FINEROOT_TURNOVER])/365.0) /
//							GC_GDM * 1000 / g_settings->sizeCell  /s->value[CN_FINE_ROOTS];
//				}
//			}
//			else
//			{
//				if (s->value[BIOMASS_FINE_ROOT_tDM] > 0.0)
//				{
//					//fineroots turnover following Marconi's idea
//					logger(g_log, "****Fine root turnover****\n");
//					s->value[BIOMASS_FINE_ROOT_tDM] -=s->turnover->fineroot[c->dos % s->turnover->FINERTOVER];
//					c->fineRootLittering +=  s->turnover->fineroot[c->dos % s->turnover->FINERTOVER] / GC_GDM * 1000 / g_settings->sizeCell;
//					c->fineRootlitN += s->turnover->fineroot[c->dos % s->turnover->FINERTOVER]/ GC_GDM * 1000 / g_settings->sizeCell  /s->value[CN_FINE_ROOTS];
//				}
//				//leaves turnover following Marconi's idea:
//				c->leafLittering += (s->turnover->leaves[c->dos % s->turnover->FINERTOVER]) / GC_GDM * 1000 / g_settings->sizeCell;
//				s->value[BIOMASS_FOLIAGE_tDM] -= (s->turnover->leaves[c->dos % s->turnover->FINERTOVER]);
//				c->leaflitN += (s->turnover->leaves[c->dos % s->turnover->FINERTOVER]) / GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_LEAVES];
//				c->leafBiomass = s->value[BIOMASS_FOLIAGE_tDM];
//
//				/*recompute LAI after turnover*/
//				/*for dominant layer with sunlit foliage*/
//				if (c->top_layer == c->heights[height].z)
//				{
//					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_tDM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) * (s->value[SLA_AVG] * GC_GDM);
//				}
//				/*for dominated shaded foliage*/
//				else
//				{
//					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_tDM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) * ((s->value[SLA_AVG] * s->value[SLA_RATIO]) * GC_GDM);
//				}
//				logger(g_log, "++Lai from foliage or reserve = %f\n", s->value[LAI]);
//			}
//
//			s->turnover->fineroot[c->dos % s->turnover->FINERTOVER] = MAX(0, s->value[DEL_ROOTS_FINE]);
//			s->turnover->leaves[c->dos % s->turnover->FINERTOVER] = MAX(0, s->value[DEL_FOLIAGE]);
//			//	logger(g_soil_log, "\narray cell:\t %d \nfineroot:\t %g\nleaves:\t %g",c->dos % s->turnover->FINERTOVER,
//			//			s->turnover->fineroot[c->dos % s->turnover->FINERTOVER], s->turnover->leaves[c->dos % s->turnover->FINERTOVER]);
//			//logger(g_soil_log, "\n\n prova resto: %d\n", dayOfTurnover);
//
//		}
//		else /* deciduous */
//		{
//
//		}
//
//
//		/*		daily stem turnover
//		logger(g_log, "****Stem turnover****\n");
//		//turnover of live stem wood to dead stem wood
//		s->value[BIOMASS_STEM_LIVE_WOOD] -= (s->value[OLD_BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Daily live stem wood passing to dead stem wood = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Live Stem Biomass = %f tDM/cell\n", s->value[OLD_BIOMASS_STEM_LIVE_WOOD]);
//		s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[OLD_BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
//
//		daily coarse root turnover
//		logger(g_log, "****Coarse root turnover****\n");
//		//turnover of live coarse root wood to coarse root dead wood
//		s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] -= (s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Daily live coarse root wood passing to dead wood = %f tDM/cell\n", s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Live Coarse root Biomass = %f tDM/cell\n", s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD]);
//		s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Dead Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
//
//		daily stem branch turnover
//		logger(g_log, "****Stem branch turnover****\n");
//		//turnover of live stem branch wood to dead stem branch wood
//		s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] -= (s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Daily live stem branch wood passing to dead stem branch wood = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Live Stem branch Biomass = %f tDM/cell\n", s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD]);
//		s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//		logger(g_log, "Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);*/
//	}
//	//fixsergio: still used biome for stem, branches and coarse roots; change it with the new turnover version
//	/*daily stem turnover*/
//	logger(g_log, "****Stem turnover****\n");
//	//turnover of live stem wood to dead stem wood
//	//Marconi version, 15/06: decreasing the proportional quantity for each day of simulation
//	dStemBiomass = MAX(0,(s->value[OLD_BIOMASS_STEM_LIVE_WOOD]) * s->value[LIVE_WOOD_TURNOVER] / 365.0);
//	s->value[BIOMASS_STEM_LIVE_WOOD_tDM] -= dStemBiomass;
//	s->value[BIOMASS_STEM_DEAD_WOOD_tDM] += dStemBiomass;
//
//	dStemBiomass = MAX(0,(s->value[OLD_BIOMASS_STEM]) * s->value[SAPWOODTTOVER] / 365.0); //old biomass all stem
//	s->value[BIOMASS_STEM_DEAD_WOOD_tDM] -= dStemBiomass; //Marconi
//	c->stemLittering =c->stemLittering + dStemBiomass / GC_GDM * 1000 / g_settings->sizeCell;
//	c->stemlitN = c->stemlitN + dStemBiomass / GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_DEAD_WOODS];
//	s->value[BIOMASS_STEM_tDM] -= dStemBiomass;
//	c->stemBiomass = s->value[BIOMASS_STEM_tDM];
//
//	//		s->value[BIOMASS_STEM_LIVE_WOOD] -= (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Daily live stem wood passing to dead stem wood = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Live Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
//	//		s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
//	//		c->stemLittering += s->value[BIOMASS_STEM_LIVE_WOOD]  /20 * s->value[LIVE_WOOD_TURNOVER]/365;
//	//		c->stemLittering = 0;
//	//		c->stemlitN = 0;
//
//	/*daily coarse root turnover*/
//	logger(g_log, "****Coarse root turnover****\n");
//	dCoarseBiomass = MAX(0.0, s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] / 365.0 * s->value[LIVE_WOOD_TURNOVER]);
//	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] -= dCoarseBiomass;
//	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] += dCoarseBiomass;
//
//	dCoarseBiomass = MAX(0.0, s->value[OLD_BIOMASS_ROOTS_COARSE] / 365.0 * s->value[COARSEROOT_TURNOVER]);
//	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] -= dCoarseBiomass;	//Marconi
//	c->coarseRootLittering += dCoarseBiomass / GC_GDM * 1000 / g_settings->sizeCell;
//	c->coarseRootlitN += dCoarseBiomass / GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_DEAD_WOODS];
//	s->value[BIOMASS_COARSE_ROOT_tDM] -= dStemBiomass;
//	c->coarseRootBiomass = s->value[BIOMASS_COARSE_ROOT_tDM];
//
//
//	//turnover of live coarse root wood to coarse root dead wood
//	//		s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] -= (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Daily live coarse root wood passing to dead wood = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Live Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
//	//		s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Dead Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
//	//		c->coarseRootLittering += s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]/20 * s->value[LIVE_WOOD_TURNOVER]/365;
//
//
//	/*daily stem branch turnover*/
//	logger(g_log, "****Stem branch turnover****\n");
//	//Marconi version, 15/06: decreasing the proportional quantity for each day of simulation
//	dBranchBiomass = MAX(0.0, s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD]/365.0 * s->value[LIVE_WOOD_TURNOVER]);
//	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] -= dBranchBiomass;
//	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] += dBranchBiomass;
//
//	dBranchBiomass = MAX(0.0, s->value[OLD_BIOMASS_BRANCH]/365.0 * s->value[BRANCHTTOVER]);
//	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] -= dBranchBiomass; //Marconi
//	c->stemBrancLittering += dBranchBiomass / GC_GDM * 1000 / g_settings->sizeCell;
//	c->stemBranclitN += dBranchBiomass / GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_DEAD_WOODS];
//	s->value[BIOMASS_BRANCH_tDM] -= dStemBiomass;
//
//	c->stemBranchBiomass = s->value[BIOMASS_BRANCH_tDM];
//
//
//	//turnover of live stem branch wood to dead stem branch wood
//	//		s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] -= (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Daily live stem branch wood passing to dead stem branch wood = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Live Stem branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
//	//		s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
//	logger(g_log, "Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
//
//	//
//	//	s->turnover->fineroot[c->dos % s->turnover->FINERTOVER] = MAX(0, s->value[DEL_ROOTS_FINE_CTEM]);
//	//	s->turnover->leaves[c->dos % s->turnover->FINERTOVER] = MAX(0, s->value[DEL_FOLIAGE]);
//	////	logger(g_soil_log, "\narray cell:\t %d \nfineroot:\t %g\nleaves:\t %g",c->dos % s->turnover->FINERTOVER,
//	////			s->turnover->fineroot[c->dos % s->turnover->FINERTOVER], s->turnover->leaves[c->dos % s->turnover->FINERTOVER]);
//	//	//logger(g_soil_log, "\n\n prova resto: %d\n", dayOfTurnover);
//}
