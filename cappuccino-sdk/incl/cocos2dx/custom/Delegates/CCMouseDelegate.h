#ifndef __CCMOUSE_DELEGATE_H
#define __CCMOUSE_DELEGATE_H

#include <ccMacros.h>
NS_CC_BEGIN

class CC_DLL CCMouseDelegate {
public:
	virtual void rightKeyDown(void);
	virtual void rightKeyUp(void);
	virtual void  scrollWheel(float, float);
};

NS_CC_END
#endif