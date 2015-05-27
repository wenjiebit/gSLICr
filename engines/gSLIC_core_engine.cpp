#pragma once
#include "gSLIC_core_engine.h"
#include <fstream>

using namespace gSLIC;
using namespace std;

gSLIC::engines::core_engine::core_engine(const objects::settings& in_settings)
{
	slic_seg_engine = new seg_engine_GPU(in_settings);
	sdkCreateTimer(&slic_timer);
}

gSLIC::engines::core_engine::~core_engine()
{
		delete slic_seg_engine;
		delete slic_timer;
}

void gSLIC::engines::core_engine::Process_Frame(UChar4Image* in_img)
{
	sdkResetTimer(&slic_timer); sdkStartTimer(&slic_timer);
	slic_seg_engine->Perform_Segmentation(in_img);
	sdkStopTimer(&slic_timer); printf("\rSegmentation in:[%.2f]ms ", sdkGetTimerValue(&slic_timer));
}

const IntImage * gSLIC::engines::core_engine::Get_Seg_Res()
{
	return slic_seg_engine->Get_Seg_Mask();
}

void gSLIC::engines::core_engine::Draw_Segmentation_Result(UChar4Image* out_img)
{
	slic_seg_engine->Draw_Segmentation_Result(out_img);
}

void gSLIC::engines::core_engine::Write_Seg_Res_To_PGM(const char* fileName)
{
	const IntImage* idx_img = slic_seg_engine->Get_Seg_Mask();
	int width = idx_img->noDims.x;
	int height = idx_img->noDims.y;
	const int* data_ptr = idx_img->GetData(MEMORYDEVICE_CPU);

	ofstream f(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	f << "P5\n" << width << " " << height << "\n65535\n";
	for (int i = 0; i < height * width; ++i)
	{
		ushort lable = (ushort)data_ptr[i];
		ushort lable_buffer = (lable << 8 | lable >> 8);
		f.write((const char*)&lable_buffer, sizeof(ushort));
	}
	f.close();
}

