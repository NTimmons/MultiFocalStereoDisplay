#ifndef _INCL_SCREENLAYOUT
#define _INCL_SCREENLAYOUT

struct SubScreen
{
	SubScreen(){}
	SubScreen(Position _pos, size _size, colour _colour):m_pos(_pos), m_size(_size), m_colour(_colour){}

	Position m_pos;
	size 	 m_size;
	colour   m_colour;
};

class ScreenLayout
{
	public:
	ScreenLayout();
	
	Position 	GetScreenPos					( unsigned int _index);
	size     	GetScreenSize					( unsigned int _index);

	void 		AdjustScreenPos					( Position 	_offset, unsigned int _index);
	void 		AdjustScreenSize				( size 		_offset, unsigned int _index);

	void 		SetScreenPos		( unsigned int _index, Position& _pos);
	void     	SetScreenSize		( unsigned int _index, size& _size);

	private:
	SubScreen	m_screenArray[1];

};

#endif