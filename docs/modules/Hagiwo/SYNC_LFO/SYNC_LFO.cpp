//https://note.com/solder_state/n/n4c600f2431c3

#include <FlexiTimer2.h>

#include <avr/io.h>
#define PWMPin 10

unsigned int frq = 50000; // PWM周波数。60kHzあたりまで機能するが、マージンとって50kHzとする。
float duty = 0.5; // duty比率
int count = 0;

byte mode = 1;//
//0=saw1
//1=saw2
//2=sine
//3=tri
//4=squ
//5=random
//6=steady

int set_freq = 1;//
int freq_max = 30;//外部クロック周期(*100usec)
int amp = 1;//change pwm duty
float amp_rate = 1.0;
int phase = 1;
int mod = 0;//self modulation

bool ext_injudge = 0;//0=use internal clock , 1 = use external clock
bool ext_pulse = 0;//0=no external in
bool old_ext_pulse = 0;
long ext_count = 0;
long ext_count_result = 0;
long old_ext_count_result = 0;
long ext_period = 0;
bool reset_count = 0;

//wave table
const static word saw1[1000] PROGMEM = {
 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,  112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,  128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,  144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159,  160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  170,  171,  172,  173,  174,  175,  176,  177,  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191,  192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207,  208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223,  224,  225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239,  240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255,  256,  257,  258,  259,  260,  261,  262,  263,  264,  265,  266,  267,  268,  269,  270,  271,  272,  273,  274,  275,  276,  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,  287,  288,  289,  290,  291,  292,  293,  294,  295,  296,  297,  298,  299,  300,  301,  302,  303,  304,  305,  306,  307,  308,  309,  310,  311,  312,  313,  314,  315,  316,  317,  318,  319,  320,  321,  322,  323,  324,  325,  326,  327,  328,  329,  330,  331,  332,  333,  334,  335,  336,  337,  338,  339,  340,  341,  342,  343,  344,  345,  346,  347,  348,  349,  350,  351,  352,  353,  354,  355,  356,  357,  358,  359,  360,  361,  362,  363,  364,  365,  366,  367,  368,  369,  370,  371,  372,  373,  374,  375,  376,  377,  378,  379,  380,  381,  382,  383,  384,  385,  386,  387,  388,  389,  390,  391,  392,  393,  394,  395,  396,  397,  398,  399,  400,  401,  402,  403,  404,  405,  406,  407,  408,  409,  410,  411,  412,  413,  414,  415,  416,  417,  418,  419,  420,  421,  422,  423,  424,  425,  426,  427,  428,  429,  430,  431,  432,  433,  434,  435,  436,  437,  438,  439,  440,  441,  442,  443,  444,  445,  446,  447,  448,  449,  450,  451,  452,  453,  454,  455,  456,  457,  458,  459,  460,  461,  462,  463,  464,  465,  466,  467,  468,  469,  470,  471,  472,  473,  474,  475,  476,  477,  478,  479,  480,  481,  482,  483,  484,  485,  486,  487,  488,  489,  490,  491,  492,  493,  494,  495,  496,  497,  498,  499,  500,  501,  502,  503,  504,  505,  506,  507,  508,  509,  510,  511,  512,  513,  514,  515,  516,  517,  518,  519,  520,  521,  522,  523,  524,  525,  526,  527,  528,  529,  530,  531,  532,  533,  534,  535,  536,  537,  538,  539,  540,  541,  542,  543,  544,  545,  546,  547,  548,  549,  550,  551,  552,  553,  554,  555,  556,  557,  558,  559,  560,  561,  562,  563,  564,  565,  566,  567,  568,  569,  570,  571,  572,  573,  574,  575,  576,  577,  578,  579,  580,  581,  582,  583,  584,  585,  586,  587,  588,  589,  590,  591,  592,  593,  594,  595,  596,  597,  598,  599,  600,  601,  602,  603,  604,  605,  606,  607,  608,  609,  610,  611,  612,  613,  614,  615,  616,  617,  618,  619,  620,  621,  622,  623,  624,  625,  626,  627,  628,  629,  630,  631,  632,  633,  634,  635,  636,  637,  638,  639,  640,  641,  642,  643,  644,  645,  646,  647,  648,  649,  650,  651,  652,  653,  654,  655,  656,  657,  658,  659,  660,  661,  662,  663,  664,  665,  666,  667,  668,  669,  670,  671,  672,  673,  674,  675,  676,  677,  678,  679,  680,  681,  682,  683,  684,  685,  686,  687,  688,  689,  690,  691,  692,  693,  694,  695,  696,  697,  698,  699,  700,  701,  702,  703,  704,  705,  706,  707,  708,  709,  710,  711,  712,  713,  714,  715,  716,  717,  718,  719,  720,  721,  722,  723,  724,  725,  726,  727,  728,  729,  730,  731,  732,  733,  734,  735,  736,  737,  738,  739,  740,  741,  742,  743,  744,  745,  746,  747,  748,  749,  750,  751,  752,  753,  754,  755,  756,  757,  758,  759,  760,  761,  762,  763,  764,  765,  766,  767,  768,  769,  770,  771,  772,  773,  774,  775,  776,  777,  778,  779,  780,  781,  782,  783,  784,  785,  786,  787,  788,  789,  790,  791,  792,  793,  794,  795,  796,  797,  798,  799,  800,  801,  802,  803,  804,  805,  806,  807,  808,  809,  810,  811,  812,  813,  814,  815,  816,  817,  818,  819,  820,  821,  822,  823,  824,  825,  826,  827,  828,  829,  830,  831,  832,  833,  834,  835,  836,  837,  838,  839,  840,  841,  842,  843,  844,  845,  846,  847,  848,  849,  850,  851,  852,  853,  854,  855,  856,  857,  858,  859,  860,  861,  862,  863,  864,  865,  866,  867,  868,  869,  870,  871,  872,  873,  874,  875,  876,  877,  878,  879,  880,  881,  882,  883,  884,  885,  886,  887,  888,  889,  890,  891,  892,  893,  894,  895,  896,  897,  898,  899,  900,  901,  902,  903,  904,  905,  906,  907,  908,  909,  910,  911,  912,  913,  914,  915,  916,  917,  918,  919,  920,  921,  922,  923,  924,  925,  926,  927,  928,  929,  930,  931,  932,  933,  934,  935,  936,  937,  938,  939,  940,  941,  942,  943,  944,  945,  946,  947,  948,  949,  950,  951,  952,  953,  954,  955,  956,  957,  958,  959,  960,  961,  962,  963,  964,  965,  966,  967,  968,  969,  970,  971,  972,  973,  974,  975,  976,  977,  978,  979,  980,  981,  982,  983,  984,  985,  986,  987,  988,  989,  990,  991,  992,  993,  994,  995,  996,  997,  998,  999
};
const static word saw2[1000] PROGMEM = {
 999,  998,  997,  996,  995,  994,  993,  992,  991,  990,  989,  988,  987,  986,  985,  984,  983,  982,  981,  980,  979,  978,  977,  976,  975,  974,  973,  972,  971,  970,  969,  968,  967,  966,  965,  964,  963,  962,  961,  960,  959,  958,  957,  956,  955,  954,  953,  952,  951,  950,  949,  948,  947,  946,  945,  944,  943,  942,  941,  940,  939,  938,  937,  936,  935,  934,  933,  932,  931,  930,  929,  928,  927,  926,  925,  924,  923,  922,  921,  920,  919,  918,  917,  916,  915,  914,  913,  912,  911,  910,  909,  908,  907,  906,  905,  904,  903,  902,  901,  900,  899,  898,  897,  896,  895,  894,  893,  892,  891,  890,  889,  888,  887,  886,  885,  884,  883,  882,  881,  880,  879,  878,  877,  876,  875,  874,  873,  872,  871,  870,  869,  868,  867,  866,  865,  864,  863,  862,  861,  860,  859,  858,  857,  856,  855,  854,  853,  852,  851,  850,  849,  848,  847,  846,  845,  844,  843,  842,  841,  840,  839,  838,  837,  836,  835,  834,  833,  832,  831,  830,  829,  828,  827,  826,  825,  824,  823,  822,  821,  820,  819,  818,  817,  816,  815,  814,  813,  812,  811,  810,  809,  808,  807,  806,  805,  804,  803,  802,  801,  800,  799,  798,  797,  796,  795,  794,  793,  792,  791,  790,  789,  788,  787,  786,  785,  784,  783,  782,  781,  780,  779,  778,  777,  776,  775,  774,  773,  772,  771,  770,  769,  768,  767,  766,  765,  764,  763,  762,  761,  760,  759,  758,  757,  756,  755,  754,  753,  752,  751,  750,  749,  748,  747,  746,  745,  744,  743,  742,  741,  740,  739,  738,  737,  736,  735,  734,  733,  732,  731,  730,  729,  728,  727,  726,  725,  724,  723,  722,  721,  720,  719,  718,  717,  716,  715,  714,  713,  712,  711,  710,  709,  708,  707,  706,  705,  704,  703,  702,  701,  700,  699,  698,  697,  696,  695,  694,  693,  692,  691,  690,  689,  688,  687,  686,  685,  684,  683,  682,  681,  680,  679,  678,  677,  676,  675,  674,  673,  672,  671,  670,  669,  668,  667,  666,  665,  664,  663,  662,  661,  660,  659,  658,  657,  656,  655,  654,  653,  652,  651,  650,  649,  648,  647,  646,  645,  644,  643,  642,  641,  640,  639,  638,  637,  636,  635,  634,  633,  632,  631,  630,  629,  628,  627,  626,  625,  624,  623,  622,  621,  620,  619,  618,  617,  616,  615,  614,  613,  612,  611,  610,  609,  608,  607,  606,  605,  604,  603,  602,  601,  600,  599,  598,  597,  596,  595,  594,  593,  592,  591,  590,  589,  588,  587,  586,  585,  584,  583,  582,  581,  580,  579,  578,  577,  576,  575,  574,  573,  572,  571,  570,  569,  568,  567,  566,  565,  564,  563,  562,  561,  560,  559,  558,  557,  556,  555,  554,  553,  552,  551,  550,  549,  548,  547,  546,  545,  544,  543,  542,  541,  540,  539,  538,  537,  536,  535,  534,  533,  532,  531,  530,  529,  528,  527,  526,  525,  524,  523,  522,  521,  520,  519,  518,  517,  516,  515,  514,  513,  512,  511,  510,  509,  508,  507,  506,  505,  504,  503,  502,  501,  500,  499,  498,  497,  496,  495,  494,  493,  492,  491,  490,  489,  488,  487,  486,  485,  484,  483,  482,  481,  480,  479,  478,  477,  476,  475,  474,  473,  472,  471,  470,  469,  468,  467,  466,  465,  464,  463,  462,  461,  460,  459,  458,  457,  456,  455,  454,  453,  452,  451,  450,  449,  448,  447,  446,  445,  444,  443,  442,  441,  440,  439,  438,  437,  436,  435,  434,  433,  432,  431,  430,  429,  428,  427,  426,  425,  424,  423,  422,  421,  420,  419,  418,  417,  416,  415,  414,  413,  412,  411,  410,  409,  408,  407,  406,  405,  404,  403,  402,  401,  400,  399,  398,  397,  396,  395,  394,  393,  392,  391,  390,  389,  388,  387,  386,  385,  384,  383,  382,  381,  380,  379,  378,  377,  376,  375,  374,  373,  372,  371,  370,  369,  368,  367,  366,  365,  364,  363,  362,  361,  360,  359,  358,  357,  356,  355,  354,  353,  352,  351,  350,  349,  348,  347,  346,  345,  344,  343,  342,  341,  340,  339,  338,  337,  336,  335,  334,  333,  332,  331,  330,  329,  328,  327,  326,  325,  324,  323,  322,  321,  320,  319,  318,  317,  316,  315,  314,  313,  312,  311,  310,  309,  308,  307,  306,  305,  304,  303,  302,  301,  300,  299,  298,  297,  296,  295,  294,  293,  292,  291,  290,  289,  288,  287,  286,  285,  284,  283,  282,  281,  280,  279,  278,  277,  276,  275,  274,  273,  272,  271,  270,  269,  268,  267,  266,  265,  264,  263,  262,  261,  260,  259,  258,  257,  256,  255,  254,  253,  252,  251,  250,  249,  248,  247,  246,  245,  244,  243,  242,  241,  240,  239,  238,  237,  236,  235,  234,  233,  232,  231,  230,  229,  228,  227,  226,  225,  224,  223,  222,  221,  220,  219,  218,  217,  216,  215,  214,  213,  212,  211,  210,  209,  208,  207,  206,  205,  204,  203,  202,  201,  200,  199,  198,  197,  196,  195,  194,  193,  192,  191,  190,  189,  188,  187,  186,  185,  184,  183,  182,  181,  180,  179,  178,  177,  176,  175,  174,  173,  172,  171,  170,  169,  168,  167,  166,  165,  164,  163,  162,  161,  160,  159,  158,  157,  156,  155,  154,  153,  152,  151,  150,  149,  148,  147,  146,  145,  144,  143,  142,  141,  140,  139,  138,  137,  136,  135,  134,  133,  132,  131,  130,  129,  128,  127,  126,  125,  124,  123,  122,  121,  120,  119,  118,  117,  116,  115,  114,  113,  112,  111,  110,  109,  108,  107,  106,  105,  104,  103,  102,  101,  100,  99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0
};
const static word sine[1000] PROGMEM = {
 500,  500,  500,  500,  510,  510,  510,  520,  520,  520,  530,  530,  530,  540,  540,  540,  550,  550,  550,  550,  560,  560,  560,  570,  570,  570,  580,  580,  580,  590,  590,  590,  590,  600,  600,  600,  610,  610,  610,  620,  620,  620,  630,  630,  630,  630,  640,  640,  640,  650,  650,  650,  660,  660,  660,  660,  670,  670,  670,  680,  680,  680,  680,  690,  690,  690,  700,  700,  700,  710,  710,  710,  710,  720,  720,  720,  720,  730,  730,  730,  740,  740,  740,  740,  750,  750,  750,  750,  760,  760,  760,  770,  770,  770,  770,  780,  780,  780,  780,  790,  790,  790,  790,  800,  800,  800,  800,  810,  810,  810,  810,  820,  820,  820,  820,  830,  830,  830,  830,  830,  840,  840,  840,  840,  850,  850,  850,  850,  860,  860,  860,  860,  860,  870,  870,  870,  870,  870,  880,  880,  880,  880,  880,  890,  890,  890,  890,  890,  900,  900,  900,  900,  900,  900,  910,  910,  910,  910,  910,  920,  920,  920,  920,  920,  920,  930,  930,  930,  930,  930,  930,  930,  940,  940,  940,  940,  940,  940,  940,  950,  950,  950,  950,  950,  950,  950,  960,  960,  960,  960,  960,  960,  960,  960,  960,  970,  970,  970,  970,  970,  970,  970,  970,  970,  970,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  1000, 990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  990,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  980,  970,  970,  970,  970,  970,  970,  970,  970,  970,  970,  960,  960,  960,  960,  960,  960,  960,  960,  960,  950,  950,  950,  950,  950,  950,  950,  940,  940,  940,  940,  940,  940,  940,  930,  930,  930,  930,  930,  930,  930,  920,  920,  920,  920,  920,  920,  910,  910,  910,  910,  910,  900,  900,  900,  900,  900,  900,  890,  890,  890,  890,  890,  880,  880,  880,  880,  880,  870,  870,  870,  870,  870,  860,  860,  860,  860,  860,  850,  850,  850,  850,  840,  840,  840,  840,  830,  830,  830,  830,  830,  820,  820,  820,  820,  810,  810,  810,  810,  800,  800,  800,  800,  790,  790,  790,  790,  780,  780,  780,  780,  770,  770,  770,  770,  760,  760,  760,  750,  750,  750,  750,  740,  740,  740,  740,  730,  730,  730,  720,  720,  720,  720,  710,  710,  710,  710,  700,  700,  700,  690,  690,  690,  680,  680,  680,  680,  670,  670,  670,  660,  660,  660,  660,  650,  650,  650,  640,  640,  640,  630,  630,  630,  630,  620,  620,  620,  610,  610,  610,  600,  600,  600,  590,  590,  590,  590,  580,  580,  580,  570,  570,  570,  560,  560,  560,  550,  550,  550,  550,  540,  540,  540,  530,  530,  530,  520,  520,  520,  510,  510,  510,  500,  500,  500,  500,  490,  490,  490,  480,  480,  480,  470,  470,  470,  460,  460,  460,  450,  450,  450,  440,  440,  440,  440,  430,  430,  430,  420,  420,  420,  410,  410,  410,  400,  400,  400,  400,  390,  390,  390,  380,  380,  380,  370,  370,  370,  360,  360,  360,  360,  350,  350,  350,  340,  340,  340,  330,  330,  330,  330,  320,  320,  320,  310,  310,  310,  310,  300,  300,  300,  290,  290,  290,  280,  280,  280,  280,  270,  270,  270,  270,  260,  260,  260,  250,  250,  250,  250,  240,  240,  240,  240,  230,  230,  230,  220,  220,  220,  220,  210,  210,  210,  210,  200,  200,  200,  200,  190,  190,  190,  190,  180,  180,  180,  180,  170,  170,  170,  170,  160,  160,  160,  160,  160,  150,  150,  150,  150,  140,  140,  140,  140,  130,  130,  130,  130,  130,  120,  120,  120,  120,  120,  110,  110,  110,  110,  110,  100,  100,  100,  100,  100,  90, 90, 90, 90, 90, 90, 80, 80, 80, 80, 80, 70, 70, 70, 70, 70, 70, 60, 60, 60, 60, 60, 60, 60, 50, 50, 50, 50, 50, 50, 50, 40, 40, 40, 40, 40, 40, 40, 30, 30, 30, 30, 30, 30, 30, 30, 30, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 30, 30, 30, 30, 30, 30, 30, 30, 30, 40, 40, 40, 40, 40, 40, 40, 50, 50, 50, 50, 50, 50, 50, 60, 60, 60, 60, 60, 60, 60, 70, 70, 70, 70, 70, 70, 80, 80, 80, 80, 80, 90, 90, 90, 90, 90, 90, 100,  100,  100,  100,  100,  110,  110,  110,  110,  110,  120,  120,  120,  120,  120,  130,  130,  130,  130,  130,  140,  140,  140,  140,  150,  150,  150,  150,  160,  160,  160,  160,  160,  170,  170,  170,  170,  180,  180,  180,  180,  190,  190,  190,  190,  200,  200,  200,  200,  210,  210,  210,  210,  220,  220,  220,  220,  230,  230,  230,  240,  240,  240,  240,  250,  250,  250,  250,  260,  260,  260,  270,  270,  270,  270,  280,  280,  280,  280,  290,  290,  290,  300,  300,  300,  310,  310,  310,  310,  320,  320,  320,  330,  330,  330,  330,  340,  340,  340,  350,  350,  350,  360,  360,  360,  360,  370,  370,  370,  380,  380,  380,  390,  390,  390,  400,  400,  400,  400,  410,  410,  410,  420,  420,  420,  430,  430,  430,  440,  440,  440,  440,  450,  450,  450,  460,  460,  460,  470,  470,  470,  480,  480,  480,  490,  490,  490
};
const static word tri[1000] PROGMEM = {
 0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100,  102,  104,  106,  108,  110,  112,  114,  116,  118,  120,  122,  124,  126,  128,  130,  132,  134,  136,  138,  140,  142,  144,  146,  148,  150,  152,  154,  156,  158,  160,  162,  164,  166,  168,  170,  172,  174,  176,  178,  180,  182,  184,  186,  188,  190,  192,  194,  196,  198,  200,  202,  204,  206,  208,  210,  212,  214,  216,  218,  220,  222,  224,  226,  228,  230,  232,  234,  236,  238,  240,  242,  244,  246,  248,  250,  252,  254,  256,  258,  260,  262,  264,  266,  268,  270,  272,  274,  276,  278,  280,  282,  284,  286,  288,  290,  292,  294,  296,  298,  300,  302,  304,  306,  308,  310,  312,  314,  316,  318,  320,  322,  324,  326,  328,  330,  332,  334,  336,  338,  340,  342,  344,  346,  348,  350,  352,  354,  356,  358,  360,  362,  364,  366,  368,  370,  372,  374,  376,  378,  380,  382,  384,  386,  388,  390,  392,  394,  396,  398,  400,  402,  404,  406,  408,  410,  412,  414,  416,  418,  420,  422,  424,  426,  428,  430,  432,  434,  436,  438,  440,  442,  444,  446,  448,  450,  452,  454,  456,  458,  460,  462,  464,  466,  468,  470,  472,  474,  476,  478,  480,  482,  484,  486,  488,  490,  492,  494,  496,  498,  500,  502,  504,  506,  508,  510,  512,  514,  516,  518,  520,  522,  524,  526,  528,  530,  532,  534,  536,  538,  540,  542,  544,  546,  548,  550,  552,  554,  556,  558,  560,  562,  564,  566,  568,  570,  572,  574,  576,  578,  580,  582,  584,  586,  588,  590,  592,  594,  596,  598,  600,  602,  604,  606,  608,  610,  612,  614,  616,  618,  620,  622,  624,  626,  628,  630,  632,  634,  636,  638,  640,  642,  644,  646,  648,  650,  652,  654,  656,  658,  660,  662,  664,  666,  668,  670,  672,  674,  676,  678,  680,  682,  684,  686,  688,  690,  692,  694,  696,  698,  700,  702,  704,  706,  708,  710,  712,  714,  716,  718,  720,  722,  724,  726,  728,  730,  732,  734,  736,  738,  740,  742,  744,  746,  748,  750,  752,  754,  756,  758,  760,  762,  764,  766,  768,  770,  772,  774,  776,  778,  780,  782,  784,  786,  788,  790,  792,  794,  796,  798,  800,  802,  804,  806,  808,  810,  812,  814,  816,  818,  820,  822,  824,  826,  828,  830,  832,  834,  836,  838,  840,  842,  844,  846,  848,  850,  852,  854,  856,  858,  860,  862,  864,  866,  868,  870,  872,  874,  876,  878,  880,  882,  884,  886,  888,  890,  892,  894,  896,  898,  900,  902,  904,  906,  908,  910,  912,  914,  916,  918,  920,  922,  924,  926,  928,  930,  932,  934,  936,  938,  940,  942,  944,  946,  948,  950,  952,  954,  956,  958,  960,  962,  964,  966,  968,  970,  972,  974,  976,  978,  980,  982,  984,  986,  988,  990,  992,  994,  996,  998,  1000, 998,  996,  994,  992,  990,  988,  986,  984,  982,  980,  978,  976,  974,  972,  970,  968,  966,  964,  962,  960,  958,  956,  954,  952,  950,  948,  946,  944,  942,  940,  938,  936,  934,  932,  930,  928,  926,  924,  922,  920,  918,  916,  914,  912,  910,  908,  906,  904,  902,  900,  898,  896,  894,  892,  890,  888,  886,  884,  882,  880,  878,  876,  874,  872,  870,  868,  866,  864,  862,  860,  858,  856,  854,  852,  850,  848,  846,  844,  842,  840,  838,  836,  834,  832,  830,  828,  826,  824,  822,  820,  818,  816,  814,  812,  810,  808,  806,  804,  802,  800,  798,  796,  794,  792,  790,  788,  786,  784,  782,  780,  778,  776,  774,  772,  770,  768,  766,  764,  762,  760,  758,  756,  754,  752,  750,  748,  746,  744,  742,  740,  738,  736,  734,  732,  730,  728,  726,  724,  722,  720,  718,  716,  714,  712,  710,  708,  706,  704,  702,  700,  698,  696,  694,  692,  690,  688,  686,  684,  682,  680,  678,  676,  674,  672,  670,  668,  666,  664,  662,  660,  658,  656,  654,  652,  650,  648,  646,  644,  642,  640,  638,  636,  634,  632,  630,  628,  626,  624,  622,  620,  618,  616,  614,  612,  610,  608,  606,  604,  602,  600,  598,  596,  594,  592,  590,  588,  586,  584,  582,  580,  578,  576,  574,  572,  570,  568,  566,  564,  562,  560,  558,  556,  554,  552,  550,  548,  546,  544,  542,  540,  538,  536,  534,  532,  530,  528,  526,  524,  522,  520,  518,  516,  514,  512,  510,  508,  506,  504,  502,  500,  498,  496,  494,  492,  490,  488,  486,  484,  482,  480,  478,  476,  474,  472,  470,  468,  466,  464,  462,  460,  458,  456,  454,  452,  450,  448,  446,  444,  442,  440,  438,  436,  434,  432,  430,  428,  426,  424,  422,  420,  418,  416,  414,  412,  410,  408,  406,  404,  402,  400,  398,  396,  394,  392,  390,  388,  386,  384,  382,  380,  378,  376,  374,  372,  370,  368,  366,  364,  362,  360,  358,  356,  354,  352,  350,  348,  346,  344,  342,  340,  338,  336,  334,  332,  330,  328,  326,  324,  322,  320,  318,  316,  314,  312,  310,  308,  306,  304,  302,  300,  298,  296,  294,  292,  290,  288,  286,  284,  282,  280,  278,  276,  274,  272,  270,  268,  266,  264,  262,  260,  258,  256,  254,  252,  250,  248,  246,  244,  242,  240,  238,  236,  234,  232,  230,  228,  226,  224,  222,  220,  218,  216,  214,  212,  210,  208,  206,  204,  202,  200,  198,  196,  194,  192,  190,  188,  186,  184,  182,  180,  178,  176,  174,  172,  170,  168,  166,  164,  162,  160,  158,  156,  154,  152,  150,  148,  146,  144,  142,  140,  138,  136,  134,  132,  130,  128,  126,  124,  122,  120,  118,  116,  114,  112,  110,  108,  106,  104,  102,  100,  98, 96, 94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8,  6,  4,  2
};
const static word squ[1000] PROGMEM = {
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000
};


