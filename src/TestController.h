#ifndef _INCL_TEST_CONTR
#define _INCL_TEST_CONTR

#include <vector>

#include <iostream>

enum ContrMode
{
	eMode_TIMER,
	eMode_INPUT,
	eMode_INPUT_DEPTH,
	eMode_CALIBRATE
};


struct ContrEntry
{
	void InitDistanceInput( int _renderModeA, float _a, float _b, float _c, int _answer)
	{
		inputMode = eMode_INPUT_DEPTH;

		renderModeA = _renderModeA;
		blendModeA 	= 5;
		projModeA 	= 3;

		timeMs 		= 0.f;
		startTime 	= 0.f;

		renderModeB = 0;
		blendModeB 	= 0.f;
		projModeB 	= 0;

		depthA = _a;
		depthB = _b;
		depthC = _c;
		answer = _answer;
	}

	void InitTimer( int _renderModeA, int _blendModeA, int _projModeA, float _timeMS)
	{
		inputMode = eMode_TIMER;

		renderModeA = _renderModeA;
		blendModeA 	= _blendModeA;
		projModeA 	= _projModeA;

		timeMs 		= _timeMS;
		startTime 	= 0.f;

		renderModeB = 0;
		blendModeB 	= 0.f;
		projModeB 	= 0;

		depthA = 0.f;
		depthB = 0.f;
		depthC = 0.f;
	}

	void InitCalibration()
	{
		inputMode = eMode_CALIBRATE;

		renderModeA = 4;
		blendModeA 	= 0;
		projModeA 	= 0;

		timeMs 		= 0;
		startTime 	= 0.f;
		renderModeB = 0;
		blendModeB 	= 0.f;
		projModeB 	= 0;

		depthA = 0.f;
		depthB = 0.f;
		depthC = 0.f;
	}

	void InitInput( int _renderModeA, int _blendModeA, int _projModeA, int _renderModeB, int _blendModeB, int _projModeB)
	{
		inputMode 	= eMode_INPUT;

		renderModeA = _renderModeA;
		blendModeA 	= _blendModeA;
		projModeA 	= _projModeA;

		timeMs 		= 0.f;
		startTime 	= 0.f;

		renderModeB = _renderModeB;
		blendModeB 	= _blendModeB;
		projModeB 	= _projModeB;

		depthA = 0.f;
		depthB = 0.f;
		depthC = 0.f;
	}



	int 		renderModeA;
	float 		blendModeA;
	int 		projModeA;

	int 		renderModeB;
	float 		blendModeB;
	int 		projModeB;

	ContrMode 	inputMode;

	float 		timeMs;
	float 		startTime;

	float		depthA;
	float		depthB;
	float		depthC;
	int 		answer;
};



class TestController
{
	public:
	TestController() : m_started(false)
	{}

	void Start()
	{
		m_started = true;
		m_active = m_WorkStack[0];
		OutputCommand();
	}

	void Add( ContrEntry& _entry )
	{
		m_WorkStack.push_back(_entry);
	}	


	void Update(float _time, int _input)
	{
		if(!m_started)
		{
			return;
		}

		//Waiting for input to move on.
		if(m_active.inputMode == eMode_CALIBRATE)
		{

			if(_input == 2)
			{
				std::cerr << "Recieved input, ending calibration\n";
				Pop(_time);
			}
			return;
		}
		//Waiting for positive or negative result
		else if( m_active.inputMode == eMode_INPUT)
		{
			if(_input == 1 || _input == 0)
			{
				std::cerr << "Recieved input\n";
				m_results.push_back( (_input == 1 ? true : false) );

				std::ofstream output;
				output.open("output.csv", std::ios::out | std::ios::app);
				output << "Somename," << 	m_active.depthA 	<< "," << m_active.depthB 		<< "," << m_active.depthC << "," << m_active.answer << "," <<	
											m_active.renderModeA<< "," << m_active.renderModeB 	<< "," <<
											m_active.blendModeA << "," << m_active.blendModeB 	<< "," <<
											m_active.projModeA 	<< "," << m_active.projModeB 	<< "," << (_input == 1 ? true : false) << "\n";
				output.close();
			
				Pop(_time);
				return;
			}
			else
			{
				return;
			}

		}
		//Waiting for positive or negative result
		else if( m_active.inputMode == eMode_INPUT_DEPTH)
		{
			if(_input == 1 || _input == 0 || _input == 2)
			{
				std::cerr << "Recieved input\n";
				m_results.push_back( (_input == m_active.answer ? true : false) );

				std::ofstream output;
				output.open("output.csv", std::ios::out | std::ios::app);
				output << "Somename," << 	m_active.depthA 	<< "," << m_active.depthB 		<< "," << m_active.depthC << "," << m_active.answer << "," << 	
											m_active.renderModeA<< "," << m_active.renderModeB 	<< "," <<
											m_active.blendModeA << "," << m_active.blendModeB 	<< "," <<
											m_active.projModeA 	<< "," << m_active.projModeB 	<< "," << (_input == m_active.answer ? true : false) << "\n";
				output.close();
			
				Pop(_time);
				return;
			}
			else
			{
				return;
			}

		}
		else if(m_active.inputMode == eMode_TIMER)
		{
			if(_time - m_active.startTime > m_active.timeMs)
			{
				std::cerr << "End of timer\n";
				Pop(_time);
			}

			return;
		}
	}

	void OutputCommand()
	{
		if(m_active.inputMode == eMode_TIMER)
		{
			std::cerr << "Timer Started... \n";
		}			
		else if(m_active.inputMode == eMode_CALIBRATE)
		{
			std::cerr << "Press '/' to end calbibration \n";
		}			
		else if(m_active.inputMode == eMode_INPUT)
		{
			std::cerr << "Use '<' and '>' to vote first or second... \n";
		}	
	}

	void Pop( float _time)
	{
		m_WorkStack.erase(m_WorkStack.begin());
		if(m_WorkStack.size())
		{
			m_active = m_WorkStack[0];
			m_active.startTime = _time;
		
			OutputCommand();
		}
		else
		{
			std::cerr << "List complete.";
			m_started = false;
		}

	}

	ContrEntry Get()
	{	
		return m_active;
	}
	
	bool 					m_started;

	std::vector<bool>		m_results;

	ContrEntry				m_active;
	std::vector<ContrEntry> m_WorkStack;
};






#endif
