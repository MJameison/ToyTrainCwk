#include "CatmullRom.h"

#include <iostream>

std::vector<CatmullRom::CatmullRomPoint> CatmullRom::EvaluateSpline(const std::vector<glm::vec3>& points, bool closedLoop) {
	std::vector<CatmullRomPoint> finalPoints;
	unsigned int numPoints = points.size();
	numPoints = closedLoop ? numPoints : numPoints - 2; // Stop early with open loops first and last point are used for tangents only

	for (unsigned int i = 0; i < numPoints; ++i) {
		glm::vec3 p0 = points[(i)     % numPoints];
		glm::vec3 p1 = points[(i + 1) % numPoints];
		glm::vec3 p2 = points[(i + 2) % numPoints];
		glm::vec3 p3 = points[(i + 3) % numPoints];

		std::vector<CatmullRomPoint> newPoints = EvaluateArc(p0, p1, p2, p3, 40, closedLoop && (i == numPoints - 1));
		for (auto &p : newPoints) {
			finalPoints.push_back(p);
		}
	}

	return finalPoints;
}

std::vector<CatmullRom::CatmullRomPoint> CatmullRom::EvaluateArc(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, unsigned int resolution, bool finalClosedLoop) {
	// Calculate the tangents at p1 and p2
	glm::vec3 m0 = p2 - p0;
	glm::vec3 m1 = p3 - p1;
	m0 *= 0.5f;
	m1 *= 0.5f;

	std::vector<CatmullRomPoint> points;
	
	float pointStep = 1.0f / resolution;
	for (unsigned int i = 0; i < resolution; ++i) {
		float t = pointStep * i;
		CatmullRomPoint p = EvaluatePoint(p1, p2, m0, m1, t);

		points.push_back(p);
	}

	if (finalClosedLoop)
		points.push_back(EvaluatePoint(p1, p2, m0, m1, 1.0f));

	return points;
}

CatmullRom::CatmullRomPoint CatmullRom::EvaluatePoint(glm::vec3 start, glm::vec3 end, glm::vec3 startTan, glm::vec3 endTan, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	CatmullRomPoint point;
	// Curve formula:
	// (2t^3 - 3t^2 + 1) * p0 + (t^3 - 2t^2 + t) * m0 + (-2t^3 + 3t^2) * p1 + (t^3 - t^2) * m1
	point.position = (2*t3 - 3*t2 + 1) * start
		+ (t3 - 2*t2 + t) * startTan
		+ (-2*t3 + 3*t2) * end
		+ (t3 - t2) * endTan;
	// p'(t) = (6t^2 - 6t) * p0 + (3t^2 -4t + 1) * m0 + (-6t^2 + 6t) * p1 + (3t^2 -2t) * m1
	point.tangent = glm::normalize(
		(6*t2 - 6*t) * start
		+ (3*t2 - 4*t + 1) * startTan
		+ (-6*t2 + 6*t) * end
		+ (3*t2 - 2*t) * endTan
	);
	// Normal is up by default so we use cross product to calculate binormal
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 binormal = glm::normalize(glm::cross(up, point.tangent));
	// Normal is cross of tangent and bitangent
	point.normal = glm::normalize(glm::cross(point.tangent, binormal));

	return point;
}
