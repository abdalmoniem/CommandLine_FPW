/*
   Command Line First Person Shooter (FPS) Engine 

   License
   ~~~~~~~
   Copyright (C) 2018  hifnawy
   This program comes with ABSOLUTELY NO WARRANTY.
   This is free software, and you are welcome to redistribute it
   under certain conditions; See license for details.

   GNU GPLv3
   https://github.com/abdalmoniem/CommandLine_FPW/blob/master/LICENSE

   IMPORTANT!!!!
   ~~~~~~~~~~~~~
   READ ME BEFORE RUNNING!!! This program expects the console dimensions to be set to 
   120 Columns by 40 Rows. I recommend a small font (Consolas for example) at size 16. You can do this
   by running the program, and right clicking on the console title bar, and specifying 
   the properties. You can also choose to default to them in the future.

   Future Modifications
   ~~~~~~~~~~~~~~~~~~~~
   1) Shade block segments based on angle from player, i.e. less light reflected off
   walls at side of player. Walls straight on are brightest.
   2) Find an interesting and optimised ray-tracing method. I'm sure one must exist
   to more optimally search the map space
   3) Add bullets!
   4) Add bad guys!

   Author
   ~~~~~~
   AbdAlMoniem AlHifnawy
   hifnawy_moniem@hotmail.com

   Last Updated: 23/01/2019
*/

/***************************************************/
/*            Declared Local Definitions           */  
/***************************************************/

#define UNICODE

#define MAP_TYPE_MAZE      1
#define MAP_TYPE_RANDOM    2
#define MAP_TYPE_EXPLORE   3

/***************************************************/
/*               Header Files Includes             */  
/***************************************************/

#include <cmath>
#include <ctime>
#include <vector>
#include <chrono>
#include <cwchar>
#include <cstdio>
#include <cstdlib> 
#include <utility>
#include <iostream>
#include <algorithm>
#include <Windows.h>

using namespace std;

/***************************************************/
/*          Declared Function Prototypes           */  
/***************************************************/

void clear(HANDLE console);
char **mazeInit(int rows, int cols, char wall);
void mazeStep(char **a, int *rows, int *cols, int r, int c);
void mazeWalk(char **maze,int rows, int cols);
void mapInit(wstring* map, int mapType);

/***************************************************/
/*             Declared Local Variables            */  
/***************************************************/

int nScreenWidth = 120;			         // Console Screen Size X (columns)
int nScreenHeight = 40;			         // Console Screen Size Y (rows)
int nMapWidth = 33;				         // World Dimensions
int nMapHeight = 33;

float fPlayerX = 31.0f;			         // Player Start Position
float fPlayerY = 17.0f;
float fPlayerA = 0.0f;			         // Player Start Rotation in Radians
// float fPlayerA_Deg = 0.0f;		         // Player Start Rotation in Degrees
float fFOV = 3.14159f / 3.0f;	         // Field of View
float fDepth = 16.0f;			         // Maximum rendering distance
float fSpeed = 3.0f;			            // Walking Speed
float fPlayerRotationSpeed = 2.0f;     // Rotation Speed
bool showMap = false;                  // map visibility state

