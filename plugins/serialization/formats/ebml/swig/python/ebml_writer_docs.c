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
/* AUTHOR  : Richard Carlson, Aby Louw                                              */
/* DATE    : December 2009                                                          */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* Python documentation strings for SEbmlWrite.                                     */
/*                                                                                  */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/

%define ebml_writer_DOCSTRING
"""
SEbmlWrite(path [, doc_type = 'spct' [, doc_type_version = 1 [, doc_type_read_version = 1 [,
	   ebml_version = 1 [, ebml_read_version = 1 [, max_id_width = 4 [,
	   max_size_width = 4]]]]]]])

EBML data writer class. Create an Ebml writer object with a path to the file
to write to and EBML header information. The header will be written to file.
The file is closed when the reader is deleted.

:param path: Full path and file name of the file to write to.
:type path: string
:param doc_type: An ASCII string that identifies the type of the document.
:type doc_type: string
:param doc_type_version: The version of document type to which the document conforms to.
:type doc_type_version: int
:param doc_type_read_version: The minimum ``doc_type_version`` version an interpreter has to
       			      support in order to read the document.
:type doc_type_read_version: int
:param ebml_version: The version of EBML to which the document conforms to.
:type ebml_version: int
:param ebml_read_version: The minimum EBML version a parser has to support in order to read
       			  the document.
:type ebml_read_version: int
:param max_id_width: The maximum width of the ``IDs`` used in this document.
:type max_id_width: int
:param max_size_width: The maximum width of the size descriptors used in this document.
:type max_size_width: int

:return: EBML data writer object.
:rtype: SEbmlWrite

:note: Speect does not support ``max_id_width`` or ``max_size_width`` greater than 4.
"""
%enddef

%feature("autodoc", ebml_writer_DOCSTRING) SEbmlWrite;


%define write_uint_DOCSTRING
"""
write_uint(val, id)

Write an unsigned integer to the EBML data source.

:param val: The unsigned integer to write.
:type val: int
:param id: The ID of the unsigned integer element.
:type id: int
"""
%enddef

%feature("autodoc", write_uint_DOCSTRING) SEbmlWrite::write_uint;


%define write_sint_DOCSTRING
"""
write_sint(val, id)

Write an signed integer to the EBML data source.

:param val: The signed integer to write.
:type val: int
:param id: The ID of the signed integer element.
:type id: int
"""
%enddef

%feature("autodoc", write_sint_DOCSTRING) SEbmlWrite::write_sint;

%define write_string_DOCSTRING
"""
write_string(val, id)

Write a string to the EBML data source.

:param val: The string to write.
:type val: string
:param id: The ID of the string.
:type id: int
"""
%enddef

%feature("autodoc", write_string_DOCSTRING) SEbmlWrite::write_str;


%define write_double_DOCSTRING
"""
write_double(val, id)

Write a double value to the EBML data source.

:param val: The double value to write.
:type val: float
:param id: The ID of the double element.
:type id: int
"""
%enddef

%feature("autodoc", write_double_DOCSTRING) SEbmlWrite::write_double;


%define write_object_DOCSTRING
"""
write_object(val, id)

Write an object to the EBML data source.

:param val: The object to write.
:type val: SObject
:param id: The ID of the object.
:type id: int

:note: The object must have an appropriate *serialization formatter*
       defined and registered for the ``spct_ebml`` format. If not,
       then the object can not be written.
"""
%enddef

%feature("autodoc", write_object_DOCSTRING) SEbmlWrite::write_object;


%define start_container_DOCSTRING
"""
start_container(id)

Start a container element with the given id.

:param id: The ID of the container.
:type id: int
"""
%enddef

%feature("autodoc", start_container_DOCSTRING) SEbmlWrite::start_container;


%define end_container_DOCSTRING
"""
end_container(id)

End (close) a previously started a container element.
"""
%enddef

%feature("autodoc", end_container_DOCSTRING) SEbmlWrite::end_container;

