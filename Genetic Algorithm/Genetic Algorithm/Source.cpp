#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <math.h>


using namespace std;
const string faculty[] = { "Gharibi", "Gladbach", "Hare", "Nait-Abdesselam", "Shah", "Song", "Uddin", "Xu", "Zaman", "Zein el Din"}; // List of potential teachers for a class.
const string rooms[] = {"Katz 003", "FH 216", "Royall 206", "Royall 201", "FH 310", "Haag 201", "Haag 301", "MNLC 325", "Bloch 119"}; // List of all the rooms available for a class.
const string classes[] = {"CS101A", "CS101B", "CS191A", "CS191B", "CS201", "CS291", "CS303", "CS304", "CS394", "CS449", "CS451"}; // List of the classes.
const int times[] = { 11, 12, 13, 14, 15 }; // Times class can take place. Operates on the 24-hour system.

//A course. Part of our schedule
struct course {
	string instructor;
	string room;
	string name;
	int time;
};

//A schedule, a creature in our popluation
struct schedule {
	course courses[sizeof(classes)/(sizeof(string))];
	double fitness = 0;
};

double calcFitness(schedule mySchedule);

//Comparison class for our min-heap
class cmp
{
public:
	int operator() (const schedule left, const schedule right)
	{
		return left.fitness > right.fitness;
	}
};
//Our min-heap of schedules.
priority_queue<schedule, vector<schedule>, cmp> population;
 
//Generates our first population at random. Input the desired population size as the parameter.
void first_population(int popSize)
{	
	srand(time(0));
	for (int i = 0; i < popSize; i++)
	{
		schedule schedule;
		for (int j = 0; j < 11; j++) 
		{
			schedule.courses[j].instructor = faculty[rand() % 10];
			schedule.courses[j].name = classes[j];
			schedule.courses[j].room = rooms[rand()%9];
			schedule.courses[j].time = times[rand()%5];
		}
		schedule.fitness = calcFitness(schedule);
		population.push(schedule);
	}
}

