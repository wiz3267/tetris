
//класс FIXED - число с фиксированной запятой
class FIXED
{
	long i;
public:
	FIXED() {}	//конструктор
	FIXED(float n)
	{
		i=n*256;
	}
	operator int ()
	{
		return i>>8;
	}
	void operator += (FIXED f)
	{
		i+=f.i;
	}
};

//класс FLOAT - число с фиксированной запятой
class FLOAT
{
	long i;
public:
	FLOAT() {}	//конструктор
	FLOAT(float n)
	{
		i=n*256;
	}
	operator int ()
	{
		return i>>8;
	}
	void operator += (FLOAT f)
	{
		i+=f.i;
	}
	void operator -= (FLOAT f)
	{
		i-=f.i;
	}

	void operator -= (int f)
	{
		i-=f*256;
	}

	void operator *= (int n)
	{
		i*=n;
	}

	int operator > (int n)
	{
		if ( (i>>8) > n ) return 1;
		else return 0;
	}

	int operator < (int n)
	{
		if ( (i>>8) < n ) return 1;
		else return 0;
	}

	int operator == (int n)
	{
		if ( (i>>8) == n ) return 1;
		else return 0;
	}

	int operator >= (int n)
	{
		if ( (i>>8) >= n ) return 1;
		else return 0;
	}

	int operator <= (int n)
	{
		if ( (i>>8) <= n ) return 1;
		else return 0;
	}

};