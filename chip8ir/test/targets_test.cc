/*
 * targets_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <gtest/gtest.h>

#include "targets.h"

class TargetsTest : public ::testing::Test {
protected:
    Targets targets;
};

TEST_F(TargetsTest, add_empty_optional_does_nothing)
{
    targets.add(std::nullopt);
    targets.sort_vectors();

    EXPECT_EQ(targets.get_label(0x200), "");
}

TEST_F(TargetsTest, add_single_target)
{
    targets.add(DecodedTarget{0x234, JUMP});
    targets.sort_vectors();

    EXPECT_EQ(targets.get_label(0x234), "LABEL0");
}

TEST_F(TargetsTest, multiple_targets_same_kind)
{
    targets.add(DecodedTarget{0x300, SUBROUTINE});
    targets.add(DecodedTarget{0x200, SUBROUTINE});
    targets.add(DecodedTarget{0x280, SUBROUTINE});

    targets.sort_vectors();

    EXPECT_EQ(targets.get_label(0x200), "FUNC0");
    EXPECT_EQ(targets.get_label(0x280), "FUNC1");
    EXPECT_EQ(targets.get_label(0x300), "FUNC2");
}

TEST_F(TargetsTest, duplicate_targets_are_removed)
{
    targets.add(DecodedTarget{0x234, JUMP});
    targets.add(DecodedTarget{0x234, JUMP});
    targets.add(DecodedTarget{0x234, JUMP});

    targets.sort_vectors();

    EXPECT_EQ(targets.get_label(0x234), "LABEL0");
}

TEST_F(TargetsTest, same_address_different_kinds)
{
    targets.add(DecodedTarget{0x400, JUMP});
    targets.add(DecodedTarget{0x400, SUBROUTINE});

    targets.sort_vectors();

    EXPECT_EQ(targets.get_label(0x400), "FUNC0"); // SUBROUTINE checked first?
}

TEST_F(TargetsTest, unknown_address_returns_empty)
{
    targets.add(DecodedTarget{0x200, JUMP});
    targets.sort_vectors();

    EXPECT_EQ(targets.get_label(0x201), "");
}

TEST_F(TargetsTest, indexed_list_contents)
{
    targets.add(DecodedTarget{0x300, INDEXED});
    targets.add(DecodedTarget{0x200, INDEXED});
    targets.add(DecodedTarget{0x200, INDEXED});

    targets.sort_vectors();

    auto list = targets.get_index_list();
    ASSERT_EQ(list.size(), 2);
    EXPECT_EQ(list[0], 0x200);
    EXPECT_EQ(list[1], 0x300);
}

#ifndef NDEBUG
TEST_F(TargetsTest, get_label_without_sort_asserts)
{
    targets.add(DecodedTarget{0x200, JUMP});
    EXPECT_DEATH(targets.get_label(0x200), ".*");
}
#endif
