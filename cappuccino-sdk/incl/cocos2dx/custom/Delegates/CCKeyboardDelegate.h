#ifndef __CCKEYBOARD_DELEGATE_H
#define __CCKEYBOARD_DELEGATE_H

#include <ccMacros.h>
NS_CC_BEGIN

enum enumKeyCodes;
class CC_DLL CCKeyboardDelegate {
public:
	virtual void keyDown(enumKeyCodes);
	virtual void keyUp(enumKeyCodes);
};

NS_CC_END
#endif