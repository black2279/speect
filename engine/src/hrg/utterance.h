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
/* DATE    : 1 November 2008                                                        */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* The Utterance class implementation. An Utterance consists of a set of Relations  */
/* which in turn consists of a set of Items.                                        */
/*                                                                                  */
/* Loosely based on cst_utterance of Flite, http://cmuflite.org (1.3),              */
/* EST_Utterance of Edinburgh Speech Tools,                                         */
/* http://www.cstr.ed.ac.uk/projects/speech_tools (1.2.96) and Utterance of         */
/* FreeTTS, http://sourceforge.net/projects/freetts (1.2).                          */
/* Note that this is a derived work with no verbatim source code from above         */
/* mentioned projects.                                                              */
/*                                                                                  */
/************************************************************************************/

#ifndef _SPCT_UTTERANCE_H__
#define _SPCT_UTTERANCE_H__


/************************************************************************************/
/*  Flite license, cst_utterance                                                    */
/*                                                                                  */
/*                       Language Technologies Institute                            */
/*                          Carnegie Mellon University                              */
/*                             Copyright (c) 1999                                   */
/*                             All Rights Reserved.                                 */
/*                                                                                  */
/*       Permission is hereby granted, free of charge, to use and distribute        */
/*       this software and its documentation without restriction, including         */
/*       without limitation the rights to use, copy, modify, merge, publish,        */
/*       distribute, sublicense, and/or sell copies of this work, and to            */
/*       permit persons to whom this work is furnished to do so, subject to         */
/*       the following conditions:                                                  */
/*        1. The code must retain the above copyright notice, this list of          */
/*           conditions and the following disclaimer.                               */
/*        2. Any modifications must be clearly marked as such.                      */
/*        3. Original authors' names are not deleted.                               */
/*        4. The authors' names are not used to endorse or promote products         */
/*           derived from this software without specific prior written              */
/*           permission.                                                            */
/*                                                                                  */
/*       CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK               */
/*       DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING            */
/*       ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT         */
/*       SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE            */
/*       FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES          */
/*       WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN         */
/*       AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,                */
/*       ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF             */
/*       THIS SOFTWARE.                                                             */
/*                                                                                  */
/*       Author:  Alan W Black                                                      */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/* Edinburgh Speech Tools license,  EST_Utterance                                   */
/*                                                                                  */
/*                     Centre for Speech Technology Research                        */
/*                          University of Edinburgh, UK                             */
/*                           Copyright (c) 1995,1996                                */
/*                             All Rights Reserved.                                 */
/*       Permission is hereby granted, free of charge, to use and distribute        */
/*       this software and its documentation without restriction, including         */
/*       without limitation the rights to use, copy, modify, merge, publish,        */
/*       distribute, sublicense, and/or sell copies of this work, and to            */
/*       permit persons to whom this work is furnished to do so, subject to         */
/*       the following conditions:                                                  */
/*        1. The code must retain the above copyright notice, this list of          */
/*           conditions and the following disclaimer.                               */
/*        2. Any modifications must be clearly marked as such.                      */
/*        3. Original authors' names are not deleted.                               */
/*        4. The authors' names are not used to endorse or promote products         */
/*           derived from this software without specific prior written              */
/*           permission.                                                            */
/*       THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK              */
/*       DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING            */
/*       ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT         */
/*       SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE           */
/*       FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES          */
/*       WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN         */
/*       AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,                */
/*       ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF             */
/*       THIS SOFTWARE.                                                             */
/*                                                                                  */
/*       Author :  Paul Taylor                                                      */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* FreeTTS license, Utterance                                                       */
/*                                                                                  */
/*      Portions Copyright 2001-2004 Sun Microsystems, Inc.                         */
/*      Portions Copyright 1999-2001 Language Technologies Institute,               */
/*      Carnegie Mellon University.                                                 */
/*      All Rights Reserved.  Use is subject to license terms.                      */
/*                                                                                  */
/*      Permission is hereby granted, free of charge, to use and distribute         */
/*      this software and its documentation without restriction, including          */
/*      without limitation the rights to use, copy, modify, merge, publish,         */
/*      distribute, sublicense, and/or sell copies of this work, and to             */
/*      permit persons to whom this work is furnished to do so, subject to          */
/*      the following conditions:                                                   */
/*                                                                                  */
/*       1. The code must retain the above copyright notice, this list of           */
/*          conditions and the following disclaimer.                                */
/*       2. Any modifications must be clearly marked as such.                       */
/*       3. Original authors' names are not deleted.                                */
/*       4. The authors' names are not used to endorse or promote products          */
/*          derived from this software without specific prior written               */
/*          permission.                                                             */
/*                                                                                  */
/*      SUN MICROSYSTEMS, INC., CARNEGIE MELLON UNIVERSITY AND THE                  */
/*      CONTRIBUTORS TO THIS WORK DISCLAIM ALL WARRANTIES WITH REGARD TO THIS       */
/*      SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND           */
/*      FITNESS, IN NO EVENT SHALL SUN MICROSYSTEMS, INC., CARNEGIE MELLON          */
/*      UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR      */
/*      CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF      */
/*      USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR       */
/*      OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR      */
/*      PERFORMANCE OF THIS SOFTWARE.                                               */
/*                                                                                  */
/*      Authors: Willie Walker, Paul Lamere, Philip Kwok                            */
/*                                                                                  */
/************************************************************************************/


