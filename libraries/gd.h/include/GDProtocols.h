#ifndef __GDPROTOCOLS_H__
#define __GDPROTOCOLS_H__

#include <gd.h>

namespace gd {

	class FLAlertLayer;

	class FLAlertLayerProtocol {
	public:
		virtual void FLAlert_Clicked(gd::FLAlertLayer*, bool btn2) {};
	};
}

#endif