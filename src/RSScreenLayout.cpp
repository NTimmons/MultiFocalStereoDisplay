#include "RenderScene.h"
#include <iostream>


ScreenLayout::ScreenLayout()
{

	m_screenArray[0].m_pos = Position(-1.f    , 0.f, 0.f); 
	m_screenArray[1].m_pos = Position(-0.5f   , 0.f, 0.f);
	m_screenArray[2].m_pos = Position(0.0f   , 0.f, 0.f);
	m_screenArray[3].m_pos = Position(0.5f   , 0.f, 0.f);

	m_screenArray[0].m_size = size(0.25f, 0.25f);
	m_screenArray[1].m_size = size(0.25f, 0.25f);
	m_screenArray[2].m_size = size(0.25f, 0.25f);
	m_screenArray[3].m_size = size(0.25f, 0.25f);

	m_screenArray[0].m_colour = colour(1.f, 0.f, 0.f, 1.f);
	m_screenArray[1].m_colour = colour(0.f, 1.f, 0.f, 1.f);
	m_screenArray[2].m_colour = colour(0.f, 0.f, 1.f, 1.f);
	m_screenArray[3].m_colour = colour(1.f, 1.f, 0.f, 1.f);

}
	
Position 	ScreenLayout::GetScreenPos		( unsigned int _index)
{
	return m_screenArray[_index].m_pos;
}

size     	ScreenLayout::GetScreenSize		( unsigned int _index)
{
	return m_screenArray[_index].m_size;
}

void 		ScreenLayout::AdjustScreenPos	( Position _offset, unsigned int _index)
{
	m_screenArray[_index].m_pos.pos[0] += _offset.pos[0];
	m_screenArray[_index].m_pos.pos[1] += _offset.pos[1];
	m_screenArray[_index].m_pos.pos[2] += _offset.pos[2];
}

void 		ScreenLayout::AdjustScreenSize	( size 	_offset, unsigned int _index)
{
	m_screenArray[_index].m_size.w += _offset.w;
	m_screenArray[_index].m_size.h += _offset.h;
}