/**
 * @file utterance.h
 * Utterance class implementation.
 */


/**
 * @ingroup SHrg
 * @defgroup SUtterance Utterance
 * The Utterance class implementation. An Utterance consists of a set of #SRelation
 * which in turn consists of a set of #SItem.
 * @{
 */


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "include/common.h"
#include "base/errdbg/errdbg.h"
#include "containers/containers.h"
#include "hrg/relation.h"


/************************************************************************************/
/*                                                                                  */
/* Begin external c declaration                                                     */
/*                                                                                  */
/************************************************************************************/
S_BEGIN_C_DECLS


/************************************************************************************/
/*                                                                                  */
/* Macros                                                                           */
/*                                                                                  */
/************************************************************************************/

/**
 * @name Utility Macros
 * @{
 */


/**
 * @hideinitializer
 * Return the given #SUtterance child class object as a #SUtterance object.
 * @param SELF The given object.
 * @return Given object as #SUtterance* type.
 * @note This casting is not safety checked.
 */
#define S_UTTERANCE(SELF)    ((SUtterance *)(SELF))


/**
 * @hideinitializer
 * Call the given function method of the given #SUtterance,
 * see full description #S_UTTERANCE_CALL for usage.
 * @param SELF The given #SUtterance*.
 * @param FUNC The function method of the given object to call.
 * @note This casting is not safety checked.
 * @note Example usage: @code S_UTTERANCE_CALL(self, func)(param1, param2, ..., paramN); @endcode
 * where @c param1, @c param2, ..., @c paramN are the parameters passed to the object function
 * @c func.
 */
#define S_UTTERANCE_CALL(SELF, FUNC)				\
	((SUtteranceClass *)S_OBJECT_CLS(SELF))->FUNC


/**
 * @hideinitializer
 * Test if the given function method of the given #SUtterance
 * can be called.
 * @param SELF The given #SUtterance*.
 * @param FUNC The function method of the given object to check.
 * @return #TRUE if function can be called, otherwise #FALSE.
 * @note This casting is not safety checked.
 */
#define S_UTTERANCE_METH_VALID(SELF, FUNC)		\
	S_UTTERANCE_CALL(SELF, FUNC) ? TRUE : FALSE


/**
 * @}
 */


/************************************************************************************/
/*                                                                                  */
/* SUtterance definition                                                            */
/*                                                                                  */
/************************************************************************************/

/**
 * The SUtterance structure.
 * An Utterance consists of a set of #SRelation which in turn consists of a set of #SItem.
 * @extends SObject
 */
struct SUtterance
{
	/**
	 * @protected Inherit from #SObject.
	 */
	SObject  obj;

	/**
	 * @protected The voice that this utterance is associated with.
	 */
	SVoice  *voice;

	/**
	 * @protected Utterance features.
	 */
	SMap    *features;

	/**
	 * @protected Relations in utterance.
	 */
	SMap    *relations;

 	/**
	 * @protected Locking mutex.
	 */
	S_DECLARE_MUTEX(utt_mutex);
};


/************************************************************************************/
/*                                                                                  */
/* SUtteranceClass definition                                                       */
/*                                                                                  */
/************************************************************************************/

/**
 * The SUtteranceClass structure.
 * @extends SObjectClass
 */