double calcFitness(schedule mySchedule)
{
	const int enrollment[] = { 50, 50, 50, 50, 50, 50, 60, 25, 20, 60, 100 }; // Expected enrollment size
	const int capacity[] = { 45, 30, 75, 50, 108, 60, 75, 450, 60 }; // Capacity of classrooms.
	double fitness = 0; // Overall fitness score calculated
	//List of preferred instructors
	const string preferred[11][4] = { {"Gladbach", "Gharibi", "Hare", "Zein el Din"}, {"Gladbach", "Gharibi", "Hare", "Zein el Din"},{"Gladbach", "Gharibi", "Hare", "Zein el Din"}, {"Gladbach", "Gharibi", "Hare", "Zein el Din"}, {"Gladbach", "Hare", "Zein el Din", "Shah"},
		{"Gharibi", "Hare", "Zein el Din", "Song"}, {"Gladbach", "Zein el Din", "Hare"}, {"Gladbach", "Hare", "Xu"}, {"Xu", "Song"}, {"Xu", "Song", "Shah"}, {"Xu", "Song", "Shah"}};
	
	//List of other instructors
	const string other[11][6] = { {"Zaman", "Nait-Abdesselam"}, {"Zaman", "Nait-Abdesselam"}, {"Zaman", "Nait-Abdesselam"}, {"Zaman", "Nait-Abdesselam"}, {"Zaman", "Nait-Abdesselam", "Song"}, 
		{"Zaman", "Nait-Abdesselam", "Shah", "Xu"}, {"Zaman", "Song", "Shah"}, {"Zaman", "Song", "Shah", "Nait-Abdesselam", "Uddin", "Zein el Din"}, {"Nait-Abdesselam", "Zein el Din"}, 
		{"Zein el Din", "Uddin"}, {"Zein el Din", "Uddin", "Nait-Abdesselam", "Hare"}};
	
	int n = sizeof(rooms) / sizeof(rooms[0]);
	int courseLoad[10] = {0,0,0,0,0,0,0,0,0,0}; // Current course load of all 10 faculty

	//Calculates fitness based on instructor, room capacity, and classes occupying a room at the same time.
	for (int i = 0; i < (sizeof(mySchedule.courses) / sizeof(course)); i++)
	{
		for (int j = (i + 1); j < (sizeof(mySchedule.courses) / sizeof(course)); j++)
		{
			// For when two classes are scheduled for the same room at the same time
			if ((mySchedule.courses[i].room == mySchedule.courses[j].room) && (mySchedule.courses[i].time == mySchedule.courses[j].time))
			{
				fitness -= 0.5;
			}

		}
		

		// If the room capacity is good.
		if (capacity[(find(rooms, rooms + sizeof(rooms) / sizeof(rooms[0]), mySchedule.courses[i].room) - rooms)] - enrollment[i] < 0)
		{
			fitness -= 0.5;
		}
		else if (capacity[(find(rooms, rooms + sizeof(rooms) / sizeof(rooms[0]), mySchedule.courses[i].room) - rooms)] / enrollment[i] >= 6)
		{
			fitness -= 0.4;
		}
		else if (capacity[(find(rooms, rooms + sizeof(rooms) / sizeof(rooms[0]), mySchedule.courses[i].room) - rooms)] / enrollment[i] >= 3)
		{
			fitness -= 0.2;
		}
		else
		{
			fitness += 0.3;
		}

		//Calculates based on the preferred instructor, other instructor, or random faculty member.
		if ([&]()
			{
				for (int j = 0; j < 
					sizeof(preferred[(find(classes, classes + sizeof(classes) / sizeof(classes[0]), mySchedule.courses[i].name) - classes)]) / sizeof(preferred[(find(classes, classes + sizeof(classes) / sizeof(classes[0]), mySchedule.courses[i].name) - classes)][0]); j ++)
				{
					if (preferred[(find(classes, classes + sizeof(classes) / sizeof(classes[0]), mySchedule.courses[i].name) - classes)][j] == mySchedule.courses[i].instructor)
					{
						return true;
					}
				}
				return false;
			}())
		{
			fitness += 0.5;
		}
		else if ([&]()
			{
				for (int j = 0; j < sizeof(other[(find(classes, classes + sizeof(classes) / sizeof(classes[0]), mySchedule.courses[i].name) - classes)]) / sizeof(other[(find(classes, classes + sizeof(classes) / sizeof(classes[0]), mySchedule.courses[i].name) - classes)][0]); j++)
				{
					if (other[(find(classes, classes + sizeof(classes) / sizeof(classes[0]), mySchedule.courses[i].name) - classes)][j] == mySchedule.courses[i].instructor)
					{
						return true;
					}
				}

				return false;
			}())
		{
			fitness += 0.2;
		}
		else
		{
			fitness -= 0.1;
		}

		//Instructor load.
		courseLoad[(find(faculty, faculty + sizeof(faculty) / sizeof(faculty[0]), mySchedule.courses[i].instructor) - faculty)] += 1;
	}

	//Calculates fitness based on instructor load.
	for (int i = 0; i < (sizeof(courseLoad) / sizeof(courseLoad[0])); i++)
	{
		if (courseLoad[i] > 4) // If an instructor is scheduled for more than 4 classes
		{
			fitness -= 0.5;
		}
		else if( (i != 7) && (courseLoad[i] == 1 || courseLoad[i] == 2)) // If an instructor is schedule 1 to 2 courses.
		{
			fitness -= 0.4;
		}

		// If a teacher has more than 1 class on there course load check to see if they are scheduled to teach classes at the same time.
		if (courseLoad[i] > 1)
		{
			if ([&]() // If an instructor teaches more than one class at a time
				{
					for (int j = 0; j < (sizeof(mySchedule.courses) / sizeof(course)); j++)
					{
						if (mySchedule.courses[j].instructor == faculty[i])
						{
							for (int z = j + 1; z < (sizeof(mySchedule.courses) / sizeof(course)); z++)
							{
								if ((mySchedule.courses[j].instructor == mySchedule.courses[z].instructor) && (mySchedule.courses[j].time == mySchedule.courses[z].time))
								{
									return true;
								}
							}
						}
					}
					return false;
				}())
			{
				fitness -= 0.5;
			}
			else // Otherwise a professor has only 1 time class per time slot.
			{
				fitness += 0.2;
			}	
		}
	}

	//Course specific adjustments 
	// CS101 sections time difference
	if ( ((mySchedule.courses[0].time - mySchedule.courses[1].time)^1) > 4) // If the difference in time between the two CS101 courses is greater than 4
	{
		fitness += 0.5;
	}
	else if (mySchedule.courses[0].time == mySchedule.courses[1].time)
	{
		fitness -= 0.5;
	}

	//CS191 time differences
	if (((mySchedule.courses[2].time - mySchedule.courses[3].time) ^ 1) > 4) // If the difference in time between the two CS191 courses is greater than 4
	{
		fitness += 0.5;
	}
	else if (mySchedule.courses[2].time == mySchedule.courses[3].time)
	{
		fitness -= 0.5;
	}

	// If CS191 and CS101 sections are taught sequentially, at the same time, or seperated by an hour.
	for (int i = 0; i < 2; i++)
	{
		for (int j = 2; j < 4; j++)
		{
			if (mySchedule.courses[i].time == mySchedule.courses[j].time)
			{
				fitness -= 0.25;
			}
			else if (((mySchedule.courses[i].time - mySchedule.courses[j].time) ^ 1) == 2) // If the classes are seperated by 1 hour.
			{
				fitness += 0.25;
			}
			else if (((mySchedule.courses[i].time - mySchedule.courses[j].time) ^ 1) == 1)
			{
				if ((mySchedule.courses[i].room == "Katz 003") || (mySchedule.courses[j].room == "Katz 003")) // If one is in Katz.
				{
					if (mySchedule.courses[i].room != mySchedule.courses[j].room)
					{
						fitness -= 0.4;
					}
				}
				else if ((mySchedule.courses[i].room == "Bloch 119") || (mySchedule.courses[j].room == "Bloch 119")) // If one is in Bloch
				{
					if (mySchedule.courses[i].room != mySchedule.courses[j].room)
					{
						fitness -= 0.4;
					}
				}
				else // All other cases.
				{
					fitness += 0.5;
				}
			}
		}
	}
	return fitness;
}

