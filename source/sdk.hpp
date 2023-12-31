#pragma once

namespace sdk {
	class cc_scheduler {
	public:
		void set_time_scale(float val);
	};

	class cc_director {
	public:
		static cc_director* shared_director();
		cc_scheduler* get_scheduler();
	};

	class channel_control {
	public:
		static channel_control* get_gd_channel();
		void set_pitch(float pitch);
	};
}