int main(int argc, char **argv)
{
   // Create Screen Buffer
   wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
   HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
   DWORD dwBytesWritten = 0;

   // Create Map of world space # = wall block, . = space
   wstring map;

   // hConsole.CursorVisible = false;
   SetConsoleActiveScreenBuffer(hConsole);

   if (argc == 2)
   {
      string sArg = argv[1];
      
      if (sArg.compare("maze") == 0)
      {
         mapInit(&map, MAP_TYPE_MAZE);
      }
      else if (sArg.compare("random") == 0)
      {
         mapInit(&map, MAP_TYPE_RANDOM);
      }
      else if (sArg.compare("explore") == 0)
      {
         mapInit(&map, MAP_TYPE_EXPLORE);
      }
      else
      {
         cout << "Usage: %s [maze|random|explore]" << endl;
         exit(0);
      }
   }
   else
   {
      mapInit(&map, MAP_TYPE_EXPLORE);
   }

   chrono::system_clock::time_point tp1 = chrono::system_clock::now();
   chrono::system_clock::time_point tp2;
   
   while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
   {
      // We'll need time differential per frame to calculate modification
      // to movement speeds, to ensure consistant movement, as ray-tracing
      // is non-deterministic
      tp2 = chrono::system_clock::now();
      
      chrono::duration<float> elapsedTime = tp2 - tp1;
      
      tp1 = tp2;
      
      float fElapsedTime = elapsedTime.count();

      // Handle Left movement and collision
      if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
      {
         fPlayerX -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
         fPlayerY += sinf(fPlayerA) * fSpeed * fElapsedTime;;

         if (map.c_str()[((int) fPlayerX * nMapWidth) + (int)fPlayerY] == '#')
         {
            fPlayerX += cosf(fPlayerA) * fSpeed * fElapsedTime;;
            fPlayerY -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
         }
      }

      // Handle Right movement and collision
      if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
      {
         fPlayerX += cosf(fPlayerA) * fSpeed * fElapsedTime;;
         fPlayerY -= sinf(fPlayerA) * fSpeed * fElapsedTime;;

         if (map.c_str()[((int) fPlayerX * nMapWidth) + (int)fPlayerY] == '#')
         {
            fPlayerX -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
            fPlayerY += sinf(fPlayerA) * fSpeed * fElapsedTime;;
         }
      }
      
      // Handle Forwards movement & collision
      if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
      {
         fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
         fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
         if (map.c_str()[((int) fPlayerX * nMapWidth) + (int)fPlayerY] == '#')
         {
            fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
            fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
         }			
      }

      // Handle Backwards movement & collision
      if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
      {
         fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
         fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
         if (map.c_str()[((int) fPlayerX * nMapWidth) + (int) fPlayerY] == '#')
         {
            fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
            fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
         }
      }

      // Handle CCW rotation
      if ((GetAsyncKeyState((unsigned short) 'Q') & 0x8000) || (GetAsyncKeyState((unsigned short) 'O') & 0x8000))
      {
         fPlayerA -= fPlayerRotationSpeed * fElapsedTime;
      }

      // Handle CW rotation
      if ((GetAsyncKeyState((unsigned short) 'E') & 0x8000) || (GetAsyncKeyState((unsigned short) 'P') & 0x8000))
      {
         fPlayerA += fPlayerRotationSpeed * fElapsedTime;
      }
      
      // Handle movement speed
      if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
      {
         fSpeed = 10.0f;			   // Increase walking speed (RUN)
      }
      else
      {
         fSpeed = 3.0f;             // Decrease walking speed
      }

      // Handle map visibility
      if (GetAsyncKeyState((unsigned short) 'M') & 0x0001)
      {
         showMap = !showMap;
      }
      

      for (int x = 0; x < nScreenWidth; x++)
      {
         // For each column, calculate the projected ray angle into world space
         float fRayAngle = (fPlayerA - (fFOV / 2.0f)) + (((float) x / (float) nScreenWidth) * fFOV);

         // Find distance to wall
         float fStepSize = 0.05f;	      // Increment size for ray casting, decrease to increase										
         float fDistanceToWall = 0.0f;    // resolution

         bool bHitWall = false;		      // Set when ray hits wall block
         bool bBoundary = false;		      // Set when ray hits boundary between two wall blocks

         float fEyeX = sinf(fRayAngle);   // Unit vector for ray in player space
         float fEyeY = cosf(fRayAngle);

         // Incrementally cast ray from player, along ray angle, testing for intersection with a block
         while (!(bHitWall) && (fDistanceToWall < fDepth))
         {
            fDistanceToWall += fStepSize;
            int nTestX = (int) (fPlayerX + (fEyeX * fDistanceToWall));
            int nTestY = (int) (fPlayerY + (fEyeY * fDistanceToWall));
            
            // Test if ray is out of bounds
            if ((nTestX < 0) || (nTestX >= nMapWidth) || (nTestY < 0) || (nTestY >= nMapHeight))
            {
               bHitWall = true;			// Just set distance to maximum depth
               fDistanceToWall = fDepth;
            }
            else
            {
               // Ray is inbounds so test to see if the ray cell is a wall block
               if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
               {
                  // Ray has hit wall
                  bHitWall = true;

                  // To highlight tile boundaries, cast a ray from each corner
                  // of the tile, to the player. The more coincident this ray
                  // is to the rendering ray, the closer we are to a tile 
                  // boundary, which we'll shade to add detail to the walls
                  vector<pair<float, float>> p;

                  // Test each corner of hit tile, storing the distance from
                  // the player, and the calculated dot product of the two rays
                  for (int tx = 0; tx < 2; tx++)
                  {
                     for (int ty = 0; ty < 2; ty++)
                     {
                        // Angle of corner to eye
                        float vy = (float) nTestY + ty - fPlayerY;
                        float vx = (float) nTestX + tx - fPlayerX;
                        float d = sqrt((vx * vx) + (vy * vy)); 
                        float dot = ((fEyeX * vx) / d) + ((fEyeY * vy) / d);
                        p.push_back(make_pair(d, dot));
                     }
                  }

                  // Sort Pairs from closest to farthest
                  sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) { return left.first < right.first; });
                  
                  // First two/three are closest (we will never see all four)
                  float fBound = 0.01;
                  if (acos(p.at(0).second) < fBound)
                  {
                     bBoundary = true;
                  }
                  if (acos(p.at(1).second) < fBound)
                  {
                     bBoundary = true;
                  }
                  if (acos(p.at(2).second) < fBound)
                  {
                     bBoundary = true;
                  }
               }
            }
         }
      
         // Calculate distance to ceiling and floor
         int nCeiling = (float) (nScreenHeight / 2.0) - (nScreenHeight / (float) fDistanceToWall);
         int nFloor = nScreenHeight - nCeiling;

         // Shader walls based on distance
         short nShade = ' ';
         // Very close
         if (fDistanceToWall <= (fDepth / 4.0f))
         {
            nShade = 0x2588;	
         }
         else if (fDistanceToWall < (fDepth / 3.0f))
         {
            nShade = 0x2593;
         }
         else if (fDistanceToWall < (fDepth / 2.0f))
         {
            nShade = 0x2592;
         }
         else if (fDistanceToWall < fDepth)
         {
            nShade = 0x2591;
         }
         // Too far away
         else
         {
            nShade = ' ';
         }

         if (bBoundary)
         {
            nShade = ' '; // Black it out
            // nShade = 0x2588; // Black it out
         }
         
         for (int y = 0; y < nScreenHeight; y++)
         {
            // Each Row
            if (y <= nCeiling)
            {
               screen[(y * nScreenWidth) + x] = ' ';
            }
            else if ((y > nCeiling) && (y <= nFloor))
            {
               screen[(y * nScreenWidth) + x] = nShade;
            }
            // Floor
            else
            {				
               // Shade floor based on distance
               float b = 1.0f - (((float) y - nScreenHeight / 2.0f) / ((float) nScreenHeight / 2.0f));
               if (b < 0.25)
               {
                  nShade = '#';
               }
               else if (b < 0.5)
               {
                  nShade = 'x';
               }
               else if (b < 0.75)
               {
                  nShade = '.';
               }
               else if (b < 0.9)
               {
                  nShade = '-';
               }
               else
               {
                  nShade = ' ';
               }
               screen[(y * nScreenWidth) + x] = nShade;
            }
         }
      }

      // Display Map
      if (showMap)
      {
         int screenIndex;

         for (int nx = 0; nx < (nMapWidth + 4); nx++)
         {
            screenIndex = ((((nScreenHeight - nMapHeight) / 2) - 1) * nScreenWidth) + (((nScreenWidth - nMapWidth) / 2) - 2) + nx;
            screen[screenIndex] = ' ';
         }

         for (int nx = 0; nx < nMapWidth; nx++)
         {
            for (int ny = 0; ny < nMapHeight; ny++)
            {
               screenIndex = (ny + ((nScreenHeight - nMapHeight) / 2)) * nScreenWidth + (((nScreenWidth - nMapWidth) / 2) - 2);
               screen[screenIndex] = ' ';

               screenIndex = (ny + ((nScreenHeight - nMapHeight) / 2)) * nScreenWidth + (((nScreenWidth - nMapWidth) / 2) - 1);
               screen[screenIndex] = ' ';

               screenIndex = (ny + ((nScreenHeight - nMapHeight) / 2)) * nScreenWidth + (((nScreenWidth - nMapWidth) / 2) - 0) + nx;
               screen[screenIndex] = map[ny * nMapWidth + nx];
               // screen[screenIndex] = a[ny][nx];


               screenIndex = (ny + ((nScreenHeight - nMapHeight) / 2)) * nScreenWidth + (((nScreenWidth - nMapWidth) / 2) - 0) + nMapWidth;
               screen[screenIndex] = ' ';

               screenIndex = (ny + ((nScreenHeight - nMapHeight) / 2)) * nScreenWidth + (((nScreenWidth - nMapWidth) / 2) + 1) + nMapWidth;
               screen[screenIndex] = ' ';
            }
         }

         for (int nx = 0; nx < (nMapWidth + 4); nx++)
         {
            screenIndex = (((nScreenHeight - nMapHeight) / 2) + nMapWidth)*nScreenWidth + (((nScreenWidth - nMapWidth) / 2) - 2) + nx;
            screen[screenIndex] = ' ';
         }

         screenIndex = ((int)fPlayerX + ((nScreenHeight - nMapHeight) / 2)) * nScreenWidth + (int)fPlayerY + (((nScreenWidth - nMapWidth) / 2) - 0);
         screen[screenIndex] = 'Y';

         /*
         fPlayerA_Deg = abs(fmod((fPlayerA*57.2958f), 360.0f));

         if ((fPlayerA_Deg >= 0) && (fPlayerA_Deg < 90))
         {
            screen[((int)fPlayerX+1) * nScreenWidth + (int)fPlayerY] = '<';
         }
         else if ((fPlayerA_Deg >= 90) && (fPlayerA_Deg < 180))
         {
            screen[((int)fPlayerX+1) * nScreenWidth + (int)fPlayerY] = 'v';
         }
         else if ((fPlayerA_Deg >= 180) && (fPlayerA_Deg < 270))
         {
            screen[((int)fPlayerX+1) * nScreenWidth + (int)fPlayerY] = '>';
         }
         else if ((fPlayerA_Deg >= 270) && (fPlayerA_Deg < 360))
         {
            screen[((int)fPlayerX+1) * nScreenWidth + (int)fPlayerY] = '^';
         }
         */
      }

      // Display Stats
      swprintf(screen, 50, L"(X: %05.2f) (Y: %05.2f) (A: %06.2f) (FPS: %06.2f)", fPlayerX, fPlayerY, fmod((fPlayerA*57.2958f), 360.0f), (1.0f / fElapsedTime));

      // Display Controls
      swprintf(&screen[((nScreenHeight - 1) * nScreenWidth) + 2], 117, L"%s", "(ESC: Exit) (SHIFT: run) (M: map) (W: forward) (S: backward) (A: left) (D: right) (O/Q: rotate CCW) (P/E: rotate CW)");

      // Display Frame
      screen[nScreenWidth * nScreenHeight - 1] = '\0';
      WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
   }

   clear(hConsole);

   return 0;
}

