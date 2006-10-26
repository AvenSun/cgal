// TODO: Add licence
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: $
// $Id: $
// 
//
// Author(s)     : Sebastian Limbach  <slimbach@mpi-sb.mpg.de>

#ifndef CGAL_REAL_EMBEDDABLE_TRAITS_H
#define CGAL_REAL_EMBEDDABLE_TRAITS_H

#include <CGAL/basic.h>
#include <CGAL/functional_base.h> // Unary_function, Binary_function
#include <CGAL/Algebraic_structure_traits.h> // For Is_zero functor

CGAL_BEGIN_NAMESPACE


template< class Real_embeddable_ > 
class Real_embeddable_traits {
  public:
    typedef Real_embeddable_  Real_embeddable;
    typedef CGAL::Tag_false   Is_real_embeddable;

    typedef CGAL::Null_functor Abs;
    typedef CGAL::Null_functor Sign;
    typedef CGAL::Null_functor Is_finite;
    typedef CGAL::Null_functor Is_positive;
    typedef CGAL::Null_functor Is_negative;
    typedef CGAL::Null_functor Is_zero;
    typedef CGAL::Null_functor Compare;
    typedef CGAL::Null_functor To_double;
    typedef CGAL::Null_functor To_interval;
};

namespace INTERN_RET {
    template< class Real_embeddable, class AST_is_zero >
    class Is_zero_selector
      : public Unary_function< Real_embeddable, bool > {
      public:        
        //! the function call.
        bool operator()( const Real_embeddable& x ) const {
            return AST_is_zero()(x);
        }
    };
    
    template< class Real_embeddable >
    class Is_zero_selector< Real_embeddable, CGAL::Null_functor >
      : public Unary_function< Real_embeddable, bool > {
      public:        
        //! the function call.
        bool operator()( const Real_embeddable& x ) const {
            return x == Real_embeddable(0);
        }
    };
    
} // INTERN_RET

template< class Real_embeddable_ >
class Real_embeddable_traits_base {
  public:
    typedef Real_embeddable_  Real_embeddable;
    typedef CGAL::Tag_true    Is_real_embeddable; 
        
    //! The generic \c Is_zero functor implementation uses one comparison
    typedef INTERN_RET::Is_zero_selector< Real_embeddable, 
                typename Algebraic_structure_traits< Real_embeddable >::Is_zero 
                                        > Is_zero;
    
    //! The generic \c Is_finite functor returns true
    class Is_finite
      : public Unary_function< Real_embeddable, bool > {
      public:
        bool operator()( const Real_embeddable& ) const {
          return true;
        }
    };
    //! The generic \c Abs functor implementation
    //! uses one comparisons and the unary minus if necessary.
    class Abs
      : public Unary_function< Real_embeddable, Real_embeddable > {
      public:
        //! the function call.
        Real_embeddable  operator()( const Real_embeddable& x ) const {
          return( x < Real_embeddable(0) ) ? -x : x;
        }
    };
    
    //! The generic \c Sign functor implementation uses two comparisons.
    class Sign 
      : public Unary_function< Real_embeddable, CGAL::Sign > {
      public:
        //! the function call.
        CGAL::Sign operator()( const Real_embeddable& x ) const {
          if ( x < Real_embeddable(0))
            return CGAL::NEGATIVE;
          if ( x > Real_embeddable(0))
            return CGAL::POSITIVE;
          return CGAL::ZERO;
        }
    };
    
    //! The generic \c Is_positive functor implementation uses one comparison.
    class Is_positive 
      : public Unary_function< Real_embeddable, bool > {
      public:        
        //! the function call.
        bool operator()( const Real_embeddable& x ) const {
          return x > Real_embeddable(0);
        }
    };
    
    //! The generic \c Is_negative functor implementation uses one comparison.
    class Is_negative 
      : public Unary_function< Real_embeddable, bool > {
      public:        
        //! the function call.
        bool operator()( const Real_embeddable& x ) const {
          return x < Real_embeddable(0);
        }
    };
        
    //! The generic \c Compare functor implementation uses two comparisons.
    class Compare 
      : public Binary_function< Real_embeddable, Real_embeddable, 
                                Comparison_result > {
      public:
        //! the function call.
        CGAL::Comparison_result operator()( const Real_embeddable& x, 
                                            const Real_embeddable& y) const {
          if( x < y )
            return CGAL::SMALLER;
          if( x > y )
            return CGAL::LARGER;
         return CGAL::EQUAL;
        }
        
        CGAL_IMPLICIT_INTEROPERABLE_BINARY_OPERATOR_WITH_RT( Real_embeddable,
                                                      CGAL::Comparison_result );
    };

    typedef CGAL::Null_functor To_double;
    typedef CGAL::Null_functor To_interval;
};

// Some common functors to be used by RET specializations
namespace INTERN_RET {
  template< class Real_embeddable, class Is_real_embeddable >
  class Real_embeddable_traits_base_selector;
  
  template< class Real_embeddable >
  class Real_embeddable_traits_base_selector< Real_embeddable, CGAL::Tag_false >
    : public Real_embeddable_traits< CGAL::Null_tag > {};
    
  template< class Real_embeddable >
  class Real_embeddable_traits_base_selector< Real_embeddable, CGAL::Tag_true >
    : public Real_embeddable_traits_base< Real_embeddable > {};

  template< class Real_embeddable >
  class To_double_by_conversion 
    : public Unary_function< Real_embeddable, double > {
    public:      
      //! the function call.
      double operator()( const Real_embeddable& x ) const {
        return static_cast<double>(x);
      }
  };
  
  template< class Real_embeddable >
  class To_interval_by_conversion 
    : public Unary_function< Real_embeddable, std::pair< double, double > > {
    public:      
        //! the function call.
      std::pair<double, double> operator()( const Real_embeddable& x ) const {

        return std::pair<double,double>( x, x );
      }
  };  
} // INTERN_RET

CGAL_END_NAMESPACE

#endif  // CGAL_REAL_EMBEDDABLE_TRAITS_H
