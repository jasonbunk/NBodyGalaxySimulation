//-----------------------------------------------------------------
// July 27, 2012
// Jason Bunk
// Templates.h
//
// Basic classes: lines, points (without any physics).
// Not real "templates" since they can't be exported out of DLL.
//-----------------------------------------------------------------

#ifndef __JPHYSICS_TEMPLATES_H__
#define __JPHYSICS_TEMPLATES_H__

//#include "exports.h"
#define JPHYSICS_API
#define JPHYS_FLOAT_UNIT double
#include <math.h> //sin, cos, etc...
#include <iostream>

//#define PRINT_OUT_WITHOUT_PARENTHESES 1


namespace phys
{
	class point;
	class vec3;

	typedef JPHYSICS_API point		vec2;
	typedef JPHYSICS_API point		point2d;
	typedef JPHYSICS_API vec3		point3d;

	class vec2_polar;
	class vec3_spherical;



	class JPHYSICS_API point
	{
	public:
		JPHYS_FLOAT_UNIT x,y;

		point() : x(0), y(0) {}
		point(JPHYS_FLOAT_UNIT XX, JPHYS_FLOAT_UNIT YY) {x=XX; y=YY;}
		
		double& operator()(int i) //to use (1)...(2) mathematician's notation
		{
			switch(i)
			{
			case 1: return x;
			case 2: return y;
			}
			std::cout << "JPHYSICS_API vec2 FAIL!!! access () out of bounds" << std::endl;
			return x;
		}
		double& operator[](int i) //to use [0]...[1] programmer's notation
		{
			switch(i)
			{
			case 0: return x;
			case 1: return y;
			}
			std::cout << "JPHYSICS_API vec2 FAIL!!! access [] out of bounds" << std::endl;
			return x;
		}

		inline JPHYS_FLOAT_UNIT getLength() const
		{
			return sqrt(x*x + y*y);
		}

		inline JPHYS_FLOAT_UNIT getLengthSquared() const
		{
			return (x*x + y*y);
		}

		//normalized
		void GenerateRandomDirection();

		inline static JPHYS_FLOAT_UNIT dot(const point& p1, const point& p2)
		{
			return (p1.x*p2.x)+(p1.y*p2.y);
		}

		inline static JPHYS_FLOAT_UNIT cross(const point& p1, const point& p2)
		{
			return (p1.x*p2.y)-(p2.x*p1.y);
		}

		vec2_polar GetEquivalent_polar() const;

		point GetNormalized() const;
		JPHYS_FLOAT_UNIT Normalize();
		void LengthClamp(JPHYS_FLOAT_UNIT max_length);

		void LengthClampNormal()
		{
			JPHYS_FLOAT_UNIT LENG = getLength();

			if(LENG > 1.0)
			{
				x /= LENG;
				y /= LENG;
			}
		}
		
		void Rotate(JPHYS_FLOAT_UNIT angle) //radians of course!
		{
			JPHYS_FLOAT_UNIT c1 = cos(angle);
			JPHYS_FLOAT_UNIT s2 = sin(angle);

			angle = x; //save this, so the calculation of 'y' isn't affected when 'x = ...'

			x = angle * c1 - y * s2;
			y = angle * s2 + y * c1;
		}

		inline void Nullify()
		{
			x=0.0;	y=0.0;
		}

		//operators
		
		friend std::ostream& operator<< (std::ostream &out, const point &cPoint)
		{
			// Since operator<< is a friend of this class, we can access its members directly.
#if PRINT_OUT_WITHOUT_PARENTHESES
			out << cPoint.x << "\t" << cPoint.y;
#else
			out << "(" << cPoint.x << ", " << cPoint.y << ")";
#endif
			return out;
		}

