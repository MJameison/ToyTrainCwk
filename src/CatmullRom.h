#pragma once

#include <glm/glm.hpp>
#include <vector>

class CatmullRom {
public:
	struct CatmullRomPoint {
		glm::vec3 position;
		glm::vec3 tangent;
		glm::vec3 normal;
	};

	static std::vector<CatmullRomPoint> EvaluateSpline(const std::vector<glm::vec3>& points, bool closedLoop);
	static std::vector<CatmullRomPoint> EvaluateArc(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, unsigned int resolution, bool finalClosedLoop=false);
	static CatmullRomPoint EvaluatePoint(glm::vec3 start, glm::vec3 end, glm::vec3 startTan, glm::vec3 endTan, float t);
};

