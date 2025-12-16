/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <gp_Pnt.hxx>
#include <array>

namespace Mx3d {
    struct BndBoxCoords {
	double xmin;
	double ymin;
	double zmin;
	double xmax;
	double ymax;
	double zmax;


	gp_Pnt center() const {
		return {
		  (this->xmax + this->xmin) / 2.,
		  (this->ymax + this->ymin) / 2.,
		  (this->zmax + this->zmin) / 2.
		};
	}
	std::array<gp_Pnt, 8> vertices() const {
		return {
			{
				{ this->xmin, this->ymin, this->zmax },
				{ this->xmax, this->ymin, this->zmax },
				{ this->xmin, this->ymin, this->zmin },
				{ this->xmax, this->ymin, this->zmin },
				{ this->xmin, this->ymax, this->zmax },
				{ this->xmax, this->ymax, this->zmax },
				{ this->xmin, this->ymax, this->zmin },
				{ this->xmax, this->ymax, this->zmin }
			}
		};
	}
	gp_Pnt minVertex() const {
		return { this->xmin, this->ymin, this->zmin };
	}
	gp_Pnt maxVertex() const {
		return { this->xmax, this->ymax, this->zmax };
	}
	static BndBoxCoords get(const Bnd_Box& box) {
		BndBoxCoords bbc = {};
		if (!box.IsVoid())
			box.Get(bbc.xmin, bbc.ymin, bbc.zmin, bbc.xmax, bbc.ymax, bbc.zmax);
		return bbc;
	}
};
}