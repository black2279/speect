/************************************************************************************/
/* Copyright (c) 2008-2009 The Department of Arts and Culture,                      */
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
/* DATE    : 29 October 2009                                                        */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* A loader for the voice features in the voice configuration file.                 */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "voicemanager/loaders/features.h"


/************************************************************************************/
/*                                                                                  */
/* Function implementations                                                         */
/*                                                                                  */
/************************************************************************************/

S_LOCAL SMap *_s_get_voice_features(const SMap *voiceConfig, s_erc *error)
{
	SMap *features;
	const SObject *tmp;
	const SMap *voiceConfigFeatures;
	s_bool key_present;


	S_CLR_ERR(error);

	/* create a map container for features */
	features = S_MAP(S_NEW("SMapList", error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_get_voice_features",
				  "Failed to create new map for voice features"))
		return NULL;

	SMapListInit(&features, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_get_voice_features",
				  "Failed to initialize new map for voice features"))
		return NULL;

	/* look for "features" key in voiceConfig map */
	key_present = SMapObjectPresent(voiceConfig, "features", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_get_voice_features",
				  "Call to \"SMapObjectPresent\" failed for \'features\' key"))
		return NULL;

	if (!key_present)
	{
		/* no defined features */
		return features;
	}

	/* get features from voiceConfig */
	tmp = SMapGetObject(voiceConfig, "features", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_get_voice_features",
				  "Call to \"SMapGetObject\" failed"))
	{
		S_DELETE(features, "_s_get_voice_features", error);
		return NULL;
	}


	voiceConfigFeatures = S_CAST(tmp, SMap, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_get_voice_features",
				  "Call to \"S_CAST (SMap)\" failed for \'features\' object"))
	{
		S_DELETE(features, "_s_get_voice_features", error);
		return NULL;
	}


	/* make a copy and return the copy */
	SMapCopy(features, voiceConfigFeatures, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_get_voice_features",
				  "Call to \"SMapCopy\" failed"))
	{
		S_DELETE(features, "_s_get_voice_features", error);
		return NULL;
	}

	return features;
}
