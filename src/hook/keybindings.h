#pragma once

#define STRINGENTRY_LETTERS  1
#define STRINGENTRY_SPACE    2
#define STRINGENTRY_NUMBERS  4
#define STRINGENTRY_CAPS     8
#define STRINGENTRY_SYMBOLS  16
#define STRINGENTRY_FILENAME 32

typedef long InterfaceKey;
enum InterfaceKeyType {
  INTERFACEKEY_STRING_A000,
  INTERFACEKEY_STRING_A001,
  INTERFACEKEY_STRING_A002,
  INTERFACEKEY_STRING_A003,
  INTERFACEKEY_STRING_A004,
  INTERFACEKEY_STRING_A005,
  INTERFACEKEY_STRING_A006,
  INTERFACEKEY_STRING_A007,
  INTERFACEKEY_STRING_A008,
  INTERFACEKEY_STRING_A009,
  INTERFACEKEY_STRING_A010,
  INTERFACEKEY_STRING_A011,
  INTERFACEKEY_STRING_A012,
  INTERFACEKEY_STRING_A013,
  INTERFACEKEY_STRING_A014,
  INTERFACEKEY_STRING_A015,
  INTERFACEKEY_STRING_A016,
  INTERFACEKEY_STRING_A017,
  INTERFACEKEY_STRING_A018,
  INTERFACEKEY_STRING_A019,
  INTERFACEKEY_STRING_A020,
  INTERFACEKEY_STRING_A021,
  INTERFACEKEY_STRING_A022,
  INTERFACEKEY_STRING_A023,
  INTERFACEKEY_STRING_A024,
  INTERFACEKEY_STRING_A025,
  INTERFACEKEY_STRING_A026,
  INTERFACEKEY_STRING_A027,
  INTERFACEKEY_STRING_A028,
  INTERFACEKEY_STRING_A029,
  INTERFACEKEY_STRING_A030,
  INTERFACEKEY_STRING_A031,
  INTERFACEKEY_STRING_A032,
  INTERFACEKEY_STRING_A033,
  INTERFACEKEY_STRING_A034,
  INTERFACEKEY_STRING_A035,
  INTERFACEKEY_STRING_A036,
  INTERFACEKEY_STRING_A037,
  INTERFACEKEY_STRING_A038,
  INTERFACEKEY_STRING_A039,
  INTERFACEKEY_STRING_A040,
  INTERFACEKEY_STRING_A041,
  INTERFACEKEY_STRING_A042,
  INTERFACEKEY_STRING_A043,
  INTERFACEKEY_STRING_A044,
  INTERFACEKEY_STRING_A045,
  INTERFACEKEY_STRING_A046,
  INTERFACEKEY_STRING_A047,
  INTERFACEKEY_STRING_A048,
  INTERFACEKEY_STRING_A049,
  INTERFACEKEY_STRING_A050,
  INTERFACEKEY_STRING_A051,
  INTERFACEKEY_STRING_A052,
  INTERFACEKEY_STRING_A053,
  INTERFACEKEY_STRING_A054,
  INTERFACEKEY_STRING_A055,
  INTERFACEKEY_STRING_A056,
  INTERFACEKEY_STRING_A057,
  INTERFACEKEY_STRING_A058,
  INTERFACEKEY_STRING_A059,
  INTERFACEKEY_STRING_A060,
  INTERFACEKEY_STRING_A061,
  INTERFACEKEY_STRING_A062,
  INTERFACEKEY_STRING_A063,
  INTERFACEKEY_STRING_A064,
  INTERFACEKEY_STRING_A065,
  INTERFACEKEY_STRING_A066,
  INTERFACEKEY_STRING_A067,
  INTERFACEKEY_STRING_A068,
  INTERFACEKEY_STRING_A069,
  INTERFACEKEY_STRING_A070,
  INTERFACEKEY_STRING_A071,
  INTERFACEKEY_STRING_A072,
  INTERFACEKEY_STRING_A073,
  INTERFACEKEY_STRING_A074,
  INTERFACEKEY_STRING_A075,
  INTERFACEKEY_STRING_A076,
  INTERFACEKEY_STRING_A077,
  INTERFACEKEY_STRING_A078,
  INTERFACEKEY_STRING_A079,
  INTERFACEKEY_STRING_A080,
  INTERFACEKEY_STRING_A081,
  INTERFACEKEY_STRING_A082,
  INTERFACEKEY_STRING_A083,
  INTERFACEKEY_STRING_A084,
  INTERFACEKEY_STRING_A085,
  INTERFACEKEY_STRING_A086,
  INTERFACEKEY_STRING_A087,
  INTERFACEKEY_STRING_A088,
  INTERFACEKEY_STRING_A089,
  INTERFACEKEY_STRING_A090,
  INTERFACEKEY_STRING_A091,
  INTERFACEKEY_STRING_A092,
  INTERFACEKEY_STRING_A093,
  INTERFACEKEY_STRING_A094,
  INTERFACEKEY_STRING_A095,
  INTERFACEKEY_STRING_A096,
  INTERFACEKEY_STRING_A097,
  INTERFACEKEY_STRING_A098,
  INTERFACEKEY_STRING_A099,
  INTERFACEKEY_STRING_A100,
  INTERFACEKEY_STRING_A101,
  INTERFACEKEY_STRING_A102,
  INTERFACEKEY_STRING_A103,
  INTERFACEKEY_STRING_A104,
  INTERFACEKEY_STRING_A105,
  INTERFACEKEY_STRING_A106,
  INTERFACEKEY_STRING_A107,
  INTERFACEKEY_STRING_A108,
  INTERFACEKEY_STRING_A109,
  INTERFACEKEY_STRING_A110,
  INTERFACEKEY_STRING_A111,
  INTERFACEKEY_STRING_A112,
  INTERFACEKEY_STRING_A113,
  INTERFACEKEY_STRING_A114,
  INTERFACEKEY_STRING_A115,
  INTERFACEKEY_STRING_A116,
  INTERFACEKEY_STRING_A117,
  INTERFACEKEY_STRING_A118,
  INTERFACEKEY_STRING_A119,
  INTERFACEKEY_STRING_A120,
  INTERFACEKEY_STRING_A121,
  INTERFACEKEY_STRING_A122,
  INTERFACEKEY_STRING_A123,
  INTERFACEKEY_STRING_A124,
  INTERFACEKEY_STRING_A125,
  INTERFACEKEY_STRING_A126,
  INTERFACEKEY_STRING_A127,
  INTERFACEKEY_STRING_A128,
  INTERFACEKEY_STRING_A129,
  INTERFACEKEY_STRING_A130,
  INTERFACEKEY_STRING_A131,
  INTERFACEKEY_STRING_A132,
  INTERFACEKEY_STRING_A133,
  INTERFACEKEY_STRING_A134,
  INTERFACEKEY_STRING_A135,
  INTERFACEKEY_STRING_A136,
  INTERFACEKEY_STRING_A137,
  INTERFACEKEY_STRING_A138,
  INTERFACEKEY_STRING_A139,
  INTERFACEKEY_STRING_A140,
  INTERFACEKEY_STRING_A141,
  INTERFACEKEY_STRING_A142,
  INTERFACEKEY_STRING_A143,
  INTERFACEKEY_STRING_A144,
  INTERFACEKEY_STRING_A145,
  INTERFACEKEY_STRING_A146,
  INTERFACEKEY_STRING_A147,
  INTERFACEKEY_STRING_A148,
  INTERFACEKEY_STRING_A149,
  INTERFACEKEY_STRING_A150,
  INTERFACEKEY_STRING_A151,
  INTERFACEKEY_STRING_A152,
  INTERFACEKEY_STRING_A153,
  INTERFACEKEY_STRING_A154,
  INTERFACEKEY_STRING_A155,
  INTERFACEKEY_STRING_A156,
  INTERFACEKEY_STRING_A157,
  INTERFACEKEY_STRING_A158,
  INTERFACEKEY_STRING_A159,
  INTERFACEKEY_STRING_A160,
  INTERFACEKEY_STRING_A161,
  INTERFACEKEY_STRING_A162,
  INTERFACEKEY_STRING_A163,
  INTERFACEKEY_STRING_A164,
  INTERFACEKEY_STRING_A165,
  INTERFACEKEY_STRING_A166,
  INTERFACEKEY_STRING_A167,
  INTERFACEKEY_STRING_A168,
  INTERFACEKEY_STRING_A169,
  INTERFACEKEY_STRING_A170,
  INTERFACEKEY_STRING_A171,
  INTERFACEKEY_STRING_A172,
  INTERFACEKEY_STRING_A173,
  INTERFACEKEY_STRING_A174,
  INTERFACEKEY_STRING_A175,
  INTERFACEKEY_STRING_A176,
  INTERFACEKEY_STRING_A177,
  INTERFACEKEY_STRING_A178,
  INTERFACEKEY_STRING_A179,
  INTERFACEKEY_STRING_A180,
  INTERFACEKEY_STRING_A181,
  INTERFACEKEY_STRING_A182,
  INTERFACEKEY_STRING_A183,
  INTERFACEKEY_STRING_A184,
  INTERFACEKEY_STRING_A185,
  INTERFACEKEY_STRING_A186,
  INTERFACEKEY_STRING_A187,
  INTERFACEKEY_STRING_A188,
  INTERFACEKEY_STRING_A189,
  INTERFACEKEY_STRING_A190,
  INTERFACEKEY_STRING_A191,
  INTERFACEKEY_STRING_A192,
  INTERFACEKEY_STRING_A193,
  INTERFACEKEY_STRING_A194,
  INTERFACEKEY_STRING_A195,
  INTERFACEKEY_STRING_A196,
  INTERFACEKEY_STRING_A197,
  INTERFACEKEY_STRING_A198,
  INTERFACEKEY_STRING_A199,
  INTERFACEKEY_STRING_A200,
  INTERFACEKEY_STRING_A201,
  INTERFACEKEY_STRING_A202,
  INTERFACEKEY_STRING_A203,
  INTERFACEKEY_STRING_A204,
  INTERFACEKEY_STRING_A205,
  INTERFACEKEY_STRING_A206,
  INTERFACEKEY_STRING_A207,
  INTERFACEKEY_STRING_A208,
  INTERFACEKEY_STRING_A209,
  INTERFACEKEY_STRING_A210,
  INTERFACEKEY_STRING_A211,
  INTERFACEKEY_STRING_A212,
  INTERFACEKEY_STRING_A213,
  INTERFACEKEY_STRING_A214,
  INTERFACEKEY_STRING_A215,
  INTERFACEKEY_STRING_A216,
  INTERFACEKEY_STRING_A217,
  INTERFACEKEY_STRING_A218,
  INTERFACEKEY_STRING_A219,
  INTERFACEKEY_STRING_A220,
  INTERFACEKEY_STRING_A221,
  INTERFACEKEY_STRING_A222,
  INTERFACEKEY_STRING_A223,
  INTERFACEKEY_STRING_A224,
  INTERFACEKEY_STRING_A225,
  INTERFACEKEY_STRING_A226,
  INTERFACEKEY_STRING_A227,
  INTERFACEKEY_STRING_A228,
  INTERFACEKEY_STRING_A229,
  INTERFACEKEY_STRING_A230,
  INTERFACEKEY_STRING_A231,
  INTERFACEKEY_STRING_A232,
  INTERFACEKEY_STRING_A233,
  INTERFACEKEY_STRING_A234,
  INTERFACEKEY_STRING_A235,
  INTERFACEKEY_STRING_A236,
  INTERFACEKEY_STRING_A237,
  INTERFACEKEY_STRING_A238,
  INTERFACEKEY_STRING_A239,
  INTERFACEKEY_STRING_A240,
  INTERFACEKEY_STRING_A241,
  INTERFACEKEY_STRING_A242,
  INTERFACEKEY_STRING_A243,
  INTERFACEKEY_STRING_A244,
  INTERFACEKEY_STRING_A245,
  INTERFACEKEY_STRING_A246,
  INTERFACEKEY_STRING_A247,
  INTERFACEKEY_STRING_A248,
  INTERFACEKEY_STRING_A249,
  INTERFACEKEY_STRING_A250,
  INTERFACEKEY_STRING_A251,
  INTERFACEKEY_STRING_A252,
  INTERFACEKEY_STRING_A253,
  INTERFACEKEY_STRING_A254,
  INTERFACEKEY_STRING_A255,
  INTERFACEKEY_KEYBINDING_COMPLETE,
  INTERFACEKEYNUM,
};

