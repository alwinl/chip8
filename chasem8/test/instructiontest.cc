/*
 * instructiontest.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "instruction.h"

#include <gtest/gtest.h>

struct TestCaseData
{
    std::string test_name;
    std::vector<std::string> arguments;
    bool expect_success;
    uint16_t expected_binary;  // Only used if expect_success == true
};

template<typename InstructionType>
class InstructionEmitTest : public ::testing::TestWithParam<TestCaseData>
{
public:
    uint16_t read_uint16_from_stream(std::ostringstream& output)
    {
        const std::string& result = output.str();
        if (result.size() != 2)
            throw std::runtime_error("Expected exactly 2 bytes in output.");

        return (static_cast<uint8_t>(result[0]) << 8) | static_cast<uint8_t>(result[1]);
    }

    void run_test()
    {
        const auto& param = GetParam();
        SymbolTable sym;
        std::ostringstream output;

        InstructionType instr(param.arguments, sym);

        if (param.expect_success)
        {
            ASSERT_NO_THROW(instr.emit_binary(output));
            EXPECT_EQ(read_uint16_from_stream(output), param.expected_binary);
        }
        else
        {
            EXPECT_THROW(instr.emit_binary(output), std::exception);
        }
    }
};

// class DBTest   : public InstructionEmitTest<DBInstruction> {};
// class DWTest   : public InstructionEmitTest<DWInstruction> {};
class CLSTest  : public InstructionEmitTest<CLSInstruction> {};
class RETTest  : public InstructionEmitTest<RETInstruction> {};
class SYSTest  : public InstructionEmitTest<SYSInstruction> {};
class JPTest   : public InstructionEmitTest<JPInstruction> {};
class CALLTest : public InstructionEmitTest<CALLInstruction> {};
class SETest   : public InstructionEmitTest<SEInstruction> {};
class SNETest  : public InstructionEmitTest<SNEInstruction> {};
class LDTest   : public InstructionEmitTest<LDInstruction> {};
class ADDTest  : public InstructionEmitTest<ADDInstruction> {};
class ORTest   : public InstructionEmitTest<ORInstruction> {};
class ANDTest  : public InstructionEmitTest<ANDInstruction> {};
class XORTest  : public InstructionEmitTest<XORInstruction> {};
class SUBTest  : public InstructionEmitTest<SUBInstruction> {};
class SHRTest  : public InstructionEmitTest<SHRInstruction> {};
class SUBNTest : public InstructionEmitTest<SUBNInstruction> {};
class SHLTest  : public InstructionEmitTest<SHLInstruction> {};
class RNDTest  : public InstructionEmitTest<RNDInstruction> {};
class DRWTest  : public InstructionEmitTest<DRWInstruction> {};
class SKPTest  : public InstructionEmitTest<SKPInstruction> {};
class SKPNTest : public InstructionEmitTest<SKPNInstruction> {};

// TEST_P( DBTest,   EmitBinary ) { run_test(); }
// TEST_P( DWTest,   EmitBinary ) { run_test(); }
TEST_P( CLSTest,  EmitBinary ) { run_test(); }
TEST_P( RETTest,  EmitBinary ) { run_test(); }
TEST_P( SYSTest,  EmitBinary ) { run_test(); }
TEST_P( JPTest,   EmitBinary ) { run_test(); }
TEST_P( CALLTest, EmitBinary ) { run_test(); }
TEST_P( SETest,   EmitBinary ) { run_test(); }
TEST_P( SNETest,  EmitBinary ) { run_test(); }
TEST_P( LDTest,   EmitBinary ) { run_test(); }
TEST_P( ADDTest,  EmitBinary ) { run_test(); }
TEST_P( ORTest,   EmitBinary ) { run_test(); }
TEST_P( ANDTest,  EmitBinary ) { run_test(); }
TEST_P( XORTest,  EmitBinary ) { run_test(); }
TEST_P( SUBTest,  EmitBinary ) { run_test(); }
TEST_P( SHRTest,  EmitBinary ) { run_test(); }
TEST_P( SUBNTest, EmitBinary ) { run_test(); }
TEST_P( SHLTest,  EmitBinary ) { run_test(); }
TEST_P( RNDTest,  EmitBinary ) { run_test(); }
TEST_P( DRWTest,  EmitBinary ) { run_test(); }
TEST_P( SKPTest,  EmitBinary ) { run_test(); }
TEST_P( SKPNTest, EmitBinary ) { run_test(); }

// INSTANTIATE_TEST_SUITE_P( Chasem8, DBTest,
//     ::testing::Values(
//         // success cases
//         TestCaseData{ "DataBytes", {}, true, 0x00E0 },

//         // failure cases
//         TestCaseData{ "UnexpectedArgument", { "V1" }, false, 0 },
//         TestCaseData{ "UnexpectedArguments", { "V1", ",", "0x200" }, false, 0 }
//     ),
//     [](const ::testing::TestParamInfo<TestCaseData>& info) {
//         return info.param.test_name;
//     }
// );

// INSTANTIATE_TEST_SUITE_P( Chasem8, DWTest,
//     ::testing::Values(
//         // success cases
//         TestCaseData{ "DataWords", {}, true, 0x00E0 },

//         // failure cases
//         TestCaseData{ "UnexpectedArgument", { "V1" }, false, 0 },
//         TestCaseData{ "UnexpectedArguments", { "V1", ",", "0x200" }, false, 0 }
//     ),
//     [](const ::testing::TestParamInfo<TestCaseData>& info) {
//         return info.param.test_name;
//     }
// );

INSTANTIATE_TEST_SUITE_P( Chasem8, CLSTest,
    ::testing::Values(
        // success cases
        TestCaseData{ "ClearScreen", {}, true, 0x00E0 },

        // failure cases
        TestCaseData{ "UnexpectedArgument", { "V1" }, false, 0 },
        TestCaseData{ "UnexpectedArguments", { "V1", ",", "0x200" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, RETTest,
    ::testing::Values(
        // success cases
        TestCaseData{ "ReturnFromSubroutine", {}, true, 0x00EE },

        // failure cases
        TestCaseData{ "UnexpectedArgument", { "0x200" }, false, 0 },
        TestCaseData{ "UnexpectedArguments", { "V0", ",", "V1" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SYSTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "SystemCallLowAddress", { "0x123" }, true, 0x0123 },
        TestCaseData{ "SystemCallZero", { "0x0" }, true, 0x0000 },
        TestCaseData{ "SystemCallDecimal", { "255" }, true, 0x00FF }, // decimal within valid range

        // Failure cases
        TestCaseData{ "AddressTooHigh", { "0x234" }, false, 0 }, // > 0x1FF not allowed
        TestCaseData{ "MissingArgument", {}, false, 0 },
        TestCaseData{ "InvalidArgument", { "VG" }, false, 0 },
        TestCaseData{ "TooManyArguments", { "0x123", ",", "0x001" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, JPTest,
    ::testing::Values(
        // success cases
        TestCaseData{ "EmitsJumpToAddress", { "0x300" }, true, 0x1300 },
        TestCaseData{ "EmitsJumpViaV0", { "V0", ",", "0x456" }, true, 0xB456 },

        // failure cases
        TestCaseData{ "ThrowsOnInvalidRegisterJump", { "V1", ",", "0x400" }, false, 0 }  // Invalid register
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, CALLTest,
    ::testing::Values(
      // Success cases
      TestCaseData{ "CallSimpleAddress", { "0x300" }, true, 0x2300 },
      TestCaseData{ "CallHexAddress", { "0x456" }, true, 0x2456 },
      TestCaseData{ "CallDecimalAddress", { "789" }, true, 0x2315 },  // 789 in hex = 0x315

      // Failure cases
      TestCaseData{ "CallMissingAddress", {}, false, 0 },
      TestCaseData{ "CallGarbageArgument", { "ABC" }, false, 0 },
      TestCaseData{ "CallTooHighAddress", { "0x1000" }, false, 0 },
      TestCaseData{ "CallRegisterInsteadOfAddress", { "V1" }, false, 0 }
  ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SETest,
    ::testing::Values(
        // success cases
        TestCaseData{ "RegisterToRegisterComparison", {"V1", ",", "V2"}, true,  0x5120 }, // SE V1, V2 -> 5xy0 format
        TestCaseData{ "RegisterToRegisterComparison2", {"VB, VC"}, true,  0x5BC0 }, // SE VB, VC -> 5xy0 format
        TestCaseData{ "RegisterToImmediateComparison", {"V3", ",", "0xAB"}, true, 0x33AB }, // SE V3, 0xAB -> 3xkk format
        TestCaseData{ "RegisterToImmediateComparison2", {"VA, 0xFF"}, true, 0x3AFF }, // SE VA, 0xFF -> 3xkk format

        // failure cases
        TestCaseData{ "MissingArgument", {"V1"}, false, 0 },             // Missing second argument
        TestCaseData{ "FirstArgNotRegister", {"A1", ",", "V2"}, false, 0 },   // First arg not a register
        TestCaseData{ "FirstArgNotRegisterNumeric", {"123", ",", "V2"}, false, 0 },   // First arg not a register
        TestCaseData{ "InvalidImmediate", {"V1", ",", "XYZ"}, false, 0 },  // Invalid second argument
        TestCaseData{ "InvalidRegister", {"VG", ",", "V1"}, false, 0 },    // Invalid register name
        TestCaseData{ "ImmediateTooLarge", { "V1", ",", "0x123" }, false, 0 }    // Immediate value too large
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SNETest,
    ::testing::Values(
        // success cases
        TestCaseData{ "RegisterToRegisterComparison", {"V1", ",", "V2"}, true,  0x9120 }, // SNE V1, V2 -> 5xy0 format
        TestCaseData{ "RegisterToRegisterComparison2", {"VB, VC"}, true,  0x9BC0 }, // SNE VB, VC -> 5xy0 format
        TestCaseData{ "RegisterToImmediateComparison", {"V3", ",", "0xAB"}, true, 0x43AB }, // SNE V3, 0xAB -> 3xkk format
        TestCaseData{ "RegisterToImmediateComparison2", {"VA, 0xFF"}, true, 0x4AFF }, // SNE VA, 0xFF -> 3xkk format

        // failure cases
        TestCaseData{ "MissingArgument", {"V1"}, false, 0 },             // Missing second argument
        TestCaseData{ "FirstArgNotRegister", {"A1", ",", "V2"}, false, 0 },   // First arg not a register
        TestCaseData{ "FirstArgNotRegisterNumeric", {"123", ",", "V2"}, false, 0 },   // First arg not a register
        TestCaseData{ "InvalidImmediate", {"V1", ",", "XYZ"}, false, 0 },  // Invalid second argument
        TestCaseData{ "InvalidRegister", {"VG", ",", "V1"}, false, 0 },    // Invalid register name
        TestCaseData{ "ImmediateTooLarge", { "V1", ",", "0x123" }, false, 0 }    // Immediate value too large
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, LDTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "LoadImmediate", { "V1", ",", "0x7F" }, true, 0x617F },   // 6xkk
        TestCaseData{ "LoadRegister", { "V2", ",", "V3" }, true, 0x8230 },       // 8xy0
        TestCaseData{ "LoadDelayTimerIntoVx", { "V4", ",", "DT" }, true, 0xF407 }, // Fx07
        TestCaseData{ "LoadKeyIntoVx", { "V5", ",", "K" }, true, 0xF50A },         // Fx0A
        TestCaseData{ "LoadAddressIntoI", { "I,0x456" }, true, 0xA456 }, // Annn

        // Success cases for Vx into timers
        TestCaseData{ "LoadVxIntoDelayTimer", { "DT", ",", "V6" }, true, 0xF615 }, // Fx15
        TestCaseData{ "LoadVxIntoSoundTimer", { "ST", ",", "V7" }, true, 0xF718 }, // Fx18
        TestCaseData{ "LoadFIntoI", { "F", ",", "V8" }, true, 0xF829 },            // Fx29
        TestCaseData{ "LoadBCDIntoMemory", { "B", ",", "V9" }, true, 0xF933 },     // Fx33
        TestCaseData{ "StoreRegisters", { "[I]", ",", "VA" }, true, 0xFA55 },      // Fx55
        TestCaseData{ "LoadRegistersFromMemory", { "VB", ",", "[I]" }, true, 0xFB65 }, // Fx65

        // Failure cases
        TestCaseData{ "MissingArgument", {}, false, 0 },
        TestCaseData{ "InvalidAddressFormat", { "XYZ" }, false, 0 },
        TestCaseData{ "AddressOutOfRange", { "0x1234" }, false, 0 }, // > 0xFFF
        TestCaseData{ "MissingArguments", { "V1" }, false, 0 },
        TestCaseData{ "InvalidRegisterFirstArg", { "X1", ",", "0x20" }, false, 0 },
        TestCaseData{ "InvalidSecondArg", { "V2", ",", "XYZ" }, false, 0 },
        TestCaseData{ "TooBigImmediate", { "V1", ",", "0x1FF" }, false, 0 }, // > 0xFF
        TestCaseData{ "UnknownSpecialRegister", { "ABC", ",", "V2" }, false, 0 },
        TestCaseData{ "InvalidMemoryLoadSyntax", { "VA", ",", "I" }, false, 0 },
        TestCaseData{ "InvalidMemoryStoreSyntax", { "I", ",", "VA" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, ADDTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "RegistersSimple", { "V5", ",", "V6" }, true, 0x8564 },
        TestCaseData{ "RegistersHexLetters", { "V4, VB" }, true, 0x84B4 },

        // Failure cases
        TestCaseData{ "MissingArgument", { "V5" }, false, 0 },
        TestCaseData{ "FirstArgNotRegister", { "C5", ",", "V2" }, false, 0 },
        TestCaseData{ "SecondArgNotRegister", { "V1, XYZ" }, false, 0 },
        TestCaseData{ "InvalidRegisterName", { "VG", ",", "V2" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, ORTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "RegistersSimple", { "V1", ",", "V2" }, true, 0x8121 },
        TestCaseData{ "RegistersHexLetters", { "VA, VB" }, true, 0x8AB1 },

        // Failure cases
        TestCaseData{ "MissingArgument", { "V1" }, false, 0 },
        TestCaseData{ "FirstArgNotRegister", { "A1", ",", "V2" }, false, 0 },
        TestCaseData{ "SecondArgNotRegister", { "V1, 123" }, false, 0 },
        TestCaseData{ "InvalidRegisterName", { "VG", ",", "V2" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, ANDTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "RegistersSimple", { "V3", ",", "V4" }, true, 0x8342 },
        TestCaseData{ "RegistersHexLetters", { "VC, VD" }, true, 0x8CD2 },

        // Failure cases
        TestCaseData{ "MissingArgument", { "V1" }, false, 0 },
        TestCaseData{ "FirstArgNotRegister", { "A1", ",", "V2" }, false, 0 },
        TestCaseData{ "SecondArgNotRegister", { "V1, XYZ" }, false, 0 },
        TestCaseData{ "InvalidRegisterName", { "VG", ",", "V2" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, XORTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "RegistersSimple", { "V5", ",", "V6" }, true, 0x8563 },
        TestCaseData{ "RegistersHexLetters", { "VE, VF" }, true, 0x8EF3 },

        // Failure cases
        TestCaseData{ "MissingArgument", { "V5" }, false, 0 },
        TestCaseData{ "FirstArgNotRegister", { "C5", ",", "V2" }, false, 0 },
        TestCaseData{ "SecondArgNotRegister", { "V1, XYZ" }, false, 0 },
        TestCaseData{ "InvalidRegisterName", { "VG", ",", "V2" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SUBTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "RegistersSimple", { "V5", ",", "V6" }, true, 0x8565 },
        TestCaseData{ "RegistersHexLetters", { "V4, VB" }, true, 0x84B5 },

        // Failure cases
        TestCaseData{ "MissingArgument", { "V5" }, false, 0 },
        TestCaseData{ "FirstArgNotRegister", { "C5", ",", "V2" }, false, 0 },
        TestCaseData{ "SecondArgNotRegister", { "V1, XYZ" }, false, 0 },
        TestCaseData{ "InvalidRegisterName", { "VG", ",", "V2" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SHRTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "RegistersSimple", { "V5", ",", "V6" }, true, 0x8566 },
        TestCaseData{ "RegistersHexLetters", { "V4, VB" }, true, 0x84B6 },

        // Failure cases
        TestCaseData{ "MissingArgument", { "V5" }, false, 0 },
        TestCaseData{ "FirstArgNotRegister", { "C5", ",", "V2" }, false, 0 },
        TestCaseData{ "SecondArgNotRegister", { "V1, XYZ" }, false, 0 },
        TestCaseData{ "InvalidRegisterName", { "VG", ",", "V2" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SUBNTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "RegistersSimple", { "V5", ",", "V6" }, true, 0x8567 },
        TestCaseData{ "RegistersHexLetters", { "V4, VB" }, true, 0x84B7 },

        // Failure cases
        TestCaseData{ "MissingArgument", { "V5" }, false, 0 },
        TestCaseData{ "FirstArgNotRegister", { "C5", ",", "V2" }, false, 0 },
        TestCaseData{ "SecondArgNotRegister", { "V1, XYZ" }, false, 0 },
        TestCaseData{ "InvalidRegisterName", { "VG", ",", "V2" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SHLTest,
    ::testing::Values(
        // Success cases
        TestCaseData{ "RegistersSimple", { "V5", ",", "V6" }, true, 0x856E },
        TestCaseData{ "RegistersHexLetters", { "V4, VB" }, true, 0x84BE },

        // Failure cases
        TestCaseData{ "MissingArgument", { "V5" }, false, 0 },
        TestCaseData{ "FirstArgNotRegister", { "C5", ",", "V2" }, false, 0 },
        TestCaseData{ "SecondArgNotRegister", { "V1, XYZ" }, false, 0 },
        TestCaseData{ "InvalidRegisterName", { "VG", ",", "V2" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, RNDTest,
    ::testing::Values(
        TestCaseData{ "RandomIntoV0", { "V0", ",", "0x7F" }, true, 0xC07F },
        TestCaseData{ "RandomIntoVF", { "VF", ",", "0xFF" }, true, 0xCFFF },
        TestCaseData{ "RandomIntoVA", { "VA", ",", "0x01" }, true, 0xCA01 },

        // Failure cases
        TestCaseData{ "MissingArguments", { "V1" }, false, 0 },
        TestCaseData{ "FirstNotRegister", { "10", ",", "0xFF" }, false, 0 },
        TestCaseData{ "GarbageRegister", { "XYZ", ",", "0xFF" }, false, 0 },
        TestCaseData{ "MissingComma", { "V2", "0x12" }, false, 0 },
        TestCaseData{ "InvalidByte", { "V3", ",", "XYZ" }, false, 0 },
        TestCaseData{ "TooBigImmediate", { "V4", ",", "0x123" }, false, 0 },
        TestCaseData{ "InvalidRegisterVG", { "VG", ",", "0x55" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, DRWTest,
    ::testing::Values(
      // Success cases
      TestCaseData{ "DrawSimple", { "V0", ",", "V1", ",", "5" }, true, 0xD015 },
      TestCaseData{ "DrawBigRegisters", { "VA", ",", "VF", ",", "C" }, true, 0xDAFC },
      TestCaseData{ "DrawLowerRegisters", { "V2", ",", "V3", ",", "A" }, true, 0xD23A },

      // Failure cases
      TestCaseData{ "MissingArguments", { "V1", ",", "V2" }, false, 0 },
      TestCaseData{ "FirstArgNotRegister", { "1", ",", "V2", ",", "5" }, false, 0 },
      TestCaseData{ "SecondArgNotRegister", { "V1", ",", "2", ",", "5" }, false, 0 },
      TestCaseData{ "ThirdArgNotNibble", { "V1", ",", "V2", ",", "20" }, false, 0 },  // nibble > 0xF
      TestCaseData{ "GarbageRegister", { "XYZ", ",", "V1", ",", "5" }, false, 0 },
      TestCaseData{ "GarbageNibble", { "V1", ",", "V2", ",", "XYZ" }, false, 0 }
  ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SKPTest,
    ::testing::Values(
       // Success cases
       TestCaseData{ "SkipIfKeyPressedV0", { "V0" }, true, 0xE09E },
       TestCaseData{ "SkipIfKeyPressedVF", { "VF" }, true, 0xEF9E },
       TestCaseData{ "SkipIfKeyPressedVA", { "VA" }, true, 0xEA9E },

       // Failure cases
       TestCaseData{ "MissingRegister", {}, false, 0 },
       TestCaseData{ "NotARegister", { "123" }, false, 0 },
       TestCaseData{ "GarbageInput", { "XYZ" }, false, 0 },
       TestCaseData{ "InvalidRegisterVG", { "VG" }, false, 0 }
   ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);

INSTANTIATE_TEST_SUITE_P( Chasem8, SKPNTest,
    ::testing::Values(
        TestCaseData{ "SkipIfKeyNotPressedV0", { "V0" }, true, 0xE0A1 },
        TestCaseData{ "SkipIfKeyNotPressedVF", { "VF" }, true, 0xEFA1 },
        TestCaseData{ "SkipIfKeyNotPressedVB", { "VB" }, true, 0xEBA1 },

        // Failure cases
        TestCaseData{ "MissingRegister", {}, false, 0 },
        TestCaseData{ "NotARegister", { "456" }, false, 0 },
        TestCaseData{ "GarbageInput", { "WXYZ" }, false, 0 },
        TestCaseData{ "InvalidRegisterVG", { "VG" }, false, 0 }
    ),
    [](const ::testing::TestParamInfo<TestCaseData>& info) {
        return info.param.test_name;
    }
);




TEST(Chasem8_DBInstructionTest, ExplodedArguments)
{
    SymbolTable sym;
    std::vector<std::string> args = { "0x1", ",", "2", ",", "003", ",", "0xFF" };

    DBInstruction instr(args, sym);

    std::ostringstream output;
    instr.emit_binary(output);

    std::string result = output.str();
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(static_cast<uint8_t>(result[0]), 0x01);
    EXPECT_EQ(static_cast<uint8_t>(result[1]), 0x02);
    EXPECT_EQ(static_cast<uint8_t>(result[2]), 0x03);
    EXPECT_EQ(static_cast<uint8_t>(result[3]), 0xFF);
}

TEST(Chasem8_DBInstructionTest, CombinedArguments)
{
    SymbolTable sym;
    std::vector<std::string> args = { "0x1,2, 003 ,   0xFF " };

    DBInstruction instr(args, sym);

    std::ostringstream output;
    instr.emit_binary(output);

    std::string result = output.str();
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(static_cast<uint8_t>(result[0]), 0x01);
    EXPECT_EQ(static_cast<uint8_t>(result[1]), 0x02);
    EXPECT_EQ(static_cast<uint8_t>(result[2]), 0x03);
    EXPECT_EQ(static_cast<uint8_t>(result[3]), 0xFF);
}

TEST(Chasem8_DBInstructionTest, ThrowsOnInvalidByteValue)
{
    SymbolTable sym;
    std::vector<std::string> args = { "256" };

    EXPECT_THROW(DBInstruction instr(args, sym), std::runtime_error);
}

TEST(Chasem8_DBInstructionTest, ThrowsOnEmptyArgs)
{
    SymbolTable sym;
    std::vector<std::string> args = {};

    EXPECT_THROW(DBInstruction instr(args, sym), std::runtime_error);
}
