#ifndef __ABSTRACT_TASK_H__
#define __ABSTRACT_TASK_H__
#include "model/AbstractJsonObject.h"
class AbstractTask : public AbstractJsonObject {
   public:
    virtual std::string getShortName() = 0;
};
#endif
