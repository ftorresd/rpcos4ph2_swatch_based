/**
 * @file    MP7MiniPODPorts.hpp
 * @author  Alessandro Thea
 * @brief   MP7Processor Rx and Tx ports implementations
 * @date    February 2015
 */

#ifndef __SWATCH_MP7_MP7MINIPODPORTS_HPP__
#define __SWATCH_MP7_MP7MINIPODPORTS_HPP__


// SWATCH headers
#include "swatch/mp7/MP7Ports.hpp"

namespace  mp7 {
class MiniPODSlave;
}

namespace swatch {

namespace mp7 {

struct FibreInfo {
  std::string geo;
  std::string minipod;
  uint32_t id;
};

class MP7RxMiniPODPort : public MP7RxPort {

public:
  MP7RxMiniPODPort( const std::string& aId, uint32_t aChannelID, ::mp7::MP7MiniController& aController );
  virtual ~MP7RxMiniPODPort() {}

protected:
  virtual void retrieveMetricValues();

  const ::mp7::MiniPODSlave& mRxMiniPOD;
private:

  core::SimpleMetric<double>& mOpticalPower;

  static const std::map<uint32_t, FibreInfo> kRxMiniPODMapping;

  const ::mp7::MiniPODSlave& findRxMiniPOD( );
};


class MP7TxMiniPODPort : public MP7TxPort {

public:
  MP7TxMiniPODPort( const std::string& aId, uint32_t aChannelID, ::mp7::MP7MiniController& aController );
  virtual ~MP7TxMiniPODPort() {}

protected:
  virtual void retrieveMetricValues();

private:

};

}
}


#endif /* __SWATCH_MP7_MP7MINIPODPORTS_HPP__ */
