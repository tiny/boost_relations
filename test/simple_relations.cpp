/*!
  @file       simple_relations.cpp
  @brief      main test file for boost::relations

  @author     Robert McInnis
  @date       september 10, 2016
  @par        copyright (c) 2016 Solid ICE Technologies, Inc.  All rights reserved.

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#include <stdio.h>
#include <boost/relations/entity.hpp>
#include <boost/relations/reciprocal.hpp>

namespace boost { namespace relations {


EntityMgr     population ;
ReciprocalMgr recipMgr ;


void reciprocal( const std::string &a, const std::string &b )
{
  recipMgr.set( a, b ) ;
} // :: reciprocal 

void reciprocal( const std::string &a, const std::string &b, const std::vector<std::string> &meta ) 
{
  if (meta.size() >= 2)
    recipMgr.set( a, b, meta[0], meta[1] ) ;
} // :: reciprocal 

void apply_reciprocals( Entity &e1, const std::string &relation, Entity &e2 ) 
{
  ReciprocalVec        &vec = recipMgr.get( relation ) ;
  ReciprocalVec_iter    it ;

  for (it = vec.begin(); it != vec.end(); it++)
  {
    if ((*it).fits(e2))
    {
      e2.link( (*it).name, e1 ) ;
    }
  }

  e1.link( relation, e2 ) ;
} // :: apply_reciprocals

void create_entities()
{
  population.get(  1 ).meta({ "firstname", "joe",
                              "lastname" , "smith",
                              "gender"   , "male" 
                           }) ;

  population.get(  2 ).meta({ "firstname", "fred",
                              "lastname" , "smith",
                              "gender"   , "male" 
                           }) ;

  population.get(  3 ).meta({ "firstname", "april",
                              "lastname" , "smith",
                              "gender"   , "female" 
                           }) ;

  population.get(  4 ).meta({ "firstname", "greg",
                              "lastname" , "smith",
                              "gender"   , "male" 
                           }) ;

  population.get(  5 ).meta({ "firstname", "bobby-sue",
                              "lastname" , "smith",
                              "gender"   , "female" 
                           }) ;

  population.get(  6 ).meta({ "firstname", "tommy",
                              "lastname" , "smith",
                              "gender"   , "male" 
                           }) ;

} // :: create_entities()

void link( uint32_t  id1, const std::string &relation, uint32_t id2 ) 
{
  Entity &e1 = population.get( id1 ) ;
  Entity &e2 = population.get( id2 ) ;

  apply_reciprocals( e1, relation, e2 ) ;
} // :: link

void create_relations()
{
  // define reciprocal relation types 
  // -  sometimes meta data must must
  //
  reciprocal ( "married" , "wife"    , { "gender", "female" }) ;
  reciprocal ( "married" , "husband" , { "gender", "male" }) ;

  reciprocal ( "husband" , "wife"    , { "gender", "female" }) ;
  reciprocal ( "wife"    , "husband" , { "gender", "male" }) ;

  reciprocal ( "son"     , "parent" ) ;
  reciprocal ( "son"     , "father" , { "gender", "male" }) ;
  reciprocal ( "son"     , "mother" , { "gender", "female" }) ;

  reciprocal ( "father"  , "child" ) ;
  reciprocal ( "father"  , "son"      , { "gender", "male" }) ;
  reciprocal ( "father"  , "daughter" , { "gender", "female" }) ;

  reciprocal ( "mother"  , "child" ) ;
  reciprocal ( "mother"  , "son"      , { "gender", "male" }) ;
  reciprocal ( "mother"  , "daughter" , { "gender", "female" }) ;

  reciprocal ( "daughter", "parent" ) ;
  reciprocal ( "daughter", "father" , { "gender", "male" }) ;
  reciprocal ( "daughter", "mother" , { "gender", "female" }) ;

  reciprocal ( "nephew"  , "uncle"  , { "gender", "male" }) ;
  reciprocal ( "nephew"  , "aunt"   , { "gender", "female" }) ;
  reciprocal ( "niece"   , "uncle"  , { "gender", "male" }) ;
  reciprocal ( "niece"   , "aunt"   , { "gender", "female" }) ;

  reciprocal ( "uncle"   , "nephew" , { "gender", "male" }) ;
  reciprocal ( "uncle"   , "niece"  , { "gender", "female" }) ;

  reciprocal ( "aunt"    , "nephew" , { "gender", "male" }) ;
  reciprocal ( "aunt"    , "niece"  , { "gender", "female" }) ;

  reciprocal ( "sister"  , "sibling" ) ;
  reciprocal ( "sister"  , "brother", { "gender", "male" }) ;
  reciprocal ( "sister"  , "sister" , { "gender", "female" }) ;

  reciprocal ( "brother" , "sibling" ) ;
  reciprocal ( "brother" , "brother", { "gender", "male" }) ;
  reciprocal ( "brother" , "sister" , { "gender", "female" }) ;

  // now actually define the relationships between the entities
  // - i used entity-ids instead of a name.  
  // -  unique names might be a pain, but it'd be easy enough to implement
  //
  link(  1, "son"     ,  2 ) ;  // 1 == joe smith   ; 2 == fred smith
  link(  2, "married" ,  3 ) ;  // 3 == april smith 
  link(  4, "son"     ,  2 ) ;  // 4 == greg smith
  link(  5, "daughter",  2 ) ;  // 5 == bobby-sue smith
  link(  6, "son"     ,  2 ) ;  // 6 == tommy smith

} // :: create_relations

// untwist the pretzal
//
void list_relations()
{
  Entity  &e = population.get( 2 ) ;

  RelationMap       &links = e.relations() ;
  RelationMap_iter   it ;
  EntityVec_iter     it2 ;

  std::string  a, b, is_the ;

  for (it = links.begin(); it != links.end(); it++)
  {
    for (it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
    {
      //  'a' is-the 'father' (of) 'b'
      //
      a      = e.name() ;
      is_the = (*it).first ;
      b      = (*it2)->name() ;
      
      printf( "%s   %s   %s \n", a.c_str(), is_the.c_str(), b.c_str() ) ;
    }
  }
} // :: list_relations

}} ;

//-----------------------------------------------------------------------------
//
//

int main()
{
  boost :: relations :: create_entities() ;
  boost :: relations :: create_relations() ;
  boost :: relations :: list_relations() ;

  return 0 ;
} // :: main

