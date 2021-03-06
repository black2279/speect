/************************************************************************************/
/* Copyright (c) 2012 The Department of Arts and Culture,                           */
/* The Government of the Republic of South Africa.                                  */
/*                                                                                  */
/* Contributors:  Meraka Institute, CSIR, South Africa.                             */
/*                                                                                  */
/* Permission is hereby granted, free of charge, to any person obtaining a copy     */
/* of this software and associated documentation files (the "Software"), to deal    */
/* in the Software without restriction, including without limitation the rights     */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/* copies of the Software, and to permit persons to whom the Software is            */
/* furnished to do so, subject to the following conditions:                         */
/* The above copyright notice and this permission notice shall be included in       */
/* all copies or substantial portions of the Software.                              */
/*                                                                                  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN        */
/* THE SOFTWARE.                                                                    */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* AUTHOR  : Aby Louw                                                               */
/* DATE    : May 2012                                                               */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* An utterance processor to do HTS Engine (mixed excitation) synthesis of a        */
/* segment relation stream.                                                         */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "synthesize_hts_engine.h"
#include "audio.h"


/************************************************************************************/
/*                                                                                  */
/* Defines                                                                          */
/*                                                                                  */
/************************************************************************************/

/* default values for HTS Engine params */
#define SPCT_DEF_SAMPLING_RATE 16000
#define SPCT_DEF_FPERIOD 80
#define SPCT_DEF_ALPHA 0.42
#define SPCT_DEF_STAGE 0.0
#define SPCT_DEF_BETA 0.0
#define SPCT_DEF_AUDIO_BUFF_SIZE 1600
#define SPCT_DEF_UV_THRESHOLD 0.5
#define SPCT_DEF_GV_WEIGHT_MCP 0.7
#define SPCT_DEF_GV_WEIGHT_LF0 1.0


/************************************************************************************/
/*                                                                                  */
/* Typedefs                                                                         */
/*                                                                                  */
/************************************************************************************/

typedef struct
{
	int sampling_rate;
	int fperiod;
	double alpha;
	int stage;
	double beta;
	int audio_buff_size;
	double uv_threshold;
	HTS_Boolean use_log_gain;
	double gv_weight_mcp;
	double gv_weight_lf0;
	double gv_weight_str;
} hts_params;


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

/* SHTSEngineMESynthUttProc105 class declaration. */
static SHTSEngineMESynthUttProc105Class HTSEngineMESynthUttProc105Class;


/************************************************************************************/
/*                                                                                  */
/* Static function prototypes                                                       */
/*                                                                                  */
/************************************************************************************/

static hts_params *get_hts_engine_params(const SMap *features, s_bool *me, s_erc *error);

static void get_windows(const SList *windows, char ***cwindows,
						int *num, const char *voice_base_path, s_erc *error);

static void get_trees_pdfs(const SList *trees, const SList *pdfs,
						   char ***ctrees, char ***cpdfs, int *num,
						   const char *voice_base_path, s_erc *error);

static void load_hts_engine_data(const SMap *data,
								 SHTSEngineMESynthUttProc105 *HTSsynth,
								 const char *voice_base_path, s_erc *error);

static void filter_constructor(SHTSEngineMESynthUttProc105 *HTSsynth, s_erc *error);

static void filter_destructor(SHTSEngineMESynthUttProc105 *HTSsynth);

static void check_and_change_rate_volume(SHTSEngineMESynthUttProc105 *HTSsynth,
										 const SUtterance *utt, s_erc *error);

static void check_and_change_tone(SHTSEngineMESynthUttProc105 *HTSsynth,
								  const SUtterance *utt, s_erc *error);


/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/

/* local functions to register and free classes */
S_LOCAL void _s_hts_engine_me_synth_utt_proc_105_class_reg(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_reg(S_OBJECTCLASS(&HTSEngineMESynthUttProc105Class), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_hts_engine_me_synth_utt_proc_class_reg",
			  "Failed to register SHTSEngineMESynthUttProc105Class");
}


S_LOCAL void _s_hts_engine_me_synth_utt_proc_105_class_free(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_free(S_OBJECTCLASS(&HTSEngineMESynthUttProc105Class), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_hts_engine_me_synth_utt_proc_class_free",
			  "Failed to free SHTSEngineMESynthUttProc105Class");
}


/************************************************************************************/
/*                                                                                  */
/* Static function implementations                                                  */
/*                                                                                  */
/************************************************************************************/

static void check_and_change_rate_volume(SHTSEngineMESynthUttProc105 *HTSsynth,
										 const SUtterance *utt, s_erc *error)
{
	const SVoice *voice;
	const SObject *tmp;
	float var;


	S_CLR_ERR(error);

	/* get voice */
	voice = SUtteranceVoice(utt, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "check_and_change_rate_volume",
				  "Call to \"SUtteranceVoice\" failed"))
		return;

	if (voice == NULL)
		return;

	/* get rate feature */
	tmp = SVoiceGetFeature(voice, "rate", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "check_and_change_rate_volume",
				  "Call to \"SVoiceGetFeature\" failed"))
		return;

	if (tmp != NULL)
	{
		var = SObjectGetFloat(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "check_and_change_rate_volume",
					  "Call to \"SObjectGetFloat\" failed"))
			return;

		if (var != 1.0)
			HTS_Label_set_speech_speed(&(HTSsynth->engine.label), var);
	}

	/* get volume feature */
	tmp = SVoiceGetFeature(voice, "volume", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "check_and_change_rate_volume",
				  "Call to \"SVoiceGetFeature\" failed"))
		return;

	if (tmp == NULL)
		return;

	var = SObjectGetFloat(tmp, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "check_and_change_rate_volume",
				  "Call to \"SObjectGetFloat\" failed"))
		return;

	if (var == 1.0)
		return;
	else
		HTS_Engine_set_volume(&(HTSsynth->engine), var);
}


