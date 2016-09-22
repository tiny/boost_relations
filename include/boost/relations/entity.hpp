/*!
  @file       entity.hpp
  @brief      Entity relations class definition

  @author     Robert McInnis
  @date       september 21, 2016
  @par        copyright (c) 2016 Solid ICE Technologies, Inc.  All rights reserved.

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cstdint>
#include <map>
#include <vector>

namespace boost { namespace relations {

class Entity ;

typedef std::vector<std::string>                           MetaVec ;
typedef std::vector<std::string>::iterator                 MetaVec_iter ;

typedef std::map< std::string, MetaVec >                   MetaMap ;
typedef std::map< std::string, MetaVec >::iterator         MetaMap_iter ;
typedef std::map< std::string, MetaVec >::value_type       MetaMap_pair ;

typedef std::map<Entity*, std::string>                     EntityRelationMap ;
typedef std::map<Entity*, std::string>::iterator           EntityRelationMap_iter ;
typedef std::map<Entity*, std::string>::value_type         EntityRelationMap_pair ;

typedef std::vector<Entity*>                               EntityVec ;
typedef std::vector<Entity*>::iterator                     EntityVec_iter ;

typedef std::map< std::string, EntityVec >                 RelationMap ;
typedef std::map< std::string, EntityVec >::iterator       RelationMap_iter ;
typedef std::map< std::string, EntityVec >::value_type     RelationMap_pair ;

// class:  Entity
// desc:   root relatable construct 
//
class Entity
{
  private :
    static const std::string Unknown;

    uint32_t       _id ;
    MetaMap        _meta ;
    RelationMap    _links ;

    bool           vec_exists( EntityVec &v, Entity *e )
                   {  return (std::find(v.begin(), v.end(), e) != v.end()) ;
                   }
    bool           vec_exists( MetaVec &v, const std::string &e )
                   {  return (std::find(v.begin(), v.end(), e) != v.end()) ;
                   }

  public  :
                   Entity( uint32_t id_ ) 
                   {
                     _id = id_ ;
                   }

    bool           is_linked( const std::string &type_, Entity &other ) 
                   {
                     RelationMap_iter  it = _links.find( type_ ) ;
                     if (it == _links.end())
                       return false ;
                     return (vec_exists( (*it).second, &other )) ;
                   }
    void           link( const std::string &type_, Entity &other ) 
                   {
                     RelationMap_iter  it = _links.find( type_ ) ;
                     if (it == _links.end())
                       _links.insert(RelationMap_pair( type_, { &other } )) ;
                     else if (vec_exists( (*it).second, &other ) == false)
                       (*it).second.push_back( &other ) ;
                   }
    void           meta( const std::string &name, const std::string &value ) 
                   {
                     MetaMap_iter  it = _meta.find( name ) ;
                     if (it == _meta.end())
                       _meta.insert(MetaMap_pair( name, { value } )) ;
                     else if (vec_exists( (*it).second, value ) == false)
                       (*it).second.push_back( value ) ;
                   }
    void           meta( const std::vector< std::string > &pairs ) 
                   {
                     std::vector< std::string >::const_iterator  it ;
                     for (it = pairs.begin(); it != pairs.end(); it++)
                     {
                       std::string  key( (*it) ) ;
                       it++ ;
                       if (it == pairs.end())
                         break ;
                       std::string  value( (*it) ) ;
                       meta( key, value ) ;
                     }
                   }
    MetaVec       *get_meta( const std::string &type_ ) 
                   {
                     MetaMap_iter  it = _meta.find( type_ ) ;
                     return (it == _meta.end()) ? nullptr : &(*it).second ;
                   }
    EntityVec     *relation( const std::string &type_ ) 
                   {
                     RelationMap_iter  it = _links.find( type_ ) ;
                     return (it == _links.end()) ? nullptr : &(*it).second ;
                   }
    RelationMap   &relations() { return _links ; }

    const std::string &name() { 
                     MetaMap_iter  it = _meta.find( "firstname" ) ; 
                     if ((it == _meta.end()) || ((*it).second.size() == 0))
                       return Unknown ;
                     return ((*it).second)[0] ;
                   }

    void           find_relations( EntityRelationMap &data, const std::string &path )
                   {
                     EntityRelationMap_iter   dit ;
                     RelationMap_iter         it ;

                     // parent
                     if ((it = _links.find( "parent" )) != _links.end())
                     {
                       for (EntityVec_iter eit = (*it).second.begin(); eit != (*it).second.end(); eit++)
                       {
                         dit = data.find( (*eit) ) ;
                         if (dit == data.end())
                         {
                           // new entity
                           data.insert( EntityRelationMap_pair( (*eit), path + 'c' )) ;
                           (*eit)->find_relations( data, path + 'c' ) ;
                         }
                       }
                     }

                     // child
                     if ((it = _links.find( "child" )) != _links.end())
                     {
                       for (EntityVec_iter eit = (*it).second.begin(); eit != (*it).second.end(); eit++)
                       {
                         dit = data.find( (*eit) ) ;
                         if (dit == data.end())
                         {
                           // new entity
                           data.insert( EntityRelationMap_pair( (*eit), path + 'p' )) ;
                           (*eit)->find_relations( data, path + 'p' ) ;
                         }
                       }
                     }

                     // sibling
                     if ((it = _links.find( "sibling" )) != _links.end())
                     {
                       for (EntityVec_iter eit = (*it).second.begin(); eit != (*it).second.end(); eit++)
                       {
                         dit = data.find( (*eit) ) ;
                         if (dit == data.end())
                         {
                           // new entity
                           data.insert( EntityRelationMap_pair( (*eit), path + 's' )) ;
                           (*eit)->find_relations( data, path + 's' ) ;
                         }
                       }
                     }

                     // spouse
                     if ((it = _links.find( "spouse" )) != _links.end())
                     {
                       for (EntityVec_iter eit = (*it).second.begin(); eit != (*it).second.end(); eit++)
                       {
                         dit = data.find( (*eit) ) ;
                         if (dit == data.end())
                         {
                           // new entity
                           data.insert( EntityRelationMap_pair( (*eit), path + 'S' )) ;
                           // stop traversing at spouse
//                           (*eit)->find_relations( data, path + 's' ) ;
                         }
                       }
                     }
                   }
    void           find_relations( EntityRelationMap &data )
                   {
                     std::string  path( "" ) ;
                     find_relations( data, path ) ;
                   }

    uint32_t       id() const { return _id ; }
} ; // class Entity

std::string const Entity::Unknown = std::string("unknown");

typedef std::map< uint32_t, Entity* >               EntityMap ;
typedef std::map< uint32_t, Entity* >::iterator     EntityMap_iter ;
typedef std::map< uint32_t, Entity* >::value_type   EntityMap_pair ;

// class:  EntityMgr
// desc:   Entity manager class that contains and manages all the entities
//
class EntityMgr
{
  private :
    EntityMap           _entities ;

  public  :
                        EntityMgr() {}

    Entity             &get( uint32_t id ) 
                        {
                          EntityMap_iter  it = _entities.find( id ) ;
                          if (it == _entities.end())
                          {
                            Entity  *e = new Entity( id ) ;
                            _entities.insert(EntityMap_pair( id, e )) ;
                            return *e ;
                          }
                          
                          return *(*it).second ;
                        }

    uint32_t            size() const { return _entities.size(); }
    uint32_t            total_relations()
                        {
                          uint32_t total = 0;
                          for (EntityMap_iter it = _entities.begin(); it != _entities.end(); it++)
                            total += (*(*it).second).relations().size();
                          return total;
                        }
} ; // class EntityMgr

}} ; // namespace

#endif


