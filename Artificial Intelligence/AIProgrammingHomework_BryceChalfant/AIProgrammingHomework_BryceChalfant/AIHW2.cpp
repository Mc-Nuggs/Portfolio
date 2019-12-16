#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <time.h>
#include <deque>
#include <vector>
void cluster(int, std::deque<std::pair<float, float>>);
std::string HMM(std::vector<std::vector<double>> transitionTable, std::vector<std::vector<double>> emissionTable, std::vector<double> startProb, std::string emmisions, std::string);
double sigmoid(double);
double derivative(double);
void trainingData(std::vector<std::vector<double>>&, std::vector<std::vector<double>>&);
void train(std::vector<std::vector<double>>&, std::vector<std::vector<double>>&, std::vector<double>, std::vector<double>, double);
std::vector<double> NeuralNet(std::vector<std::vector<double>>&, std::vector<std::vector<double>>&, std::vector<double>, std::vector<double>);
double error(std::vector<double>, std::vector<double>);
int findEmissions(char, std::string emisionList);
double dot(std::vector<double>, std::vector <std::vector<double>>);
int const INPUTLAYERNODES = 4;
int const OUTPUTLAYERNODES = 3;
int AMOUNTOFHIDDEN = 0;
int main() {
	char choice;
	std::cout << "What do you want to see? C for cluster, H for HMM, N for NN. Press q to quit\n";
	std::cin >> choice;
	while (choice != 'q' && choice != 'Q') {
		if (choice == 'c' || choice == 'C') {
			int pointsToMake;
			int kCentroids;
			srand(((unsigned)time(NULL)));
			std::deque<std::pair<float, float>> allPoints;
			//Input from the user.

			std::cout << "How many points do you want to make?\n";
			std::cin >> pointsToMake;
			std::cout << "How many centroids do you want to make?\n";
			std::cin >> kCentroids;

			//start making the random points 

			for (int x = 0; x < pointsToMake; x++) {
				allPoints.push_back(std::make_pair((rand() % 51) - 25, (rand() % 51) - 25)); // creating points with random values between -25 and 25
			}
			cluster(kCentroids, allPoints);//Passes all the points created to the cluster function to create the clusters and output them
			std::cout << std::endl;
			std::cout << "What do you want to see? C for cluster, H for HMM, N for NN. Press q to quit\n";
			std::cin >> choice;
		}
		else if (choice == 'h' || choice == 'H') {

			srand(time(NULL)); //set up random number generator for transition and emission chance
			int numberOfStates;
			char possibleEmissions[100];
			std::cout << "Please input the number of states\n";
			std::cin >> numberOfStates;
			std::cout << "Please input the possible emissions as a string\n";
			std::cin >> possibleEmissions; //retrieves the list of possible emissions from the user
			std::string emissionsList = possibleEmissions;
			int numberOfEmissions = emissionsList.size();
			std::vector<std::vector<double>> transitionTable;
			for (int x = 0; x < numberOfStates; x++) {
				transitionTable.push_back(std::vector<double>(numberOfStates, 0.0)); // sets up the transition table which is a nxn matrix
			}
			std::vector<std::vector<double>> emissionsTable;
			for (int x = 0; x < numberOfStates; x++) {
				emissionsTable.push_back(std::vector<double>(numberOfEmissions, 0.0)); // sets up the emissions table which is a nxm matrix
			}//Now we have to fill the tables with random values 
			std::vector<double> startPorb;
			double limit = 0;

			for (int x = 0; x < numberOfStates; x++) {
				double randVal = (rand() % 10) / 10.0;

				if (limit + randVal >= 1.0) {//makes it where if the overall probability is one nothing gets added
					randVal = 1 - limit;
				}limit += randVal;
				startPorb.push_back(randVal);
			}//Fills in the start probablity 

			for (int x = 0; x < numberOfStates; x++) { // randomly assigns the transition prob
				for (int y = 0; y < numberOfStates; y++) {
					double limity = 0;
					double limitx = 0;
					double randVal = (rand() % 10) / 10.0;

					for (int hor = 0; hor < numberOfStates; hor++) {
						limity += transitionTable[x][hor];
					}
					for (int ver = 0; ver < numberOfStates; ver++) {
						limitx += transitionTable[ver][y];
					}
					if (limity + randVal >= 1.0&& limitx + randVal >= 1.0) {
						double limit = fmin(limity, limitx);
						if (limit <= 1) {
							randVal = 1 - limit;
						}
						else
							randVal = 0;
					}
					else if (limity + randVal >= 1.0) {
						if (limity <= 1) {
							randVal = 1 - limity;
						}
						else
							randVal = 0;
					}
					else if (limitx + randVal >= 1.0) {
						if (limitx <= 1) {
							randVal = 1 - limitx;
						}
						else
							randVal = 0;
					}

					transitionTable[x][y] = randVal;
				}
			}
			std::cout << std::endl;
			for (int x = 0; x < numberOfStates; x++) {//Randomly assigns the emission prob
				for (int y = 0; y < numberOfEmissions; y++) {
					double limity = 0;
					double limitx = 0;
					double randVal = (rand() % 10) / 10.0;

					for (int hor = 0; hor < numberOfEmissions; hor++) {
						limity += emissionsTable[x][hor];
					}
					for (int ver = 0; ver < numberOfStates; ver++) {
						limitx += emissionsTable[ver][y];
					}
					if (limity + randVal >= 1.0&& limitx + randVal >= 1.0) {
						double limit = fmin(limity, limitx);
						if (limit <= 1) {
							randVal = 1 - limit;
						}
						else
							randVal = 0;
					}
					else if (limity + randVal >= 1.0) {
						if (limity <= 1) {
							randVal = 1 - limity;
						}
						else
							randVal = 0;
					}
					else if (limitx + randVal >= 1.0) {
						if (limitx <= 1) {
							randVal = 1 - limitx;
						}
						else
							randVal = 0;
					}

					emissionsTable[x][y] = randVal;
				}
			}
			std::cout << std::endl;
			std::cout << "Intial probability table\n";
			for (int i = 0; i < transitionTable.size(); i++) {

				std::cout << startPorb[i] << " ";
			}
			std::cout << std::endl;
			std::cout << "Transition table\n";
			for (int i = 0; i < transitionTable.size(); i++) {
				for (int j = 0; j < transitionTable.size(); j++) {
					std::cout << transitionTable[i][j] << " ";
				}
				std::cout << std::endl;
			}
			std::cout << "Emissions table\n";
			for (int i = 0; i < transitionTable.size(); i++) {
				for (int j = 0; j < emissionsTable[0].size(); j++) {
					std::cout << emissionsTable[i][j] << " ";
				}
				std::cout << std::endl;
			}
			std::cout << "Please input the sequence(From the possible list given earlier)\n";
			char sequenceC[100];
			std::cin >> sequenceC;
			std::string sequence = sequenceC;
			std::cerr << "before HMM func\n";
			std::cout << HMM(transitionTable, emissionsTable, startPorb, sequence, emissionsList).c_str() << std::endl;
			std::cout << "What do you want to see? C for cluster, H for HMM, N for NN. Press q to quit\n";
			std::cin >> choice;
		}
		else if (choice == 'n' || choice == 'N') {
			srand(time(NULL));
			int amountOfNodes;
			std::cout << "Please input how many nodes the hidden layer will have \n";
			std::cin >> amountOfNodes;
			AMOUNTOFHIDDEN = amountOfNodes;
			std::vector<std::vector<double>> inputToHidden;
			//Now we have to make the edges between the input layer and the hidden layer
			for (int i = 0; i < INPUTLAYERNODES; i++) {
				inputToHidden.push_back(std::vector<double>());
				for (int j = 0; j < amountOfNodes; j++) {
					double edgeWeight = (((rand() % 20) - 10) / 10.0);
					inputToHidden[i].push_back(edgeWeight);
				}
			}
			std::cout << "Input to hidden lay before training\n";
			for (int i = 0; i < INPUTLAYERNODES; i++) {

				for (int j = 0; j < AMOUNTOFHIDDEN; j++) {
					std::cout << inputToHidden[i][j] << ' ';
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;

			//Here we make the matrix for the edges that go from hidden layer to output layer
			std::vector<std::vector<double>> hiddenToOutput;
			for (int i = 0; i < amountOfNodes; i++) {
				hiddenToOutput.push_back(std::vector<double>());
				for (int j = 0; j < OUTPUTLAYERNODES; j++) {
					double edgeWeight = (((rand() % 20) - 10) / 10.0);
					hiddenToOutput[i].push_back(edgeWeight);
				}
			}
			std::cout << "hidden to output lay before training\n";
			for (int i = 0; i < AMOUNTOFHIDDEN; i++) {
				for (int j = 0; j < OUTPUTLAYERNODES; j++) {
					std::cout << hiddenToOutput[i][j] << ' ';
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			std::vector<std::vector<double>> inputs;
			std::vector<std::vector<double>> outputs;
			trainingData(inputs, outputs);
			std::cout << "How many iteration do you want?\n";
			int iteration;
			std::cin >> iteration;
			double errorRate;
			for (int i = 0; i < iteration; i++) {
				int randSelect = rand() % 100;
				auto output = NeuralNet(inputToHidden, hiddenToOutput, inputs[randSelect], outputs[randSelect]);
				std::cout << "The current error rate is: " << error(output, outputs[randSelect]) << std::endl;
				errorRate = error(output, outputs[randSelect]);
				train(inputToHidden, hiddenToOutput, inputs[randSelect], output, errorRate);
			}
			std::cout << "Input to hidden lay after training\n";
			for (int i = 0; i < INPUTLAYERNODES; i++) {

				for (int j = 0; j < AMOUNTOFHIDDEN; j++) {
					std::cout << inputToHidden[i][j] << ' ';
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			std::cout << "hidden to output lay after training\n";
			for (int i = 0; i < AMOUNTOFHIDDEN; i++) {
				for (int j = 0; j < OUTPUTLAYERNODES; j++) {
					std::cout << hiddenToOutput[i][j] << ' ';
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			std::cout << "The final error rate is: " << errorRate << std::endl;

			std::cout << "What do you want to see? C for cluster, H for HMM, N for NN. Press q to quit\n";
			std::cin >> choice;
		}

	}
	return 0;
}
double dot(std::vector<double> first, std::vector< std::vector<double>> second) {
	double dot = 0;

	for (int j = 0; j < first.size(); j++) {
		for (int k = 0; k < second[0].size(); k++) {
			dot += first[j] * second[j][k];
		}
	}
	return dot;
}
double error(std::vector<double> outout, std::vector<double>TrueOutput) {
	double sum = 0;
	for (int i = 0; i < OUTPUTLAYERNODES; i++) {
		sum += (outout[i] - TrueOutput[i]);
	}

	return sqrt(abs(sum));
}
void train(std::vector<std::vector<double>> &inputToHidden, std::vector<std::vector<double>> &hiddenToOutput, std::vector<double> inputs, std::vector<double> output, double error) {

	double adjust;
	double delta = 0;
	double dot = 0;
	//have to adjust weights with individual adjustments for multipling the error with the input then the derivative of the output 


	for (int i = 0; i < OUTPUTLAYERNODES; i++) {
		delta = error * derivative(output[i]);
	}
	for (int i = 0; i < INPUTLAYERNODES; i++) {
		dot += inputs[i] * delta;
	}
	adjust = dot;

	for (int i = 0; i < INPUTLAYERNODES; i++) {
		for (int j = 0; j < inputToHidden[0].size(); j++) {
			inputToHidden[i][j] += adjust;
		}
	}

	for (int i = 0; i < hiddenToOutput[0].size(); i++) {
		for (int j = 0; j < OUTPUTLAYERNODES; j++) {
			hiddenToOutput[i][j] += adjust;
		}
	}

}
double sigmoid(double val) {
	return  (1 / (1 + exp(-val)));
}
double derivative(double val) {
	return val * (1 - val);
}
void trainingData(std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& outputs) {
	for (int i = 0; i < 100; i++) {
		inputs.push_back(std::vector<double>());
		for (int j = 0; j < INPUTLAYERNODES; j++)
			inputs[i].push_back(rand() % 2);
	}
	for (int i = 0; i < 100; i++) {
		outputs.push_back(std::vector<double>());
		for (int j = 0; j < OUTPUTLAYERNODES; j++)
			outputs[i].push_back(rand() % 2);
	}
}
std::vector<double> NeuralNet(std::vector<std::vector<double>>&inputToHidden, std::vector<std::vector<double>>&hiddenToOutput, std::vector<double> inputs, std::vector<double> outputs) {
	std::vector<double> hiddenLayerActive;
	std::vector<double> output;

	//Runs the inputs through the input  to hidden layer
	for (int i = 0; i < AMOUNTOFHIDDEN; i++) {
		hiddenLayerActive.push_back(sigmoid(dot(inputs, inputToHidden)));
	}

	for (int i = 0; i < OUTPUTLAYERNODES; i++) {
		output.push_back(sigmoid(dot(hiddenLayerActive, hiddenToOutput)));
	}
	return output;
}
int findEmissions(char emission, std::string emissionList) {
	for (int i = 0; i < emissionList.size(); i++) {
		if (emissionList[i] == emission) {
			return i;
		}
	}
}
std::string HMM(std::vector<std::vector<double>> transitionTable, std::vector<std::vector<double>> emissionTable, std::vector<double> startProb, std::string emmisions, std::string emissionsList) {
	//doing viterbi. Used this psuedocode to help https://en.wikipedia.org/wiki/Viterbi_algorithm
	std::vector<std::vector<double>> seqProb;
	std::vector<std::vector<double>> seqPrev;
	int intialChar = findEmissions(emmisions[0], emissionsList);
	for (int i = 0; i < transitionTable.size(); i++) { //Setting the initial probablities for viterbi
		seqProb.push_back(std::vector<double>(emmisions.size(), 0));
		seqPrev.push_back(std::vector<double>(emmisions.size(), 0));
		seqProb[i][0] = startProb[i] * emissionTable[i][intialChar];
		seqPrev[i][0] = 0;
	}

	for (int j = 1; j < emmisions.size(); j++) { // filling in the probability table of viterbi so we can use it to find the opitmal sequence stored in
		for (int i = 0; i < transitionTable[0].size(); i++) {
			seqProb[i][j] = seqProb[0][j - 1] * transitionTable[0][i] * emissionTable[i][findEmissions(emmisions[j], emissionsList)];

			for (int s = 1; s < transitionTable.size(); s++) {
				double probOfSequence = seqProb[s][j - 1] * transitionTable[s][i] * emissionTable[i][findEmissions(emmisions[j], emissionsList)];
				if (seqProb[i][j] < probOfSequence) {
					seqProb[i][j] = probOfSequence;
					seqPrev[i][j] = s;
				}
			}
		}
	}

	std::vector<int> optimalSeq(emmisions.size(), 0); //Now we use the probablities to find the state sequence
	int last = emmisions.size() - 1;
	double max = seqProb[0][last];
	for (int i = 1; i < transitionTable.size(); i++) {
		if (max < seqProb[i][last]) {
			optimalSeq[last] = i;
			max = seqProb[i][last];

		}
	}
	if (max == 0) {
		return "The sequence is impossibe\n";
	}
	for (int i = last; i >= 1; i--) {
		optimalSeq[i - 1] = seqPrev[optimalSeq[i]][i];
	}
	std::string output;
	for (int i = 0; i < emmisions.size(); i++) {
		output.append(std::to_string(optimalSeq[i]));
		if (i + 1 != emmisions.size()) {
			output.append(", ");
		}
	}
	output.append(" and the probablity was ");
	output.append(std::to_string(max));
	return output;
}
void cluster(int kCentroids, std::deque < std::pair < float, float >> allPoints) {
	int iteration;
	float threshold;
	std::vector<std::vector<std::pair<float, float>>> clusters; // this is a matrix of the points with the centroids at the first slot of each row

	std::cout << "How many iterations do you want to run?\n";
	std::cin >> iteration;
	std::cout << "What is the threshold.(Can be a float)\n";
	std::cin >> threshold;

	for (int x = 0; x < iteration; x++) {
		if (x == 0) {
			std::cout << "K randomly selected centroids: ";
			for (int y = 0; y < kCentroids; y++) {
				std::vector<std::pair<float, float>> centroid;
				centroid.push_back(std::make_pair(rand() % 26, rand() % 26));// creates the centroids at random points
				clusters.push_back(centroid); //pushes the centroid vector into the matrix
				std::cout << clusters[y][0].first << "," << clusters[y][0].second << " ";
			}
		}
		else {// have to put the points back into the deque so that they can be reclustered 
			for (int cen = 0; cen < kCentroids; cen++) {
				while (clusters[cen].size() != 1) {
					allPoints.push_back(clusters[cen].back());
					clusters[cen].pop_back();
				}
			}
		}
		//Now we have to calculate which points belong in which cluster
		for (int cen = 0; cen < kCentroids; cen++) {
			for (int x = 0; x < allPoints.size(); x++) {
				float dist = sqrt(pow(clusters[cen][0].first - allPoints[x].first, 2)*1.0 + pow(clusters[cen][0].second - allPoints[x].second, 2) * 1.0);
				if (dist <= threshold) {
					clusters[cen].push_back(allPoints[x]);
					allPoints.erase(allPoints.begin() + x);//removes from deque so that we can ID outliers
				}
			}
		}
		std::cout << std::endl;
		//Output the current iteration of clusters
		for (int cen = 0; cen < kCentroids; cen++) {
			std::cout << "Current Cluster " << cen << std::endl;
			for (int points = 1; points < clusters[cen].size(); points++) {
				std::cout << "(" << clusters[cen][points].first << "," << clusters[cen][points].second << ") ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		//Output outliers for this iteration
		std::cout << "Outliers: ";
		for (int i = 0; i < allPoints.size(); i++) {// this won't run if there is no outliers 
			std::cout << "(" << allPoints[i].first << "," << allPoints[i].second << ") ";
		}
		//Calculate the new centroids of the clusters 
		for (int cen = 0; cen < kCentroids; cen++) {
			clusters[cen][0].second = 0;
			clusters[cen][0].first = 0;
			for (int points = 1; points < clusters[cen].size(); points++) {
				clusters[cen][0].first += clusters[cen][points].first;
				clusters[cen][0].second += clusters[cen][points].second;
			}
			clusters[cen][0].first = clusters[cen][0].first / clusters[cen].size();
			clusters[cen][0].second = clusters[cen][0].second / clusters[cen].size();
			std::cout << "New Centroid: " << "(" << clusters[cen][0].first << "," << clusters[cen][0].second << ") ";
		}
	}
}