typedef struct
{
 	/* Class members */
	/**
	 * @protected Inherit from #SObjectClass.
	 */
	SObjectClass  _inherit;

 	/* Class methods */
	/**
	 * @protected Voice function pointer.
	 * Get the voice that the given utterance is associated with.
	 * @param self The given utterance.
	 * @param error Error code.
	 * @return Pointer to the voice.
	 */
	const SVoice    *(*voice)           (const SUtterance *self, s_erc *error);

	/**
	 * @protected NewRelation function pointer.
	 * Create a new named relation in given utterance. Creates a
	 * relation, initializes it (#SRelationInit), and sets it in the
	 * utterance (#SUtteranceClass::set_relation).
	 *
	 * @param self The given utterance.
	 * @param name The new relation's name.
	 * @param error Error code.
	 *
	 * @return Pointer to newly created relation.
	 */
	SRelation       *(*new_relation)    (SUtterance *self, const char *name,
										 s_erc *error);

	/**
	 * @protected GetRelation function pointer.
	 * Return the named relation.
	 * @param self The given utterance.
	 * @param name The name of the relation to return.
	 * @param error Error code.
	 * @return Pointer to named relation.
	 */
	const SRelation *(*get_relation)    (const SUtterance *self, const char *name,
										 s_erc *error);

	/**
	 * @protected SetRelation function pointer.
	 * Set the named relation in the given utterance.
	 *
	 * @param self The utterance.
	 * @param rel The relation to set in the utterance.
	 * @param error Error code.
	 *
	 * @note The relation must have a name, and will be set in the
	 * utterance with this name. If a relation with the same name already
	 * exists in the utterance then it will be deleted.
	 */
	void             (*set_relation)    (SUtterance *self, SRelation *rel,
										 s_erc *error);

	/**
	 * @protected DelRelation function pointer.
	 * Delete the named relation.
	 * @param self The given utterance.
	 * @param name The name of the relation to delete.
	 * @param error Error code.
	 */
	void             (*del_relation)    (SUtterance *self, const char *name,
										 s_erc *error);

	/**
	 * @protected HaveRelation function pointer.
	 * Query the presence of a named relation.
	 * @param self The given utterance.
	 * @param name The name of the relation.
	 * @param error Error code.
	 * @return #TRUE if found, else #FALSE.
	 */
	s_bool           (*have_relation)   (const SUtterance *self, const char *name,
										 s_erc *error);

	/**
	 * @protected RelationKeys function pointer.
	 * Get the keys of the relations in the given
	 * utterance.
	 *
	 * @param self The given utterance.
	 * @param error Error code.
	 *
	 * @return Pointer to a #SList containing the keys of the
	 * relations of the utterance. The list objects (keys) are string
	 * objects and are accessed with #SObjectGetString.
	 *
	 * @note Caller is responsible for the memory of the returned
	 * #SList object.
	 */
	SList           *(*relations_keys)  (const SUtterance *self, s_erc *error);
} SUtteranceClass;


/************************************************************************************/
/*                                                                                  */
/* Function prototypes                                                              */
/*                                                                                  */
/************************************************************************************/

/**
 * @name Initialize
 * @{
 */


/**
 * Initialize a newly created utterance for the given voice.
 * @public @memberof SUtterance
 *
 * @param self The newly created utterance to initialize.
 * @param voice The voice associated with this utterance.
 * @param error Error code.
 *
 * @note If this function fails the utterance will be deleted and the @c
 * self pointer will be set to @c NULL.
 */
S_API void SUtteranceInit(SUtterance **self, const SVoice *voice, s_erc *error);


/**
 * @}
 */


/**
 * @name Voice
 * @{
 */


/**
 * Get the voice that this utterance is associated with.
 * @public @memberof SUtterance
 * @param self The utterance.
 * @param error Error code.
 * @return Pointer to the voice.
 */
S_API const SVoice *SUtteranceVoice(const SUtterance *self, s_erc *error);


/**
 * @}
 */


/**
 * @name Relations
 * @{
 */


/**
 * Create a new named relation in given utterance.  Creates a
 * relation, initializes it (#SRelationInit), and sets it in the
 * utterance (#SUtteranceSetRelation).
 * @public @memberof SUtterance
 *
 * @param self The utterance.
 * @param name The new relation's name.
 * @param error Error code.
 *
 * @return Pointer to newly created relation.
 */
S_API SRelation *SUtteranceNewRelation(SUtterance *self, const char *name, s_erc *error);


