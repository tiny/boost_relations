/*!
  @file       reciprocal.hpp
  @brief      Reciprocal relations class definition

  @author     Robert McInnis
  @date       september 21, 2016
  @par        copyright (c) 2016 Solid ICE Technologies, Inc.  All rights reserved.

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef RECIPROCAL_HPP
#define RECIPROCAL_HPP

#include <boost/relations/entity.hpp>

namespace boost { namespace relations {

class Reciprocal
{
  public  :
    std::string    name ;
    std::string    cond_key ;   // meta condition
    std::string    cond_value ; // meta condition

                   Reciprocal( const Reciprocal &c ) { *this = c ; }
                   Reciprocal( const std::string &name_ ) : name( name_ ) {}
                   Reciprocal( const std::string &name_, const std::string &meta_key, const std::string &meta_value ) 
                   : name( name_ ), cond_key( meta_key ), cond_value( meta_value ) {}

    Reciprocal    &operator= ( const Reciprocal &c )
                   {
                     if (this != &c)
                     {
                       name       = c.name ;
                       cond_key   = c.cond_key ;
                       cond_value = c.cond_value ;
                     }
                     return *this ;
                   }
    bool           fits( Entity &other ) 
                   { if (cond_key.empty())  return true ;
                     MetaVec  *meta = other.get_meta( cond_key ) ;
                     if (meta == NULL)  return false ;
                     MetaVec_iter  it ;
                     for (it = meta->begin(); it != meta->end(); it++)
                     {
                       if ((*it) == cond_value)
                         return true ;
                     }
                     return false ;
                   }
} ; // class Reciprocal

typedef std::vector<Reciprocal>            ReciprocalVec ;
typedef std::vector<Reciprocal>::iterator  ReciprocalVec_iter ;

typedef std::map< std::string, ReciprocalVec >               ReciprocalMap ;
typedef std::map< std::string, ReciprocalVec >::iterator     ReciprocalMap_iter ;
typedef std::map< std::string, ReciprocalVec >::value_type   ReciprocalMap_pair ;

class ReciprocalMgr
{
  private :
    ReciprocalMap  _ties ;

  public  :
                   ReciprocalMgr() {}

    ReciprocalVec &get( const std::string &a )  // a == 'son' of 'son of parent'
                   {
                     ReciprocalMap_iter it = _ties.find( a ) ;
                     if (it == _ties.end())
                     {
                       _ties.insert( ReciprocalMap_pair( a, {} )) ;
                       it = _ties.find( a ) ;
                     }
                     return (*it).second ;
                   }

    void           set( const std::string &a, const std::string &b ) // ie:  'son' of 'parent'; a == 'son' and b == 'parent'
                   {
                     ReciprocalVec  &vec( get( a ) ) ;
                     vec.push_back( Reciprocal( b ) ) ;
                   }
    void           set( const std::string &a, const std::string &b, const std::string &meta_key, const std::string &meta_value ) 
                   {
                     ReciprocalVec  &vec( get( a ) ) ;
                     vec.push_back( Reciprocal( b, meta_key, meta_value ) ) ;
                   }

    uint32_t       size() const { return _ties.size(); }
} ; // class ReciprocalMgr

}} ; // namespace

#endif