		inline point & operator*=(const JPHYS_FLOAT_UNIT & scalar)
		{
			(*this).x *= scalar;
			(*this).y *= scalar;
			return (*this);
		}
		inline point & operator/=(const JPHYS_FLOAT_UNIT & scalar)
		{
			(*this).x /= scalar;
			(*this).y /= scalar;
			return (*this);
		}
		inline point & operator+=(const point &rhs)
		{
			(*this).x += rhs.x;
			(*this).y += rhs.y;
			return (*this);
		}
		inline point & operator-=(const point &rhs)
		{
			(*this).x -= rhs.x;
			(*this).y -= rhs.y;
			return (*this);
		}
		inline const point operator+(const point & rhs) const
		{
			return point(*this) += rhs;
		}
		inline const point operator-(const point & rhs) const
		{
			return point(*this) -= rhs;
		}
		inline const point operator*(const JPHYS_FLOAT_UNIT & scalar) const
		{
			return point(*this) *= scalar;
		}
		inline const point operator/(const JPHYS_FLOAT_UNIT & scalar) const
		{
			return point(*this) /= scalar;
		}
		bool operator==(const point &other) const;
		bool operator!=(const point &other) const;
		
		void print(std::ostream & sout) const; //as column vector
	};


	class JPHYSICS_API vec3
	{
	public:
		JPHYS_FLOAT_UNIT x,y,z;

		vec3() : x(0.0), y(0.0), z(0.0) {}
		vec3(JPHYS_FLOAT_UNIT XX, JPHYS_FLOAT_UNIT YY, JPHYS_FLOAT_UNIT ZZ) {x=XX; y=YY; z=ZZ;}
		
		double& operator()(int i) //to use (1)...(3) mathematician's notation
		{
			switch(i)
			{
			case 1: return x;
			case 2: return y;
			case 3: return z;
			}
			std::cout << "JPHYSICS_API vec3 FAIL!!! access () out of bounds" << std::endl;
			return x;
		}
		double& operator[](int i) //to use [0]...[2] programmer's notation
		{
			switch(i)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			}
			std::cout << "JPHYSICS_API vec3 FAIL!!! access [] out of bounds" << std::endl;
			return x;
		}

		inline JPHYS_FLOAT_UNIT getLength() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		inline JPHYS_FLOAT_UNIT getLengthSquared() const
		{
			return (x*x + y*y + z*z);
		}

		inline JPHYS_FLOAT_UNIT getLength_XYplane() const
		{
		    return sqrt(x*x + y*y);
		}
		inline JPHYS_FLOAT_UNIT getLengthSquared_XYplane() const
		{
			return (x*x + y*y);
		}

		//normalized
		void GenerateRandomDirection();

		inline static JPHYS_FLOAT_UNIT dot(const vec3& p1, const vec3& p2)
		{
			return (p1.x*p2.x)+(p1.y*p2.y)+(p1.z*p2.z);
		}

		inline static vec3 cross(const vec3& p1, const vec3& p2)
		{
			return vec3(p1.y*p2.z - p1.z*p2.y,
						p1.z*p2.x - p1.x*p2.z,
						p1.x*p2.y - p1.y*p2.x);
		}
		
		vec3 GetXYPlaneProjection() const
		{
			return vec3(x, y, 0.0);
		}
		vec3_spherical GetEquivalent_spherical() const;
		
		vec3 GetNormalized() const;
		JPHYS_FLOAT_UNIT Normalize();
		void LengthClamp(JPHYS_FLOAT_UNIT max_length);

		void LengthClampNormal()
		{
			JPHYS_FLOAT_UNIT LENG = getLength();

			if(LENG > 1.0)
			{
				x /= LENG;
				y /= LENG;
				z /= LENG;
			}
		}

		/*Rotate this vector by angle theta around an arbitrary axis
		Positive angles are anticlockwise looking down the axis towards the origin.
		Assume right hand coordinate system.
		*/
		void Rotate(JPHYS_FLOAT_UNIT theta_radians, vec3 axis)
		{
		   vec3 q(0.0, 0.0, 0.0);
		   JPHYS_FLOAT_UNIT costheta,sintheta;

		   axis.Normalize();

		   costheta = cos(theta_radians);
		   sintheta = sin(theta_radians);

		   q.x += (costheta + (1 - costheta) * axis.x * axis.x) * x;
		   q.x += ((1 - costheta) * axis.x * axis.y - axis.z * sintheta) * y;
		   q.x += ((1 - costheta) * axis.x * axis.z + axis.y * sintheta) * z;

		   q.y += ((1 - costheta) * axis.x * axis.y + axis.z * sintheta) * x;
		   q.y += (costheta + (1 - costheta) * axis.y * axis.y) * y;
		   q.y += ((1 - costheta) * axis.y * axis.z - axis.x * sintheta) * z;

		   q.z += ((1 - costheta) * axis.x * axis.z - axis.y * sintheta) * x;
		   q.z += ((1 - costheta) * axis.y * axis.z + axis.x * sintheta) * y;
		   q.z += (costheta + (1 - costheta) * axis.z * axis.z) * z;

		   (*this) = q;
		}