static void check_and_change_tone(SHTSEngineMESynthUttProc105 *HTSsynth,
								  const SUtterance *utt, s_erc *error)
{
	const SVoice *voice;
	const SObject *tmp;
	float var;


	S_CLR_ERR(error);

	/* get voice */
	voice = SUtteranceVoice(utt, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "check_and_change_tone",
				  "Call to \"SUtteranceVoice\" failed"))
		return;

	if (voice == NULL)
		return;

	/* get half-tone feature */
	tmp = SVoiceGetFeature(voice, "half-tone", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "check_and_change_tone",
				  "Call to \"SVoiceGetFeature\" failed"))
		return;

	if (tmp == NULL)
		return;

	var = SObjectGetFloat(tmp, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "check_and_change_tone",
				  "Call to \"SObjectGetFloat\" failed"))
		return;

	if (var != 0.0)
	{
		int i;
		double f;

		for (i = 0; i < HTS_SStreamSet_get_total_state(&(HTSsynth->engine.sss)); i++)
		{
			f = HTS_SStreamSet_get_mean(&(HTSsynth->engine.sss), 1, i, 0); /* logf0 is stream 1 */
			f += var * log(2.0) / 12;
			if (f < log(10.0))
				f = log(10.0);
			HTS_SStreamSet_set_mean(&(HTSsynth->engine.sss), 1, i, 0, f);
		}
	}
}


static hts_params *get_hts_engine_params(const SMap *features, s_bool *me, s_erc *error)
{
	hts_params *engine_params;
	const char *tmp;


	S_CLR_ERR(error);

	engine_params = S_CALLOC(hts_params, 1);
	if (engine_params == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "get_hts_engine_params",
				  "Failed to allocate memory for 'hts_params' object");
		return NULL;
	}

	engine_params->sampling_rate = (int)SMapGetIntDef(features, "sampling_rate",
													  SPCT_DEF_SAMPLING_RATE, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetIntDef\" failed"))
		goto quit_error;

	engine_params->fperiod = (int)SMapGetIntDef(features, "fperiod",
												SPCT_DEF_FPERIOD, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetIntDef\" failed"))
		goto quit_error;

	engine_params->alpha = (double)SMapGetFloatDef(features, "alpha",
												   SPCT_DEF_ALPHA, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->stage = (double)SMapGetFloatDef(features, "stage",
												   SPCT_DEF_STAGE, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->beta = (double)SMapGetFloatDef(features, "beta",
												   SPCT_DEF_BETA, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->audio_buff_size = (int)SMapGetIntDef(features, "audio_buff_size",
														SPCT_DEF_AUDIO_BUFF_SIZE, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetIntDef\" failed"))
		goto quit_error;

	engine_params->uv_threshold = (double)SMapGetFloatDef(features, "uv_threshold",
														  SPCT_DEF_UV_THRESHOLD, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->gv_weight_lf0 = (double)SMapGetFloatDef(features, "gv_weight_lf0",
														   SPCT_DEF_GV_WEIGHT_LF0, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->gv_weight_mcp = (double)SMapGetFloatDef(features, "gv_weight_mcp",
														   SPCT_DEF_GV_WEIGHT_MCP, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;


	(*me) = SMapObjectPresent(features, "gv_weight_str", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapObjectPresent\" failed"))
		goto quit_error;

	if (*me)
	{
		engine_params->gv_weight_str = (double)SMapGetFloat(features, "gv_weight_str",
															error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_hts_engine_params",
					  "Call to \"SMapGetFloat\" failed"))
			goto quit_error;
	}

	/* set "use_log_gain" to FALSE as default */
	engine_params->use_log_gain = FALSE;

	tmp = SMapGetStringDef(features, "use_log_gain", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetStringDef\" failed"))
		goto quit_error;

	if (tmp != NULL)
	{
		int scomp;


		scomp = s_strcmp("TRUE", tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_hts_engine_params",
					  "Call to \"s_strcmp\" failed"))
			goto quit_error;

		if (scomp == 0)
			engine_params->use_log_gain = TRUE;
	}

	/* all OK, no errors */
	return engine_params;

	/* error clean up */
quit_error:
	S_FREE(engine_params);
	return NULL;
}


