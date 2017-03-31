//#include <fstream>
//#include <iostream>
//#include <deque>
//#include <vector>
//#include <iomanip>
//#include <sstream>
//#include <string>
//#include <cmath>
//using namespace std;
//
//struct TN {
//  TN *left, *right;
//  int value;
//  TN(int val) : left(nullptr), right(nullptr), value(val) { }
//};
//
//// Find the maximum height of the binary tree
//int maxHeight(TN *p) {
//  if (!p) return 0;
//  int leftHeight = maxHeight(p->left);
//  int rightHeight = maxHeight(p->right);
//  return (leftHeight > rightHeight) ? leftHeight + 1: rightHeight + 1;
//}
//
//// Convert an integer value to string
//string intToString(int val) {
//  ostringstream ss;
//  ss << val;
//  return ss.str();
//}
//
//// Print the arm branches (eg, /    \ ) on a line
//void printBranches(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const deque<TN*>& nodesQueue, ostream& out) {
//  deque<TN*>::const_iterator iter = nodesQueue.begin();
//  for (int i = 0; i < nodesInThisLevel / 2; i++) {
//    out << ((i == 0) ? setw(startLen-1) : setw(nodeSpaceLen-2)) << "" << ((*iter++) ? "/" : " ");
//    out << setw(2*branchLen+2) << "" << ((*iter++) ? "\\" : " ");
//  }
//  out << endl;
//}
//
//// Print the branches and node (eg, ___10___ )
//void printNodes(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const deque<TN*>& nodesQueue, ostream& out) {
//  deque<TN*>::const_iterator iter = nodesQueue.begin();
//  for (int i = 0; i < nodesInThisLevel; i++, iter++) {
//    out << ((i == 0) ? setw(startLen) : setw(nodeSpaceLen)) << "" << ((*iter && (*iter)->left) ? setfill('_') : setfill(' '));
//    out << setw(branchLen+2) << ((*iter) ? intToString((*iter)->value) : "");
//    out << ((*iter && (*iter)->right) ? setfill('_') : setfill(' ')) << setw(branchLen) << "" << setfill(' ');
//  }
//  out << endl;
//}
//
//// Print the leaves only (just for the bottom row)
//void printLeaves(int indentSpace, int level, int nodesInThisLevel, const deque<TN*>& nodesQueue, ostream& out) {
//  deque<TN*>::const_iterator iter = nodesQueue.begin();
//  for (int i = 0; i < nodesInThisLevel; i++, iter++) {
//    out << ((i == 0) ? setw(indentSpace+2) : setw(2*level+2)) << ((*iter) ? intToString((*iter)->value) : "");
//  }
//  out << endl;
//}
//
//// Pretty formatting of a binary tree to the output stream
//// @ param
//// level  Control how wide you want the tree to sparse (eg, level 1 has the minimum space between nodes, while level 2 has a larger space between nodes)
//// indentSpace  Change this to add some indent space to the left (eg, indentSpace of 0 means the lowest level of the left node will stick to the left margin)
//void printPretty(TN *root, int level, int indentSpace, ostream& out) {
//  int h = maxHeight(root);
//  int nodesInThisLevel = 1;
//
//  int branchLen = 2*((int)pow(2.0,h)-1) - (3-level)*(int)pow(2.0,h-1);  // eq of the length of branch for each node of each level
//  int nodeSpaceLen = 2 + (level+1)*(int)pow(2.0,h);  // distance between left neighbor node's right arm and right neighbor node's left arm
//  int startLen = branchLen + (3-level) + indentSpace;  // starting space to the first node to print of each level (for the left most node of each level only)
//
//  deque<TN*> nodesQueue;
//  nodesQueue.push_back(root);
//  for (int r = 1; r < h; r++) {
//    printBranches(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);
//    branchLen = branchLen/2 - 1;
//    nodeSpaceLen = nodeSpaceLen/2 + 1;
//    startLen = branchLen + (3-level) + indentSpace;
//    printNodes(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);
//
//    for (int i = 0; i < nodesInThisLevel; i++) {
//      TN *currNode = nodesQueue.front();
//      nodesQueue.pop_front();
//      if (currNode) {
//	      nodesQueue.push_back(currNode->left);
//	      nodesQueue.push_back(currNode->right);
//      } else {
//        nodesQueue.push_back(NULL);
//        nodesQueue.push_back(NULL);
//      }
//    }
//    nodesInThisLevel *= 2;
//  }
//  printBranches(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);
//  printLeaves(indentSpace, level, nodesInThisLevel, nodesQueue, out);
//}
//
//template<class T>
//void insert (TN*& t, T to_insert) {
//  if (t == nullptr)
//    t = new TN(to_insert);    //nullptr implicit for left/right subtrees
//  else
//    if (to_insert < t->value)
//      insert(t->left, to_insert);
//    else if (to_insert > t->value)
//      insert(t->right, to_insert);
//}
//
//
//void insert_r(TN* t, int element) {
//	if (t == nullptr) {
//		t = new TN(element);
//	}
//	else if (element > t->value) {
//		insert_r(t->right, element);
//	}
//	else {
//		insert_r(t->left, element);
//	}
//}
//
//int remove_closest(TN*& t) {
//  if (t->right == nullptr) {
//    int to_return = t->value;
//    TN* to_delete = t;
//    t = t->left;
//    delete to_delete;
//    return to_return;
//  }else
//    return remove_closest(t->right);
//}
//
//template<class T>
//void remove (TN*& t, T to_remove) {
//  if (t == nullptr)
//    return;
//  else
//    if (to_remove == t->value) {
//      if (t->left == nullptr) {
//        TN* to_delete = t;
//        t = t->right;
//        delete to_delete;
//      }else if (t->right == nullptr) {
//        TN* to_delete = t;
//        t = t->left;
//        delete to_delete;
//      }else
//        t->value = remove_closest(t->left);
//    }else
//      remove( (to_remove < t->value ? t->left : t->right), to_remove);
//}
//
//
//int size(TN* t) {
//  if (t == nullptr)
//    return 0;
//  else
//    return 1 + size(t->left) + size(t->right);
//}
//
//template <class T>
//bool all_less(TN* t, T v) {
//	bool result;
//	if (t == nullptr) {
//		std::cout << "true" << std::endl;
//		return result;
//	}
//	result = (t->value < v && all_less(t->left, v) && all_less(t->right, v));
//	return result;
//}
//
//int main() {
//	TN* t = new TN(20);
//	std::vector<int> v {10, 15, 4, 34, 23, 7, 40, 12, 31, 30, 16, 9, 5, 45, 42};
//	for (auto i : v) {
//		insert(t, i);
//	}
//	insert_r(t, 15);
//	printPretty(t, 1, 0, cout);
//	//all_less(t, 2);
//	//std::cout << all_less(t, 100) << std::endl;
//
//
//  return 0;
//}
//
//
//
