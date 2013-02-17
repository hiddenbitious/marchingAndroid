#ifndef _METABALL_H_
#define _METABALL_H_


#include "../globals.h"

class C_Metaball {
	public:
		C_Vertex position;
		float radius;

		void Constructor(void) {
			position.x = 0.0f; position.y = 0.0f; position.z = 0.0f;
			radius = 0.0f;
		}
};

#endif