		// Description: Calculates the angle between two 3-D vectors.
		// Parameters:
		//  v0           - [input]  The first vector.
		//  v1           - [input]  The second vector.
		//  reflex_angle - [output] The reflex angle (the larger angle between them).
		// Returns: The smaller angle in radians.
		static JPHYS_FLOAT_UNIT CalcAngleBetween(vec3 v0, vec3 v1, JPHYS_FLOAT_UNIT* reflex_angle = 0 )
		{
		  // Unitize the input vectors
		  v0.Normalize();
		  v1.Normalize();

		  JPHYS_FLOAT_UNIT dot = vec3::dot( v0, v1 );

		  // Force the dot product of the two input vectors to
		  // fall within the domain for inverse cosine, which
		  // is -1 <= x <= 1. This will prevent runtime
		  // "domain error" math exceptions.
		  dot = ( dot < -1.0 ? -1.0 : ( dot > 1.0 ? 1.0 : dot ) );

		  JPHYS_FLOAT_UNIT angle = acos( dot );

		  if( reflex_angle )
			*reflex_angle = (6.283185307 - angle);

		  return angle;
		}

		inline void Nullify()
		{
			x=0.0;	y=0.0; z=0.0;
		}

		//operators
		
		friend std::ostream& operator<< (std::ostream &out, const vec3 &cPoint)
		{
			// Since operator<< is a friend of this class, we can access its members directly.
#if PRINT_OUT_WITHOUT_PARENTHESES
			out << cPoint.x << "\t" << cPoint.y << "\t" << cPoint.z;
#else
			out << "(" << cPoint.x << ", " << cPoint.y << ", " << cPoint.z << ")";
#endif
			return out;
		}
		
		inline vec3 & operator*=(const JPHYS_FLOAT_UNIT & scalar)
		{
			(*this).x *= scalar;
			(*this).y *= scalar;
			(*this).z *= scalar;
			return (*this);
		}
		inline vec3 & operator/=(const JPHYS_FLOAT_UNIT & scalar)
		{
			(*this).x /= scalar;
			(*this).y /= scalar;
			(*this).z /= scalar;
			return (*this);
		}
		inline vec3 & operator+=(const vec3 &rhs)
		{
			(*this).x += rhs.x;
			(*this).y += rhs.y;
			(*this).z += rhs.z;
			return (*this);
		}
		inline vec3 & operator-=(const vec3 &rhs)
		{
			(*this).x -= rhs.x;
			(*this).y -= rhs.y;
			(*this).z -= rhs.z;
			return (*this);
		}
		inline const vec3 operator+(const vec3 & rhs) const
		{
			return vec3(*this) += rhs;
		}
		inline const vec3 operator-(const vec3 & rhs) const
		{
			return vec3(*this) -= rhs;
		}
		inline const vec3 operator*(const JPHYS_FLOAT_UNIT & scalar) const
		{
			return vec3(*this) *= scalar;
		}
		inline const vec3 operator/(const JPHYS_FLOAT_UNIT & scalar) const
		{
			return vec3(*this) /= scalar;
		}
		bool operator==(const vec3 &other) const;
		bool operator!=(const vec3 &other) const;
		
