#include "hdmi_core.h"

__s32 is_exp = 0;
__u32 rgb_only = 0;
__u16 edid_id = 0xdead;
__u8 Device_Support_VIC[512];

static __s32 is_hdmi;
static __s32 is_yuv;
static __u8	EDID_Buf[HDMI_EDID_LEN];

static __u8 EDID_Default[HDMI_EDID_LEN] = 
#if 0
{
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x31, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x16, 0x01, 0x03, 0x6d, 0x23, 0x1a, 0x78, 0xea, 0x5e, 0xc0, 0xa4, 0x59, 0x4a, 0x98, 0x25,
	0x20, 0x50, 0x54, 0x00, 0x08, 0x00, 0x61, 0x40, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x64, 0x19, 0x00, 0x40, 0x41, 0x00, 0x26, 0x30, 0x08, 0x90,
	0x36, 0x00, 0x63, 0x0a, 0x11, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xff, 0x00, 0x4c, 0x69, 0x6e,
	0x75, 0x78, 0x20, 0x23, 0x30, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x3b,
	0x3d, 0x2f, 0x31, 0x07, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfc,
	0x00, 0x4c, 0x69, 0x6e, 0x75, 0x78, 0x20, 0x58, 0x47, 0x41, 0x0a, 0x20, 0x20, 0x20, 0x00, 0x55,
};
#else
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x24, 0x6B, 0x50, 0x19, 0x01, 0x01, 0x01, 0x01, 
	0x10, 0x1A, 0x01, 0x03, 0x00, 0x2C, 0x17, 0x78, 0x2A, 0xC9, 0x05, 0xA3, 0x57, 0x4B, 0x9C, 0x25, 
	0x12, 0x50, 0x54, 0xBF, 0xEF, 0x00, 0x45, 0x4A, 0x95, 0x00, 0x81, 0x80, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x30, 0x2A, 0x40, 0xC8, 0x60, 0x84, 0x64, 0x30, 0x18, 0x50, 
	0x13, 0x00, 0xB0, 0xF0, 0x10, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x35, 0x30, 0x35, 
	0x33, 0x44, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x31, 0x0A, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x38, 
	0x4B, 0x1F, 0x53, 0x0E, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 
	0x00, 0x56, 0x47, 0x41, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x33,
};
#endif

static __u8 exp0[16] =
{
	0x36,0x74,0x4d,0x53,0x74,0x61,0x72,0x20,0x44,0x65,0x6d,0x6f,0x0a,0x20,0x20,0x38
};

static __u8 exp1[16] =
{
	0x2d,0xee,0x4b,0x4f,0x4e,0x41,0x4b,0x20,0x54,0x56,0x0a,0x20,0x20,0x20,0x20,0xa5
};

static void DDC_Init(void)
{
	#if 1
	printk("@@-ken-@@: DDC_Init set default edid\n");
	memcpy(EDID_Buf, EDID_Default, sizeof(EDID_Default));
	#else
	memset(EDID_Buf, 0, sizeof(EDID_Buf));
	#endif
}

/*
void send_ini_sequence()
{
    int i,j;
    set_wbit(HDMI_BASE + 0x524,BIT3);
    for(i=0;i<9;i++)
    {
       for(j=0;j<200;j++);		//for simulation, delete it
       clr_wbit(HDMI_BASE + 0x524,BIT2);

       for(j=0;j<200;j++);		//for simulation, delete it
       set_wbit(HDMI_BASE + 0x524,BIT2);

    }
    clr_wbit(HDMI_BASE + 0x524,BIT3);
    clr_wbit(HDMI_BASE + 0x524,BIT1);

    return;

}*/
__s32 DDC_Read(char cmd,char pointer,char offset,int nbyte,char * pbuf)
{
	__inf("DDC_Read\n");
	if(bsp_hdmi_ddc_read(cmd, pointer, offset, 128, pbuf) < 0) {
		__inf("DDC_Read failed, use default ...\n");
		DDC_Init();
	}
	return 0;
}



