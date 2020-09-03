/*
 * File:   ChannelDescriptorCollection.hpp
 * Author: ale
 *
 * Created on April 8, 2016, 7:27 PM
 */

#ifndef __SWATCH_MP7_CHANNELDESCRIPTORCOLLECTION_HPP__
#define __SWATCH_MP7_CHANNELDESCRIPTORCOLLECTION_HPP__


#include <stdint.h>                     // for uint32_t
#include <string>                       // for string, allocator, etc
#include <vector>                       // for vector

// Boost Headers
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

// SWATCH Headers
#include "swatch/core/exception.hpp"
#include "swatch/mp7/ChannelDescriptor.hpp"


// forward declarations
namespace boost {
template <typename Signature> class function;
}

namespace swatch {
namespace mp7 {

// Typedefs
typedef boost::function<bool (const ChannelDescriptor&) > ChannelDescriptorRule_t;
typedef ChannelDescriptorRule_t ChannelRule_t;

class ChannelDescriptorCollection {
public:
  ChannelDescriptorCollection();
  virtual ~ChannelDescriptorCollection();

  /**
   *
   * @param aDescriptor
   */
  void insert(const ChannelDescriptor& aDescriptor );

  /**
   *
   * @param aId
   * @return
   */
  const ChannelDescriptor& getById(const std::string& aId ) const;

  /**
   *
   * @param aChannelId
   * @return
   */
  const ChannelDescriptor& getByChannelId( uint32_t aChannelId ) const;

  /**
   * Id getter
   * FIXME: set or vector?
   * @return set of object ids
   */
  std::vector<std::string> ids() const;

  /**
   * Channel ids getter
   * FIXME: set or vector?
   * @return set of channel ids
   */
  std::vector<uint32_t> channels() const;

  /**
   */
  std::vector<std::string> findIds( const ChannelDescriptorRule_t& aRule ) const;

  /**
   */
  std::vector<std::string> filterIds( const std::vector<std::string>& aIds, const ChannelDescriptorRule_t& aRule ) const;

  /**
   *
   * @param aIds
   * @return
   */
  std::vector<uint32_t> idsToChans( const std::vector<std::string>& aIds) const;

  /**
   *
   * @param aChannelIds
   * @return
   */
  std::vector<std::string> chansToIds( const std::vector<uint32_t>& aChannelIds) const;

  /**
   */
  void ensureIdsExist(const std::vector<std::string>& aIds) const;

  /**
   */
  void ensureChannelsExist(const std::vector<uint32_t>& aChannelIds) const;

  void ensureIdsExistAndMatchRule(const std::vector<std::string>& aIds, const ChannelDescriptorRule_t& aRule) const;

private:

  struct byId {};
  struct byChannel {};

  typedef boost::multi_index::multi_index_container<
    ChannelDescriptor,
    boost::multi_index::indexed_by<
      // Primary Index: Obj Id
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<byId>,
        boost::multi_index::member<ChannelDescriptor,std::string,&ChannelDescriptor::mId>
      >
    ,
    // Secondary index, Channel Id
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<byChannel>,
        boost::multi_index::member<ChannelDescriptor,uint32_t,&ChannelDescriptor::mChannelId>
      >
    >

  > Container_t;

  //!
  Container_t mContainer;
};

SWATCH_DEFINE_EXCEPTION(ChannelDescriptorInsertionError)
SWATCH_DEFINE_EXCEPTION(ChannelDescriptorNotFound)
SWATCH_DEFINE_EXCEPTION(DescriptorIdsNotFound)
SWATCH_DEFINE_EXCEPTION(ChannelDescriptorFailsPrerequisites)

} // namespace mp7
} // namespace swatch


#endif  /* SWATCH_MP7_CHANNELDESCRIPTORMAP_HPP */

