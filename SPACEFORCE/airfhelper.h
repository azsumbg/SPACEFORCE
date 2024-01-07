#pragma once

#ifdef AIRFHELPER_EXPORTS 
#define AIRFHELPER_API _declspec(dllexport)
#else 
#define AIRFHELPER_API _declspec(dllimport)
#endif

enum class types {
	no_type = 0, ship = 1, big_asteroid = 2, mid_asteroid = 3, small_asteroid = 4,
	meteor = 5, explosion = 6, vlaser = 7, vstrong_laser = 8, spare = 9, station = 10,
	nebula1 = 11, nebula2 = 12, big_star = 13, mid_star = 14, small_star = 15,
	hlaser = 16, hstrong_laser = 17
};

enum class dirs { stop = 0, up = 1, down = 2, left = 3, right = 4, up_left = 5, up_right = 6, down_left = 7, down_right = 8 };

enum class DLRESULT { DL_OK = 0, DL_FAIL = 1, DL_IN = 2, DL_OUT = 3, DL_NOT_SUPPORTED = 4 };

constexpr float scr_width = 816.0f; // Screen 800 * 700
constexpr float scr_height = 739.0f;

constexpr float frame_min_width = -216.0f; //Frame +/-200 of screen
constexpr float frame_max_width = 1016.0f;

constexpr float frame_min_height = -239.0f;
constexpr float frame_max_height = 939.0f;

class AIRFHELPER_API PRIME
{
	private:
		static inline int count = 0;

		static void add_counter()
		{
			count++;
		}

		static void sub_counter()
		{
			count--;
		}

	protected:
		float width = 0;
		float height = 0;

	public:
		float x = 0;
		float y = 0;
		float ex = 0;
		float ey = 0;

		PRIME(float _x, float _y, float _width = 1.0f, float _height = 1.0f)
		{
			x = _x;
			y = _y;
			width = _width;
			height = _height;
			ex = x + width;
			ey = y + height;

			add_counter();
		}

		virtual ~PRIME() 
		{
			sub_counter();
		}

		void SetEdges()
		{
			ex = x + width;
			ey = y + height;
		}

		void NewDims(float _new_width, float _new_height)
		{
			width = _new_width;
			height = _new_height;
			ex = x + width;
			ey = y + height;
		}

		virtual DLRESULT Release()
		{
			if (count > 0)
			{
				sub_counter();
				delete this;
				return DLRESULT::DL_OK;
			}
			return DLRESULT::DL_FAIL;
		}
};

class AIRFHELPER_API OBJECTS :public PRIME
{
	protected:
		static inline int ref = 0;

		static void AddRef()
		{
			ref++;
		}

		static void RemoveRef()
		{
			ref--;
		}

		float speed = 0;

		types type = types::no_type;

		int max_frames = 0;
		int frame_delay = 0;
		int current_frame = 0;

	public:
		dirs dir = dirs::stop;
		int lifes = 0;
		
		OBJECTS(float __x, float __y, float __width, float __height, types __type) :PRIME(__x, __y, __width, __height)
		{
			AddRef();
			type = __type;
		}

		virtual ~OBJECTS()
		{
			RemoveRef();
		}

		virtual DLRESULT Release() override
		{
			if (ref > 0)
			{
				RemoveRef();
				delete this;
				return DLRESULT::DL_OK;
			}

			return DLRESULT::DL_FAIL;
		}

		types GetType()const
		{
			return type;
		}

		virtual DLRESULT SetType(types _which_type) = 0;

		virtual void Move(float _speed = 1.0f) = 0;

		virtual int GetFrame() = 0;
};

typedef PRIME* prime_ptr;
typedef OBJECTS* obj_ptr;

extern AIRFHELPER_API prime_ptr iPrimeFactory(types _type, float _x, float _y);
extern AIRFHELPER_API obj_ptr iObjectFactory(types _type, float _x, float _y);