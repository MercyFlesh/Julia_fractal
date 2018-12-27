#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

#define COLOR_RGB(R, G, B) (((R) << 16) | ((G) << 8) | (B))

namespace gfx
{
	class complecs
	{
		float Re, Im;

	public:
		explicit complecs(float Re = 0, float Im = 0) : Re(Re), Im(Im) {};

		inline complecs operator+(complecs A) const { return complecs(Re + A.Re, Im + A.Im); }

		inline complecs operator*(complecs A) const { return complecs(Re * A.Re - Im * A.Im, Im * A.Re + Re * A.Im); }

		inline float Len() const { return Re * Re + Im * Im; }

		~complecs() = default;
	};

	const int
		W = 100,
		H = 100;

	class julia
	{
	private:
		HBITMAP hBmp;
		ULONG *Frame = NULL;
		float DeltaTime, SyncTime;

		void Timer()
		{
			long t = clock();
			static long StartTime = 0, OldTime, PauseTime, FrameCount;

			if (StartTime == 0)
				StartTime = OldTime = t;

			SyncTime = (t - StartTime - PauseTime) / (float)CLOCKS_PER_SEC;
			DeltaTime = (t - OldTime) / (float)CLOCKS_PER_SEC;

			OldTime = t;
		}

		int Iter(complecs Z, complecs C) const
		{
			int N = 0;

			while (N <= 254 && Z.Len() <= 4)
				Z = Z * Z + C, N++;

			return N;
		}

		void Build(float Re, float Im)
		{
			ULONG t = clock();
			float
				R = 1,
				G = 1,
				B = 1;

			register int N;
			register complecs C(Re, Im), M;

			for (int y = 0; y < H; y++)
				for (int x = 0; x < W; x++)
				{
					M = complecs(x * 2.0 / W - 1, y * 2.0 / H - 1),
						N = Iter(M * M, C);
					Frame[x + W * y] = COLOR_RGB((int)(N * 7 / R), (int)(N * 3 / G), (int)(N * 2 / B));
				}
		}

		void Session()
		{
			register HDC
				hBMDC = CreateCompatibleDC(GetDC(NULL)),
				hDRAWDC = GetDC(NULL);

			while (true)
			{
				Timer();

				ZeroMemory(Frame, sizeof(ULONG) * W * H);

				if (GetKeyState('Q')) //&& GetKeySqtate(VK_Control))
					break;

				Build(.5F * cos(SyncTime), 2.0F * sin(SyncTime));

				SelectObject(hBMDC, hBmp);
				BitBlt(hDRAWDC, 0, 0, W, H, hBMDC, 0, 0, SRCCOPY);
			}
			DeleteObject(hBMDC);
			DeleteObject(hDRAWDC);
		}

	public:
		julia() : Frame(new ULONG[W * H])
		{
			BITMAPINFOHEADER BmI;

			memset((void *)&BmI, 0, sizeof(BITMAPINFOHEADER));
			BmI.biSize = sizeof(BITMAPINFOHEADER);
			BmI.biBitCount = 32;
			BmI.biPlanes = 1;
			BmI.biCompression = BI_RGB;
			BmI.biWidth = W;
			BmI.biHeight = -H;
			BmI.biSizeImage = W * H * 4;
			BmI.biSizeImage = 0;
			BmI.biClrUsed = 0;
			BmI.biClrImportant = 0;
			BmI.biXPelsPerMeter = BmI.biYPelsPerMeter = 0;

			hBmp = CreateDIBSection(NULL, (BITMAPINFO *)&BmI, DIB_RGB_COLORS, (void **)(&Frame), NULL, 0);

			Session();
		}

		~julia()
		{
			DeleteObject(hBmp);
		}
	};
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	gfx::julia();
	
	
	InvalidateRect(NULL, NULL, 0);
	return 0;

	
}