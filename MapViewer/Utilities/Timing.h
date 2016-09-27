#pragma once

#pragma once

namespace Utilities {

	///Calculates FPS and also limits FPS
	class Timing {
	public:
		Timing();

		// Initializes the FPS limiter. For now, this is
		// analogous to setMaxFPS
		void init(float maxFPS);

		// Sets the desired max FPS
		void setMaxFPS(float maxFPS);

		void begin();

		// Return second this frame
		static float getDelta(){ return _frameTime * 0.001f; }

		// end() will return the current FPS as a float
		float end();
	private:
		// Calculates the current FPS
		void calculateFPS();

		// Variables
		float _fps;
		float _maxFPS;
		
		unsigned int _startTicks;

		static float _frameTime;
	};

}