static void get_trees_pdfs(const SList *trees, const SList *pdfs,
						   char ***ctrees, char ***cpdfs, int *num,
						   const char *voice_base_path, s_erc *error)
{
	size_t tsize;
	size_t psize;
	SIterator *itr;
	int counter;


	S_CLR_ERR(error);
	tsize = SListSize(trees, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_trees_pdfs",
				  "Call to \"SListSize\" failed"))
		return;

	psize = SListSize(pdfs, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_trees_pdfs",
				  "Call to \"SListSize\" failed"))
		return;

	if (tsize != psize)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "get_trees_pdfs",
				  "Given trees and pdfs sizes mismatch");
		return;
	}

	*num = tsize;
	*ctrees = S_CALLOC(char*, tsize);
	if (*ctrees == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "get_trees_pdfs",
				  "Failed to allocate memory for 'char*' object");
		return;
	}

	itr = S_ITERATOR_GET(trees, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_trees_pdfs",
				  "Call to \"S_ITERATOR_GET\" failed"))
	{
		S_FREE(*ctrees);
		return;
	}

	counter = 0;
	while (itr != NULL)
	{
		const char *tmp;


		tmp = SObjectGetString(SIteratorObject(itr, error), error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_trees_pdfs",
					  "Call to \"SIteratorObject/SObjectGetString\" failed"))
		{
			S_FREE(*ctrees);
			return;
		}

		/* get data path, the one in the config file may be relative
		 * to the voice base path
		 */
		(*ctrees)[counter++] = s_path_combine(voice_base_path, tmp,
											  error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_trees_pdfs",
					  "Call to \"s_path_combine\" failed"))
		{
			S_FREE(*ctrees);
			return;
		}

		itr = SIteratorNext(itr);
	}

	*cpdfs = S_CALLOC(char*, tsize);
	if (*cpdfs == NULL)
	{
		S_FREE(*ctrees);
		S_FTL_ERR(error, S_MEMERROR,
				  "get_trees_pdfs",
				  "Failed to allocate memory for 'char*' object");
		return;
	}

	itr = S_ITERATOR_GET(pdfs, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_trees_pdfs",
				  "Call to \"S_ITERATOR_GET\" failed"))
	{
		S_FREE(*ctrees);
		S_FREE(*cpdfs);
		return;
	}

	counter = 0;
	while (itr != NULL)
	{
		const char *tmp;


		tmp = SObjectGetString(SIteratorObject(itr, error), error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_trees_pdfs",
					  "Call to \"SIteratorObject/SObjectGetString\" failed"))
		{
			S_FREE(*ctrees);
			S_FREE(*cpdfs);
			return;
		}

		/* get data path, the one in the config file may be relative
		 * to the voice base path
		 */
		(*cpdfs)[counter++] = s_path_combine(voice_base_path, tmp,
											 error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_trees_pdfs",
					  "Call to \"s_path_combine\" failed"))
		{
			S_FREE(*ctrees);
			S_FREE(*cpdfs);
			return;
		}

		itr = SIteratorNext(itr);
	}
}


static void get_windows(const SList *windows, char ***cwindows,
						int *num, const char *voice_base_path, s_erc *error)
{
	size_t tsize;
	SIterator *itr;
	int counter;


	S_CLR_ERR(error);
	tsize = SListSize(windows, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_windows",
				  "Call to \"SListSize\" failed"))
		return;

	*num = tsize;
	*cwindows = S_CALLOC(char*, tsize);
	if (*cwindows == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "get_windows",
				  "Failed to allocate memory for 'char*' object");
		return;
	}

	itr = S_ITERATOR_GET(windows, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_windows",
				  "Call to \"S_ITERATOR_GET\" failed"))
	{
		S_FREE(*cwindows);
		return;
	}

	counter = 0;
	while (itr != NULL)
	{
		const char *tmp;


		tmp = SObjectGetString(SIteratorObject(itr, error), error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_windows",
					  "Call to \"SIteratorObject/SObjectGetString\" failed"))
		{
			S_FREE(*cwindows);
			return;
		}

		/* get data path, the one in the config file may be relative
		 * to the voice base path
		 */
		(*cwindows)[counter++] = s_path_combine(voice_base_path, tmp,
												error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_windows",
					  "Call to \"s_path_combine\" failed"))
		{
			S_FREE(*cwindows);
			return;
		}

		itr = SIteratorNext(itr);
	}
}


static void filter_constructor(SHTSEngineMESynthUttProc105 *HTSsynth, s_erc *error)
{
	S_CLR_ERR(error);

	HTSsynth->xp_sig = S_MALLOC(double, HTSsynth->me_filter_order);
	if (HTSsynth->xp_sig == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "filter_constructor",
				  "Failed to allocate memory for mixed excitation pulse signal");
		return;
	}

	HTSsynth->xn_sig = S_MALLOC(double, HTSsynth->me_filter_order);
	if (HTSsynth->xn_sig == NULL)
	{
		S_FREE(HTSsynth->xp_sig);
		S_FTL_ERR(error, S_MEMERROR,
				  "filter_constructor",
				  "Failed to allocate memory for mixed excitation noise signal");
		return;
	}

	HTSsynth->hp = S_MALLOC(double, HTSsynth->me_filter_order);
	if (HTSsynth->hp == NULL)
	{
		S_FREE(HTSsynth->xp_sig);
		S_FREE(HTSsynth->xn_sig);
		S_FTL_ERR(error, S_MEMERROR,
				  "filter_constructor",
				  "Failed to allocate memory for mixed excitation pulse shaping filter");
		return;
	}

	HTSsynth->hn = S_MALLOC(double, HTSsynth->me_filter_order);
	if (HTSsynth->hn == NULL)
	{
		S_FREE(HTSsynth->xp_sig);
		S_FREE(HTSsynth->xn_sig);
		S_FREE(HTSsynth->hp);
		S_FTL_ERR(error, S_MEMERROR,
				  "filter_constructor",
				  "Failed to allocate memory for mixed excitation noise shaping filter");
		return;
	}
}