		void print(std::ostream & sout) const; //as column vector
	};

	class JPHYSICS_API vec2_polar
	{
	public:
		JPHYS_FLOAT_UNIT r, phi;

		vec2_polar() : r(0.0), phi(0.0) {}
		vec2_polar(JPHYS_FLOAT_UNIT RADIUS, JPHYS_FLOAT_UNIT PHI) : r(RADIUS), phi(PHI) {}

		inline void Nullify()
		{
			r = 0.0;
			phi = 0.0;
		}
		
		static inline vec2 to_cartesian(const vec2_polar & polar_source)
		{
			return vec2(polar_source.r * cos(polar_source.phi),
						polar_source.r * sin(polar_source.phi));
		}
		vec2 to_cartesian() const {return to_cartesian(*this);}
		
		friend std::ostream& operator<< (std::ostream &out, const vec2_polar &cPoint)
		{
			// Since operator<< is a friend of this class, we can access its members directly.
#if PRINT_OUT_WITHOUT_PARENTHESES
			out << cPoint.r << "\t" << cPoint.phi;
#else
			out << "(" << cPoint.r << ", " << cPoint.phi << ")";
#endif
			return out;
		}
		void print(std::ostream & sout) const; //as "column vector"
	};
	
	class JPHYSICS_API vec3_spherical
	{
	public:
		JPHYS_FLOAT_UNIT r, phi, theta; //theta goes from 0 to pi... where 0 is the north pole... pi is the south pole

		vec3_spherical() : r(0.0), phi(0.0), theta(0.0) {}
		vec3_spherical(JPHYS_FLOAT_UNIT RADIUS, JPHYS_FLOAT_UNIT PHI, JPHYS_FLOAT_UNIT THETA) : r(RADIUS), phi(PHI), theta(THETA) {}

		inline void Nullify()
		{
			r = 0.0;
			phi = 0.0;
			theta = 0.0;
		}
		
		static vec3 to_cartesian(const vec3_spherical & spherical_source);
		vec3 to_cartesian() const {return to_cartesian(*this);}

		//assuming this contains (dr/dt, dphi/dt, dtheta/dt)... and NOT anything like (dr/dt, r*sin(theta)*dphi/dt, r*dtheta/dt)
		static vec3 convert_spherical_vel_to_cartesian_vel(const vec3_spherical & sphericalsource_pos, const vec3_spherical & sphericalsource_vel);
		
		friend std::ostream& operator<< (std::ostream &out, const vec3_spherical &cPoint)
		{
			// Since operator<< is a friend of this class, we can access its members directly.
#if PRINT_OUT_WITHOUT_PARENTHESES
			out << cPoint.r << "\t" << cPoint.phi << "\t" << cPoint.theta;
#else
			out << "(" << cPoint.r << ", " << cPoint.phi << ", " << cPoint.theta << ")";
#endif
			return out;
		}
		void print(std::ostream & sout) const; //as "column vector"
	};
	
