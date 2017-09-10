// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <algorithm>

// template <RandomAccessIterator Iter>
//   requires ShuffleIterator<Iter>
//         && LessThanComparable<Iter::value_type>
//   void
//   push_heap(Iter first, Iter last);

#include <stl2/detail/algorithm/push_heap.hpp>
#include <memory>
#include <random>
#include <algorithm>
#include <functional>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace stl2 = __stl2;

namespace { std::mt19937 gen; }

const auto push_heap = make_testable_1([](auto&&... args) {
	return stl2::push_heap(stl2::forward<decltype(args)>(args)...);
});

void test(int N)
{

	int* ia = new int [N];
	for (int i = 0; i < N; ++i)
		ia[i] = i;
	std::shuffle(ia, ia+N, gen);
	for (int i = 0; i <= N; ++i)
	{
		::push_heap(ia, ia+i).check([&](int *r){CHECK(r == ia + i);});
		CHECK(std::is_heap(ia, ia+i));
	}
	delete [] ia;
}

void test_comp(int N)
{
	int* ia = new int [N];
	for (int i = 0; i < N; ++i)
		ia[i] = i;
	std::shuffle(ia, ia+N, gen);
	for (int i = 0; i <= N; ++i)
	{
		::push_heap(ia, ia+i, std::greater<int>()).check([&](int *r){CHECK(r == ia+i);});
		CHECK(std::is_heap(ia, ia+i, std::greater<int>()));
	}
	delete [] ia;
}

struct S
{
	int i;
};

void test_proj(int N)
{
	S* ia = new S [N];
	int* ib = new int [N];
	for (int i = 0; i < N; ++i)
		ia[i].i = i;
	std::shuffle(ia, ia+N, gen);
	for (int i = 0; i <= N; ++i)
	{
		::push_heap(ia, ia+i, std::greater<int>(), &S::i).check([&](S *r){CHECK(r == ia+i);});
		std::transform(ia, ia+i, ib, std::mem_fn(&S::i));
		CHECK(std::is_heap(ib, ib+i, std::greater<int>()));
	}
	delete [] ia;
	delete [] ib;
}

struct indirect_less
{
	template <class P>
	bool operator()(const P& x, const P& y) const
		{return *x < *y;}
};

void test_move_only(int N)
{
	std::unique_ptr<int>* ia = new std::unique_ptr<int> [N];
	for (int i = 0; i < N; ++i)
		ia[i].reset(new int(i));
	std::shuffle(ia, ia+N, gen);
	for (int i = 0; i <= N; ++i)
	{
		::push_heap(ia, ia+i, indirect_less()).check([&](std::unique_ptr<int> *r){CHECK(r == ia+i);});
		CHECK(std::is_heap(ia, ia+i, indirect_less()));
	}
	delete [] ia;
}

int main()
{
	test(1000);
	test_comp(1000);
	test_proj(1000);
	test_move_only(1000);

	{
		int const N = 1000;
		S* ia = new S [N];
		int* ib = new int [N];
		for (int i = 0; i < N; ++i)
			ia[i].i = i;
		std::shuffle(ia, ia+N, gen);
		for (int i = 0; i <= N; ++i)
		{
			CHECK(stl2::push_heap(stl2::ext::make_range(ia, ia+i), std::greater<int>(), &S::i).get_unsafe() == ia+i);
			std::transform(ia, ia+i, ib, std::mem_fn(&S::i));
			CHECK(std::is_heap(ib, ib+i, std::greater<int>()));
		}
		delete [] ia;
		delete [] ib;
	}

	return test_result();
}
