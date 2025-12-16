/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <variant>

namespace Mx3d {

	struct MeasureNone {};

	struct MeasureDistance {
		enum class Type {
			None, Mininmum, CenterToCenter
		};

		gp_Pnt pnt1;
		gp_Pnt pnt2;
		double value;
		Type type = Type::None;
	};

	struct MeasureCircle {
		gp_Pnt pntAnchor;
		bool isArc = false;
		gp_Circ value;
	};

	struct MeasureAngle {
		gp_Pnt pnt1;
		gp_Pnt pnt2;
		gp_Pnt pntCenter;
		double value;
	};

	struct MeasureLength {
		gp_Pnt middlePnt;
		double value;
	};

	struct MeasureArea {
		gp_Pnt middlePnt;
		double value;
	};

	struct MeasureBoundingBox {
		gp_Pnt cornerMin;
		gp_Pnt cornerMax;
		double xLength;
		double yLength;
		double zLength;
		double volume;
	};


	using MeasureValue = std::variant<
		MeasureNone,
		gp_Pnt,
		MeasureCircle,
		MeasureDistance,
		MeasureAngle,
		MeasureLength,
		MeasureArea,
		MeasureBoundingBox
	>;
}