//--------------------------------------
	
	class JPHYSICS_API mat2x2
	{
	public:
		double m[2][2]; //i,j notation... i=row, j=column
		
		mat2x2();
		mat2x2(const mat2x2 & other);
		mat2x2(std::string call_this_the_identity);
		
		void MakeMeIdentity()
		{
			m[0][0] = 1.0;
			m[1][0] = 0.0;
			m[0][1] = 0.0;
			m[1][1] = 1.0;
		}
		
		double& operator()(int i, int j) //to use (1,1)...(3,3) notation
		{
			if(i<1 || j<1 || i>2 || j>2)
			{
				std::cout << "JPHYSICS_API mat3x3 FAIL!!! access out of bounds" << std::endl;
				return m[0][0];
			}
			return m[i-1][j-1];
		}
		
		double GetDet() const;
		static mat2x2 mult(const mat2x2 & m1, const mat2x2 & m2);


		mat2x2 & operator+=(const mat2x2 &rhs)
		{
			for(char i=0; i<2; i++)
			{
				for(char j=0; j<2; j++)
				{
					m[i][j] += rhs.m[i][j];
				}
			}
			return *this;
		}

		mat2x2 & operator-=(const mat2x2 &rhs)
		{
			for(char i=0; i<2; i++)
			{
				for(char j=0; j<2; j++)
				{
					m[i][j] -= rhs.m[i][j];
				}
			}
			return *this;
		}
		
		const mat2x2 operator+(const mat2x2 &other) const
		{
			return mat2x2(*this) += other;
		}
		const mat2x2 operator-(const mat2x2 &other) const
		{
			return mat2x2(*this) -= other;
		}
		
		const mat2x2 operator*(const mat2x2 &other) const
		{
			return mult(*this, other);
		}
		
		const phys::vec2 operator*(const phys::vec2 & other) const
		{
			phys::vec2 retval;
			
			for(char i=0; i<2; i++)
			{
				retval[i] += (m[i][0]*other.x + m[i][1]*other.y);
			}

			return retval;
		}
		
		void print(std::ostream & sout) const;
	};


	class JPHYSICS_API mat3x3
	{
	public:
		double m[3][3]; //i,j notation... i=row, j=column

		
		mat3x3();
		
		mat3x3(	double a11, double a12, double a13,
				double a21, double a22, double a23,
				double a31, double a32, double a33);

		mat3x3(const mat3x3 & other);
		mat3x3(std::string call_this_the_identity);


		void MakeMeIdentity()
		{
			m[0][0] = 1.0;	m[0][1] = 0.0;	m[0][2] = 0.0;
			m[1][0] = 0.0;	m[1][1] = 1.0;	m[1][2] = 0.0;
			m[2][0] = 0.0;	m[2][1] = 0.0;	m[2][2] = 1.0;
		}
		void MakeMeZero()
		{
			m[0][0] = 0.0;	m[0][1] = 0.0;	m[0][2] = 0.0;
			m[1][0] = 0.0;	m[1][1] = 0.0;	m[1][2] = 0.0;
			m[2][0] = 0.0;	m[2][1] = 0.0;	m[2][2] = 0.0;
		}

		double& operator()(int i, int j) //to use (1,1)...(3,3) notation
		{
			if(i<1 || j<1 || i>3 || j>3)
			{
				std::cout << "JPHYSICS_API mat3x3 FAIL!!! access out of bounds" << std::endl;
				return m[0][0];
			}
			return m[i-1][j-1];
		}


		double GetDet() const;
		mat3x3 GetInverse() const;

		static mat3x3 mult(const mat3x3 & m1, const mat3x3 & m2);


		mat3x3 & operator+=(const mat3x3 &rhs)
		{
			for(char i=0; i<3; i++)
			{
				for(char j=0; j<3; j++)
				{
					m[i][j] += rhs.m[i][j];
				}
			}
			return *this;
		}
		mat3x3 & operator-=(const mat3x3 &rhs)
		{
			for(char i=0; i<3; i++)
			{
				for(char j=0; j<3; j++)
				{
					m[i][j] -= rhs.m[i][j];
				}
			}
			return *this;
		}
		

		mat3x3 & operator*=(const double & scalar)
		{
			for(char i=0; i<3; i++)
			{
				for(char j=0; j<3; j++)
				{
					m[i][j] *= scalar;
				}
			}
			return *this;
		}
		mat3x3 & operator/=(const double & scalar)
		{
			for(char i=0; i<3; i++)
			{
				for(char j=0; j<3; j++)
				{
					m[i][j] /= scalar;
				}
			}
			return *this;
		}
		

		const mat3x3 operator+(const mat3x3 &other) const
		{
			return mat3x3(*this) += other;
		}
		const mat3x3 operator-(const mat3x3 &other) const
		{
			return mat3x3(*this) -= other;
		}
		const mat3x3 operator*(const mat3x3 &other) const
		{
			return mult(*this, other);
		}

		
		const mat3x3 operator*(const double & scalar) const
		{
			return mat3x3(*this) *= scalar;
		}
		const mat3x3 operator/(const double & scalar) const
		{
			return mat3x3(*this) /= scalar;
		}

		
		const phys::vec3 operator*(const phys::vec3 & other) const
		{
			phys::vec3 retval;
			
			for(char i=0; i<3; i++)
			{
				retval[i] += (m[i][0]*other.x + m[i][1]*other.y + m[i][2]*other.z);
			}

			return retval;
		}

		void print(std::ostream & sout) const;

		mat2x2 get_mat2x2_by_excluding_one(int i, int j) const;

		
		void MakePassiveRotation_X_axis(double theta);
		void MakePassiveRotation_Y_axis(double theta);
		void MakePassiveRotation_Z_axis(double theta);
	};

