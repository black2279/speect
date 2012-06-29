/************************************************************************************/
/* Copyright (c) 2009-2011 The Department of Arts and Culture,                      */
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
/* DATE    : December 2009                                                          */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* A tokenizer class implementation.                                                */
/* Loosely based on EST_Token of Edinburgh Speech Tools,                            */
/* http://www.cstr.ed.ac.uk/projects/speech_tools (1.2.96)                          */
/* Note that this is a derived work with no verbatim source code from above         */
/* mentioned project.                                                               */
/*                                                                                  */
/************************************************************************************/

#ifndef _SPCT_TOKENIZER_H__
#define _SPCT_TOKENIZER_H__


/************************************************************************************/
/* Edinburgh Speech Tools license,  EST_Token                                       */
/*                                                                                  */
/*                     Centre for Speech Technology Research                        */
/*                          University of Edinburgh, UK                             */
/*                           Copyright (c) 1996                                     */
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
/*       Author :  Alan W Black                                                     */
/*                                                                                  */
/************************************************************************************/

/**
 * @file tokenizer.h
 * A tokenizer class implementation.
 */


/**
 * @defgroup STokenizer Tokenizer
 * A tokenizer class implementation. A class that allows the reading
 * of tokens (#SToken) from a file or string.  It automatically
 * tokenizes a file based on user definable whitespace and
 * punctuation.
 *
 * The definitions of whitespace and punctuation are user definable.
 * Also support for single character symbols is included.  Single
 * character symbols are @e always are treated as individual tokens
 * irrespective of their white space context.  Also a quote mode can
 * be used to read quoted tokens.
 *
 * The setting of whitespace, pre and post punctuation, single
 * character symbols and quote mode must be done (immediately) after
 * opening the stream.
 *
 * There is no unget but peek provides look ahead of one token.
 *
 * The default character symbols are:
 * <table>
 *  <tr>
 *   <td> White-space </td>
 *   <td> @code \t \n \r @endcode </td>
 *  </tr>
 *  <tr>
 *   <td> Single characters </td>
 *   <td> @code ( ) { } [ ] @endcode </td>
 *  </tr>
 *  <tr>
 *   <td> Pre-punctuation </td>
 *   <td> @code " ' ` ( { [ @endcode </td>
 *  </tr>
 *  <tr>
 *   <td> Post-punctuation </td>
 *   <td> @code " ' ` . , : ; ! ? ( ) { } [ ] @endcode </td>
 *  </tr>
 * </table>
 * @{
 */


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "include/common.h"
#include "base/errdbg/errdbg.h"
#include "utils/token.h"


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
 * @hideinitializer
 * Return the given #STokenizer child/parent class object as a
 * #STokenizer object.
 *
 * @param SELF The given object.
 *
 * @return Given object as #STokenizer* type.
 * @note This casting is not safety checked.
 */
#define S_TOKENIZER(SELF)    ((STokenizer *)(SELF))


/************************************************************************************/
/*                                                                                  */
/* STokenizer definition                                                            */
/*                                                                                  */
/************************************************************************************/

/**
 * The tokenizer structure.
 * @extends SObject
 */
typedef struct
{
	/**
	 * @protected Inherit from #SObject.
	 */
	SObject     obj;

	/**
	 * @protected Preceding white-space symbols definition.
	 */
	char       *white_space_chars;

	/**
	 * @protected Single character symbols definition.
	 */
	char       *single_char_symbols;

	/**
	 * @protected Pre-punctuation symbols definition.
	 */
	char       *pre_punc_symbols;

	/**
	 * @protected Post-punctuation symbols definition.
	 */
	char       *post_punc_symbols;

	/**
	 * @protected Current token.
	 */
	SToken     *currentToken;

	/**
	 * @protected End-of-file flag.
	 */
	s_bool      eof;

	/**
	 * @protected Current character.
	 */
	uint32      current_char;

	/**
	 * @protected Peeked flag.
	 */
	s_bool      peeked;

	/**
	 * @protected Quote character.
	 */
	uint32      quote;

	/**
	 * @protected Escape character.
	 */
	uint32      escape;

	/**
	 * @protected Quote mode flag.
	 */
	s_bool      quote_mode;
} STokenizer;


/************************************************************************************/
/*                                                                                  */
/* STokenizerClass definition                                                       */
/*                                                                                  */
/************************************************************************************/