static void GetEDIDData(__u8 block,__u8 *buf)
{
	__u8 i;
	__u8 * pbuf = buf + 128 * block;
	__u8 offset = (block & 0x01) ? 128 : 0;

	DDC_Read(Explicit_Offset_Address_E_DDC_Read, block>>1, offset, 128, pbuf);

	////////////////////////////////////////////////////////////////////////////
	__inf("Sink : EDID bank %d:\n",block);

	__inf(" 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n");
	__inf(" ===============================================================================================\n");

	for (i = 0; i < 8; i++) {
		__inf(" %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x  %2.2x\n",
			pbuf[i*16 + 0 ],pbuf[i*16 + 1 ],pbuf[i*16 + 2 ],pbuf[i*16 + 3 ],
			pbuf[i*16 + 4 ],pbuf[i*16 + 5 ],pbuf[i*16 + 6 ],pbuf[i*16 + 7 ],
			pbuf[i*16 + 8 ],pbuf[i*16 + 9 ],pbuf[i*16 + 10],pbuf[i*16 + 11],
			pbuf[i*16 + 12],pbuf[i*16 + 13],pbuf[i*16 + 14],pbuf[i*16 + 15]
		);
	}
	__inf(" ===============================================================================================\n");

	return;
}

/////////////////////////////////////////////////////////////////////
// ParseEDID()
// Check EDID check sum and EDID 1.3 extended segment.
/////////////////////////////////////////////////////////////////////
static __s32 EDID_CheckSum(__u8 block,__u8 *buf)
{
	__s32 i = 0, CheckSum = 0;
	__u8 *pbuf = buf + 128*block;

	for( i = 0, CheckSum = 0 ; i < 128 ; i++ ) {
		CheckSum += pbuf[i] ;
		CheckSum &= 0xFF ;
	}
	if( CheckSum != 0 ) {
		pr_err("EDID block %d checksum error\n",block);
		return -1 ;
	}

	return 0;
}
static __s32 EDID_Header_Check(__u8 *pbuf)
{
	if( pbuf[0] != 0x00 ||
		pbuf[1] != 0xFF ||
		pbuf[2] != 0xFF ||
		pbuf[3] != 0xFF ||
		pbuf[4] != 0xFF ||
		pbuf[5] != 0xFF ||
		pbuf[6] != 0xFF ||
		pbuf[7] != 0x00)
	{
		__inf("EDID block0 header error\n");
		return -1 ;
	}

	return 0;
}

static __s32 EDID_Version_Check(__u8 *pbuf)
{
	__inf("EDID version: %d.%d ",pbuf[0x12],pbuf[0x13]) ;
	if( (pbuf[0x12]!= 0x01) || (pbuf[0x13]!=0x03)) {
		__inf("Unsupport EDID format,EDID parsing exit\n");
		return -1;
	}

	return 0;
}

