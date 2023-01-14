/* Copyright Jukka Jylänki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file Cylinder.h
	@author Jukka Jylänki
	@brief The Cylinder geometry object. */
#pragma once

#include "../MathGeoLibFwd.h"
#include "LineSegment.h"

MATH_BEGIN_NAMESPACE

/// A 3D cylinder 
class Cylinder
{
public:
	LineSegment l;
	float r;
	Cylinder() {}
	Cylinder(const LineSegment &endPoints, float radius);

	Cylinder(const float3 &bottomPoint, const float3 &topPoint, float radius);
	float LineLength() const;

	float Height() const;
	float3 Center() const;
	float3 Centroid() const { return l.CenterPoint(); } ///< [similarOverload: Center]

};
MATH_END_NAMESPACE