void clear(HANDLE hConsole)
{
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   DWORD count;
   DWORD cellCount;
   COORD homeCoords = { 0, 0 };

   if (hConsole == INVALID_HANDLE_VALUE) return;

   // Get the number of cells in the current buffer
   if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
   cellCount = csbi.dwSize.X * csbi.dwSize.Y;

   // Fill the entire buffer with spaces
   if (!FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', cellCount, homeCoords, &count)) return;

   // Fill the entire buffer with the current colors and attributes
   if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count)) return;

   // Move the cursor home
   SetConsoleCursorPosition(hConsole, homeCoords);
}

char **mazeInit(int rows, int cols, char wall)
{
   char **a = new char* [((2 * rows + 1) * sizeof(char*))]; /* 3x3 with overlap of 1 */
   for(int i = 0; i < 2 * rows + 1; i++)
   {
      if(!(a[i] = new char[(2 * cols + 1)]))
      {
         fprintf(stderr, "Out of memory.\r\n");
         exit(1);
      }
      for(int j = 0; j < 2 * cols + 1; j++)
      {
         a[i][j] = (wall) ? wall : '*';
      }
   }
   return a;
}
void mazeStep(char **a, int *rows, int *cols, int r, int c)
{
   int i, vector[3][2];

   #define ROW vector[i][0]
   #define COL vector[i][1]
   
   while(1)
   {
      i = 0; /* look around */
      if((r > 1) && (a[r - 2][c] != '.'))
      {
         vector[i][0] = r - 2;
         vector[i][1] = c;
         i++;
      }
      if((r < ((*rows) * 2) - 1)	&& (a[r + 2][c] != '.'))
      {
         vector[i][0] = r + 2;
         vector[i][1] = c;
         i++;
      
      }
      if((c > 1) && (a[r][c-2] != '.'))
      {
         vector[i][0] = r;
         vector[i][1] = c - 2;
         i++;
      }
      if((c < ((*cols) * 2) - 1)	&& (a[r][c+2] != '.'))
      {
         vector[i][0] = r;
         vector[i][1] = c + 2;
         i++;
      }
      
      if(!i)
      {
         break; /* check for dead end */
      }

      i = (int) (i * (rand() / (RAND_MAX + 1.0))); /* choose a path */
      a[(vector[i][0] + r) / 2][(vector[i][1] + c) / 2] = '.';	/* knock out a wall */
      a[vector[i][0]][vector[i][1]] = '.';			/* clear to it */
      mazeStep(a, rows, cols, vector[i][0], vector[i][1]);		/* repeat */
   }
}
void mazeWalk(char **maze,int rows, int cols)
{
   /* this starts at one side but you can start anywhere */
   /* there is a way out no matter where your exits are */
   int i,r,c;
   c = cols | 1;
   maze[0][c] = '.';
   maze[2 * rows][c] = '#';
   i = (int) (2 * (rand() / (RAND_MAX + 1.0)));
   c = (i) ? 1 : (2 * cols) - 1;
   r = rows | 1;
   maze[r][c] = '.';

   mazeStep(maze, &rows, &cols, r, c);
}

