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

enum BlendModes
{
	eBlend_Linear,
	eBlend_Box,
	eBlend_NonLinear,
	eBlend_Near,
	eBlend_Far,
	eBlend_Projective
};

enum EyeSeperationModes
{
	eEye_Sep,
	eEye_None
};

enum ProjectionModes
{
	eProj_OffAxis,
	eProj_ToeIn,
	eProj_Oblique
};

enum StereoModes
{
	eStereo_Left,
	eStereo_Right,
	eStereo_Both
};


enum SceneModes
{
	eScene_Test,
	eScene_Distance,
	eScene_Rotation,
	eScene_Translation,
	eScene_Calibration,
	eScene_Decision,
	eScene_Static
};


struct ContrEntry
{
	void InitDistanceInput( SceneModes _renderModeA, BlendModes _blendMode, StereoModes _stereo, EyeSeperationModes _eyeSep, float _da, float _db, float _dc, float _sa, float _sb, float _sc, int _answer)
	{
		inputMode 	= eMode_INPUT_DEPTH;

		renderModeA = _renderModeA;
		blendModeA 	= _blendMode;
		projModeA 	= eProj_Oblique;

		timeMs 		= 0.f;
		startTime 	= 0.f;

		renderModeB = 0;
		blendModeB 	= 0.f;
		projModeB 	= 0;

		depthA 		= _da;
		depthB 		= _db;
		depthC 		= _dc;
		
		scaleA 		= _sa;
		scaleB 		= _sb;
		scaleC 		= _sc;

		answer 		= _answer;

		stereoModeA = _stereo;
		stereoModeB = _stereo;

		eyeSeperationA = _eyeSep;
		eyeSeperationB = _eyeSep;
	}

	void InitTimer( SceneModes _renderModeA, StereoModes _stereoA,  EyeSeperationModes _eyeSep, BlendModes _blendModeA, ProjectionModes _projModeA, float _timeMS)
	{
		inputMode 	= eMode_TIMER;

		renderModeA = _renderModeA;
		blendModeA 	= _blendModeA;
		projModeA 	= _projModeA;

		timeMs 		= _timeMS;
		startTime 	= 0.f;

		renderModeB = 0;
		blendModeB 	= 0.f;
		projModeB 	= 0;

		depthA 		= 0.f;
		depthB 		= 0.f;
		depthC 		= 0.f;
		scaleA 		= 0.f;
		scaleB 		= 0.f;
		scaleC 		= 0.f;

		stereoModeA = _stereoA;
		stereoModeB = _stereoA;

		eyeSeperationA = _eyeSep;
		eyeSeperationB = _eyeSep;
	}

	void InitCalibration()
	{
		inputMode 	= eMode_CALIBRATE;

		renderModeA = eScene_Calibration;
		blendModeA 	= 0;
		projModeA 	= 0;

		timeMs 		= 0;
		startTime 	= 0.f;
		renderModeB = 0;
		blendModeB 	= 0.f;
		projModeB 	= 0;

		depthA 		= 0.f;
		depthB 		= 0.f;
		depthC 		= 0.f;
		scaleA 		= 0.f;
		scaleB 		= 0.f;
		scaleC 		= 0.f;

		stereoModeA = eStereo_Both;
		stereoModeB = eStereo_Both;

		eyeSeperationA = eEye_Sep;
		eyeSeperationB = eEye_Sep;
	}

	void InitInput( SceneModes _renderModeA, StereoModes _stereoA, EyeSeperationModes _eyeSepA, BlendModes _blendModeA, ProjectionModes _projModeA, SceneModes _renderModeB, StereoModes _stereoB, EyeSeperationModes _eyeSepB, BlendModes _blendModeB, ProjectionModes _projModeB)
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

		depthA 		= 0.f;
		depthB 		= 0.f;
		depthC 		= 0.f;
		scaleA 		= 0.f;
		scaleB 		= 0.f;
		scaleC 		= 0.f;

		stereoModeA = _stereoA;
		stereoModeB = _stereoB;

		eyeSeperationA = _eyeSepA;
		eyeSeperationB = _eyeSepB;
	}

	int 		renderModeA;
	int 		blendModeA;
	int 		projModeA;

	int 		renderModeB;
	int 		blendModeB;
	int 		projModeB;

	ContrMode 	inputMode;

	StereoModes stereoModeA;
	StereoModes stereoModeB;

	float 		timeMs;
	float 		startTime;

	float		depthA;
	float		depthB;
	float		depthC;

	float		scaleA;
	float		scaleB;
	float		scaleC;

	EyeSeperationModes eyeSeperationA;
	EyeSeperationModes eyeSeperationB;

	int 		answer;
};

class TestController
{
	public:
	TestController() : m_started(false), m_name("No Name Set")
	{}

