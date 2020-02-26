/*
Author: Kevin Deming
Last Edit Date: 2/25/2020
Description: This code is meant to be an example of my island problem solution. It currently tests binary (0,1) maps for islands of 1's.
For more information on the method a write up and explination is avalible on my git page.
www.github.com/idot107

Current State: Working code and method. Some small improvements are being considered.

Notes: 
1) Adding padding so that the taken in vector starts at (1,1) instead of (0,0). 
   This makes it so I dont have to tackle the starting row case or the first column case.
  
2) Making it able to read more than 1 file or be able to select the file is a feature to be added.

3) Make the input only require the file and be able to adjust for the size of the map automatically.

4) Multithreading the process so that folders of maps can be read efficiently.

*/


#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <ctime>


//Struct for 2 by X array.
struct islandData 
{
	int refNumber;
	int yCoord;

	//constructor for islandData
	islandData(int ref, int y)
	{
		refNumber = ref;
		yCoord = y;
	}
};

//Function Initialization
void readMap(std::vector< std::vector<int> > &mapVector);

//Has menu and function calls.
int main()
{
	int input, x, y;
	//Get the Dimentions of the map
	std::cout << "Please enter in the dimentions of the map." << std::endl;
	std::cout << "Width(x-Coord): ";
	std::cin >> input;
	if (input > 0)
	{
		x = input;
	}
	std::cout << "Height(y-Coord): ";
	std::cin >> input;
	if (input > 0)
	{
		y = input;
	}
	//Variables:
	std::vector< std::vector<int> > map(y, std::vector<int>(x, 0)); //2d map vector of size (x,y)
	std::ifstream mapFile;
	char current;
	x = 0;
	y = 0;

	//Take in a comma and newline separated file and convert it into a vector.
	mapFile.open("islandMap.txt");
	if (!mapFile.is_open())
	{
		std::cout << "ERROR the file could not be opened." << std::endl;
		system("pause");
		return 1;
	}

	//Read each char. Could be done better, but this is an arbitrary part. will detect errors in map.
	while (!mapFile.eof())
	{
		//look for value
		mapFile.get(current);
		if (current == '0')
		{
			map[y][x] = 0;
		}
		else if (current == '1')
		{
			map[y][x] = 1;
		}
		else if (current == '\n')
		{
			y++;
			x = 0;
		}
		else if (current == ',')
		{
			std::cout << "ERROR: Missing value at Row: " << y+1 << " Column: " << x+1 << " in file." << std::endl;
			system("pause");
			return 1;
		}
		else
		{
			std::cout << "ERROR: Invalid character \"" << current << "\" found in file. (OR missing value)" << std::endl;
			std::cout << "On Row : " << y + 1 << " Column : " << x + 1 << std::endl;
			system("pause");
			return 1;
		}
		 
		//look for comma or newline
		mapFile.get(current);
		if (current == ',')
		{
			x++;
		}
		else if (current == '\n')
		{
			y++;
			x = 0;
		}
		else if (mapFile.eof())
		{
			break;
		}
		else
		{
			std::cout << "ERROR: Invalid character \"" << current << "\" found in file. (OR missing comma)" << std::endl;
			std::cout << "On Row : " << y + 1 << " Column : " << x + 1 << std::endl;
			system("pause");
			return 1;
		}
	}

	mapFile.close();
 
	
	//start timer
	std::clock_t startTime;
	double totalTime;
	startTime = std::clock();

	//This takes the map given and runs the algorithm and prints the results.
	readMap(map);

	//end timer.
	totalTime = (std::clock() - startTime) / (double)CLOCKS_PER_SEC;
	std::cout << "Time(seconds): " << totalTime << std::endl;
	


	//Pause the program to see results.
	system("pause");

	//Exit program.
	return 0;
}