static __s32 Parse_DTD_Block(__u8 *pbuf)
{
	__u32 	pclk,sizex,Hblanking,sizey,Vblanking,/*Hsync_offset,Hsync_plus,
	Vsync_offset,Vsync_plus,H_image_size,V_image_size,H_Border,
	V_Border,*/pixels_total,frame_rate;
	pclk 		= ( (__u32)pbuf[1]	<< 8) + pbuf[0];
	sizex 		= (((__u32)pbuf[4] 	<< 4) & 0x0f00) + pbuf[2];
	Hblanking 	= (((__u32)pbuf[4] 	<< 8) & 0x0f00) + pbuf[3];
	sizey 		= (((__u32)pbuf[7] 	<< 4) & 0x0f00) + pbuf[5];
	Vblanking 	= (((__u32)pbuf[7] 	<< 8) & 0x0f00) + pbuf[6];
//	Hsync_offset= (((__u32)pbuf[11] << 2) & 0x0300) + pbuf[8];
//	Hsync_plus 	= (((__u32)pbuf[11] << 4) & 0x0300) + pbuf[9];
//	Vsync_offset= (((__u32)pbuf[11] << 2) & 0x0030) + (pbuf[10] >> 4);
//	Vsync_plus 	= (((__u32)pbuf[11] << 4) & 0x0030) + (pbuf[8] & 0x0f);
//	H_image_size= (((__u32)pbuf[14] << 4) & 0x0f00) + pbuf[12];
//	V_image_size= (((__u32)pbuf[14] << 8) & 0x0f00) + pbuf[13];
//	H_Border 	=  pbuf[15];
//	V_Border 	=  pbuf[16];

	pixels_total = (sizex + Hblanking) * (sizey + Vblanking);

	if( (pbuf[0] == 0) && (pbuf[1] == 0) && (pbuf[2] == 0)) {
		return 0;
	}

	if(pixels_total == 0) {
		return 0;
	} else {
		frame_rate = (pclk * 10000) /pixels_total;
	}

	if ((frame_rate == 59) || (frame_rate == 60))	{
		if ((sizex== 720) && (sizey == 240)) {
			Device_Support_VIC[HDMI1440_480I] = 1;
		}
		if ((sizex== 720) && (sizey == 480)) {
			//Device_Support_VIC[HDMI480P] = 1;
		}
 		if ((sizex== 800) && (sizey == 480)) {
 			Device_Support_VIC[HDMI800_480P] = 1;
		}
		if ((sizex== 1024) && (sizey == 768)) {
			Device_Support_VIC[HDMI1024_768P] = 1;
		}
		if ((sizex== 1280) && (sizey == 1024)) {
			Device_Support_VIC[HDMI1280_1024P] = 1;
		}
		if ((sizex== 1280) && (sizey == 768)) {
			Device_Support_VIC[HDMI1280_768P] = 1;
		}
		if ((sizex== 1280) && (sizey == 800)) {
			Device_Support_VIC[HDMI1280_800P] = 1;
		}
		if ((sizex== 1360) && (sizey == 768)) {
			Device_Support_VIC[HDMI1360_768P] = 1;
		}
		if ((sizex== 1366) && (sizey == 768)) {
			Device_Support_VIC[HDMI1366_768P] = 1;
		}
		if ((sizex== 1440) && (sizey == 900)) {
			Device_Support_VIC[HDMI1440_900P] = 1;
		}
		if ((sizex== 1680) && (sizey == 1050)) {
			Device_Support_VIC[HDMI1680_1050P] = 1;
 		}
		if ((sizex== 1600) && (sizey == 900)) {
			Device_Support_VIC[HDMI1600_900P] = 1;
 		}
		if ((sizex== 2048) && (sizey == 1536)) {
			Device_Support_VIC[HDMI2048_1536P] = 1;
		}
		if ((sizex== 1280) && (sizey == 720)) {
			Device_Support_VIC[HDMI720P_60] = 1;
		}
		if ((sizex== 1920) && (sizey == 540)) {
			Device_Support_VIC[HDMI1080I_60] = 1;
		}
		if ((sizex== 1920) && (sizey == 1080)) {
			Device_Support_VIC[HDMI1080P_60] = 1;
		}
	}
	else if ((frame_rate == 49) || (frame_rate == 50)) {
		if ((sizex== 720) && (sizey == 288)) {
			Device_Support_VIC[HDMI1440_576I] = 1;
		}
		if ((sizex== 720) && (sizey == 576)) {
			Device_Support_VIC[HDMI576P] = 1;
		}
 		if ((sizex== 800) && (sizey == 480)) {
 			Device_Support_VIC[HDMI800_480P] = 1;
		}
		if ((sizex== 1024) && (sizey == 768)) {
			Device_Support_VIC[HDMI1024_768P] = 1;
		}
		if ((sizex== 1280) && (sizey == 1024)) {
			Device_Support_VIC[HDMI1280_1024P] = 1;
		}
		if ((sizex== 1366) && (sizey == 768)) {
			Device_Support_VIC[HDMI1366_768P] = 1;
		}
		if ((sizex== 1440) && (sizey == 900)) {
			Device_Support_VIC[HDMI1440_900P] = 1;
		}
		if ((sizex== 1680) && (sizey == 1050)) {
			Device_Support_VIC[HDMI1680_1050P] = 1;
 		}
		if ((sizex== 1600) && (sizey == 900)) {
			Device_Support_VIC[HDMI1600_900P] = 1;
 		}
		if ((sizex== 2048) && (sizey == 1536)) {
			Device_Support_VIC[HDMI2048_1536P] = 1;
		}
		if ((sizex== 1280) && (sizey == 720)) {
			Device_Support_VIC[HDMI720P_50] = 1;
		}
		if ((sizex== 1920) && (sizey == 540)) {
			Device_Support_VIC[HDMI1080I_50] = 1;
		}
		if ((sizex== 1920) && (sizey == 1080)) {
			Device_Support_VIC[HDMI1080P_50] = 1;
		}
	}
	else if ((frame_rate == 23) || (frame_rate == 24)) {
		if ((sizex== 1920) && (sizey == 1080)) {
			Device_Support_VIC[HDMI1080P_24] = 1;
		}
	}
	__inf("offset: 0x%x - PCLK=%d\tXsize=%d\tYsize=%d\tFrame_rate=%d\n",
				(pbuf - EDID_Buf),pclk*10000,sizex,sizey,frame_rate);

	return 0;
}

