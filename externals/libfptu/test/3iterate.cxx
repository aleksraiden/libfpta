/*
 *  Fast Positive Tuples (libfptu), aka Позитивные Кортежи
 *  Copyright 2016-2022 Leonid Yuriev <leo@yuriev.ru>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "fptu_test.h"

#include <stdlib.h>

static bool field_filter_any(const fptu_field *, void *context, void *param) {
  (void)context;
  (void)param;
  return true;
}

static bool field_filter_none(const fptu_field *, void *context, void *param) {
  (void)context;
  (void)param;
  return false;
}

TEST(Iterate, Empty) {
  char space_exactly_noitems[sizeof(fptu_rw)];
  fptu_rw *pt =
      fptu_init(space_exactly_noitems, sizeof(space_exactly_noitems), 0);
  ASSERT_NE(nullptr, pt);
  ASSERT_STREQ(nullptr, fptu::check(pt));
  EXPECT_EQ(0u, fptu_space4items(pt));
  EXPECT_EQ(0u, fptu_space4data(pt));
  EXPECT_EQ(0u, fptu_junkspace(pt));
  ASSERT_EQ(fptu_end_rw(pt), fptu_begin_rw(pt));

  const fptu_field *end = fptu_end_rw(pt);
  EXPECT_EQ(end, fptu::first(end, end, 0, fptu_any));
  EXPECT_EQ(end, fptu::next(end, end, 0, fptu_any));
  EXPECT_EQ(end, fptu::first(end, end, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(end, fptu::next(end, end, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(end, fptu::first(end, end, field_filter_none, nullptr, nullptr));
  EXPECT_EQ(end, fptu::next(end, end, field_filter_none, nullptr, nullptr));

  EXPECT_EQ(0u, fptu::field_count(pt, 0, fptu_any));
  EXPECT_EQ(0u, fptu::field_count(pt, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(0u, fptu::field_count(pt, field_filter_none, nullptr, nullptr));

  fptu_ro ro = fptu_take_noshrink(pt);
  ASSERT_STREQ(nullptr, fptu::check(ro));
  ASSERT_EQ(fptu_end_ro(ro), fptu_begin_ro(ro));
  EXPECT_EQ(fptu_end_rw(pt), fptu_end_ro(ro));
  EXPECT_EQ(fptu_begin_rw(pt), fptu_begin_ro(ro));
  EXPECT_EQ(0u, fptu::field_count(ro, 0, fptu_any));
  EXPECT_EQ(0u, fptu_field_count_ro_ex(ro, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(0u,
            fptu_field_count_ro_ex(ro, field_filter_none, nullptr, nullptr));
}

TEST(Iterate, Simple) {
  char space[fptu_buffer_enough];
  fptu_rw *pt = fptu_init(space, sizeof(space), fptu_max_fields);
  ASSERT_NE(nullptr, pt);
  ASSERT_STREQ(nullptr, fptu::check(pt));

  EXPECT_EQ(FPTU_OK, fptu_upsert_null(pt, 0));
  ASSERT_STREQ(nullptr, fptu::check(pt));
  EXPECT_EQ(1, fptu_end_rw(pt) - fptu_begin_rw(pt));

  const fptu_field *end = fptu_end_rw(pt);
  const fptu_field *begin = fptu_begin_rw(pt);
  fptu_ro ro = fptu_take_noshrink(pt);
  ASSERT_STREQ(nullptr, fptu::check(ro));

  EXPECT_EQ(begin,
            fptu_first_ex(begin, end, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(end, fptu_next_ex(begin, end, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(begin, fptu::first(begin, end, 0, fptu_any));
  EXPECT_EQ(end, fptu::next(begin, end, 0, fptu_any));

  EXPECT_EQ(end,
            fptu_first_ex(begin, end, field_filter_none, nullptr, nullptr));
  EXPECT_EQ(end, fptu_next_ex(begin, end, field_filter_none, nullptr, nullptr));
  EXPECT_EQ(end, fptu::first(begin, end, 1, fptu_any));
  EXPECT_EQ(end, fptu::next(begin, end, 1, fptu_any));

  EXPECT_EQ(fptu_end_rw(pt), fptu_end_ro(ro));
  EXPECT_EQ(fptu_begin_rw(pt), fptu_begin_ro(ro));

  EXPECT_EQ(1u, fptu::field_count(pt, 0, fptu_any));
  EXPECT_EQ(1u, fptu::field_count(pt, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(1u, fptu::field_count(ro, 0, fptu_any));
  EXPECT_EQ(1u, fptu::field_count(ro, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(0u, fptu::field_count(pt, field_filter_none, nullptr, nullptr));
  EXPECT_EQ(0u, fptu::field_count(ro, field_filter_none, nullptr, nullptr));

  EXPECT_EQ(FPTU_OK, fptu_upsert_null(pt, 1));
  ASSERT_STREQ(nullptr, fptu::check(pt));
  end = fptu_end_rw(pt);
  begin = fptu_begin_rw(pt);
  ro = fptu_take_noshrink(pt);
  ASSERT_STREQ(nullptr, fptu::check(ro));

  EXPECT_EQ(begin, fptu_begin_ro(ro));
  EXPECT_EQ(end, fptu_end_ro(ro));
  EXPECT_EQ(begin, fptu::first(begin, end, 1, fptu_any));
  EXPECT_EQ(end, fptu::next(begin, end, 1, fptu_any));

  EXPECT_EQ(1u, fptu::field_count(pt, 0, fptu_any));
  EXPECT_EQ(1u, fptu::field_count(pt, 1, fptu_any));
  EXPECT_EQ(2u, fptu::field_count(pt, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(1u, fptu::field_count(ro, 0, fptu_any));
  EXPECT_EQ(1u, fptu::field_count(ro, 1, fptu_any));
  EXPECT_EQ(2u, fptu::field_count(ro, field_filter_any, nullptr, nullptr));
  EXPECT_EQ(0u, fptu::field_count(pt, field_filter_none, nullptr, nullptr));
  EXPECT_EQ(0u, fptu::field_count(ro, field_filter_none, nullptr, nullptr));

  for (unsigned n = 1; n < 11; n++) {
    SCOPED_TRACE("n = " + std::to_string(n));
    EXPECT_EQ(FPTU_OK, fptu_insert_uint32(pt, 2, 42));
    ASSERT_STREQ(nullptr, fptu::check(pt));
    end = fptu_end_rw(pt);
    begin = fptu_begin_rw(pt);
    ro = fptu_take_noshrink(pt);
    ASSERT_STREQ(nullptr, fptu::check(ro));

    EXPECT_EQ(begin, fptu_begin_ro(ro));
    EXPECT_EQ(end, fptu_end_ro(ro));

    EXPECT_EQ(1u, fptu::field_count(pt, 0, fptu_any));
    EXPECT_EQ(1u, fptu::field_count(pt, 1, fptu_any));
    EXPECT_EQ(n, fptu::field_count(pt, 2, fptu_any));
    EXPECT_EQ(n, fptu::field_count(pt, 2, fptu_uint32));
    EXPECT_EQ(0u, fptu::field_count(pt, 2, fptu_null));
    EXPECT_EQ(2u + n,
              fptu::field_count(pt, field_filter_any, nullptr, nullptr));
    EXPECT_EQ(0u, fptu::field_count(pt, field_filter_none, nullptr, nullptr));

    EXPECT_EQ(1u, fptu::field_count(ro, 0, fptu_any));
    EXPECT_EQ(1u, fptu::field_count(ro, 0, fptu_null));
    EXPECT_EQ(1u, fptu::field_count(ro, 1, fptu_any));
    EXPECT_EQ(n, fptu::field_count(ro, 2, fptu_any));
    EXPECT_EQ(0u, fptu::field_count(ro, 3, fptu_uint32));
    EXPECT_EQ(2u + n,
              fptu::field_count(ro, field_filter_any, nullptr, nullptr));
    EXPECT_EQ(0u, fptu::field_count(ro, field_filter_none, nullptr, nullptr));

    EXPECT_EQ((ptrdiff_t)(2u + n), fptu_end_rw(pt) - fptu_begin_rw(pt));
  }
}

TEST(Iterate, Filter) {
  char space[fptu_buffer_enough];
  fptu_rw *pt = fptu_init(space, sizeof(space), fptu_max_fields);
  ASSERT_NE(nullptr, pt);
  ASSERT_STREQ(nullptr, fptu::check(pt));

  EXPECT_EQ(FPTU_OK, fptu_upsert_null(pt, 9));
  EXPECT_EQ(FPTU_OK, fptu_upsert_uint16(pt, 9, 2));
  EXPECT_EQ(FPTU_OK, fptu_upsert_uint32(pt, 9, 3));
  EXPECT_EQ(FPTU_OK, fptu_upsert_int32(pt, 9, 4));
  EXPECT_EQ(FPTU_OK, fptu_upsert_int64(pt, 9, 5));
  EXPECT_EQ(FPTU_OK, fptu_upsert_uint64(pt, 9, 6));
  EXPECT_EQ(FPTU_OK, fptu_upsert_fp32(pt, 9, 7));
  EXPECT_EQ(FPTU_OK, fptu_upsert_fp64(pt, 9, 8));
  EXPECT_EQ(FPTU_OK, fptu_upsert_cstr(pt, 9, "cstr"));

  ASSERT_STREQ(nullptr, fptu::check(pt));
  // TODO: check array-only filter

  for (unsigned n = 0; n < 11; n++) {
    SCOPED_TRACE("n = " + std::to_string(n));

    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_null));
    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_uint16));
    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_uint32));
    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_uint64));
    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_int32));
    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_int64));
    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_fp32));
    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_fp64));
    EXPECT_EQ((n == 9u) ? 1u : 0u, fptu::field_count(pt, n, fptu_cstr));

    EXPECT_EQ((n == 9u) ? 9u : 0u, fptu::field_count(pt, n, fptu_any));
    EXPECT_EQ((n == 9u) ? 2u : 0u, fptu::field_count(pt, n, fptu_any_int));
    EXPECT_EQ((n == 9u) ? 3u : 0u, fptu::field_count(pt, n, fptu_any_uint));
    EXPECT_EQ((n == 9u) ? 2u : 0u, fptu::field_count(pt, n, fptu_any_fp));

    EXPECT_EQ(0u, fptu::field_count(pt, n, fptu_opaque));
    EXPECT_EQ(0u, fptu::field_count(pt, n, fptu_nested));
    EXPECT_EQ(0u, fptu::field_count(pt, n, fptu_filter_mask(fptu_farray)));
    EXPECT_EQ(0u, fptu::field_count(pt, n, fptu_ffilter));
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
