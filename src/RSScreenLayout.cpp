#include "RenderScene.h"
#include <iostream>

ScreenLayout::ScreenLayout()
{
	m_screenArray[0].m_pos 		= Position(-1.f    , 0.f, 0.f); 
	m_screenArray[0].m_size 	= size(0.25f, 0.25f);
	m_screenArray[0].m_colour 	= colour(1.f, 0.f, 0.f, 1.f);
}
	
Position 	ScreenLayout::GetScreenPos		( unsigned int _index)
{

	return m_screenArray[_index].m_pos;
}

size     	ScreenLayout::GetScreenSize		( unsigned int _index)
{
	return m_screenArray[_index].m_size;
}

void 	ScreenLayout::SetScreenPos		( unsigned int _index, Position& _pos)
{
	 m_screenArray[_index].m_pos = _pos;
}

void     	ScreenLayout::SetScreenSize		( unsigned int _index, size& _size)
{
	m_screenArray[_index].m_size = _size;
}


void 		ScreenLayout::AdjustScreenPos	( Position _offset, unsigned int _index)
{
	m_screenArray[_index].m_pos.pos[0] += _offset.pos[0];
	m_screenArray[_index].m_pos.pos[1] += _offset.pos[1];
	m_screenArray[_index].m_pos.pos[2] += _offset.pos[2];
}

void 		ScreenLayout::AdjustScreenSize	( size 	_offset, unsigned int _index)
{
	//std::cerr << "Adjusting size by: " << _offset.w << "-" << _offset.h << "\n";
	m_screenArray[_index].m_size.w += _offset.w;
	m_screenArray[_index].m_size.h += _offset.h;
	//std::cerr << "New size : " << m_screenArray[_index].m_size.w << "-" << m_screenArray[_index].m_size.h << "\n";
}