static void filter_destructor(SHTSEngineMESynthUttProc105 *HTSsynth)
{
	int i;

	for (i = 0; i < HTSsynth->me_num_filters; i++)
	{
		if (HTSsynth->me_filter[i] != NULL)
			S_FREE(HTSsynth->me_filter[i]);
	}

	if (HTSsynth->me_filter != NULL)
		S_FREE(HTSsynth->me_filter);

	if (HTSsynth->xp_sig != NULL)
		S_FREE(HTSsynth->xp_sig);

	if (HTSsynth->xn_sig != NULL)
		S_FREE(HTSsynth->xn_sig);

	if (HTSsynth->hp != NULL)
		S_FREE(HTSsynth->hp);

	if (HTSsynth->hn != NULL)
		S_FREE(HTSsynth->hn);

	if (HTSsynth->pd_filter != NULL)
		S_FREE(HTSsynth->pd_filter);
}


static void load_hts_engine_data(const SMap *data,
								 SHTSEngineMESynthUttProc105 *HTSsynth,
								 const char *voice_base_path, s_erc *error)
{
	const SMap *tmp;
	const SList *trees;
	const SList *pdfs;
	const SList *windows;
	int num;
	int i;
	char **ctrees;
	char **cpdfs;
	char **cwindows;
	int num_win;
	const char *gv;
	const char *gv_tree;
	const char *gv_switch;
	const char *pd_filter;
	HTS_Engine *engine = &(HTSsynth->engine);


	S_CLR_ERR(error);

	/* duration */
	tmp = S_MAP(SMapGetObjectDef(data, "duration", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (tmp == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'duration' HTS Engine data");
		return;
	}

	trees = S_LIST(SMapGetObjectDef(tmp, "trees", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (trees == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'duration:trees' HTS Engine data");
		return;
	}

	pdfs = S_LIST(SMapGetObjectDef(tmp, "pdfs", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (pdfs == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'duration:pdfs' HTS Engine data");
		return;
	}

	get_trees_pdfs(trees, pdfs, &ctrees, &cpdfs, &num, voice_base_path, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_trees_pdfs\" failed"))
		return;

	HTS_Engine_load_duration_from_fn(engine, cpdfs, ctrees, 1);
	for (i = 0; i < num; i++)
	{
		S_FREE(ctrees[i]);
		S_FREE(cpdfs[i]);
	}

	S_FREE(ctrees);
	S_FREE(cpdfs);


	/* log F0 */
	tmp = S_MAP(SMapGetObjectDef(data, "log F0", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (tmp == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'log F0' HTS Engine data");
		return;
	}

	trees = S_LIST(SMapGetObjectDef(tmp, "trees", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (trees == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'log F0:trees' HTS Engine data");
		return;
	}

	pdfs = S_LIST(SMapGetObjectDef(tmp, "pdfs", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (pdfs == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'log F0:pdfs' HTS Engine data");
		return;
	}

	get_trees_pdfs(trees, pdfs, &ctrees, &cpdfs, &num, voice_base_path, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_trees_pdfs\" failed"))
		return;

	windows = S_LIST(SMapGetObjectDef(tmp, "windows", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (windows == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'log F0:windows' HTS Engine data");
		return;
	}

	get_windows(windows, &cwindows, &num_win, voice_base_path, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_windows\" failed"))
		return;

	/* log f0 is stream 1, and msd_flag is TRUE */
	HTS_Engine_load_parameter_from_fn(engine, cpdfs, ctrees, cwindows,
									  1, TRUE, num_win, 1);

	for (i = 0; i < num; i++)
	{
		S_FREE(ctrees[i]);
		S_FREE(cpdfs[i]);
	}

	S_FREE(ctrees);
	S_FREE(cpdfs);

	for (i = 0; i < num_win; i++)
		S_FREE(cwindows[i]);

	S_FREE(cwindows);

	/* log f0 gv */
	gv = SMapGetStringDef(tmp, "gv-lf0", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetStringDef\" failed"))
		return;

	/* log f0 gv tree */
	gv_tree = SMapGetStringDef(tmp, "tree-gv-lf0", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetStringDef\" failed"))
		return;

	if (gv != NULL)
	{
		char *combined_path_gv;
		char *combined_path_gv_tree;


		/* get data path, the one in the config file may be relative
		 * to the voice base path
		 */
		combined_path_gv = s_path_combine(voice_base_path, gv,
										  error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"s_path_combine\" failed"))
			return;

		if (gv_tree != NULL)
		{
			/* get data path, the one in the config file may be relative
			 * to the voice base path
			 */
			combined_path_gv_tree = s_path_combine(voice_base_path, gv_tree,
												   error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "load_hts_engine_data",
						  "Call to \"s_path_combine\" failed"))
			{
				S_FREE(combined_path_gv);
				return;
			}
		}
		else
		{
			combined_path_gv_tree = NULL;
		}


		HTS_Engine_load_gv_from_fn(engine, (char**)&combined_path_gv,
								   (char**)&combined_path_gv_tree, 1, 1);
		S_FREE(combined_path_gv);
		if (combined_path_gv_tree != NULL)
			S_FREE(combined_path_gv_tree);
	}

	/* spectrum */
	tmp = S_MAP(SMapGetObjectDef(data, "spectrum", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (tmp == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'spectrum' HTS Engine data");
		return;
	}

	trees = S_LIST(SMapGetObjectDef(tmp, "trees", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (trees == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'spectrum:trees' HTS Engine data");
		return;
	}

	pdfs = S_LIST(SMapGetObjectDef(tmp, "pdfs", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (pdfs == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'spectrum:pdfs' HTS Engine data");
		return;
	}

	get_trees_pdfs(trees, pdfs, &ctrees, &cpdfs, &num, voice_base_path, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_trees_pdfs\" failed"))
		return;

	windows = S_LIST(SMapGetObjectDef(tmp, "windows", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (windows == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'spectrum:windows' HTS Engine data");
		return;
	}

	get_windows(windows, &cwindows, &num_win, voice_base_path, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_windows\" failed"))
		return;

	/* spectrum is stream 0, and msd_flag is FALSE */
	HTS_Engine_load_parameter_from_fn(engine, cpdfs, ctrees, cwindows,
									  0, FALSE, num_win, 1);

	for (i = 0; i < num; i++)
	{
		S_FREE(ctrees[i]);
		S_FREE(cpdfs[i]);
	}

	S_FREE(ctrees);
	S_FREE(cpdfs);

	for (i = 0; i < num_win; i++)
		S_FREE(cwindows[i]);

	S_FREE(cwindows);

	/* spectrum gv */
	gv = SMapGetStringDef(tmp, "gv-mgc", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetStringDef\" failed"))
		return;

	/* spectrum gv tree */
	gv_tree = SMapGetStringDef(tmp, "tree-gv-mgc", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetStringDef\" failed"))
		return;

	if (gv != NULL)
	{
		char *combined_path_gv;
		char *combined_path_gv_tree;


		/* get data path, the one in the config file may be relative
		 * to the voice base path
		 */
		combined_path_gv = s_path_combine(voice_base_path, gv,
										  error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"s_path_combine\" failed"))
			return;

		if (gv_tree != NULL)
		{
			/* get data path, the one in the config file may be relative
			 * to the voice base path
			 */
			combined_path_gv_tree = s_path_combine(voice_base_path, gv_tree,
												   error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "load_hts_engine_data",
						  "Call to \"s_path_combine\" failed"))
			{
				S_FREE(combined_path_gv);
				return;
			}
		}
		else
		{
			combined_path_gv_tree = NULL;
		}


		HTS_Engine_load_gv_from_fn(engine, (char**)&combined_path_gv,
								   (char**)&combined_path_gv_tree, 0, 1);
		S_FREE(combined_path_gv);
		if (combined_path_gv_tree != NULL)
			S_FREE(combined_path_gv_tree);
	}

	/* band strengths */
	if (HTSsynth->me == TRUE)
	{
		const char *me_filter;


		tmp = S_MAP(SMapGetObjectDef(data, "strengths", NULL, error));
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"SMapGetObjectDef\" failed"))
			return;

		if (tmp == NULL)
		{
			S_CTX_ERR(error, S_FAILURE,
					  "load_hts_engine_data",
					  "Failed to find 'strengths' HTS Engine data");
			return;
		}

		trees = S_LIST(SMapGetObjectDef(tmp, "trees", NULL, error));
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"SMapGetObjectDef\" failed"))
			return;

		if (trees == NULL)
		{
			S_CTX_ERR(error, S_FAILURE,
					  "load_hts_engine_data",
					  "Failed to find 'strengths:trees' HTS Engine data");
			return;
		}

		pdfs = S_LIST(SMapGetObjectDef(tmp, "pdfs", NULL, error));
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"SMapGetObjectDef\" failed"))
			return;

		if (pdfs == NULL)
		{
			S_CTX_ERR(error, S_FAILURE,
					  "load_hts_engine_data",
					  "Failed to find 'strengths:pdfs' HTS Engine data");
			return;
		}

		get_trees_pdfs(trees, pdfs, &ctrees, &cpdfs, &num, voice_base_path, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"get_trees_pdfs\" failed"))
			return;

		windows = S_LIST(SMapGetObjectDef(tmp, "windows", NULL, error));
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"SMapGetObjectDef\" failed"))
			return;

		if (windows == NULL)
		{
			S_CTX_ERR(error, S_FAILURE,
					  "load_hts_engine_data",
					  "Failed to find 'strengths:windows' HTS Engine data");
			return;
		}

		get_windows(windows, &cwindows, &num_win, voice_base_path, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"get_windows\" failed"))
			return;

		/* strengths is stream 2, and msd_flag is FALSE */
		HTS_Engine_load_parameter_from_fn(engine, cpdfs, ctrees, cwindows,
										  2, FALSE, num_win, 1);

		for (i = 0; i < num; i++)
		{
			S_FREE(ctrees[i]);
			S_FREE(cpdfs[i]);
		}

		S_FREE(ctrees);
		S_FREE(cpdfs);

		for (i = 0; i < num_win; i++)
			S_FREE(cwindows[i]);

		S_FREE(cwindows);

		/* strengths gv */
		gv = SMapGetStringDef(tmp, "gv-str", NULL, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"SMapGetStringDef\" failed"))
			return;

		/* strengths gv tree */
		gv_tree = SMapGetStringDef(tmp, "tree-gv-str", NULL, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"SMapGetStringDef\" failed"))
			return;

		if (gv != NULL)
		{
			char *combined_path_gv;
			char *combined_path_gv_tree;


			/* get data path, the one in the config file may be relative
			 * to the voice base path
			 */
			combined_path_gv = s_path_combine(voice_base_path, gv,
											  error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "load_hts_engine_data",
						  "Call to \"s_path_combine\" failed"))
				return;

			if (gv_tree != NULL)
			{
				/* get data path, the one in the config file may be relative
				 * to the voice base path
				 */
				combined_path_gv_tree = s_path_combine(voice_base_path, gv_tree,
													   error);
				if (S_CHK_ERR(error, S_CONTERR,
							  "load_hts_engine_data",
							  "Call to \"s_path_combine\" failed"))
				{
					S_FREE(combined_path_gv);
					return;
				}
			}
			else
			{
				combined_path_gv_tree = NULL;
			}


			HTS_Engine_load_gv_from_fn(engine, (char**)&combined_path_gv,
									   (char**)&combined_path_gv_tree, 2, 1);
			S_FREE(combined_path_gv);
			if (combined_path_gv_tree != NULL)
				S_FREE(combined_path_gv_tree);
		}

		/* mixed excitation filter */
		me_filter = SMapGetStringDef(tmp, "mixed excitation filter", NULL, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"SMapGetStringDef\" failed"))
			return;

		if (me_filter != NULL)
		{
			char *combined_path;

			/* get data path, the one in the config file may be relative
			 * to the voice base path
			 */
			combined_path = s_path_combine(voice_base_path, me_filter,
										   error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "load_hts_engine_data",
						  "Call to \"s_path_combine\" failed"))
				return;

			HTS_Engine_load_me_filter_from_fn(combined_path, &(HTSsynth->me_filter),
											  &(HTSsynth->me_num_filters), &(HTSsynth->me_filter_order));
			S_FREE(combined_path);

			/* allocate memory for other filter buffers */
			filter_constructor(HTSsynth, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "load_hts_engine_data",
						  "Call to \"filter_constructor\" failed"))
				return;
		}
		else
		{
			S_CTX_ERR(error, S_FAILURE,
					  "load_hts_engine_data",
					  "Failed to find 'strengths:mixed excitation filter' HTS Engine data");
			return;
		}
	}

	/* gv switch */
	gv_switch = SMapGetStringDef(data, "gv-switch", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (gv_switch != NULL)
	{
		char *combined_path;


		/* get data path, the one in the config file may be relative
		 * to the voice base path
		 */
		combined_path = s_path_combine(voice_base_path, gv_switch,
									   error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"s_path_combine\" failed"))
			return;

		HTS_Engine_load_gv_switch_from_fn(engine, combined_path);
		S_FREE(combined_path);
	}

	/* pulse dispersion filter */
	pd_filter = SMapGetStringDef(data, "pulse dispersion filter", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (pd_filter != NULL)
	{
		char *combined_path;


		/* get data path, the one in the config file may be relative
		 * to the voice base path
		 */
		combined_path = s_path_combine(voice_base_path, pd_filter,
									   error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "load_hts_engine_data",
					  "Call to \"s_path_combine\" failed"))
			return;

		HTS_Engine_load_pd_filter_from_fn(combined_path, &(HTSsynth->pd_filter),
										  &(HTSsynth->pd_filter_order));
		S_FREE(combined_path);
	}
}