	void Start(std::string _name)
	{
		m_name = _name;

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

				OutputResult((_input == 1 ? true : false));
			
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
			if(_input == 0 || _input == 1 || _input == 2)
			{
				std::cerr << "Recieved input\n";
				m_results.push_back( (_input == m_active.answer ? true : false) );

				bool answer = (_input == m_active.answer ? true : false);
				OutputResult(answer);
			
				if(_input == m_active.answer)
					std::cerr << "Correct\n";
				else
					std::cerr << "Wrong\n";

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

	void OutputResult(bool _answer)
	{
		std::ofstream output;

		std::string BlendModesNames[6]
		{
			"Linear",
			"Box",
			"NonLinear",
			"Near",
			"Far",
			"Projective"
		};

		std::string EyeSeperationModesNames[2]
		{
			"Separated",
			"No_Separation"
		};

		std::string ProjectionModesNames[3]
		{
			"OffAxis",
			"ToeIn",
			"Oblique"
		};

		std::string StereoModesNames[3]
		{
			"MonoLeft",
			"MonoRight",
			"Stereo"
		};

		std::string SceneModesNames[7] = 
		{
			"Test",
			"Distance",
			"Rotation",
			"Translation",
			"Calibration",
			"Decision",
			"Static"
		};

		if( m_active.renderModeA == eScene_Test)
		{
			output.open("output_test.csv", std::ios::out | std::ios::app);
			output << m_name << "," << 	m_active.scaleA 								<< "," << m_active.scaleB 								<< "," << m_active.scaleC << "," <<
									m_active.depthA 									<< "," << m_active.depthB 								<< "," << m_active.depthC << "," << m_active.answer << "," << 	
									StereoModesNames[m_active.stereoModeA]				<< "," << StereoModesNames[m_active.stereoModeB]		<< "," <<	
									EyeSeperationModesNames[m_active.eyeSeperationA]	<< "," << EyeSeperationModesNames[m_active.eyeSeperationB]	<< "," <<	
									SceneModesNames[m_active.renderModeA]				<< "," << SceneModesNames[m_active.renderModeB]			<< "," <<
									BlendModesNames[m_active.blendModeA] 				<< "," << BlendModesNames[m_active.blendModeB] 			<< "," <<
									ProjectionModesNames[m_active.projModeA] 			<< "," << ProjectionModesNames[m_active.projModeB] 		<< "," << _answer << "\n";
			output.close();
		}

		else if( m_active.renderModeA == eScene_Translation)
		{
			output.open("output_translation.csv", std::ios::out | std::ios::app);
			output << m_name << "," << StereoModesNames[m_active.stereoModeA]				<< "," << StereoModesNames[m_active.stereoModeB]		<< "," <<	
									EyeSeperationModesNames[m_active.eyeSeperationA]	<< "," << EyeSeperationModesNames[m_active.eyeSeperationB]	<< "," <<	
									BlendModesNames[m_active.blendModeA] 				<< "," << BlendModesNames[m_active.blendModeB] 			<< "," <<
									ProjectionModesNames[m_active.projModeA] 			<< "," << ProjectionModesNames[m_active.projModeB] 		<< "," << _answer << "\n";
			output.close();
		}

		else if( m_active.renderModeA == eScene_Rotation)
		{
			output.open("output_rotation.csv", std::ios::out | std::ios::app);
			output << m_name << "," << StereoModesNames[m_active.stereoModeA]			<< "," << StereoModesNames[m_active.stereoModeB]		<< "," <<	
									EyeSeperationModesNames[m_active.eyeSeperationA]	<< "," << EyeSeperationModesNames[m_active.eyeSeperationB]	<< "," <<	
									BlendModesNames[m_active.blendModeA] 				<< "," << BlendModesNames[m_active.blendModeB ]			<< "," <<
									ProjectionModesNames[m_active.projModeA] 			<< "," << ProjectionModesNames[m_active.projModeB] 		<< "," << _answer << "\n";
			output.close();
		}

		else if( m_active.renderModeA == eScene_Static)
		{
			output.open("output_static.csv", std::ios::out | std::ios::app);
			output << m_name << "," << StereoModesNames[m_active.stereoModeA]			<< "," << StereoModesNames[m_active.stereoModeB]			<< "," <<	
									EyeSeperationModesNames[m_active.eyeSeperationA]	<< "," << EyeSeperationModesNames[m_active.eyeSeperationB]	<< "," <<	
									BlendModesNames[m_active.blendModeA] 				<< "," << BlendModesNames[m_active.blendModeB ]				<< "," <<
									ProjectionModesNames[m_active.projModeA] 			<< "," << ProjectionModesNames[m_active.projModeB] 			<< "," << _answer << "\n";
			output.close();
		}

		if( m_active.renderModeA == eScene_Distance)
		{
			output.open("output_distance.csv", std::ios::out | std::ios::app);
			output << m_name << "," << 	m_active.scaleA 									<< "," << m_active.scaleB 			<< "," << m_active.scaleC << "," <<
										m_active.depthA 									<< "," << m_active.depthB 			<< "," << m_active.depthC << "," << m_active.answer << "," << 	
										StereoModesNames[m_active.stereoModeA]				<< "," <<	
										EyeSeperationModesNames[m_active.eyeSeperationA]	<< "," <<	
										BlendModesNames[m_active.blendModeA] 				<< "," <<
										ProjectionModesNames[m_active.projModeA] 			<< "," << _answer << "\n";
			output.close();
		}







	}

	void OutputCommand()
	{
		if(m_active.inputMode == eMode_TIMER)
		{
			std::cerr << "Timer Started. \n";
		}			
		else if(m_active.inputMode == eMode_CALIBRATE)
		{
			std::cerr << "Press '/' to end calbibration. \n";
		}			
		else if(m_active.inputMode == eMode_INPUT)
		{
			std::cerr << "Use '<' and '>' to vote first or second. \n";
		}	
		else if(m_active.inputMode == eMode_INPUT_DEPTH)
		{
			std::cerr << "Use '<', '>' and '/' to vote left, middle or right object being closest. \n";
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
			exit(0);
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

	std::string 			m_name;
};






#endif