void mapInit(wstring* map, int mapType)
{
   if (mapType == MAP_TYPE_MAZE)
      {
         srand((unsigned) time(0));
         char **maze = mazeInit (nMapWidth / 2, nMapHeight / 2, '#');
         mazeWalk	(maze, nMapWidth / 2, nMapHeight / 2);

         for(int y=0;y<nMapWidth;y++)
         {
            for(int x=0;x<nMapHeight;x++)
            {
               *map += maze[y][x];
            }
         }
      }
      else if (mapType == MAP_TYPE_RANDOM)
      {
         srand((unsigned) time(0));
         *map += L"#################################";
         for (int y = 1; y < (nMapHeight - 1); y++)
         {
            *map += L"#";
            
            for (int x = 1; x < (nMapWidth - 1); x++)
            {
               if ((rand() % 100) > 90)
               {
                  *map += L"#";
               }
               else
               {
                  *map += L".";
               }
            }

            *map += L"#";
         }
         *map += L"#################################";
      }
      else if (mapType == MAP_TYPE_EXPLORE)
      {
         *map += L"#################################";
         *map += L"#...............................#";
         *map += L"#........####....##......########";
         *map += L"#...............................#";
         *map += L"#.......##......................#";
         *map += L"#.......##......................#";
         *map += L"#...........###.................#";
         *map += L"###...##........................#";
         *map += L"##..............................#";
         *map += L"#......####...##................#";
         *map += L"#......#........................#";
         *map += L"#......#........................#";
         *map += L"#...........###.................#";
         *map += L"#......#########...............##";
         *map += L"#..##.................#####....##";
         *map += L"#......###.....####........#...##";
         *map += L"#...........###................##";
         *map += L"#..............#...........#...##";
         *map += L"#..............#...........#...##";
         *map += L"#..............#...........#...##";
         *map += L"#..............#...........#...##";
         *map += L"#..............#...........#...##";
         *map += L"#..............#...........#...##";
         *map += L"#..............#..........#....##";
         *map += L"#..............#..........#....##";
         *map += L"#..............#..........#....##";
         *map += L"#..............#..........#....##";
         *map += L"#..............#..........#....##";
         *map += L"#..............#..........#....##";
         *map += L"#..............#..........#....##";
         *map += L"#..............#..........#....##";
         *map += L"#..............#..........#....##";
         *map += L"#################################";
      }
}