/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/


static void Init(void *obj, s_erc *error)
{
	SHTSEngineMESynthUttProc105 *HTSsynth = obj;


	S_CLR_ERR(error);
	HTSsynth->me = FALSE;            /* assume non mixed excitation voice     */
	HTSsynth->me_filter = NULL;      /* mixed excitation filter coefficients  */
	HTSsynth->me_num_filters = 0;    /* mixed excitation number of filters    */
	HTSsynth->me_filter_order = 0;   /* mixed excitation filters order        */
	HTSsynth->xp_sig = NULL;         /* pulse signal                          */
	HTSsynth->xn_sig = NULL;         /* noise signal                          */
	HTSsynth->hp = NULL;             /* pulse shaping filter                  */
	HTSsynth->hn = NULL;             /* noise shaping filter                  */
	HTSsynth->pd_filter = NULL;      /* pulse dispersion filter coefficients  */
	HTSsynth->pd_filter_order = 0;   /* pulse dispersion filters order        */
}


/* we need to delete the window plug-in if any */
static void Destroy(void *obj, s_erc *error)
{
	SHTSEngineMESynthUttProc105 *self = obj;


	S_CLR_ERR(error);
	HTS_Engine_clear(&(self->engine));
	filter_destructor(self);
}


static void Dispose(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static void Initialize(SUttProcessor *self, const SVoice *voice, s_erc *error)
{
	hts_params *engine_params;
	SHTSEngineMESynthUttProc105 *HTSsynth = (SHTSEngineMESynthUttProc105*)self;
	const SMap *hts_data;
	const SObject *vcfgObject;
	char *voice_base_path;


	S_CLR_ERR(error);

	/* get voice base path */
	vcfgObject = SVoiceGetFeature(voice, "config_file", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Initialize",
				  "Call to \"SVoiceGetFeature\" failed, failed to get voice config file"))
		return;

	voice_base_path = s_get_base_path(SObjectGetString(vcfgObject, error), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Initialize",
				  "Call to \"s_get_base_path/SObjectGetString\" failed"))
		return;

	/* get the HTS engine settings */
	engine_params = get_hts_engine_params(self->features, &(HTSsynth->me), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Initialize",
				  "Call to \"get_hts_engine_params\" failed"))
	{
		S_FREE(voice_base_path);
		return;
	}

	/* initialize the engine */
	if (HTSsynth->me == TRUE)
	{
		/* extra stream for strengths */
		HTS_Engine_initialize(&(HTSsynth->engine), 3);
	}
	else
	{
		HTS_Engine_initialize(&(HTSsynth->engine), 2);
	}

	/* set the engine parameters */
	HTS_Engine_set_sampling_rate(&(HTSsynth->engine), engine_params->sampling_rate);
	HTS_Engine_set_fperiod(&(HTSsynth->engine), engine_params->fperiod);
	HTS_Engine_set_alpha(&(HTSsynth->engine), engine_params->alpha);
	HTS_Engine_set_gamma(&(HTSsynth->engine), engine_params->stage);
	HTS_Engine_set_log_gain(&(HTSsynth->engine), engine_params->use_log_gain);
	HTS_Engine_set_beta(&(HTSsynth->engine), engine_params->beta);
	HTS_Engine_set_audio_buff_size(&(HTSsynth->engine), engine_params->audio_buff_size);
	HTS_Engine_set_msd_threshold(&(HTSsynth->engine), 1, engine_params->uv_threshold);
	HTS_Engine_set_gv_weight(&(HTSsynth->engine), 0, engine_params->gv_weight_mcp);
	HTS_Engine_set_gv_weight(&(HTSsynth->engine), 1, engine_params->gv_weight_lf0);

	if (HTSsynth->me == TRUE)
		HTS_Engine_set_gv_weight(&(HTSsynth->engine), 2, engine_params->gv_weight_str);

	S_FREE(engine_params);

	hts_data = S_MAP(SVoiceGetFeature(voice, "hts engine data", error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "Initialize",
				  "Call to \"SVoiceGetFeature\" failed"))
		goto quit_error;

	if (hts_data == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Initialize",
				  "Failed to get \"hts engine data\" map from voice features");
		goto quit_error;
	}

	load_hts_engine_data(hts_data, HTSsynth, voice_base_path, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Initialize",
				  "Call to \"load_hts_engine_data\" failed"))
		goto quit_error;

	HTS_Engine_set_duration_interpolation_weight(&(HTSsynth->engine), 0, 1.0);
	HTS_Engine_set_parameter_interpolation_weight(&(HTSsynth->engine), 0, 0, 1.0);
	HTS_Engine_set_parameter_interpolation_weight(&(HTSsynth->engine), 1, 0, 1.0);

	if (HTSsynth->me == TRUE)
		HTS_Engine_set_parameter_interpolation_weight(&(HTSsynth->engine), 2, 0, 1.0);

	HTS_Engine_set_gv_interpolation_weight(&(HTSsynth->engine), 0, 0, 1.0);
	HTS_Engine_set_gv_interpolation_weight(&(HTSsynth->engine), 1, 0, 1.0);

	if (HTSsynth->me == TRUE)
		HTS_Engine_set_gv_interpolation_weight(&(HTSsynth->engine), 2, 0, 1.0);

	/* all OK */
	S_FREE(voice_base_path);
	return;

	/* error clean up */
