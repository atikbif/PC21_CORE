/*
 * modbus_master.h
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: User
 */

#ifndef MODBUS_MASTER_H_
#define MODBUS_MASTER_H_

#include <stdint.h>

#define MODB1 mmb[0]
#define MODB2 mmb[1]
#define MODB3 mmb[2]
#define MODB4 mmb[3]
#define MODB5 mmb[4]
#define MODB6 mmb[5]
#define MODB7 mmb[6]
#define MODB8 mmb[7]
#define MODB9 mmb[8]
#define MODB10 mmb[9]
#define MODB11 mmb[10]
#define MODB12 mmb[11]
#define MODB13 mmb[12]
#define MODB14 mmb[13]
#define MODB15 mmb[14]
#define MODB16 mmb[15]
#define MODB17 mmb[16]
#define MODB18 mmb[17]
#define MODB19 mmb[18]
#define MODB20 mmb[19]
#define MODB21 mmb[20]
#define MODB22 mmb[21]
#define MODB23 mmb[22]
#define MODB24 mmb[23]
#define MODB25 mmb[24]
#define MODB26 mmb[25]
#define MODB27 mmb[26]
#define MODB28 mmb[27]
#define MODB29 mmb[28]
#define MODB30 mmb[29]
#define MODB31 mmb[30]
#define MODB32 mmb[31]
#define MODB33 mmb[32]
#define MODB34 mmb[33]
#define MODB35 mmb[34]
#define MODB36 mmb[35]
#define MODB37 mmb[36]
#define MODB38 mmb[37]
#define MODB39 mmb[38]
#define MODB40 mmb[39]
#define MODB41 mmb[40]
#define MODB42 mmb[41]
#define MODB43 mmb[42]
#define MODB44 mmb[43]
#define MODB45 mmb[44]
#define MODB46 mmb[45]
#define MODB47 mmb[46]
#define MODB48 mmb[47]
#define MODB49 mmb[48]
#define MODB50 mmb[49]
#define MODB51 mmb[50]
#define MODB52 mmb[51]
#define MODB53 mmb[52]
#define MODB54 mmb[53]
#define MODB55 mmb[54]
#define MODB56 mmb[55]
#define MODB57 mmb[56]
#define MODB58 mmb[57]
#define MODB59 mmb[58]
#define MODB60 mmb[59]
#define MODB61 mmb[60]
#define MODB62 mmb[61]
#define MODB63 mmb[62]
#define MODB64 mmb[63]
#define MODB65 mmb[64]
#define MODB66 mmb[65]
#define MODB67 mmb[66]
#define MODB68 mmb[67]
#define MODB69 mmb[68]
#define MODB70 mmb[69]
#define MODB71 mmb[70]
#define MODB72 mmb[71]
#define MODB73 mmb[72]
#define MODB74 mmb[73]
#define MODB75 mmb[74]
#define MODB76 mmb[75]
#define MODB77 mmb[76]
#define MODB78 mmb[77]
#define MODB79 mmb[78]
#define MODB80 mmb[79]
#define MODB81 mmb[80]
#define MODB82 mmb[81]
#define MODB83 mmb[82]
#define MODB84 mmb[83]
#define MODB85 mmb[84]
#define MODB86 mmb[85]
#define MODB87 mmb[86]
#define MODB88 mmb[87]
#define MODB89 mmb[88]
#define MODB90 mmb[89]
#define MODB91 mmb[90]
#define MODB92 mmb[91]
#define MODB93 mmb[92]
#define MODB94 mmb[93]
#define MODB95 mmb[94]
#define MODB96 mmb[95]
#define MODB97 mmb[96]
#define MODB98 mmb[97]
#define MODB99 mmb[98]
#define MODB100 mmb[99]
#define MODB101 mmb[100]
#define MODB102 mmb[101]
#define MODB103 mmb[102]
#define MODB104 mmb[103]
#define MODB105 mmb[104]
#define MODB106 mmb[105]
#define MODB107 mmb[106]
#define MODB108 mmb[107]
#define MODB109 mmb[108]
#define MODB110 mmb[109]
#define MODB111 mmb[110]
#define MODB112 mmb[111]
#define MODB113 mmb[112]
#define MODB114 mmb[113]
#define MODB115 mmb[114]
#define MODB116 mmb[115]
#define MODB117 mmb[116]
#define MODB118 mmb[117]
#define MODB119 mmb[118]
#define MODB120 mmb[119]
#define MODB121 mmb[120]
#define MODB122 mmb[121]
#define MODB123 mmb[122]
#define MODB124 mmb[123]
#define MODB125 mmb[124]
#define MODB126 mmb[125]
#define MODB127 mmb[126]
#define MODB128 mmb[127]


typedef struct {
	uint16_t start_byte_num;
	int8_t bit_offset;
	uint16_t *ptr;
}mvar;

typedef struct {
	const char *request;
	uint16_t req_length;
	uint16_t answer_length;
	const mvar *vars_ptr;
	uint16_t var_cnt;
	uint8_t wr_flag;
}mvar_reqs;

void modbus_master_process_canal1();
void modbus_master_process_canal2();

#endif /* MODBUS_MASTER_H_ */
