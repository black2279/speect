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
/* DATE    : November 2009                                                          */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* C convenience functions for SMap Python wrapper.                                 */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


%extend SMap
{
	PyObject *__getitem__(const char *key, s_erc *error)
	{
		const SObject *mapObject;
		PyObject *pobject;


		mapObject = SMapGetObjectDef($self, key, NULL, error);
		if (*error != S_SUCCESS)
			return NULL;

		if (mapObject == NULL)
		{
			PyErr_SetString(PyExc_KeyError, "key not in map");
			return NULL;
		}

		pobject = s_sobject_2_pyobject(mapObject, FALSE, error);
		if (*error != S_SUCCESS)
			return NULL;

		return pobject;
	}


	s_bool __contains__(const char *key, s_erc *error)
	{
		s_bool is_present;


		is_present = SMapObjectPresent($self, key, error);
		if (*error != S_SUCCESS)
			return FALSE;

		return is_present;
	}


	int __len__(s_erc *error)
	{
		size_t num_feats;


		num_feats = SMapSize($self, error);
		if (*error != S_SUCCESS)
			return 0;

		return (int)num_feats;
	}


	void __setitem__(const char *key, PyObject *val, s_erc *error)
	{
		SObject *newObject;


		newObject = s_pyobject_2_sobject(val, error);
		if (*error != S_SUCCESS)
			return;

		SMapSetObject($self, key, newObject, error);
		if (*error != S_SUCCESS)
			S_DELETE(newObject, "SMap::__setitem__()", error);
	}


	void __delitem__(const char *key, s_erc *error)
	{
		s_bool is_present;


		is_present = SMapObjectPresent($self, key, error);
		if (*error != S_SUCCESS)
			return;

		if (!is_present)
		{
			PyErr_SetString(PyExc_KeyError, "key not in map");
			return;
		}

		SMapObjectDelete($self, key, error);
	}


	_PMapIterator *__iter__()
	{
		_PMapIterator *pitr;
		SIterator *itr;
		s_erc error;


		S_CLR_ERR(&error);
		itr = S_ITERATOR_GET($self, &error);
		if (error != S_SUCCESS)
			return NULL;
		pitr = _make_PMapIterator(itr, &error);
		if (error != S_SUCCESS)
			return NULL;

		return pitr;
	}


%pythoncode
%{
def __str__(self):
    """
    Return a string representation of the key-value pairs that are in the map.

    :return: A string representation of the key-value pairs that are in the map.
    :rtype: string
    """

    num_features = len(self)
    if num_features > 0:
        stri = "{ "

        first = True
        count = num_features

        for ik in self:
            if not first:
                stri += '\n    '
            first = False
            count -= 1
            if count != 0:
                stri += '\'%s\' : %s,' %(ik, repr(self[ik]))
            else:
                stri += '\'%s\' : %s' %(ik, repr(self[ik]))

        if  num_features > 0:
            stri += '    }\n'

    else:
        stri = "{}"

    return stri
%}

};