void setup() {
 pinMode(PWMPin, OUTPUT);
 pinMode(3, INPUT);

 FlexiTimer2::set(5, 1.0 / 100000, timer_count); // 50usec/count
 FlexiTimer2::start();

 //for development
//  Serial.begin(9600);

}

void loop() {
 old_ext_pulse = ext_pulse;
 ext_pulse = digitalRead(3);

 //------------wave select-------------------------------
 if (analogRead(3) < 31) {
   mode = 6;//steady
 }
 else if (analogRead(3) >= 31 && analogRead(3) < 155) {
   mode = 0;//saw1
 }
 else if (analogRead(3) >= 155 && analogRead(3) < 352) {
   mode = 1;//saw2
 }
 else if (analogRead(3) >= 352 && analogRead(3) < 571) {
   mode = 2;//sin
 }
 else if (analogRead(3) >= 571 && analogRead(3) < 771) {
   mode = 3;//tri
 }
 else if (analogRead(3) >= 771 && analogRead(3) < 939) {
   mode = 4;//squ
 }
 else if (analogRead(3) >= 939) {
   mode = 5;//random
 }


 //------------phase and internal clock set-------------------------------
 if (ext_injudge == 0) { //use internal clock , phase function off
   phase = 0;
   //    freq_max = 1+(analogRead(1))/5;
   freq_max = 1 + 0.0007 * analogRead(1) * analogRead(1);
 }

 else if (ext_injudge == 1) { //use external clock , phase function on
   phase = map(analogRead(1), 0, 1023, 0, 999);
 }


 //------------selc modulation-------------------------------
 if (analogRead(5) < 31) {
   mod = 0;//no modulation
 }
 else if (analogRead(5) >= 31 && analogRead(5) < 155) {
   mod = 1;//saw1
 }
 else if (analogRead(5) >= 155 && analogRead(5) < 352) {
   mod = 2;//saw2
 }
 else if (analogRead(5) >= 352 && analogRead(5) < 571) {
   mod = 3;//sin
 }
 else if (analogRead(5) >= 571 && analogRead(5) < 771) {
   mod = 4;//tri
 }
 else if (analogRead(5) >= 771 && analogRead(5) < 939) {
   mod = 5;//squ
 }
 else if (analogRead(5) >= 939) {
   mod = 6;//random
 }

 switch (mod) {
   case 0:
     break;

   case 1:
     phase = phase + (pgm_read_word(&(saw1[count])));
     break;

   case 2:
     phase = phase + (pgm_read_word(&(saw2[count])));
     break;

   case 3:
     phase = phase + (pgm_read_word(&(sine[count])));
     break;

   case 4:
     phase = phase + (pgm_read_word(&(tri[count])));
     break;

   case 5:
     phase = phase + (pgm_read_word(&(squ[count])));
     break;

   case 6:
     phase = phase + (pgm_read_word(&(saw1[random(1, 1000)])));
     break;
 }

 //--------------amp set----------------
 amp = analogRead(0);
 amp = map(amp, 0, 1023, 1, 100);
 amp_rate = (float)amp / 100;


 //------------external in judge-------------------------------
 if ( ext_count > 160000 ) { //no external signal during 8 sec
   ext_injudge = 0;
 }
 else  {
   ext_injudge = 1;
 }


 //----------clock setting-------------

 if (ext_pulse == 1 && old_ext_pulse == 0) {
   old_ext_count_result = ext_count_result;//twice pulse average
   ext_count_result = ext_count;
   ext_period = (old_ext_count_result + ext_count_result) / 1960;
   freq_max = ext_period;
 }

 if ( old_ext_pulse == 0 && ext_pulse == 1  ) { //外部入力が有→無のとき
   ext_count = 0;
   count = 0;

 }
 // モード指定
 TCCR1A = 0b00100001;
 TCCR1B = 0b00010001;//分周比1

 // TOP値指定
 OCR1A = (unsigned int)(8000000 / frq);

 // Duty比指定
 OCR1B = (unsigned int)(8000000  / frq * duty * amp_rate);

 // for development
 //  Serial.print(ext_injudge);
 //  Serial.print(",");
 //  Serial.print(analogRead(5));
 //  Serial.println("");
}

void timer_count() {

 ext_count ++;
 set_freq ++;
 
 if (set_freq >= freq_max) {
   set_freq = 0;

   count ++;
   if (count + phase >= 1000 && mode != 5) {
     count = count - 1000;
   }

   switch (mode) {
     case 0:
       duty = (float)(pgm_read_word(&(saw1[count + phase]))) / 1000;
       break;

     case 1:
       duty = (float)(pgm_read_word(&(saw2[count + phase]))) / 1000;
       break;

     case 2:
       duty = (float)(pgm_read_word(&(sine[count + phase]))) / 1000;
       break;

     case 3:
       duty = (float)(pgm_read_word(&(tri[count + phase]))) / 1000;
       break;

     case 4:
       duty = (float)(pgm_read_word(&(squ[count + phase]))) / 1000;
       break;
   }

   //random
   if (mode == 5) {
     count ++;
     if (count >= 250) {
       count = 0;
       duty = random(1, 1000);
       duty = duty / 1000;
     }
   }

   //steady
   if (mode == 6) {
     duty = 1;
   }
 }
}