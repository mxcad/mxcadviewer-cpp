/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <gsl/span>
#include <cassert>
#include <climits>

namespace Mx3d {

	template<typename T> using Span = gsl::span<T>;

	// Returns the index of 'item' contained in 'span'
	template<typename T>
	constexpr int Span_itemIndex(Span<T> span, typename Span<T>::const_reference item)
	{
		assert(!span.empty());
		auto index = &item - &span.front();
		assert(index >= 0);
		assert(index <= INT_MAX);
		assert(&span[static_cast<typename Span<T>::size_type>(index)] == &item);
		return static_cast<int>(index);
	}

	template<typename Container>
	constexpr int Span_itemIndex(const Container& cont, typename Container::const_reference item)
	{
		return Span_itemIndex(Span<const typename Container::value_type>(cont), item);
	}

} // namespace Mx3d