static const std::unordered_map<int, int> cyrillic_utf8_to_cp1251{
  { 37072, 192 }, { 37328, 193 }, { 37584, 194 }, { 37840, 195 }, { 38096, 196 }, { 38352, 197 }, { 38608, 198 }, { 38864, 199 },
  { 39120, 200 }, { 39376, 201 }, { 39632, 202 }, { 39888, 203 }, { 40144, 204 }, { 40400, 205 }, { 40656, 206 }, { 40912, 207 },
  { 41168, 208 }, { 41424, 209 }, { 41680, 210 }, { 41936, 211 }, { 42192, 212 }, { 42448, 213 }, { 42704, 214 }, { 42960, 215 },
  { 43216, 216 }, { 43472, 217 }, { 43728, 218 }, { 43984, 219 }, { 44240, 220 }, { 44496, 221 }, { 44752, 222 }, { 45008, 223 },
  { 45264, 224 }, { 45520, 225 }, { 45776, 226 }, { 46032, 227 }, { 46288, 228 }, { 46544, 229 }, { 46800, 230 }, { 47056, 231 },
  { 47312, 232 }, { 47568, 233 }, { 47824, 234 }, { 48080, 235 }, { 48336, 236 }, { 48592, 237 }, { 48848, 238 }, { 49104, 239 },
  { 32977, 240 }, { 33233, 241 }, { 33489, 242 }, { 33745, 243 }, { 34001, 244 }, { 34257, 245 }, { 34513, 246 }, { 34769, 247 },
  { 35025, 248 }, { 35281, 249 }, { 35537, 250 }, { 35793, 251 }, { 36049, 252 }, { 36305, 253 }, { 36561, 254 }, { 36817, 255 },
  { 33232, 168 }, { 37329, 184 }, { 34000, 170 }, { 38097, 186 }, { 34768, 175 }, { 38865, 191 }, { 34512, 178 }, { 38609, 179 },
  { 37074, 165 }, { 37330, 180 }, { 36560, 161 }, { 40657, 162 }
};

static const std::unordered_set<char> invalid_filename_chars{ '<', '>', ':', '"', '/', '\\', '|', '?', '*' };