void readMap(std::vector< std::vector<int> > &mapVector)
{

	//STRUCT STRUCT STRUCT
	//create a 2 by X array that holds island ref. number and line number.
	int width = mapVector[0].size();
	int height = mapVector.size();
	std::vector< std::vector<islandData> > map(2, std::vector<islandData>(width, islandData(-1, -1)));
	//create a vector of lists. use this to store island nodes into islands
	std::vector<std::list<int>> islands((width*width / 2), std::list<int>(0));
	int counter = 0;
	int islandA = 0;
	int islandB = 0;

	
	//1) read through the map 2d vector looking for island nodes.(value 1)
	//For Just first position on map.
	if (mapVector[0][0] == 1)
	{
		//start the island list.
		islands[0].push_back(1); //doing locations as 0,0 -> 1  0,1 -> 2 and so on. 1,1 = 26
		counter++; //since added a new list.
		//record in 2 by X
		map[0][0].refNumber = 0;
		map[0][0].yCoord = 0;
		
	}
	
	//Just do math for position number.
	//For just first row special case:
	for (int b = 1; b < width; b++)
	{
		if (mapVector[0][b] == 1)
		{
			//check 2 by X to see if connected.
			if (!(map[0][b - 1].refNumber == -1))
			{
				//if connected to 1 before add to refnumber island
				islandA = map[0][b - 1].refNumber;
				islands[islandA].push_back(b + 1);
			}
			else
			{
				//else add new island FOR ADDING NEW ISLAND: Open new island at end. dont be memory efficient.
				islandA = counter;
				islands[islandA].push_back(b + 1);
				counter++;
			}

			//record in 2 by X
			map[0][b].refNumber = islandA;
			map[0][b].yCoord = 0;
		}
	}

	//Variables for holding which of the 2 by X is being held.
	int rowCheck;
	int rowFlip;
	//For the rest of the searching. (after first row.)
	for (int a = 1; a < height; a++)
	{
		//EVEN or ODD for 2 by X array.
		if (a % 2 == 0)
		{
			rowCheck = 0;
			rowFlip = 1;
		}
		else
		{
			rowCheck = 1;
			rowFlip = 0;
		}

		//Case for first symbol in each row
		if (mapVector[a][0] == 1)
		{
			//Check 2 by X, N and NE
			if (map[rowFlip][0].yCoord == a - 1)
			{
				islandA = map[rowFlip][0].refNumber;
			}
			else if (map[rowFlip][1].yCoord == a - 1)
			{
				islandA = map[rowFlip][1].refNumber;
			}
			//If new island then create a new island
			else
			{
				
				islandA = counter;
				counter++;
			}
			//Then push value to the island list.
			islands[islandA].push_back((width*a)+1);
			//Then add value to 2 by X.
			map[rowCheck][0].refNumber = islandA;
			map[rowCheck][0].yCoord = a;
		}
		
		//Case for middle symbols.
		for (int b = 1; b < width-1; b++)
		{
			if (mapVector[a][b] == 1)
			{
				islandA = -1;
				islandB = -1;
				//Check 2 by X array for the W, NW, N, and NE adjacent positions of the current node.
				// Grouping (W and NW) (N and NE) For efficient checking.
				if (map[rowCheck][b - 1].yCoord == a) //W
				{
					islandA = map[rowCheck][b - 1].refNumber;
				}
				else if (map[rowFlip][b - 1].yCoord == a - 1) //NW
				{
					islandA = map[rowFlip][b - 1].refNumber;
				}
				if (map[rowFlip][b].yCoord == a - 1) //N
				{
					islandB = map[rowFlip][b].refNumber;
				}
				else if (map[rowFlip][b + 1].yCoord == a - 1) //NE
				{
					islandB = map[rowFlip][b + 1].refNumber;
				}
				
				//Check 3 cases, AND, OR, and Else
				if(islandA != -1 && islandB != -1) //Both A and B are used.
				{
					//check if a and b are the same to check for merge case
					if (islandA != islandB) //Merge and update 2 by X for island B
					{
						//Merge the two lists.
						islands[islandA].splice(islands[islandA].end() ,islands[islandB]);
						//Now islandA list will have islandBs list. IslandB will now be empty.

						//UPDATE 2 by X
						//update upper row starting at N.
						for (int x = b; x < width - 1; x++)
						{
							//check if ref number matches islandB
							if (map[rowFlip][x].refNumber == islandB)
							{
								//update value
								map[rowFlip][x].refNumber = islandA;
							}
							//otherwise move on.
						}
						
						//update current row Stopping at W.
						for (int x = 0; x < b; x++)
						{
							//check if ref number matches islandB
							if (map[rowCheck][x].refNumber == islandB)
							{
								//update value
								map[rowCheck][x].refNumber = islandA;
							}
						}
					}
					//otherwise assume island A is correct.
				}
				//Ignore A if A is only since we will just used island A.
				else if (islandB != -1) //Only island B connected
				{
					islandA = islandB;
				}
				else if (islandA == -1 && islandB == -1) //New Island
				{
					islandA = counter;
					counter++;
				}
			
				//At this point islandA holds current refNum
				//Add to refNUm list
				islands[islandA].push_back((width*a) + b + 1); //a and b are array counts, width is starting at 1 count.

				//Update 2 by X with current location
				map[rowCheck][b].refNumber = islandA;
				map[rowCheck][b].yCoord = a;
			
			}
			//repeat for all middle symbols.
		}
		
		//Case for last symbol.
		if (mapVector[a][width - 1] == 1)
		{
			//Check 2 by X (W, NW, and N)
			if (map[rowCheck][width - 2].yCoord == a) //W
			{
				islandA = map[rowCheck][width - 2].refNumber;
			}
			else if (map[rowFlip][width - 2].yCoord == a - 1) //NW
			{
				islandA = map[rowFlip][width-2].refNumber;
			}
			else if (map[rowFlip][width - 1].yCoord == a - 1)
			{
				islandA = map[rowFlip][width - 1].refNumber;
			}
			//If new island then create a new island
			else
			{
				islandA = counter;
				counter++;
			}

			//Then push value to the island list.
			islands[islandA].push_back(width*(a+1));
			
			//Then add value to 2 by X.
			
			map[rowCheck][width - 1].refNumber = islandA;
			map[rowCheck][width - 1].yCoord = a;
		}

	}
	
	std::cout << "Map Read Complete." << std::endl;
	std::cout << std::endl;
	
	//Collect Results:
	//Variables for results.
	int islandCount = 0;
	int smallest = 0;
	int biggest = 0;
	int row, column;
	//Run through island list.
	for (int i = 0; i < (width*width / 2); i++)
	{
		//Check if this list has values(aka is an island)
		if (islands[i].size() > 0)
		{
			//Print out the values of each island to confirm it works.
			/*
			std::cout << "Island #" << i << ": ";

			for (auto v : islands[i])
			{
				row = v / width;
				column = v % width;

				std::cout << "Row: " << row << " Column: " << column << ", ";
			}
			std::cout << std::endl;
			*/

			//Add to island count
			islandCount++;

			//Check smallest and biggest island
			if (islands[i].size() < smallest)
			{
				smallest = islands[i].size();
			}
			else if (islands[i].size() > biggest)
			{
				biggest = islands[i].size();
				
				if (islandCount == 1) //This is for when the first island is found. Set it to smallest and biggest.
				{
					smallest = islands[i].size();
				}
			}

		}
		
	}

	//print out results
	std::cout << "Results:" << std::endl << "-------------" << std::endl;
	std::cout << "Size: " << width << "x" << height << std::endl;
	std::cout << "Number of Islands: " << islandCount << std::endl;
	std::cout << "Biggest Island: " << biggest << std::endl;
	std::cout << "Smallest Island: " << smallest << std::endl;
	
	
	
	//After Printing go back to where it was called.
}
	
