
#ifndef __SWATCHCELL_EXAMPLE_RUNCONTROL_H__
#define __SWATCHCELL_EXAMPLE_RUNCONTROL_H__


#include "swatchcell/framework/RunControl.h"


namespace swatchcellexample {
  
class RunControl : public swatchcellframework::RunControl {
public:
  RunControl(log4cplus::Logger& aLogger, tsframework::CellAbstractContext* aContext);

  ~RunControl();

private:
  void execPostStart();

  void execPreStop();
};

} // end ns: swatchcellexample


#endif /* __SWATCHCELL_EXAMPLE_RUNCONTROL_H__ */

