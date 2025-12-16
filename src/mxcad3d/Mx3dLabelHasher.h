/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <TDF_LabelMapHasher.hxx>
namespace std {

	template<> struct hash<TDF_Label> {
		inline size_t operator()(const TDF_Label& lbl) const {
			return TDF_LabelMapHasher::HashCode(lbl, INT_MAX);
		}
	};
}