quit_error:
	HTS_Engine_clear(&(HTSsynth->engine));
	filter_destructor(HTSsynth);
	if (voice_base_path != NULL)
		S_FREE(voice_base_path);
}


static void Run(const SUttProcessor *self, SUtterance *utt,
				s_erc *error)
{
	SHTSEngineMESynthUttProc105 *HTSsynth = (SHTSEngineMESynthUttProc105*)self;
	SPlugin *audioPlugin;
	const SRelation *segmentRel;
	SAudio *audio = NULL;
	s_bool is_present;
	char **label_data = NULL;
	int label_size;
	const SItem *item;
	const SItem *itemItr;
	int counter;
	uint i;
	int frame;
	int state;
	const double rate = HTSsynth->engine.global.fperiod * 1e+7 / HTSsynth->engine.global.sampling_rate;
	int nstate;


	S_CLR_ERR(error);

	/* we require the segment relation */
	is_present = SUtteranceRelationIsPresent(utt, "Segment", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceRelationIsPresent\" failed"))
		goto quit_error;

	if (!is_present)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "Failed to find 'Segment' relation in utterance");
		goto quit_error;
	}

	segmentRel = SUtteranceGetRelation(utt, "Segment", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceGetRelation\" failed"))
		goto quit_error;

	item = SRelationHead(segmentRel, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SRelationHead\" failed"))
		goto quit_error;

	itemItr = item;
	label_size = 0;
	while (itemItr != NULL)
	{
		label_size++;
		itemItr = SItemNext(itemItr, error);
	}

	label_data = S_CALLOC(char*, label_size);

	itemItr = item;
	counter = 0;
	while (itemItr != NULL)
	{
		SObject *dFeat;
		const char *tmp;


		dFeat = SItemPathToFeatProc(itemItr, "hts_labels", error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SItemPathToFeatProc\" failed"))
			goto quit_error;

		if (dFeat == NULL)
		{
			S_CTX_ERR(error, S_FAILURE,
					  "Run",
					  "Failed to generate hts labels for segment item");
			goto quit_error;
		}

		tmp = SObjectGetString(dFeat, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SObjectGetString\" failed"))
			goto quit_error;

		label_data[counter++] = s_strdup(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"s_strdup\" failed"))
			goto quit_error;

		SItemSetObject((SItem*)itemItr, "hts_label", dFeat, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SItemSetObject\" failed"))
			goto quit_error;

		itemItr = SItemNext(itemItr, error);
	}

	/* speech synthesis part */
	HTS_Engine_load_label_from_string_list(&(HTSsynth->engine), label_data, label_size);
	check_and_change_rate_volume(HTSsynth, utt, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"check_and_change_rate_volume\" failed"))
		goto quit_error;

	HTS_Engine_create_sstream(&(HTSsynth->engine));
	check_and_change_tone(HTSsynth, utt, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"check_and_change_tone\" failed"))
		goto quit_error;

	HTS_Engine_create_pstream(&(HTSsynth->engine));

	if (HTSsynth->me == TRUE) /* mixed excitation */
	{
		HTS_Engine_create_gstream_me(&(HTSsynth->engine),
									 HTSsynth->me_num_filters, HTSsynth->me_filter_order,
									 HTSsynth->me_filter, HTSsynth->xp_sig, HTSsynth->xn_sig,
									 HTSsynth->hp, HTSsynth->hn,
									 HTSsynth->pd_filter, HTSsynth->pd_filter_order);
	}
	else
	{
		HTS_Engine_create_gstream(&(HTSsynth->engine));
	}

	nstate = HTS_Speect_ModelSet_get_nstate(&(HTSsynth->engine));
	itemItr = item;
	counter = 0;
	frame = 0;
	state = 0;
	while (itemItr != NULL)
	{
		int j;
		int duration = 0;
		float tmp;

		for (j = 0; j < nstate; j++)
			duration += HTS_Speect_SStreamSet_get_duration(&(HTSsynth->engine), state++);

		tmp = frame * rate;
		SItemSetFloat((SItem*)itemItr, "start", tmp/1e+7, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SItemSetFloat\" failed"))
			goto quit_error;

		tmp = (frame + duration) * rate;
		SItemSetFloat((SItem*)itemItr, "end", tmp/1e+7, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SItemSetFloat\" failed"))
			goto quit_error;

		frame += duration;
		itemItr = SItemNext(itemItr, error);
		counter++;
	}

	/* We need to give the utterance the audio plug-in. If we don't do
	 * this and the voice is deleted before the utterance, then the
	 * utterance can't do *anything* with the audio. Not even delete
	 * it (segfault). This should be fast because it is already
	 * loaded.
	 * Note that this happens before the audio is set. This is because
	 * utt features are a list implementation.
	 */
	audioPlugin = s_pm_load_plugin("audio.spi", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceSetFeature\" failed"))
		goto quit_error;

	SUtteranceSetFeature(utt, "audio_plugin", S_OBJECT(audioPlugin), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceSetFeature\" failed"))
	{
		S_DELETE(audioPlugin, "Run", error);
		goto quit_error;
	}

	/* create an audio object */
	audio = S_NEW(SAudio, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Failed to create new 'SAudio' object"))
		goto quit_error;

	/* set audio feature in utterance */
	SUtteranceSetFeature(utt, "audio", S_OBJECT(audio), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceSetFeature\" failed"))
	{
		S_DELETE(audio, "Run", error);
		goto quit_error;
    }

	audio->sample_rate = HTSsynth->engine.global.sampling_rate;
	audio->num_samples = (uint32)HTS_Speect_GStreamSet_get_total_nsample(&(HTSsynth->engine));
	audio->samples = S_MALLOC(float, audio->num_samples);
	if (audio->samples == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "Run",
				  "Failed to allocate memory for 'float' object");
		goto quit_error;
	}

	/* write data */
	for (i = 0; i < audio->num_samples; i++)
		audio->samples[i] = (float)(HTS_Speect_GStreamSet_get_speech(&(HTSsynth->engine), i) * 1.0);

	for (counter = 0; counter < label_size; counter++)
		S_FREE(label_data[counter]);
	S_FREE(label_data);

	HTS_Engine_refresh(&(HTSsynth->engine));

	/* all OK here */
	return;

	/* error clean-up code */
quit_error:
	if (label_data != NULL)
	{
		for (counter = 0; counter < label_size; counter++)
		{
			if (label_data[counter] != NULL)
				S_FREE(label_data[counter]);
		}

		S_FREE(label_data);
	}

	return;
}


/************************************************************************************/
/*                                                                                  */
/* SHTSEngineMESynthUttProc105 class initialization                                 */
/*                                                                                  */
/************************************************************************************/

static SHTSEngineMESynthUttProc105Class HTSEngineMESynthUttProc105Class =
{
	/* SObjectClass */
	{
		"SUttProcessor:SHTSEngineMESynthUttProc105",
		sizeof(SHTSEngineMESynthUttProc105),
		{ 0, 1},
		Init,            /* init    */
		Destroy,         /* destroy */
		Dispose,         /* dispose */
		NULL,            /* compare */
		NULL,            /* print   */
		NULL,            /* copy    */
	},
	/* SUttProcessorClass */
	Initialize,          /* initialize */
	Run                  /* run        */
};
