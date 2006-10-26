// Copyright (c) 2006  
// INRIA Sophia-Antipolis (France), 
// Max-Planck-Institute Saarbruecken (Germany).
// All rights reserved.
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Sylvain Pion, Michael Hemmer 

// This is a backward compatibility header.

#ifndef CGAL_BINARY_OPERATOR_RESULT_H
#define CGAL_BINARY_OPERATOR_RESULT_H

#include <CGAL/basic.h>
#include <CGAL/Coercion_traits.h>

// This class helps finding out the result type of mixed operators +-*/.
// For example it answers what the type of double+int is.

// This class is meant to be specialized for some number types pairs,
// when a mixed operator is defined.

CGAL_BEGIN_NAMESPACE

// fwd
template <class T1,class T2> struct Coercion_traits;

template < typename T1, typename T2 >
class Binary_operator_result{
    typedef Coercion_traits<T1,T2> CT;
public:
    typedef typename CT::Coercion_type type;
};

CGAL_END_NAMESPACE

#endif // CGAL_BINARY_OPERATOR_RESULT_H