//Generate a new child from the available parents on the min-heap minus the least fit.
void mate()
{	
	population.pop(); // Remove the least fit.
	int parent1, parent2;
	int crossOver = rand() % 10;
	int mutationRate = 1; // Chance to mutate out of 100
	schedule child;
	vector<schedule> storage;
	

	// Put all the population into an array to select two at random.
	for (int i = 0; i < (population.size()); i++)
	{
		storage.push_back(population.top());
		population.pop();
	}


	//Randomly select two different parents
	parent1 = rand() % storage.size();

	do
	{
		parent2 = rand() % storage.size();
	} while (parent1 != parent2);

	//Take genes from parent 1 as determined by crossOver line
	for (int i = 0; i < crossOver; i++)
	{
		child.courses[i] = storage[parent1].courses[i];
	}

	//Take genes from parent 2
	for (int i = crossOver; i < 11; i++)
	{
		child.courses[i] = storage[parent2].courses[i];
	}



	//Mutate
	for (int i = 0; i < 11; i++)
	{
		if (rand()%100 == mutationRate)
		{
			child.courses[i].instructor = faculty[rand() % 10];
			child.courses[i].room = rooms[rand() % 9];
			child.courses[i].time = times[rand() % 5];
		}
	}

	child.fitness = calcFitness(child);

	//Put the child on the heap
	population.push(child);

	//Restore the heap.
	while (!storage.empty())
	{
		population.push(storage[storage.size() - 1]);
		storage.pop_back();
	}
}

// Generates a new generation
void generation()
{
	for (int i = 0; i < population.size(); i++)
	{
		mate();
	}
}

//Calculates the fitness of the current generation
double calcGenFitness(priority_queue<schedule, vector<schedule>, cmp> temp)
{
	double genFitness = 0;
	for (int i = 0; i < temp.size(); i++)
	{
		genFitness += temp.top().fitness;
		temp.pop();
	}

	return genFitness / temp.size();
}

int main()
{
	double ov, nv; // Old value and new value for generation fitness
	schedule best; // Best schedule at the end of the process.
	ofstream results; // File for streaming results
	int populationSize = 30; // Size of the population pool.
	first_population(populationSize); //Create our first population.
	deque<double> recentFitness; // Generation from current fittness - 100;
	int i = 0;


	ov = calcGenFitness(population);
	cout << "Fitness old value: " << ov << endl;

	//Loops for at least 100 iterations.
	do
	{
		recentFitness.push_back(ov);

		// Removes the oldest item in the deque over 100. Thus allow us to calculate the improvement percentage of G - 100
		if (recentFitness.size() > 100)
		{
			recentFitness.pop_front();
		}

		ov = calcGenFitness(population);
		generation();
		nv = calcGenFitness(population);
		cout << "Fitness new value: " << nv << endl << "Generations: " << i << endl << "Improvement percent: " <<  fabs((((nv - ov) / ov) * 100.0)) << endl;

	} while ( (i++ < 100)  || (fabs((recentFitness.back() - recentFitness.front()) * 100) > 1.0));

	//Get the most fit creature
	while (!population.empty())
	{
		best = population.top();
		population.pop();
	}

	results.open("C:\\Users\\srs68\\Desktop\\Project2\\Genetic Algorithm\\results.txt"); // Open file for printing
	
	//Write to file.
	results << "Schedule fitness: " << best.fitness << endl;
	for (int i = 0; i < 11; i++)
		{
			results << "------------" << endl << "Course: " << best.courses[i].name << endl << "Instructor: " << best.courses[i].instructor << endl << best.courses[i].room
				<< endl << "Time: " << best.courses[i].time << endl;
		}
	results << endl;
	results.close();
}