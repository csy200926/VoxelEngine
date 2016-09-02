#pragma once
namespace SYSR
{
	class BitMap
	{
	private:
		unsigned int m_width, m_height;
		unsigned int m_imageSize;

		
	public:
		unsigned char * m_data;
		BitMap(unsigned int i_width, unsigned int i_height)
		{
			m_height = i_height;
			m_width = i_width;

			m_imageSize = m_width * m_height * 4;
			m_data = new unsigned char [m_imageSize]; 
		}
		~BitMap()
		{
			delete []m_data;
		}

		inline unsigned char * GetData();

		inline unsigned int GetImageSize() const;

		inline void Clear(unsigned char r,unsigned char g,unsigned char b,unsigned char a);
		inline void DrawPixel(int x, int y,unsigned char r,unsigned char g,unsigned char b,unsigned char a);
		inline void GetPixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a);
	};

	unsigned int BitMap::GetImageSize() const
	{
		return m_imageSize;
	}

	unsigned char * BitMap::GetData()
	{
		return m_data;
	}

	void BitMap::DrawPixel(int x, int y,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
	{
		int index = (x + y * m_width) * 4;
		m_data[index] = r;
		m_data[index + 1] = g;
		m_data[index + 2] = b;
		m_data[index + 3] = a;
	}

	void BitMap::GetPixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a)
	{
		int index = (x + y * m_width) * 4;
		r = m_data[index];
		g = m_data[index + 1];
		b = m_data[index + 2];
		a = m_data[index + 3];
	}

	void BitMap::Clear(unsigned char r,unsigned char g,unsigned char b,unsigned char a)
	{
		for (unsigned int x = 0 ; x < m_width ; x++)
		{
			for (unsigned int y = 0 ; y < m_height ; y++)
			{
				DrawPixel(x,y,r,g,b,a);
			}
		}
	}


}