/**
 * Return the named relation.
 * @public @memberof SUtterance
 * @param self The utterance.
 * @param name The name of the relation to return.
 * @param error Error code.
 * @return Pointer to named relation.
 */
S_API const SRelation *SUtteranceGetRelation(const SUtterance *self, const char *name,
											 s_erc *error);


/**
 * Set the named relation in the given utterance.
 * @public @memberof SUtterance
 *
 * @param self The utterance.
 * @param rel The relation to set in the utterance.
 * @param error Error code.
 *
 * @note The relation must have a name, and will be set in the
 * utterance with this name. If a relation with the same name already
 * exists in the utterance then it will be deleted.
 */
S_API void SUtteranceSetRelation(SUtterance *self, SRelation *rel, s_erc *error);


/**
 * Delete the named relation.
 * @public @memberof SUtterance
 * @param self The utterance.
 * @param name The name of the relation to delete.
 * @param error Error code.
 */
S_API void SUtteranceDelRelation(SUtterance *self, const char *name, s_erc *error);


/**
 * Query the presence of a named relation.
 * @public @memberof SUtterance
 * @param self The utterance.
 * @param name The name of the relation.
 * @param error Error code.
 * @return #TRUE if found, else #FALSE.
 */
S_API s_bool SUtteranceRelationIsPresent(const SUtterance *self, const char *name, s_erc *error);


/**
 * Get the keys of the relations of the given utterance.
 * @public @memberof SUtterance
 *
 * @param self The given utterance.
 * @param error Error code.
 *
 * @return Pointer to a #SList containing the keys of the relations of
 * the utterance. The list objects (keys) are string objects and are
 * accessed with #SObjectGetString.
 *
 * @note Caller is responsible for the memory of the returned #SList
 * object.
 */
S_API SList *SUtteranceRelationsKeys(const SUtterance *self, s_erc *error);


/**
 * @}
 */


/**
 * Get an id to assign to items in the utterance.
 * @private @memberof SUtterance
 * @param self The utterance.
 * @param error Error code.
 * @return An integer valued id number.
 */
S_LOCAL sint32 SUtteranceGetNextId(const SUtterance *self, s_erc *error);


/**
 * @name Features
 * @{
 */


/**
 * Get the named feature from the given utterance.
 * @public @memberof SUtterance
 * @param self The utterance.
 * @param name The feature name.
 * @param error Error code.
 * @return Pointer to #SObject object feature value.
 */
S_API const SObject *SUtteranceGetFeature(const SUtterance *self, const char *name,
										  s_erc *error);


/**
 * Set the named feature for the given utterance.
 * @public @memberof SUtterance
 * @param self The given utterance.
 * @param name The feature name (key).
 * @param object The feature object (value).
 * @param error Error code.
 */
S_API void SUtteranceSetFeature(SUtterance *self, const char *name,
								const SObject *object, s_erc *error);


/**
 * Delete the named feature for the given utterance.
 * @public @memberof SUtterance
 *
 * @param self The given utterance.
 * @param name The feature name (key).
 * @param error Error code.
 */
S_API void SUtteranceDelFeature(SUtterance *self, const char *name,
								s_erc *error);


/**
 * Query if named feature is present in the given utterance.
 * @public @memberof SUtterance
 * @param self The given utterance.
 * @param name The feature name.
 * @param error Error code.
 * @return #TRUE if present or #FALSE if not.
 */
S_API s_bool SUtteranceFeatureIsPresent(const SUtterance *self, const char *name, s_erc *error);


/**
 * Get the keys of the features of the given utterance.
 * @public @memberof SUtterance
 *
 * @param self The given utterance.
 * @param error Error code.
 *
 * @return Pointer to a #SList containing the keys of the features of
 * the utterance. The list objects (keys) are string objects and are
 * accessed with #SObjectGetString.
 *
 * @note Caller is responsible for the memory of the returned #SList
 * object.
 */
S_API SList *SUtteranceFeatKeys(const SUtterance *self, s_erc *error);


/**
 * @}
 */


/**
 * Add the SUtterance class to the object system.
 * @private
 * @param error Error code.
 */
S_LOCAL void _s_utterance_class_add(s_erc *error);


/************************************************************************************/
/*                                                                                  */
/* End external c declaration                                                       */
/*                                                                                  */
/************************************************************************************/
S_END_C_DECLS


/**
 * @}
 * end documentation
 */

#endif /* _SPCT_UTTERANCE_H__ */