/**
 * The tokenizer class structure.
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
	 * Get character. Get the next character from the tokenizer and
	 * set it the @c current_char tokenizer member.
	 *
	 * @param self The tokenizer.
	 * @param error Error code.
	 *
	 * @return The next character from the tokenizer, as UTF8
	 * character represented as a 4-byte unsigned integer.
	 */
	uint32 (* const get_char)(STokenizer *self, s_erc *error);

	/**
	 * Seek to the given position. Seek is relative to beginning of
	 * tokenizer source.
	 *
	 * @param self The tokenizer.
	 * @param pos The position to seek to.
	 * @param error Error code.
	 */
	void (* const seek)(STokenizer *self, ulong pos, s_erc *error);

	/**
	 * Query the position in the tokenizer.
	 *
	 * @param self The tokenizer.
	 * @param error Error code.
	 *
	 * @return The position in the tokenizer.
	 */
	ulong (* const tell)(const STokenizer *self, s_erc *error);

	/**
	 * Get the next token.
	 *
	 * @param self The tokenizer.
	 * @param error Error code.
	 *
	 * @return The next token.
	 */
	const SToken *(* const get_token)(STokenizer *self, s_erc *error);

	/**
	 * Peek the next token.
	 *
	 * @param self The tokenizer.
	 * @param error Error code.
	 *
	 * @return The next token (peeked).
	 */
	const SToken *(* const peek_token)(STokenizer *self, s_erc *error);

	/**
	 * Set the tokenizer white-space characters.
	 *
	 * @param self The tokenizer.
	 * @param white_space_chars The white-space characters to set.
	 * @param error Error code.
	 */
	void (* const set_whitespace_chars)(STokenizer *self, const char *white_space_chars, s_erc *error);

	/**
	 * Set the tokenizer single character symbols.
	 *
	 * @param self The tokenizer.
	 * @param single_chars The single characters symbols to set.
	 * @param error Error code.
	 */
	void (* const set_single_chars)(STokenizer *self, const char *single_chars, s_erc *error);

	/**
	 * Set the tokenizer pre-punctuation symbols.
	 *
	 * @param self The tokenizer.
	 * @param single_chars The pre-punctuation symbols to set.
	 * @param error Error code.
	 */
	void (* const set_prepunc_chars)(STokenizer *self, const char *pre_punc_chars, s_erc *error);

	/**
	 * Set the tokenizer post-punctuation symbols.
	 *
	 * @param self The tokenizer.
	 * @param single_chars The post-punctuation symbols to set.
	 * @param error Error code.
	 */
	void (* const set_postpunc_chars)(STokenizer *self, const char *post_punc_chars, s_erc *error);

	/**
	 * Set the tokenizer quote symbols.
	 *
	 * @param self The tokenizer.
	 * @param quote The quote symbol (use #s_getc to get the character
	 * in a 4-byte unsigned integer).
	 * @param escape The escape symbol (use #s_getc to get the character
	 * in a 4-byte unsigned integer).
	 * @param error Error code.
	 */
	void (* const set_quotes)(STokenizer *self, uint32 quote, uint32 escape, s_erc *error);

	/**
	 * Query if the tokenizer is in quote mode.
	 *
	 * @param self The tokenizer.
	 * @param error Error code.
	 *
	 * @return @c TRUE if in quote mode else @c FALSE
	 */
	s_bool (* const query_quote_mode)(const STokenizer *self, s_erc *error);

	/**
	 * Query if the tokenizer is at the end of the file.
	 *
	 * @param self The tokenizer.
	 * @param error Error code.
	 *
	 * @return @c TRUE if at the end of the file else @c FALSE
	 */
	s_bool (* const query_eof)(const STokenizer *self, s_erc *error);
} STokenizerClass;


/************************************************************************************/
/*                                                                                  */
/* Function prototypes                                                              */
/*                                                                                  */
/************************************************************************************/

/**
 * Get character. Get the next character from the tokenizer and
 * set it the @c current_char tokenizer member.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param error Error code.
 *
 * @return The next character from the tokenizer, as UTF8
 * character represented as a 4-byte unsigned integer.
 */
S_API uint32 STokenizerGetChar(STokenizer *self, s_erc *error);


/**
 * Seek to the given position. Seek is relative to beginning of
 * tokenizer source.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param pos The position to seek to.
 * @param error Error code.
 */
S_API void STokenizerSeek(STokenizer *self, ulong pos, s_erc *error);


/**
 * Query the position in the tokenizer.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param error Error code.
 *
 * @return The position in the tokenizer.
 */
S_API ulong STokenizerTell(const STokenizer *self, s_erc *error);


/**
 * Get the next token.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param error Error code.
 *
 * @return The next token.
 */
S_API const SToken *STokenizerGetToken(STokenizer *self, s_erc *error);


/**
 * Peek the next token.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param error Error code.
 *
 * @return The next token (peeked).
 */
S_API const SToken *STokenizerPeekToken(STokenizer *self, s_erc *error);


/**
 * Set the tokenizer white-space characters.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param white_space_chars The white-space characters to set.
 * @param error Error code.
 */
S_API void STokenizerSetWhitespaceChars(STokenizer *self, const char *white_space_chars, s_erc *error);


/**
 * Set the tokenizer single character symbols.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param single_chars The single characters symbols to set.
 * @param error Error code.
 */
S_API void STokenizerSetSingleChars(STokenizer *self, const char *single_chars, s_erc *error);


/**
 * Set the tokenizer pre-punctuation symbols.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param single_chars The pre-punctuation symbols to set.
 * @param error Error code.
 */
S_API void STokenizerSetPrePuncChars(STokenizer *self, const char *pre_punc_chars, s_erc *error);


/**
 * Set the tokenizer post-punctuation symbols.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param single_chars The post-punctuation symbols to set.
 * @param error Error code.
 */
S_API void STokenizerSetPostPuncChars(STokenizer *self, const char *post_punc_chars, s_erc *error);


/**
 * Set the tokenizer quote symbols.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param quote The quote symbol (use #s_getc to get the character
 * in a 4-byte unsigned integer).
 * @param escape The escape symbol (use #s_getc to get the character
 * in a 4-byte unsigned integer).
 * @param error Error code.
 */
S_API void STokenizerSetQuotes(STokenizer *self, uint32 quote, uint32 escape, s_erc *error);


/**
 * Query if the tokenizer is in quote mode.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param error Error code.
 *
 * @return @c TRUE if in quote mode else @c FALSE
 */
S_API s_bool STokenizerQueryQuoteMode(const STokenizer *self, s_erc *error);


/**
 * Query if the tokenizer is at the end of the file.
 *
 * @public @memberof STokenizer
 *
 * @param self The tokenizer.
 * @param error Error code.
 *
 * @return @c TRUE if at the end of the file else @c FALSE
 */
S_API s_bool STokenizerQueryEOF(const STokenizer *self, s_erc *error);


/**
 * Add the SToken class to the object system.
 * @private @memberof SToken
 * @param error Error code.
 */
S_LOCAL void _s_tokenizer_class_add(s_erc *error);

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

#endif /* _SPCT_TOKENIZER_H__ */