static __s32 Parse_VideoData_Block(__u8 *pbuf,__u8 size)
{
	int i=0;
	while(i<size) {
		Device_Support_VIC[pbuf[i] &0x7f] = 1;
		if(pbuf[i] &0x80)	{
			__inf("Parse_VideoData_Block: VIC %d(native) support\n", pbuf[i]&0x7f);
		}
		else {
			__inf("Parse_VideoData_Block: VIC %d support\n", pbuf[i]);
		}
		i++;
	}

	return 0;
}

static __s32 Parse_AudioData_Block(__u8 *pbuf,__u8 size)
{
	__u8 sum = 0;

	while(sum < size) {
		if( (pbuf[sum]&0xf8) == 0x08) {
			__inf("Parse_AudioData_Block: max channel=%d\n",(pbuf[sum]&0x7)+1);
			__inf("Parse_AudioData_Block: SampleRate code=%x\n",pbuf[sum+1]);
			__inf("Parse_AudioData_Block: WordLen code=%x\n",pbuf[sum+2]);
		}
		sum += 3;
	}
	return 0;
}

static __s32 Parse_HDMI_VSDB(__u8 * pbuf,__u8 size)
{
	__u8 index = 8;
	__u8 vic_len = 0;
	__u8 i;

	/* check if it's HDMI VSDB */
	if((pbuf[0] ==0x03) &&	(pbuf[1] ==0x0c) &&	(pbuf[2] ==0x00)) {
		is_hdmi = 1;
		__inf("Find HDMI Vendor Specific DataBlock\n");
	} else {
		return 0;
	}

	if(size <=8)
		return 0;

	if((pbuf[7]&0x20) == 0 )
		return 0;
	if((pbuf[7]&0x40) == 0x40 )
		index = index +2;
	if((pbuf[7]&0x80) == 0x80 )
		index = index +2;

	/* mandatary format support */
	if(pbuf[index]&0x80) {
		Device_Support_VIC[HDMI1080P_24_3D_FP] = 1;
		Device_Support_VIC[HDMI720P_50_3D_FP] = 1;
		Device_Support_VIC[HDMI720P_60_3D_FP] = 1;
		__inf("3D_present\n");
	} else {
		return 0;
	}

	if( ((pbuf[index]&0x60) ==1) || ((pbuf[index]&0x60) ==2) )
		__inf("3D_multi_present\n");

	vic_len = pbuf[index+1]>>5;
	for(i=0; i<vic_len; i++) {
		/* HDMI_VIC for extended resolution transmission */
		Device_Support_VIC[pbuf[index+1+1+i] + 0x100] = 1;
		__inf("Parse_HDMI_VSDB: VIC %d support\n", pbuf[index+1+1+i]);
	}

	index += (pbuf[index+1]&0xe0) + 2;
	if(index > (size+1) )
	    return 0;

	__inf("3D_multi_present byte(%2.2x,%2.2x)\n",pbuf[index],pbuf[index+1]);

	return 0;
}

static __s32 Check_EDID(__u8 *buf_src, __u8*buf_dst)
{
	__u32 i;
	
	for(i = 0; i < 2; i++)
	{
		if(buf_dst[i] != buf_src[8+i])
			return -1;
	}
	for(i = 0; i < 13; i++)
	{
		if(buf_dst[2+i] != buf_src[0x5f+i])
			return -1;
	}
	if(buf_dst[15] != buf_src[0x7f])
		return -1;
	
	return 0;
}

