//====================================================================
// Name        : projectTwo.cpp
// Author      : Kyra Christensen
// Version     : 1.0
// Description : CS-300 Project Two
//====================================================================

#include <iostream>
#include <fstream>>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// --- Data Structures ---

// Structure to hold individual course information
struct Course {
	string courseNumber;
	string courseTitle;
	vector<string> prerequisites;

	Course() {}; // Default constructor
};

// Internal structure for the Tree Nodes
struct Node {
	Course course;
	Node* left;
	Node* right;
	
	// Default constructor
	Node() {
		left = nullptr;
		right = nullptr;
	}

	// Initialize with a course
	Node(Course aCourse) : Node() {
		this->course = aCourse;
	}
};

// --- Binary Search Tree Class ---
class BinarySearchTree {

private:
	Node* root;

	void addNode(Node* node, Course course);
	void inOrder(Node* node); 
	void deleteTree(Node* node);

public:
	BinarySearchTree();
	virtual ~BinarySearchTree();
	void Insert(Course course);
	void PrintInOrder();
	Course Search(string courseNumber);
}; 

/**
* Default Constructor 
*/
BinarySearchTree::BinarySearchTree() {
	root = nullptr;
}

/**
* Destructor to prevent memory leaks
*/

BinarySearchTree::~BinarySearchTree() {
	deleteTree(root);
}

/**
* Helper to recursively delete nodes
*/
void BinarySearchTree::deleteTree(Node* node) {
	if (node != nullptr) {
		deleteTree(node->left);
		deleteTree(node->right);
		delete node;
	}
}

/**
* Insert a course into the BST
*/
void BinarySearchTree::Insert(Course course) {
	if (root == nullptr) {
		root = new Node(course);
	} else {
		this->addNode(root, course);
	}
}

/**
* Recursive helper for insertion (alphanumeric logic)
*/
void BinarySearchTree::addNode(Node* node, Course course) {
	// If new course number is less than current node, go left
	if (node->course.courseNumber.compare(course.courseNumber) > 0) {
		if (node->left == nullptr) {
			node->left = new Node(course);
		}
		else {
			this->addNode(node->left, course);
		}
	}
	// Otherwise, go right
	else {
		if (node->right == nullptr) {
			node->right = new Node(course);
		}
		else {
			this->addNode(node->right, course);
		}
	}
}

/**
* Public accessor for In-Order printing
*/
void BinarySearchTree::PrintInOrder() {
	this->inOrder(root);
}

/**
* Helper to traverse the tree (Left, Root, Right)
*/
void BinarySearchTree::inOrder(Node* node) {
	if (node != nullptr) {
		this->inOrder(node->left);
		cout << node->course.courseNumber << ": " << node->course.courseTitle << endl;
		this->inOrder(node->right);
	}
}

/**
* Search for a course by course number
*/
Course BinarySearchTree::Search(string courseNumber) {
	Node* current = root;
	while (current != nullptr) {
		if (current->course.courseNumber == courseNumber) {
			return current->course; // Found the course
		}
		else if (current->course.courseNumber.compare(courseNumber) > 0) {
			current = current->left; // Go left
		}
		else {
			current = current->right; // Go right
		}
	}
	throw runtime_error("Course not found: " + courseNumber); // If not found, throw an error
}

// --- File Parsing and Helper Functions ---

/*
* Manual string splitter to replace CSVParser requirements
*/
vector<string> tokenize(string line, char delimiter) {
	vector<string> tokens;
	stringstream ss(line);
	string token;
	while (getline(ss, token, delimiter)) {
		// Remove potential carriage returns from Windows-formatted files
		if (!token.empty() && token.back() == '\r') {
			token.erase(token.size() - 1);
		}
		tokens.push_back(token);
	}
	return tokens;
}

/*
* Function to load ddata from file into the BST
*/
void loadCourses(string fileName, BinarySearchTree* bst) {
	ifstream file(fileName);
	if (!file.is_open()) {
		cout << "Error: File '" << fileName << "' not found." << endl;
		return;
	}

	string line;
	int count = 0;
	while(getline(file, line)) {
		vector<string> tokens = tokenize(line, ',');

		//Basic validation: must have at least Number and Title
		if (tokens.size() < 2) {
			cout << "Skipping malformed line." << endl;
			continue;
		}

		Course course;
		course.courseNumber = tokens[0];
		course.courseTitle = tokens[1];

		// Any tokens after index 1 are prerequisites
		for (size_t i = 2; i < tokens.size(); i++) {
			course.prerequisites.push_back(tokens[i]);
		}

		bst->Insert(course);
		count++;
	}
	file.close();
	cout << count << " courses loaded." << endl;
}

// --- Main Menu Interface ---

int main() {
	BinarySearchTree* bst = new BinarySearchTree();
	string filename, searchKey;
	int choice = 0;

	cout << "Welcome to the ABCU Course Planner." << endl;

	while (choice != 9) {
		cout << "\n1. Load Data Structure" << endl;
		cout << "2. Print Course List" << endl;
		cout << "3. Print Course" << endl;
		cout << "9. Exit" << endl;
		cout << "What would you like to do? ";

		// Error handling for non0numeric input
		if (!(cin >> choice)) {
			cout << "Invalid input. Please enter a number." << endl;
			cin.clear();
			cin.ignore(100, '\n');
			continue;
		}

		switch (choice) {
		case 1:
			cout << "Enter the file name containg course data: ";
			cin >> filename;
			loadCourses(filename, bst);
			break;

		case 2: 
			cout << "Alphanumeric Course List:" << endl;
			bst->PrintInOrder();
			break;

		case 3:
			cout << "Enter course number: ";
			cin >> searchKey;
			// Standardize input to uppercase
			transform(searchKey.begin(), searchKey.end(), searchKey.begin(), ::toupper);

			{
				Course result = bst->Search(searchKey);
				if (!result.courseNumber.empty()) {
					cout << result.courseNumber << ", " << result.courseTitle << endl;
					cout << "Prerequisites: ";
					if (result.prerequisites.empty()) {
						cout << "None";
					}
					else {
						for (size_t i = 0; i < result.prerequisites.size(); i++) {
							cout << result.prerequisites[i];
							if (i < result.prerequisites.size() - 1) cout << ", ";
						}
					}
					cout << endl;
				}
				else {
					cout << "Course not found." << endl;
				}
			}
			break;

		case 9:
			cout << "Exiting program. Goodbye!" << endl;
			break;

		default:
			cout << choice << " is not a valid option." << endl;
			break;
		}
	}

	delete bst;  // Clean up memory
	return 0;
}