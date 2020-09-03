#include "swatch/mp7/MP7MiniPODPorts.hpp"

#include "mp7/MP7MiniController.hpp"
#include "mp7/MiniPODMasterNode.hpp"

namespace swatch {
namespace mp7 {

const std::map<uint32_t, FibreInfo> MP7RxMiniPODPort::kRxMiniPODMapping = {
  {0,
    {"bot", "rx3", 0}},
  {1,
    {"bot", "rx3", 1}},
  {2,
    {"bot", "rx3", 2}},
  {3,
    {"bot", "rx3", 3}},
  {4,
    {"bot", "rx3", 4}},
  {5,
    {"bot", "rx3", 5}},
  {6,
    {"bot", "rx3", 6}},
  {7,
    {"bot", "rx3", 7}},
  {8,
    {"bot", "rx3", 8}},
  {9,
    {"bot", "rx3", 9}},
  {10,
    {"bot", "rx3", 10}},
  {11,
    {"bot", "rx3", 11}},
  {12,
    {"bot", "rx4", 0}},
  {13,
    {"bot", "rx4", 1}},
  {14,
    {"bot", "rx4", 2}},
  {15,
    {"bot", "rx4", 3}},
  {16,
    {"bot", "rx4", 4}},
  {17,
    {"bot", "rx4", 5}},
  {18,
    {"bot", "rx4", 6}},
  {19,
    {"bot", "rx4", 7}},
  {20,
    {"bot", "rx4", 8}},
  {21,
    {"bot", "rx4", 9}},
  {22,
    {"bot", "rx4", 10}},
  {23,
    {"bot", "rx4", 11}},
  {24,
    {"bot", "rx5", 0}},
  {25,
    {"bot", "rx5", 3}},
  {26,
    {"bot", "rx5", 1}},
  {27,
    {"bot", "rx5", 2}},
  {28,
    {"bot", "rx5", 5}},
  {29,
    {"bot", "rx5", 4}},
  {30,
    {"bot", "rx5", 6}},
  {31,
    {"bot", "rx5", 7}},
  {32,
    {"bot", "rx5", 8}},
  {33,
    {"bot", "rx5", 9}},
  {34,
    {"bot", "rx5", 10}},
  {35,
    {"bot", "rx5", 11}},
  {36,
    {"top", "rx2", 1}},
  {37,
    {"top", "rx2", 0}},
  {38,
    {"top", "rx2", 3}},
  {39,
    {"top", "rx2", 2}},
  {40,
    {"top", "rx2", 5}},
  {41,
    {"top", "rx2", 4}},
  {42,
    {"top", "rx2", 6}},
  {43,
    {"top", "rx2", 7}},
  {44,
    {"top", "rx2", 8}},
  {45,
    {"top", "rx2", 10}},
  {46,
    {"top", "rx2", 9}},
  {47,
    {"top", "rx2", 11}},
  {48,
    {"top", "rx1", 1}},
  {49,
    {"top", "rx1", 0}},
  {50,
    {"top", "rx1", 3}},
  {51,
    {"top", "rx1", 2}},
  {52,
    {"top", "rx1", 5}},
  {53,
    {"top", "rx1", 4}},
  {54,
    {"top", "rx1", 7}},
  {55,
    {"top", "rx1", 6}},
  {56,
    {"top", "rx1", 9}},
  {57,
    {"top", "rx1", 8}},
  {58,
    {"top", "rx1", 10}},
  {59,
    {"top", "rx1", 11}},
  {60,
    {"top", "rx0", 1}},
  {61,
    {"top", "rx0", 0}},
  {62,
    {"top", "rx0", 3}},
  {63,
    {"top", "rx0", 2}},
  {64,
    {"top", "rx0", 5}},
  {65,
    {"top", "rx0", 4}},
  {66,
    {"top", "rx0", 7}},
  {67,
    {"top", "rx0", 6}},
  {68,
    {"top", "rx0", 9}},
  {69,
    {"top", "rx0", 8}},
  {70,
    {"top", "rx0", 10}},
  {71,
    {"top", "rx0", 11}}
};

//        lcToLogicalMap = dict(zip(range(0,72),[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
//                          25,28,26,27,30,29,31,32,33,34,35,36,62,61,64,63,66,65,67,68,69,
//                          71,70,72,50,49,52,51,54,53,56,55,58,57,59,60,38,37,40,39,42,41,
//                          44,43,46,45,47,48]))


MP7RxMiniPODPort::MP7RxMiniPODPort(const std::string& aId, uint32_t aChannelID, ::mp7::MP7MiniController& aController) :
MP7RxPort(aId, aChannelID, aController),
mRxMiniPOD(findRxMiniPOD()),
mOpticalPower(registerMetric<double>("opticalPower"))
{
//  aController.hw().getNode(kRxMiniPODMapping[aChannelID].geo);
}

// ----------------------------------------------------------------------------
const ::mp7::MiniPODSlave& 
MP7RxMiniPODPort::findRxMiniPOD()
{
  const FibreInfo& lInfo = kRxMiniPODMapping.at(mChannelID);
  
  return mDriver.hw().getNode< ::mp7::MiniPODMasterNode >("i2c.minipods_"+lInfo.geo).getRxPOD(lInfo.minipod);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void MP7RxMiniPODPort::retrieveMetricValues()
{
  MP7RxPort::retrieveMetricValues();
  
  setMetricValue<>(mOpticalPower, mRxMiniPOD.getOpticalPower(kRxMiniPODMapping.at(mChannelID).id).value);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
MP7TxMiniPODPort::MP7TxMiniPODPort(const std::string& aId, uint32_t aChannelID, ::mp7::MP7MiniController& aController) :
MP7TxPort(aId, aChannelID, aController)
{

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void MP7TxMiniPODPort::retrieveMetricValues()
{
  MP7TxPort::retrieveMetricValues();
}
// ----------------------------------------------------------------------------


}
}