#include <linux/edid.h>

__s32 ParseEDID(void)
{
	//collect the EDID ucdata of segment 0
	__u8 BlockCount ;
	__u32 i,offset ;

	__inf("\n **** Parse-EDID ****\n");

	memset(Device_Support_VIC,0,sizeof(Device_Support_VIC));
	is_hdmi = 0;
	is_yuv = 0;
	is_exp = 0;
	DDC_Init();

	GetEDIDData(0, EDID_Buf);

	if( EDID_CheckSum(0, EDID_Buf) != 0)
		goto __err;

	if( EDID_Header_Check(EDID_Buf)!= 0)
		goto __err;

	if( EDID_Version_Check(EDID_Buf)!= 0)
		goto __err;

	Parse_DTD_Block(EDID_Buf + 0x36);

	Parse_DTD_Block(EDID_Buf + 0x48);

	BlockCount = EDID_Buf[0x7E];

	if((Check_EDID(EDID_Buf,exp0) == 0)||
		(Check_EDID(EDID_Buf,exp1) == 0))
	{
		printk("*****************is_exp*****************\n");
		is_exp = 1;
	}

	if( BlockCount > 0 ) {
		if ( BlockCount > 4 )
			BlockCount = 4 ;

		for( i = 1 ; i <= BlockCount ; i++ ) {
			GetEDIDData(i, EDID_Buf) ;
			if( EDID_CheckSum(i, EDID_Buf)!= 0)
				goto __err;

			if((EDID_Buf[0x80*i+0]==2)/*&&(EDID_Buf[0x80*i+1]==1)*/)
			{
				if( (EDID_Buf[0x80*i+1]>=1)) {
					if(EDID_Buf[0x80*i+3]&0x20)
					{
						is_yuv = 1;
						__inf("device support YCbCr44 output\n");
						if(rgb_only == 1) {
							__inf("rgb only test!\n");
							is_yuv = 0;
						}
					}
				}

				offset = EDID_Buf[0x80*i+2];
				/* deal with reserved data block */
				if(offset > 4) {
					__u8 bsum = 4;
					while(bsum < offset)
					{
						__u8 tag = EDID_Buf[0x80*i+bsum]>>5;
						__u8 len = EDID_Buf[0x80*i+bsum]&0x1f;
						if( (len >0) && ((bsum + len + 1) > offset) ) {
							__inf("len or bsum size error\n");
							goto __err;
						} else {
							if( tag == 1) {
								/* ADB */
								Parse_AudioData_Block(EDID_Buf+0x80*i+bsum+1,len);
							}	else if( tag == 2) {
								/* VDB */
								Parse_VideoData_Block(EDID_Buf+0x80*i+bsum+1,len);
							}	else if( tag == 3) {
								/* vendor specific */
								Parse_HDMI_VSDB(EDID_Buf+0x80*i+bsum+1,len);
							}
						}

						bsum += (len +1);
					}
				} else {
					__inf("no data in reserved block%d\n",i);
				}

				/* deal with 18-byte timing block */
				if(offset >= 4)	{
					while(offset < (0x80-18)) {
						Parse_DTD_Block(EDID_Buf + 0x80*i + offset);
						offset += 18;
					}
				} else {
					__inf("no datail timing in block%d\n",i);
				}
			}
		}
	}

	printk("\n********************* DUMP EDID *******************\n");
	edid_print_info((void*)EDID_Buf);
	if (!edid_check_info((void*)EDID_Buf)) {
		struct edid1_info * info = (void*)EDID_Buf;
		edid_id = EDID1_INFO_PRODUCT_CODE(*info);
	} else {
		printk("Error: dump edid checksum error.\n");
	}
	printk("\n***************************************************\n");
	return 0 ;

__err:
	DDC_Init();
	printk("\n********************* DUMP Default EDID *******************\n");
	edid_print_info((void*)EDID_Buf);
	printk("\n***************************************************\n");
	return 0;
}

__u32 GetIsHdmi(void)
{
	return is_hdmi;
}

__u32 GetIsYUV(void)
{
	return is_yuv;
}

__s32 GetEdidInfo(void)
{
	return (__s32)EDID_Buf;
}

