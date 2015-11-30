/* 
 * A runnable main program that tests the functionality of class FieldInt.
 * 
 * Bitcoin cryptography library
 * Copyright (c) Project Nayuki
 * 
 * http://www.nayuki.io/page/bitcoin-cryptography-library
 * https://github.com/nayuki/Bitcoin-Cryptography-Library
 */

#include "TestHelper.hpp"
#include <cinttypes>
#include <cstdio>
#include "AsmX8664.hpp"
#include "FieldInt.hpp"


/*---- Structures ----*/

struct BinaryCase {
	const char *x;
	const char *y;
};

struct TernaryCase {
	const char *x;
	const char *y;
	const char *z;
};


// Global variables
static int numTestCases = 0;


/*---- Test cases ----*/

static void testComparison() {
	BinaryCase cases[] = {  // All hexadecimal strings must be in uppercase for strcmp() to work properly
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"F000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0123400000000000000000000000000000000000000000000000000000000000", "0123400000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000080", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"FF00000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		BinaryCase &tc = cases[i];
		int cmp = strcmp(tc.x, tc.y);
		FieldInt x(tc.x);
		FieldInt y(tc.y);
		assert((x == y) == (cmp == 0));
		assert((x != y) == (cmp != 0));
		assert((x <  y) == (cmp <  0));
		assert((x >  y) == (cmp >  0));
		assert((x <= y) == (cmp <= 0));
		assert((x >= y) == (cmp >= 0));
		numTestCases++;
	}
}