//--------------------------------------
//--------- "JPHYS_FLOAT_UNIT" lines

	class JPHYSICS_API line
	{
	public:
		point p1, p2;
		line(JPHYS_FLOAT_UNIT X1, JPHYS_FLOAT_UNIT Y1, JPHYS_FLOAT_UNIT X2, JPHYS_FLOAT_UNIT Y2)
		{
			p1.x = X1;
			p1.y = Y1;
			p2.x = X2;
			p2.y = Y2;
		}
		line(point P1, point P2) {p1=P1; p2=P2;}
		line() {}

		inline JPHYS_FLOAT_UNIT getLengthSquared() const
		{
			return ((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y));
		}
		inline JPHYS_FLOAT_UNIT getLength() const
		{
			return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y));
		}
		inline point getCenter() const
		{
			return point((p1.x+p2.x)/2.0, (p1.y+p2.y)/2.0);
		}
	};

	class JPHYSICS_API line3d
	{
	public:
		vec3 p1, p2;
		line3d(JPHYS_FLOAT_UNIT X1, JPHYS_FLOAT_UNIT Y1, JPHYS_FLOAT_UNIT Z1, JPHYS_FLOAT_UNIT X2, JPHYS_FLOAT_UNIT Y2, JPHYS_FLOAT_UNIT Z2)
		{
			p1.x = X1;
			p1.y = Y1;
			p1.z = Z1;
			p2.x = X2;
			p2.y = Y2;
			p2.z = Z2;
		}
		line3d(vec3 P1, vec3 P2) {p1=P1; p2=P2;}
		line3d() {}

		inline JPHYS_FLOAT_UNIT getLengthSquared() const
		{
			return ((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y) + (p2.z-p1.z)*(p2.z-p1.z));
		}
		inline JPHYS_FLOAT_UNIT getLength() const
		{
			return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y) + (p2.z-p1.z)*(p2.z-p1.z));
		}
		inline vec3 getCenter() const
		{
			return vec3((p1.x+p2.x)*0.5, (p1.y+p2.y)*0.5, (p1.z+p2.z)*0.5);
		}
	};

//-----------------------------------------------------------------

	class JPHYSICS_API TempLine
	{
	public:
			point * p1;
			point * p2;
		TempLine() {p1=0; p2=0;}
		TempLine(point* start, point* end) {p1=start; p2=end;}

			point GetP1() {return p1 ? (*p1) : point(0.0, 0.0);}
			point GetP2() {return p2 ? (*p2) : point(0.0, 0.0);}

			line GetLine() {return ((p1 && p2) ? line(*p1, *p2) : line(0,0,1,1));}
	};

	class JPHYSICS_API TempLine3d
	{
	public:
			vec3 * p1;
			vec3 * p2;
		TempLine3d() {p1=0; p2=0;}
		TempLine3d(vec3* start, vec3* end) {p1=start; p2=end;}

			vec3 GetP1() {return p1 ? (*p1) : vec3(0.0, 0.0, 0.0);}
			vec3 GetP2() {return p2 ? (*p2) : vec3(0.0, 0.0, 0.0);}

			line3d GetLine() {return ((p1 && p2) ? line3d(*p1, *p2) : line3d(0,0,0,1,1,1));}
	};

//======================================================================
	
	class JPHYSICS_API posvelpair
	{
	public:
		point pos, vel;

		posvelpair(JPHYS_FLOAT_UNIT posx, JPHYS_FLOAT_UNIT posy, JPHYS_FLOAT_UNIT velx, JPHYS_FLOAT_UNIT vely)
		{
			pos.x = posx;
			pos.y = posy;
			vel.x = velx;
			vel.y = vely;
		}
		posvelpair(JPHYS_FLOAT_UNIT posx, JPHYS_FLOAT_UNIT posy)
		{
			pos.x = posx;
			pos.y = posy;
			vel.x = 0.0;
			vel.y = 0.0;
		}
		posvelpair(point POS, point VEL) {pos=POS; vel=VEL;}
		posvelpair() {}
	};

	class JPHYSICS_API posvelpair3d
	{
	public:
		vec3 pos, vel;

		posvelpair3d(JPHYS_FLOAT_UNIT posx, JPHYS_FLOAT_UNIT posy, JPHYS_FLOAT_UNIT posz, JPHYS_FLOAT_UNIT velx, JPHYS_FLOAT_UNIT vely, JPHYS_FLOAT_UNIT velz)
		{
			pos.x = posx;
			pos.y = posy;
			pos.z = posz;
			vel.x = velx;
			vel.y = vely;
			vel.z = velz;
		}
		posvelpair3d(JPHYS_FLOAT_UNIT posx, JPHYS_FLOAT_UNIT posy, JPHYS_FLOAT_UNIT posz)
		{
			pos.x = posx;
			pos.y = posy;
			pos.z = posz;
			vel.x = 0.0;
			vel.y = 0.0;
			vel.z = 0.0;
		}
		posvelpair3d(vec3 POS, vec3 VEL) {pos=POS; vel=VEL;}
		posvelpair3d() {}
	};
}

#endif
