// Copyright (c) 2005  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
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

#ifndef CGAL_LONG_DOUBLE_H
#define CGAL_LONG_DOUBLE_H

#include <CGAL/basic.h>
#include <CGAL/Algebraic_structure_traits.h>
#include <CGAL/Real_embeddable_traits.h>
#include <CGAL/utils.h>
#include <CGAL/functional_base.h> // Unary_function, Binary_function

#include <utility>
#include <cmath>
#ifdef CGAL_CFG_IEEE_754_BUG
#  include <CGAL/IEEE_754_unions.h>
#endif
#ifdef __sgi
#  include <fp_class.h>
#endif

#include <CGAL/FPU.h>

CGAL_BEGIN_NAMESPACE

template<> struct Number_type_traits<long double> {
  typedef Tag_false  Has_gcd;
  typedef Tag_true   Has_division;
  typedef Tag_true   Has_sqrt;

  typedef Tag_false  Has_exact_ring_operations;
  typedef Tag_false  Has_exact_division;
  typedef Tag_false  Has_exact_sqrt;
};

template <> class Algebraic_structure_traits< long double >
  : public Algebraic_structure_traits_base< long double, 
                                            CGAL::Field_with_kth_root_tag >  {
  public:
    typedef CGAL::Tag_false            Is_exact;
                
    class Sqrt 
      : public Unary_function< Algebraic_structure, Algebraic_structure > {
      public:
        Algebraic_structure operator()( const Algebraic_structure& x ) const {
          return CGAL_CLIB_STD::sqrt( x );
        }
    };
    
    class Kth_root 
      :public Binary_function<int, Algebraic_structure, Algebraic_structure > {
      public:
        Algebraic_structure operator()( int k, 
                                        const Algebraic_structure& x) const {
          CGAL_precondition_msg( k > 0, 
                                    "'k' must be positive for k-th roots");
          return CGAL_CLIB_STD::pow(x, 1.0 / (long double)(k));
        };
    };
    
};

template <> class Real_embeddable_traits< long double > 
  : public Real_embeddable_traits_base< long double > {
  public:

    typedef CGAL::INTERN_RET::To_double_by_conversion< Real_embeddable >
                                                                  To_double;      
    class To_interval 
      : public Unary_function< Real_embeddable, std::pair< double, double > > {
      public:
        std::pair<double, double> operator()( const Real_embeddable& x ) const {

          // We hope that the long double -> double conversion
          // follows the current rounding mode.
          Protect_FPU_rounding<true> P(CGAL_FE_UPWARD);
          volatile long double mx = -x; // needed otherwise the conversion can
                                        // get factorized between d and -d...
          return std::make_pair(- (double) CGAL_IA_FORCE_TO_DOUBLE(mx),
                                (double) CGAL_IA_FORCE_TO_DOUBLE(x));
          
        }
    };

// Is_finite depends on platform
#ifdef __sgi
    class Is_finite 
      : public Unary_function< Real_embeddable, bool > {
      public:
        bool operator()( const Real_embeddable& x ) {
          switch (fp_class_d(x)) {
          case FP_POS_NORM:
          case FP_NEG_NORM:
          case FP_POS_ZERO:
          case FP_NEG_ZERO:
          case FP_POS_DENORM:
          case FP_NEG_DENORM:
              return true;
          case FP_SNAN:
          case FP_QNAN:
          case FP_POS_INF:
          case FP_NEG_INF:
              return false;
          }
          return false; // NOT REACHED
        }
    };
#elif defined CGAL_CFG_IEEE_754_BUG
    class Is_finite 
      : public Unary_function< Real_embeddable, bool > {
      public:
        bool operator()( const Real_embeddable& x ) {
          Real_embeddable d = x;
          IEEE_754_double* p = reinterpret_cast<IEEE_754_double*>(&d);
          return is_finite_by_mask_long_double( p->c.H );
        }
    };
#else
    class Is_finite 
      : public Unary_function< Real_embeddable, bool > {
      public:
        bool operator()( const Real_embeddable& x ) {
         return (x == x) && (is_valid(x-x)); 
        }
    };
#endif
    
};

#ifdef __sgi

template<>
class Is_valid< long double > 
  : public Unary_function< long double, bool > {
  public :
    bool operator()( const long_double& x ) {
      switch (fp_class_d(x)) {
      case FP_POS_NORM:
      case FP_NEG_NORM:
      case FP_POS_ZERO:
      case FP_NEG_ZERO:
      case FP_POS_INF:
      case FP_NEG_INF:
      case FP_POS_DENORM:
      case FP_NEG_DENORM:
          return true;
      case FP_SNAN:
      case FP_QNAN:
          return false;
      }
      return false; // NOT REACHED
    }  
};

#elif defined CGAL_CFG_IEEE_754_BUG

#define CGAL_EXPONENT_DOUBLE_MASK   0x7ff00000
#define CGAL_MANTISSA_DOUBLE_MASK   0x000fffff

inline
bool
is_finite_by_mask_long_double(unsigned int h)
{
  unsigned int e = h & CGAL_EXPONENT_DOUBLE_MASK;
  return ( ( e ^ CGAL_EXPONENT_DOUBLE_MASK ) != 0 );
}

inline
bool
is_nan_by_mask_long_double(unsigned int h, unsigned int l)
{
  if ( is_finite_by_mask_long_double(h) )
      return false;
  return (( h & CGAL_MANTISSA_DOUBLE_MASK ) != 0) || (( l & 0xffffffff ) != 0);
}

template<>
class Is_valid< long double > 
  : public Unary_function< long double, bool > {
  public :
    bool operator()( const long_double& x ) {
      double d = x;
      IEEE_754_double* p = reinterpret_cast<IEEE_754_double*>(&d);
      return ! ( is_nan_by_mask_long_double( p->c.H, p->c.L ));
    }  
};

#else

template<>
class Is_valid< long double > 
  : public Unary_function< long double, bool > {
  public :
    bool operator()( const long double& x ) {
      return (x == x);
    }  
};

#endif

inline
io_Read_write
io_tag(const long double &)
{ return io_Read_write(); }

CGAL_END_NAMESPACE

#endif // CGAL_LONG_DOUBLE_H