static void testAdd() {
	TernaryCase cases[] = {
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000080000000", "0000000000000000000000000000000000000000000000000000000080000000", "0000000000000000000000000000000000000000000000000000000100000000"},
		{"000000000000000000000000000000000000000000000FFFFFFFFFFFFFFFFFFF", "0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000010000000000000000000"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000001", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2D"},
		{"FF00000000000000000000000000000000000000000000000000000000000000", "FF00000000000000000000000000000000000000000000000000000000000000", "FE000000000000000000000000000000000000000000000000000001000003D1"},
		{"CF82A6E670C187AF1855BFD6A02F676D58B2659A6010EDA5B1ADA5662135EA37", "3130EC4C765E299FE200CD62BF4439F061440B57751B104BF14552A553064508", "00B39332E71FB14EFA568D395F73A15DB9F670F1D52BFDF1A2F2F80C743C3310"},
		{"F93CAB87674D5FEA845300E8BC63C9D313585200F2DA0E9B955CD07FAFB46787", "42207C06D78BD8D62C57746D35D0427AD9EDAA7D8AA2BFD39144167418E44DC4", "3B5D278E3ED938C0B0AA7555F2340C4DED45FC7E7D7CCE6F26A0E6F4C898B91C"},
		{"A30910E72FD6AC01A3EF74124085ECDA6713B0E4D7E840C4890ECD272C83A394", "71AB910A371A0D15370C19D5B1562EDB339C66C9DBC72E4194D5B3361F9208B6", "14B4A1F166F0B916DAFB8DE7F1DC1BB59AB017AEB3AF6F061DE4805E4C15B01B"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		TernaryCase &tc = cases[i];
		FieldInt x(tc.x);
		x.add(FieldInt(tc.y));
		assert(x == FieldInt(tc.z));
		numTestCases++;
	}
}


static void testSubtract() {
	TernaryCase cases[] = {
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000003", "0000000000000000000000000000000000000000000000000000000000000002", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000001", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E"},
		{"0000000000000000000000000000000000000000000000000000000000000000", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"0000000000000000000000000000000000000000000000000000000000000000", "0FFF000000000000000000050000000400003000000000000200100000000000", "F000FFFFFFFFFFFFFFFFFFFAFFFFFFFBFFFFCFFFFFFFFFFFFDFFEFFEFFFFFC2F"},
		{"484786B41F8CC919C7B47E08B918E13BCADF20777224E3B7C7334EDF6E98CBBE", "57F3E075C0014548EB29756F8C4394D1541112662C298F8BAC5211F7FC9D8930", "F053A63E5F8B83D0DC8B08992CD54C6A76CE0E1145FB542C1AE13CE671FB3EBD"},
		{"69A1B6279F3CF9E29D9D46C32AEBAB2694A25DFE291A2B6F7A8116727B811705", "290113FE9D369FE41F1CB33B29AD34666744A06A310A30A98B8D631A36133A4B", "40A0A229020659FE7E809388013E76C02D5DBD93F80FFAC5EEF3B358456DDCBA"},
		{"E5E4684E5EDDA9EEFA6D4CBF84089280750391E9E68B95C84C671B07D2AF9081", "686058D3961A761D9A60E2810487BDE706BB5DF21005E4EC1C2B68ECC7F6E241", "7D840F7AC8C333D1600C6A3E7F80D4996E4833F7D685B0DC303BB21B0AB8AE40"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		TernaryCase &tc = cases[i];
		FieldInt x(tc.x);
		x.subtract(FieldInt(tc.y));
		assert(x == FieldInt(tc.z));
		numTestCases++;
	}
}


static void testMultiply2() {
	BinaryCase cases[] = {
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000002"},
		{"0000000000000000000000000000000000000000000000000000000080000000", "0000000000000000000000000000000000000000000000000000000100000000"},
		{"00000000000000000000000000000000000000000000000000000000FFFFFFFF", "00000000000000000000000000000000000000000000000000000001FFFFFFFE"},
		{"000ABCDEF0000000000000000000000000000000000000000000000000000000", "001579BDE0000000000000000000000000000000000000000000000000000000"},
		{"8000000000000000000000000000000000000000000000000000000000000000", "00000000000000000000000000000000000000000000000000000001000003D1"},
		{"FFFF000000000000000000000000000000000000000000000000000000000000", "FFFE0000000000000000000000000000000000000000000000000001000003D1"},
		{"7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFE17", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E"},
		{"7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFE18", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2D"},
		{"34C8B477C0E20991A17F295A49F4F671EC4126CEE5A07EDEF85BA9CE12167127", "699168EF81C4132342FE52B493E9ECE3D8824D9DCB40FDBDF0B7539C242CE24E"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		BinaryCase &tc = cases[i];
		FieldInt x(tc.x);
		x.multiply2();
		assert(x == FieldInt(tc.y));
		numTestCases++;
	}
}


static void testMultiply() {
	TernaryCase cases[] = {
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"0000000000000000000000000000000000000000000000000000000000000002", "0000000000000000000000000000000000000000000000000000000000000004", "0000000000000000000000000000000000000000000000000000000000000008"},
		{"000000000000000000000000000000000000000000000000000000000000BB81", "0000000000000000000000000000000000000000000000000000000000002375", "0000000000000000000000000000000000000000000000000000000019F854F5"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2C", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC25", "000000000000000000000000000000000000000000000000000000000000001E"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE00000000", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE00000000", "000000000000000000000000000000000000000000000000FFFFF85E000E90A1"},
		{"ABC928448F874620BDB2D01F4D797EED5788CC2475334002E16E6BCC12DCF419", "D661B81BED420F5B5DD8027D1486C7D27C85E6BDB0405EC07849CFD1A7EE526C", "AB599D4FE47BEB97C94342AF197455F0436F10C40395DDD42128417885C672BA"},
		{"3720E6127667A3DE448044EE8DECD7C96F345CDA261682A4386719A387C37ED5", "F8E32435123472D4949BF98C22A87A374923A3B06B289EF15E93F0940AAB3650", "1796B08378C50FF3394B99A672D9B09593A1C6D5F5AF593B7115C786E5BF1CD3"},
		{"47E3D45C7F5A64DE0D4913911D541BBC0DF640C0920A4FB42FC6ED5ACE413D51", "E8BD16CB61DF4B53B66A35140377314B7EA9CB1099D7E1F05FC50FC49D0EDDF2", "C676783301FD88069B4B1C36863B88BAA27B9D7A9C27C9E06203DF15809CAC2F"},
		{"C3AACF7136F758A89979DD75CF45B6AF07486D43D1B5767C269210EE61C904F4", "EA31DEF20213B863699C0F370265E53F8FDCA440CC0690E6D3672928BEA51DC4", "B07726B8E2591C33DB8E3A2EB214B10B878A7E81B45420508B1ED0EC6BEF7921"},
		{"290C735EECFEDD247D92D5DB8CED26626F8196E2A6F0D7282D97354484134110", "E0BBDEF09C2E071D94F71FA78DDE9E5E785C3AF99C53DDF63F29409752488C53", "90D08D31627D37BD120577A9910D6B737DE0732F9156DE5044D88EBA3BA726F3"},
		{"926E48EB3B5E7017D3C4BDF3FCD34938BB469782991EB4D8AFA8FDC192091058", "D61C4475D5E43CE7F7F922F4BA9E1D52A94602CFBF73836EC20ECF748D7A2AB2", "0426CF89D91A2D2BC186255A9FE55BFB00679E101919C5AF0C9BFA7820060C32"},
		{"71D40203AEC9CC1A35CF06B490A742208729B7A000CA8DD32E72A7813D98AB44", "A59F39FC19046E62CB85F0654133E0323C5A21020D5F26B1A52A41626689E330", "1A4E32C22BF8B090962C78DF069FD5A5CC8F77D53C44758AA24BF6064EBB7CBD"},
		{"5CFBD891A5591D3D2B48EAFA4CF890804D8B4AF83D2CA1B1BF0D8E723575C657", "CED7A3AC8DF01F9E1FB478DDDE3DB6B20FBC61709798525099C6CE4CBE21E78E", "B36988AC85DB0D0AF431FB25EFAE653CD276BD46A465DE2FC076E078D0247CDD"},
		{"0000000000000000000000100000000004000000008000000000000000000000", "0400020000000000000400000040000000000000000000000000000000000000", "0000200000000000400020F4408A20083D521E89FFE213E0443E120003D8A200"},
		{"0000000000000000800000000000000200000000000000000000020000000000", "0000000000000004000000000000000000000000000000000000000000100000", "000008000008000000000002002007A20000000800001E882000000000000000"},
		{"0000000004000008000000000000000000000000100000000000000000000000", "0000000000000000000001000000000000000000000000000000000000000000", "00000000000000000000000400001744001E8800000000000000001000003D10"},
		{"0000000000000000000000002480000000000800000000000000008000000001", "0000000000000000000000000000000000000000000000000000000010000000", "0000000000000000024800000000008000000000000000080000000010000000"},
		{"0000000000010100000080000000000000000000000000000000000000000000", "0000000000800004002000000000000000000000000000000000000000000000", "00000000000000808005EEC8AF56B2EA27A23D10000000000000000000000000"},
		{"0000000000001900004000000000000000000000000000000000000000000000", "0000000004000000002000000000000000000020010000000000000000010000", "00000000190000A400027DA723D113ED201E88000003201913EDBF8788F44000"},
		{"0000000020000000000000000800020000000004004000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000008000000000000500010000000", "0000020000000000000000000000020000000000000000000000000002000080", "000010000000000000A000200000000010001400003D10A000E8002A62A07A20"},
		{"000000080080200822000024800000809021080140000120400020000402001A", "0800002000000081000000002080080008000011408040060000008002001818", "D410273D5D74934EC1E3F70C03F86A8F88F14D8B43A495D294960F65F535A7B1"},
		{"0000000080800584081000825000448240002002000000220000000000000010", "1000004400000000000000004000020240A08200080512000080080400090400", "85C3F6129B1069554CA017978E5A061702C737AE9B9BFE0FE9A6E4FFC946328F"},
		{"0000000000002010401800001030604101000010000000008408001000500000", "0020002000400040000000020210408008800025000080100004004001000000", "B2539614159365E59E3499117513E7C65127C0D3E6F333998C218C0FA4913626"},
		{"0009000200002008000C24200400000000040402000000004400200000000000", "00000000000820008001004400800A040002000000000004080C0000002C0010", "10B16AC39B468C6DBD6C29DC447F96B3E9771A6DE522482A3ECDEBE39FEF9709"},
		{"0800002000010240400000000004010281020000002C00200000840280000402", "0010000800000400000064000200002481000010080134001200000000001000", "A231B671857BB0BC56A1C93123161ABF32A21E0F6C9B1FF7280FBF52DB2914CA"},
		{"0041004100000A00000480060000001000405000008000020080014001200100", "0000008000021200040006000000000220000000040200000080180082042500", "3CAB4633B74E7AFD47308B17CD30789F478DE90ECF66C427CA6FE1BFEFDF2A5B"},
		{"0000000442001000102010400000000000024100010000000222000008000000", "4001000000000400000000020000088000200000004000000000008100208000", "003F3B8AE945A87904B68FC3FDE734E6634E4EE7575197E3BA1446454ABD96A3"},
		{"0040050240000002000010009004000110000000080880800804040001000000", "8000001400020200000000080000800080000000008008104000000280001000", "AC8F2C2189BC7F99E69E4A846078977F4333CBFDA90813432DD02FED00EFD90C"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		TernaryCase &tc = cases[i];
		FieldInt x(tc.x);
		x.multiply(FieldInt(tc.y));
		assert(x == FieldInt(tc.z));
		numTestCases++;
	}
}


static void testSquare() {
	BinaryCase cases[] = {
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"000000000000000000000000000000000000000000000000000000000000000D", "00000000000000000000000000000000000000000000000000000000000000A9"},
		{"0000000000000000000000000000000000000000000000000000000000010000", "0000000000000000000000000000000000000000000000000000000100000000"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2C", "0000000000000000000000000000000000000000000000000000000000000009"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2D", "0000000000000000000000000000000000000000000000000000000000000004"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"0000000000400000000100000004000000011040000000200400000000401002", "824028092388408020852B028011EF0A4223E7613008271C6082557B22225E8C"},
		{"0000000000000000000002100000000000000001000000100000000000000800", "0000420000000001002100200000010000045100103D110000000420004FBE20"},
		{"0002020040000200000000000000100000000410000000000000000000000000", "08871062B331535BB68FD1004F8408F544EECA47369C40883F044007D1000F44"},
		{"1000000008008400000000000000000000200000000000000020000000000000", "D2001483D17F00C1386F8543C11400000F46042107A67DF110460027D8A28964"},
		{"0018484A883C83014F000060A81004032E4280B134E118202CD6002001020008", "28F4F42E263EBFD3D490EBF20F0B8671C64D3AC29FFCBA6D5CCD5860C126D125"},
		{"10090024008008C8002010800015A0113822109912105300048C0280A2B00C11", "178E0E76E746FF9A20E4596880126DA1B2732E969294120BDB97100B69C08160"},
		{"0C49400420000200C031484216240A804408000240600446C122812001008040", "42D4EC4E80B8C5EF9580E5701247EA77186F63F27792136A4B8E2CF2AC8538B4"},
		{"002A8023B00101000460200300E05490104240A14300008041C0210800240102", "4289C245D69B319990DD4C39DFF331E4526BF586CE891664A7B523AAC8917BA0"},
		{"D44459EC5669C56C03485BE9C803519EEB977F8A6000F792D0916B43031EEC1C", "C50FBA1B1469D877F7391A7C4384D4ADA8B5A10B5505EA00CC822368CEF7D2ED"},
		{"03FAEC56081E36092A72FA87B01132C946A60307CD8BF2032725423D44894689", "56E50BE9E1A6AEFCCFBB4A212D495617DCC35C141F96A7FC298071E812A03C52"},
		{"AB013B2A27380172D641B9E05471E91EDB00D962610E018334B19E7FA1FFE0A9", "3EE56E4D501D9D17D064E53C9A7B5A793AAD6CC28FF7A5516205ACD7D489B750"},
		{"32F2943DA6EC0CAD5E69C0C9B1437A5EA3B9A9FFA14E92C0F6B51C25FD400661", "F7C9FB555AF3D1A3E3FA5A23C571E51F4B7835A3E6DE979C9D76626526EE81A5"},
		{"B7AAF40F663FB643BA2C24EB0BA4B4E50D159041C5F8317EA452F6851BDE308B", "E18F65F416F5D4F22E798CF6FD36CCA3F7D6579C38F07D10A78E4A3948645EDF"},
		{"E83608ECE352FC369836FB19B5EBE6B565254BF4C644B71EA2CD4227C30FFA8E", "456E921DAE39689EFDFCB5C84314066003DC0DAC3EFBC38658461191C3F2CCB2"},
		{"82914A20F06D7B6B14BB3A4E133C9502ADB2D82CFE99010507425FD69870C31A", "548B693ED55F7F9DBD465564B2B40C960A05DE3B87EEC8C464D0B0C3C04DB6FF"},
		{"F8F2C1D5A0C5005E82CF09331249D8325885D254AF460DEEDEE00DF91FCFEE7B", "61050FA8E39ADD595250DB4F27D1764D7ED576C9C1BF61992B8BA870CE83ADE2"},
		{"FFFFFFFFFFEFFFFFFDFFFFFFFFFFFFFFFEFABFFFFFFFFFFDBFFFFEFDFFFFDFFF", "29210AA31A000170C49BB1020D82C03EA73C252C808237011014BD6115E28900"},
		{"7FFFEFFFFFFFFEFFFFFEFFFFFFBFEFFFFFFFBFFFFFF7FFFFFFFF7FFFFFFFFFFF", "49C31305E11B3144A611F0C173D3EEA0CB244659C4297CD5E7D5A305B0C74694"},
		{"FFFFFFFEFFF7FFBFFFFFEFEFFFFFFFFFFFFFFFFFFFFFFFFFF5FFFFFFFFF7FFFF", "FFFFF9A2070DFC72419892F045BFB5F4F10C8903C4A670B4D3F0A82E197130A2"},
		{"FFFBFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDFFFFDFFFFFFFFFF", "E1803D100000000000000000000000000400007FC004FB0D13B9C7EBF186C23F"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		BinaryCase &tc = cases[i];
		FieldInt x(tc.x);
		x.square();
		assert(x == FieldInt(tc.y));
		numTestCases++;
	}
}


static void testReciprocal() {
	BinaryCase cases[] = {
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},  // Special value
		{"0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"0000000000000000000000000000000000000000000000000000000000000002", "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFE18"},
		{"0000000000000000000000000000000000000000000000000000000000000003", "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA9FFFFFD75"},
		{"0000000000000000000000000000000000000000000000000000000000000004", "3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBFFFFF0C"},
		{"000000000000000000000000000000000000000000000000000000000000000B", "45D1745D1745D1745D1745D1745D1745D1745D1745D1745D1745D1741745D06A"},
		{"8000000000000000000000000000000000000000000000000000000000000000", "937A320A2AA70733388D85852BE56EC3796447FDB84940B3B070123B10D03625"},
		{"FF00000000000000000000000000000000000000000000000000000000000000", "C6839CA1B70A8E27C40ACD902618D031EEA0C4C39FC464BE96CED7F5016982BC"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2C", "55555555555555555555555555555555555555555555555555555554FFFFFEBA"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2D", "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFE17"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E"},
		{"0000000040000000002000000010000000000000000000000000010000000000", "ECB197B0AF02BA21F11DC46E5176C9768DABD04632ED312A0179307A61D8651F"},
		{"0000000000000000010000000000020000000000000000000000000000000000", "CE9F88395251C47DA1B15A389BA516758397E687F84A29D032F00F6ADDC00ED2"},
		{"0100000000000000000000000002000000000000000000008002001200000000", "E7BB6E5A4665CDDD8048383E758A2BC392B318A7D9E475BF9DE1832EA11E45B1"},
		{"0000000000000000000100000000000000000000000000000000000080000000", "0BE4C6479A2C9CD9B5A3FC1B2FB6037557C5CC32D20D32E9B193252DF60DA1B0"},
		{"0000000000000000000000000000000000000000000000000000000800000000", "BA44A6A61FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF45BB5693"},
		{"0000000000800200000000000000000000000000080000000000000000000000", "86C2A63AFF967FC3296B4C0326F8C22CD462FE747CD84C10837393FEB9B4331A"},
		{"0000010000000000000000000000000000000000000002000000050080000000", "B20BA22045BA27232C6D8E73E6130E53FFE86192312336A7994AA67BB9539D8D"},
		{"0000000000000000000000200000000000002000008000000000000000000020", "3DFE456451645A345F2C283F4C23370CD91E920685CD7D35B3D63ACB4D3E4486"},
		{"2000000000100000000002080000000020000000000000000020001000000000", "C5B46C7779772C9841ABD475ACCF25FA0C655AA1252C8D3DD370B918BF0BEE42"},
		{"0002000000000000004000000000000000000040011000004200000000000100", "35FA2B66B444A2646E8EB584CF074D364C8DE82B3F9C3F4BB484871840222B87"},
		{"C000010001008000000000400040420000800000200004000002000000000000", "691762BB9927D1FDE37C393CA61A47266799D10D02384F11CEC66D471233DD76"},
		{"0000000000000000000000200001000040000000000000000000000004000000", "2F5D91FA0379199695F85ABEC690EB7BC6623DED3F436266D79B8372CEFECC3A"},
		{"002000440008840000020190000000010131060000020110400A200000020200", "72370C13808001FBDAA830FCE93D107116ECD5C4B55ACA10E17034390D8E2C32"},
		{"0000509000110410A00220028004020000020000200840404000000200000200", "A9E40B54B7FB071239FD53CA5570EEDAF3F5CFA71357FAA1AE6D80E36FEF5671"},
		{"02002000000080500010040300A0003400200000000100000200000600001000", "2731D77E65EF210F658E01903BFE82476D71C6769008B8E9B7C22235B1B950B2"},
		{"0000008020000000028101042002000090028010010000601200220001102004", "8A7C67E47823CCAC705D024189CC151E08DA9C3D437679C6DE988966D2E0737F"},
		{"580ABFE333C14A172B4EE30A6E3414B1793B5A1B7E044512F03762437316485B", "19303494171D57D45A42D7847CAEF83DDDCBB15EE09B89A22846C447234E3C9C"},
		{"5DBD48D823FC1CE863124C73536C19D4D367D1C0CF93AF3B4A9C5C042CF04127", "12AC8FBC16BCBD123504C03E64CACC63221C5DEC490A55D423EDC164D671AC8A"},
		{"7B1ACF5305BCEF21EE8262CB3501F7DFA9F4757B125EE2689B139559AB737E94", "D48B8C7717158E6A880EE7A15A6DEE751B6ECE610099BEAC041B005D9811552C"},
		{"88F92281D8B79F249434EE365A77C8331DD15960F17A2285FCBF62913DAE638E", "7FB174E4018D369FA85891F1D3B4A5C9DE9D27C89C5C6B82989DFDE444E9FDEA"},
		{"FC5CA7167DF8A7F5AC2A4B326D9EF89510C157D466160702C6EB136CD7877C0B", "CDB8FA655DD940589EE9ACED17A3E345071C950E812D3BBDCB1F83D73E898955"},
		{"7BDBDD9EBA49A09DC328AC060378AAA90149BB97877AA27EE2C0F49E78093B4A", "02A68DD563AD0FC338D7F8C7D139B2B1D6A4F6B666406377C9EC362A211347BD"},
		{"72B6E00D159EFB4E46346B6D9541CB81A0337013A3569D54A80CEEA5A03BD408", "28C8403EEF7D2642674AE68F0CD58991B69EF41F4186A8329C023CDE4D0120D2"},
		{"109CC7044F9F1EDBEE64AA8D33EA4325116183E9EB1574A600B04589C045048B", "E93CFF3C41D12B3C303DE828929B29BFFD0D1DBFC4384323A7B754E29C321442"},
		{"084F1DEF6553CF294586DCB74BA804019ADD34D2343ABBBFF19A2EA9A453CB6D", "A70CC5B12FC2DCDB8650C2616D15474460D97B8E20C0B562C8C0D278AA79F0AE"},
		{"570229D6F3C7354805CE394505BF04B653075F7B05BBB754697C88AC1BD7207E", "4DDA6708B65BD91B91403B0F84A4983DC239FA789F8FD9A533859C34A01702CF"},
		{"44A266D4F06D2376A466B7B4CD56E9C1D8A13FD15D6064CA6DA7F82F44BFA984", "F92FD6E20E9DBA93EFBCEFF5D4670BDE15AF1A6AB91E17AD1DC877192C5701AE"},
		{"1FF4707865A5C29CBD08EC6EC97ABB9C4B8ABD8D4074B2C1DD49144234E00DE7", "B3FCAA1B76C5A609169AC5026C385AAF3BEB37CBE2C34EBFD1C49F02474ECC5D"},
		{"FFFFFBF7FF7FFFFFFFFBFF7FFFFFFFFF7FFFFFFFEE7DFDBFFFFFFFBFDEFFFFFF", "10176DA98CF511E10CB469DE3C21B32F725D8CD4E5D3D1C7EF2807A46EE83919"},
		{"FBF7FF7FFFFFFFFFFFEDFFFFFFFF7FFF3FFFFFF7FFD7FFFFDFFEF9FFFF7F7FFF", "F35A05C33E845843B90941B4D9887BC338A58ACC209AA384288484D00A103E7D"},
		{"FCD9FFFFFFFDFFFFFB7BDFF7FFFFFFFFB7FBFDFFFDE7FFFFFFF5DF7FBBFFFBFF", "5872BA2F65D0258A43B9655C71CEBF2DCCB776D0E312B82F511FE018FCBC90CC"},
		{"F5DFFFDCBFFFFEFFFFFFFFFFFFCFCF7FFDFFFFFBED7FE2FFEFDF7FFFFFFFFEFE", "E0B08D6C7C1B2C06BFAE1EE52687F40F476AEB06C55B97CAE73619F19390A1F1"},
		{"FFFFFFFFFFFFFFFEFFBEEFF7FFFFFFFFFFFBFFFFFFFFFFFDFFBFFFFFFFFFFFFF", "CB316B57BB720F5F4107B2F015BAC4E448D4A8FB454C04E3D1E3D3A40417467E"},
		{"FFFFFF7FFFFFFFFFFFFFFDFFFFFFFFFFFFFFBFBFEFFFFDFFFFFFFFFBFFFFFFFF", "E93C93E84B6FBAADAF9375B370338A3FFB40548E580DF43E86B790FCCE9A1EB2"},
		{"FCFFFFFFBDFFFB7FFFFFFBFFFFFFFFFFFFFFFFF7FFFF7FFFFFFFFBFFFFEFFFEF", "4B11C164854D94DD8506E49E6F63415F84D4E87D4AEAA556DA49B1B8755C988F"},
		{"FFFFFFFFFFF7FFFFBFFFFFFFFFDFFFFFFFFFEBFFFFFFFFF7FBFFFFDFFFFFFFF7", "E92ABD8E79C2A960431ED5186B773B28150634B4348E53260BD45CDCA10007C8"},
		{"FFFFFFFFFFFD7FFFFFFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFF7FFDFFFFFFFFFF", "A4B4BC0C127D58ACF38611F6B342B7987D8ED3ECC478FBA4ED39008DC6897B40"},
		{"FFFFFFFFFFFFFFFFFFF7FFFFFFFFFFDFFFFF7FFFFFFFFFFFFFFFDFFFFFFFFFFF", "936E477CB1370054EDEDF2B53D94A76CA43634CB5E7D0164004E7CE7C2F39D7B"},
		{"FFFBFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBFFFFFF", "8C4DCE37E34C5D5440586B9B85905C9132D219E34C476B8C40C2787A23FF6AC9"},
		{"FFFFBFFFFFFFFDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "7263040B5EEE4822288B6A963B256FF428820BAC9240E07B08CA6DDBF1908888"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		BinaryCase &tc = cases[i];
		FieldInt x(tc.x);
		x.reciprocal();
		assert(x == FieldInt(tc.y));
		numTestCases++;
	}
}


static void testConstructorUint256() {
	BinaryCase cases[] = {
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"8000000000000000000000000000000000000000000000000000000000000000", "8000000000000000000000000000000000000000000000000000000000000000"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC30", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC32", "0000000000000000000000000000000000000000000000000000000000000003"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "00000000000000000000000000000000000000000000000000000001000003D0"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		BinaryCase &tc = cases[i];
		assert(FieldInt(Uint256(tc.x)) == FieldInt(tc.y));
		numTestCases++;
	}
}


static void testAsmMultiply256x256eq512() {
	TernaryCase cases[] = {
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE0000000000000000000000000000000000000000000000000000000000000001"},
		{"8000000000000000800000000000000080000000000000008000000000000000", "0000000000000000000000000000000000000000000000010000000000000001", "00000000000000000000000000000000000000000000000080000000000000010000000000000001000000000000000100000000000000008000000000000000"},
		{"0000000000000000000000000000000000000000000000010000000000000001", "8000000000000000800000000000000080000000000000008000000000000000", "00000000000000000000000000000000000000000000000080000000000000010000000000000001000000000000000100000000000000008000000000000000"},
		{"0000000000000000000000000000000100000000000000010000000000000001", "6000000000000000600000000000000060000000000000006000000000000000", "000000000000000000000000000000006000000000000000C00000000000000120000000000000012000000000000000C0000000000000006000000000000000"},
		{"6000000000000000600000000000000060000000000000006000000000000000", "0000000000000000000000000000000100000000000000010000000000000001", "000000000000000000000000000000006000000000000000C00000000000000120000000000000012000000000000000C0000000000000006000000000000000"},
		{"0000000000000001000000000000000100000000000000010000000000000001", "4000000000000000400000000000000040000000000000004000000000000000", "000000000000000040000000000000008000000000000000C0000000000000010000000000000000C00000000000000080000000000000004000000000000000"},
		{"0000000000000001000000000000000100000000000000010000000000000001", "4000000000000000400000000000000040000000000000004000000000000000", "000000000000000040000000000000008000000000000000C0000000000000010000000000000000C00000000000000080000000000000004000000000000000"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		TernaryCase &tc = cases[i];
		Uint256 x(tc.x);
		Uint256 y(tc.y);
		uint32_t z[16];
		asm_FieldInt_multiply256x256eq512(&z[0], &x.value[0], &y.value[0]);
		for (int j = 0; j < 16; j++) {
			uint32_t word;
			sscanf(&tc.z[j * 8], "%08" SCNx32, &word);
			assert(word == z[15 - j]);
		}
		numTestCases++;
	}
}


static void testAsmMultiplyBarrettStep0() {
	BinaryCase cases[] = {
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "00000000000000000000000000000000000000000000000000000001000003D0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000000000000000000000000000000000000000000000000FFFFFFFFFFFFFFFF", "00000000000000000000000000000000000000000000000000000000000003D0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC2EFFFFFFFF0000000000000000000000000000000000000001000003D0FFFFFFFEFFFFFC2F"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		BinaryCase &tc = cases[i];
		uint32_t src[16];
		for (int j = 0; j < 16; j++)
			sscanf(&tc.x[j * 8], "%08" SCNx32, &src[15 - j]);
		uint32_t dest[24];
		asm_FieldInt_multiplyBarrettStep0(&dest[0], &src[0]);
		for (int j = 0; j < 24; j++) {
			uint32_t word;
			sscanf(&tc.y[j * 8], "%08" SCNx32, &word);
			assert(word == dest[23 - j]);
		}
		numTestCases++;
	}
}


static void testAsmMultiplyBarrettStep1() {
	BinaryCase cases[] = {
		{"8000000000000000000000000000000000000000000000000000000000000000", "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFE178000000000000000000000000000000000000000000000000000000000000000"},
		{"8000000000000000000000000000000000000000000000000000000080000001", "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE187FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFE167FFFFC2F"},
		{"000000000000000000000000000000000043142D118E47CB983CCA48D7E8F111", "000000000000000000000000000000000043142D118E47CB983CCA48D7E8F110FFFFFFFFFFFFFFFFFFFFFFFFFFBCEBD1EE71B83467C335B728170EEEFFFFFE1F"},
	};
	for (unsigned int i = 0; i < ARRAY_LENGTH(cases); i++) {
		BinaryCase &tc = cases[i];
		uint32_t src[8];
		for (int j = 0; j < 8; j++)
			sscanf(&tc.x[j * 8], "%08" SCNx32, &src[7 - j]);
		uint32_t dest[16];
		asm_FieldInt_multiplyBarrettStep1(&dest[0], &src[0]);
		for (int j = 0; j < 16; j++) {
			uint32_t word;
			sscanf(&tc.y[j * 8], "%08" SCNx32, &word);
			assert(word == dest[15 - j]);
		}
		numTestCases++;
	}
}


int main(int argc, char **argv) {
	testComparison();
	testAdd();
	testSubtract();
	testMultiply2();
	testMultiply();
	testSquare();
	testReciprocal();
	testConstructorUint256();
	testAsmMultiply256x256eq512();
	testAsmMultiplyBarrettStep0();
	testAsmMultiplyBarrettStep1();
	printf("All %d test cases passed\n", numTestCases);
	return 0;
}
